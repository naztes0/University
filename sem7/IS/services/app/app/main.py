import logging
import time
from contextlib import asynccontextmanager
from pathlib import Path
from uuid import uuid4

from fastapi import FastAPI, HTTPException, Response, status
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles
from opentelemetry import trace
from opentelemetry.context import Context
from opentelemetry.trace import Status, StatusCode
from prometheus_client import CONTENT_TYPE_LATEST, generate_latest
from psycopg import Error as PsycopgError

from .database import Database
from .generator import AlreadyRunningError, LoadGenerator
from .metrics import DB_WRITE_ATTEMPTS, DB_WRITE_DURATION, WRITE_OPERATIONS
from .models import LoadConfig
from .observability import configure_logging, configure_tracing, log_event

logger = configure_logging()
tracer = trace.get_tracer("charging-app.api")
database = Database()
generator = LoadGenerator(database)


@asynccontextmanager
async def lifespan(_: FastAPI):
    await database.open()
    log_event(logger, logging.INFO, "service_started", "Application service started")
    try:
        try:
            await database.mark_orphaned_runs()
        except Exception as error:
            # Readiness reports the database failure; HTTP stays available.
            log_event(
                logger,
                logging.ERROR,
                "startup_database_unavailable",
                "Database was unavailable during startup cleanup",
                operation="mark_orphaned_runs",
                error_type=type(error).__name__,
                error=str(error),
            )
        yield
    finally:
        await generator.stop(final_status="interrupted")
        await database.close()
        log_event(logger, logging.INFO, "service_stopped", "Application service stopped")
        tracer_provider.shutdown()


app = FastAPI(
    title="Charging Station Load Generator",
    version="0.1.0",
    lifespan=lifespan,
)


static_path = Path(__file__).parent / "static"
app.mount("/static", StaticFiles(directory=static_path), name="static")


@app.get("/")
async def root():
    return FileResponse(static_path / "index.html")


@app.get("/api/stations")
async def stations():
    try:
        return await database.latest_stations()
    except Exception as error:
        raise HTTPException(status_code=503, detail="PostgreSQL недоступний") from error


@app.get("/healthz")
async def health() -> dict[str, str]:
    return {"status": "ok"}


@app.get("/metrics", include_in_schema=False)
async def metrics() -> Response:
    return Response(
        content=generate_latest(),
        headers={"Content-Type": CONTENT_TYPE_LATEST},
    )


@app.get("/readyz")
async def readiness(response: Response) -> dict[str, str]:
    if await database.is_ready():
        return {"status": "ready"}
    response.status_code = status.HTTP_503_SERVICE_UNAVAILABLE
    return {"status": "not ready"}


@app.get("/api/load/status")
async def load_status() -> dict[str, object]:
    return await generator.status()


@app.put("/api/load/config")
async def update_load_config(config: LoadConfig) -> dict[str, object]:
    return await generator.configure(config)


@app.post("/api/load/start", status_code=status.HTTP_201_CREATED)
async def start_load() -> dict[str, object]:
    try:
        return await generator.start()
    except AlreadyRunningError as error:
        raise HTTPException(status_code=status.HTTP_409_CONFLICT, detail=str(error)) from error
    except Exception as error:
        raise HTTPException(
            status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
            detail=f"PostgreSQL is unavailable: {type(error).__name__}",
        ) from error


@app.post("/api/load/stop")
async def stop_load() -> dict[str, object]:
    return await generator.stop()


@app.post("/api/demo/failed-write")
async def demonstrate_failed_write() -> dict[str, object]:
    operation_id = uuid4()
    current = await generator.status()
    run_id = current.get("run_id")
    with tracer.start_as_current_span(
        "demo.failed_write",
        context=Context(),
        attributes={
            "diagnostic.force_sample": True,
            "operation.id": str(operation_id),
        },
    ) as span:
        trace_id = format(span.get_span_context().trace_id, "032x")
        started = time.monotonic()
        try:
            await database.demonstrate_constraint_violation(operation_id)
        except PsycopgError as error:
            duration_ms = (time.monotonic() - started) * 1_000
            sqlstate = getattr(error, "sqlstate", None)
            span.record_exception(error)
            span.set_status(Status(StatusCode.ERROR, "expected constraint violation"))
            if sqlstate:
                span.set_attribute("db.response.status_code", sqlstate)
            DB_WRITE_ATTEMPTS.labels(result="error").inc()
            WRITE_OPERATIONS.labels(result="failed").inc()
            DB_WRITE_DURATION.observe(duration_ms / 1_000)
            log_event(
                logger,
                logging.ERROR,
                "demo_db_write_rejected",
                "PostgreSQL rejected the controlled invalid operation",
                operation="insert_station",
                run_id=run_id,
                operation_id=str(operation_id),
                duration_ms=round(duration_ms, 3),
                error_type=type(error).__name__,
                error=str(error),
                sqlstate=sqlstate,
            )
            return {
                "status": "expected_failure",
                "operation_id": str(operation_id),
                "run_id": run_id,
                "sqlstate": sqlstate,
                "trace_id": trace_id,
            }
        except Exception as error:
            span.record_exception(error)
            span.set_status(Status(StatusCode.ERROR, type(error).__name__))
            log_event(
                logger,
                logging.ERROR,
                "demo_db_unavailable",
                "Controlled database operation could not be executed",
                operation="insert_station",
                run_id=run_id,
                operation_id=str(operation_id),
                error_type=type(error).__name__,
                error=str(error),
            )
            raise HTTPException(
                status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
                detail="PostgreSQL is unavailable",
            ) from error

        span.set_status(Status(StatusCode.ERROR, "constraint unexpectedly missing"))
        log_event(
            logger,
            logging.CRITICAL,
            "demo_constraint_missing",
            "Controlled invalid operation was unexpectedly accepted",
            operation="insert_station",
            run_id=run_id,
            operation_id=str(operation_id),
        )
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Expected PostgreSQL constraint was not enforced",
        )


tracer_provider = configure_tracing(app)
