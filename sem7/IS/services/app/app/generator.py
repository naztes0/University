import asyncio
import contextvars
import logging
import math
import random
import time
from datetime import UTC, datetime
from uuid import UUID, uuid4

from opentelemetry import trace
from opentelemetry.context import Context
from opentelemetry.trace import Status, StatusCode
from psycopg import Error as PsycopgError
from psycopg_pool import PoolTimeout

from .database import Database
from .metrics import (
    CONFIGURED_STATIONS,
    DB_WRITE_ATTEMPTS,
    DB_WRITE_DURATION,
    GENERATOR_RUNNING,
    MAX_CONCURRENCY,
    READINGS_INSERTED,
    REQUESTED_RATE,
    STATION_READINGS,
    WRITE_OPERATIONS,
)
from .models import LoadConfig
from .observability import log_event


logger = logging.getLogger("charging")
tracer = trace.get_tracer("charging-app.generator")


class AlreadyRunningError(RuntimeError):
    pass


class LoadGenerator:
    def __init__(self, database: Database) -> None:
        self.database = database
        self._config = LoadConfig()
        self._state_lock = asyncio.Lock()
        self._task: asyncio.Task[None] | None = None
        self._stop_event = asyncio.Event()
        self._requested_final_status = "stopped"
        self._run_id: UUID | None = None
        self._started_at: datetime | None = None
        self._started_monotonic: float | None = None
        self._finished_monotonic: float | None = None
        self._run_status = "stopped"
        self._successful_readings = 0
        self._failed_operations = 0
        self._deduplicated_operations = 0
        self._last_error: str | None = None
        self._random = random.Random()
        self._update_config_metrics(self._config)

    @staticmethod
    def _update_config_metrics(config: LoadConfig) -> None:
        REQUESTED_RATE.set(config.requested_readings_per_second)
        CONFIGURED_STATIONS.set(config.station_count)
        MAX_CONCURRENCY.set(config.max_concurrency)

    async def configure(self, config: LoadConfig) -> dict[str, object]:
        async with self._state_lock:
            self._config = config
            self._update_config_metrics(config)
            log_event(
                logger,
                logging.INFO,
                "load_config_changed",
                "Load generator configuration changed",
                station_count=config.station_count,
                interval_ms=config.interval_ms,
                duration_seconds=config.duration_seconds,
                max_concurrency=config.max_concurrency,
                requested_rate=round(config.requested_readings_per_second, 3),
            )
            return self._status_unlocked()

    async def start(self) -> dict[str, object]:
        async with self._state_lock:
            if self._task is not None and not self._task.done():
                raise AlreadyRunningError("A load run is already active")

            await self.database.mark_orphaned_runs()
            run_id = uuid4()
            await self.database.create_run(
                run_id,
                self._config.station_count,
                self._config.interval_ms,
            )

            self._run_id = run_id
            self._started_at = datetime.now(UTC)
            self._started_monotonic = time.monotonic()
            self._finished_monotonic = None
            self._run_status = "running"
            self._successful_readings = 0
            self._failed_operations = 0
            self._deduplicated_operations = 0
            self._last_error = None
            self._requested_final_status = "stopped"
            self._stop_event = asyncio.Event()
            GENERATOR_RUNNING.set(1)
            self._task = asyncio.create_task(
                self._run(),
                name=f"load-run-{run_id}",
                context=contextvars.Context(),
            )
            log_event(
                logger,
                logging.INFO,
                "load_started",
                "Load generation started",
                operation="load_run",
                run_id=str(run_id),
                station_count=self._config.station_count,
                interval_ms=self._config.interval_ms,
                duration_seconds=self._config.duration_seconds,
                max_concurrency=self._config.max_concurrency,
                requested_rate=round(
                    self._config.requested_readings_per_second, 3
                ),
            )
            return self._status_unlocked()

    async def stop(self, final_status: str = "stopped") -> dict[str, object]:
        async with self._state_lock:
            task = self._task
            if task is None or task.done():
                return self._status_unlocked()
            self._requested_final_status = final_status
            self._stop_event.set()
            log_event(
                logger,
                logging.INFO,
                "load_stop_requested",
                "Load generation stop requested",
                operation="load_run",
                run_id=str(self._run_id),
                reason=final_status,
            )

        await task
        return await self.status()

    async def status(self) -> dict[str, object]:
        async with self._state_lock:
            return self._status_unlocked()

    def _status_unlocked(self) -> dict[str, object]:
        elapsed = 0.0
        if self._started_monotonic is not None:
            end = self._finished_monotonic or time.monotonic()
            elapsed = max(0.0, end - self._started_monotonic)
        actual_rate = self._successful_readings / elapsed if elapsed > 0 else 0.0
        running = self._task is not None and not self._task.done()
        return {
            "running": running,
            "run_id": str(self._run_id) if self._run_id else None,
            "run_status": self._run_status,
            "started_at": self._started_at.isoformat() if self._started_at else None,
            "elapsed_seconds": round(elapsed, 3),
            "config": self._config.model_dump(),
            "requested_readings_per_second": round(
                self._config.requested_readings_per_second, 3
            ),
            "actual_readings_per_second": round(actual_rate, 3),
            "successful_readings": self._successful_readings,
            "failed_operations": self._failed_operations,
            "deduplicated_operations": self._deduplicated_operations,
            "last_error": self._last_error,
        }

    async def _run(self) -> None:
        assert self._run_id is not None
        final_status = "completed"
        last_summary_elapsed = 0.0
        tick_number = 0
        try:
            while not self._stop_event.is_set():
                tick_started = time.monotonic()
                tick_number += 1
                async with self._state_lock:
                    config = self._config.model_copy()
                    elapsed = tick_started - (self._started_monotonic or tick_started)
                if elapsed >= config.duration_seconds:
                    break

                with tracer.start_as_current_span(
                    "load.tick",
                    context=Context(),
                    attributes={
                        "run.id": str(self._run_id),
                        "station.count": config.station_count,
                        "load.interval_ms": config.interval_ms,
                        "load.max_concurrency": config.max_concurrency,
                        "load.tick_number": tick_number,
                        # Keep one representative trace for every run. Later ticks
                        # use the configured probabilistic sampling ratio.
                        "diagnostic.force_sample": tick_number == 1,
                    },
                ) as tick_span:
                    if tick_number == 1:
                        log_event(
                            logger,
                            logging.INFO,
                            "load_trace_created",
                            "Representative load trace created",
                            operation="load_tick",
                            run_id=str(self._run_id),
                            sampled=tick_span.get_span_context().trace_flags.sampled,
                        )
                    semaphore = asyncio.Semaphore(config.max_concurrency)
                    tasks = [
                        asyncio.create_task(
                            self._write_station(
                                semaphore,
                                station_id,
                                config.interval_ms,
                            )
                        )
                        for station_id in range(1, config.station_count + 1)
                    ]
                    await asyncio.gather(*tasks)

                current_elapsed = time.monotonic() - (
                    self._started_monotonic or time.monotonic()
                )
                if current_elapsed - last_summary_elapsed >= 10:
                    last_summary_elapsed = current_elapsed
                    log_event(
                        logger,
                        logging.INFO,
                        "load_progress",
                        "Periodic load generation summary",
                        operation="load_run",
                        run_id=str(self._run_id),
                        duration_ms=round(current_elapsed * 1_000, 3),
                        successful_readings=self._successful_readings,
                        failed_operations=self._failed_operations,
                        deduplicated_operations=self._deduplicated_operations,
                    )

                remaining = config.interval_ms / 1_000 - (time.monotonic() - tick_started)
                if remaining > 0:
                    try:
                        await asyncio.wait_for(self._stop_event.wait(), timeout=remaining)
                    except TimeoutError:
                        pass

            if self._stop_event.is_set():
                final_status = self._requested_final_status
        except asyncio.CancelledError:
            final_status = "interrupted"
            raise
        except Exception as error:
            final_status = "failed"
            self._last_error = f"{type(error).__name__}: {error}"
            log_event(
                logger,
                logging.ERROR,
                "load_failed",
                "Load generation failed",
                operation="load_run",
                run_id=str(self._run_id),
                error_type=type(error).__name__,
                error=str(error),
            )
        finally:
            self._run_status = final_status
            self._finished_monotonic = time.monotonic()
            GENERATOR_RUNNING.set(0)
            try:
                await self.database.finish_run(self._run_id, final_status)
            except Exception as error:
                self._last_error = f"Could not finalize run: {type(error).__name__}: {error}"
                log_event(
                    logger,
                    logging.ERROR,
                    "load_finalize_failed",
                    "Could not persist final load status",
                    operation="load_run",
                    run_id=str(self._run_id),
                    error_type=type(error).__name__,
                    error=str(error),
                )
            elapsed_ms = 0.0
            if self._started_monotonic is not None:
                elapsed_ms = (
                    self._finished_monotonic - self._started_monotonic
                ) * 1_000
            log_event(
                logger,
                logging.INFO,
                "load_finished",
                "Load generation finished",
                operation="load_run",
                run_id=str(self._run_id),
                status=final_status,
                duration_ms=round(elapsed_ms, 3),
                successful_readings=self._successful_readings,
                failed_operations=self._failed_operations,
                deduplicated_operations=self._deduplicated_operations,
            )

    async def _write_station(
        self,
        semaphore: asyncio.Semaphore,
        station_id: int,
        interval_ms: int,
    ) -> None:
        if self._stop_event.is_set():
            return

        operation_id = uuid4()
        reading = self._make_reading(interval_ms)
        delays = (0.0, 0.2, 0.5)

        with tracer.start_as_current_span(
            "load.write_station",
            attributes={
                "run.id": str(self._run_id),
                "operation.id": str(operation_id),
                "station.id": station_id,
            },
        ) as span:
            async with semaphore:
                for attempt, delay in enumerate(delays, start=1):
                    if self._stop_event.is_set() and attempt == 1:
                        return
                    if delay:
                        await asyncio.sleep(delay)
                    attempt_started = time.monotonic()
                    try:
                        inserted = await self.database.write_reading(
                            operation_id=operation_id,
                            station_id=station_id,
                            run_id=self._run_id,
                            measured_at=datetime.now(UTC),
                            **reading,
                        )
                        DB_WRITE_ATTEMPTS.labels(result="success").inc()
                        self._successful_readings += 1
                        span.set_attribute("load.retry_count", attempt - 1)
                        span.set_attribute(
                            "load.write_result",
                            "inserted" if inserted else "deduplicated",
                        )
                        if not inserted:
                            self._deduplicated_operations += 1
                            WRITE_OPERATIONS.labels(result="deduplicated").inc()
                        else:
                            READINGS_INSERTED.inc()
                            STATION_READINGS.labels(status=reading["status"]).inc()
                            WRITE_OPERATIONS.labels(result="success").inc()
                        self._last_error = None
                        return
                    except (PsycopgError, PoolTimeout, OSError) as error:
                        DB_WRITE_ATTEMPTS.labels(result="error").inc()
                        self._last_error = f"{type(error).__name__}: {error}"
                        if attempt == len(delays):
                            span.record_exception(error)
                            span.set_status(Status(StatusCode.ERROR, type(error).__name__))
                            span.set_attribute("load.retry_count", attempt - 1)
                            span.set_attribute("load.write_result", "failed")
                            self._failed_operations += 1
                            WRITE_OPERATIONS.labels(result="failed").inc()
                            log_event(
                                logger,
                                logging.ERROR,
                                "db_write_failed",
                                "Station reading was not written after retries",
                                operation="insert_reading",
                                run_id=str(self._run_id),
                                operation_id=str(operation_id),
                                station_id=station_id,
                                error_type=type(error).__name__,
                                error=str(error),
                                sqlstate=getattr(error, "sqlstate", None),
                            )
                    finally:
                        DB_WRITE_DURATION.observe(time.monotonic() - attempt_started)

    def _make_reading(self, interval_ms: int) -> dict[str, float | str]:
        status_roll = self._random.random()
        if status_roll < 0.82:
            status = "charging"
        elif status_roll < 0.94:
            status = "idle"
        elif status_roll < 0.97:
            status = "offline"
        else:
            status = "error"

        if status == "charging":
            voltage_v = self._random.uniform(380, 420)
            power_kw = self._random.uniform(20, 300)
            current_a = power_kw * 1_000 / (math.sqrt(3) * voltage_v)
            energy_delta_kwh = power_kw * interval_ms / 3_600_000
            temperature_c = self._random.uniform(25, 65)
        elif status == "idle":
            voltage_v = self._random.uniform(380, 420)
            power_kw = 0.0
            current_a = 0.0
            energy_delta_kwh = 0.0
            temperature_c = self._random.uniform(15, 35)
        else:
            voltage_v = 0.0
            power_kw = 0.0
            current_a = 0.0
            energy_delta_kwh = 0.0
            temperature_c = self._random.uniform(-10, 55)

        return {
            "power_kw": round(power_kw, 3),
            "voltage_v": round(voltage_v, 3),
            "current_a": round(current_a, 3),
            "energy_delta_kwh": round(energy_delta_kwh, 6),
            "temperature_c": round(temperature_c, 2),
            "status": status,
        }
