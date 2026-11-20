import os
from datetime import datetime
from uuid import UUID

from psycopg.conninfo import make_conninfo
from psycopg_pool import AsyncConnectionPool


class Database:
    def __init__(self) -> None:
        required = ("DB_HOST", "DB_PORT", "DB_NAME", "DB_USER", "DB_PASSWORD")
        missing = [name for name in required if not os.getenv(name)]
        if missing:
            raise RuntimeError(f"Missing database settings: {', '.join(missing)}")

        conninfo = make_conninfo(
            "",
            host=os.environ["DB_HOST"],
            port=os.environ["DB_PORT"],
            dbname=os.environ["DB_NAME"],
            user=os.environ["DB_USER"],
            password=os.environ["DB_PASSWORD"],
            connect_timeout="3",
            options="-c statement_timeout=3000",
        )
        self.pool = AsyncConnectionPool(
            conninfo=conninfo,
            min_size=1,
            max_size=5,
            max_waiting=20,
            timeout=3,
            reconnect_timeout=15,
            open=False,
        )

    async def open(self) -> None:
        # Opening without wait keeps the HTTP server alive while PostgreSQL is unavailable.
        await self.pool.open(wait=False)

    async def close(self) -> None:
        await self.pool.close()

    async def is_ready(self) -> bool:
        try:
            async with self.pool.connection(timeout=2) as connection:
                await connection.execute("SELECT 1")
            return True
        except Exception:
            return False

    async def mark_orphaned_runs(self) -> None:
        async with self.pool.connection(timeout=3) as connection:
            await connection.execute(
                """
                UPDATE load_runs
                SET status = 'interrupted', finished_at = now()
                WHERE status = 'running'
                """
            )

    async def create_run(
        self,
        run_id: UUID,
        station_count: int,
        interval_ms: int,
    ) -> None:
        async with self.pool.connection(timeout=3) as connection:
            await connection.execute(
                """
                INSERT INTO load_runs (id, station_count, interval_ms)
                VALUES (%s, %s, %s)
                """,
                (run_id, station_count, interval_ms),
            )

    async def finish_run(self, run_id: UUID, status: str) -> None:
        allowed_statuses = {"completed", "stopped", "failed", "interrupted"}
        if status not in allowed_statuses:
            raise ValueError(f"Unsupported final run status: {status}")

        async with self.pool.connection(timeout=3) as connection:
            await connection.execute(
                """
                UPDATE load_runs
                SET status = %s, finished_at = now()
                WHERE id = %s AND status = 'running'
                """,
                (status, run_id),
            )

    async def write_reading(
        self,
        *,
        operation_id: UUID,
        station_id: int,
        run_id: UUID,
        measured_at: datetime,
        power_kw: float,
        voltage_v: float,
        current_a: float,
        energy_delta_kwh: float,
        temperature_c: float,
        status: str,
    ) -> bool:
        async with self.pool.connection(timeout=3) as connection:
            async with connection.transaction():
                cursor = await connection.execute(
                    """
                    INSERT INTO readings (
                        operation_id, station_id, run_id, measured_at,
                        power_kw, voltage_v, current_a, energy_delta_kwh,
                        temperature_c, status
                    )
                    VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
                    ON CONFLICT (operation_id) DO NOTHING
                    RETURNING id
                    """,
                    (
                        operation_id,
                        station_id,
                        run_id,
                        measured_at,
                        power_kw,
                        voltage_v,
                        current_a,
                        energy_delta_kwh,
                        temperature_c,
                        status,
                    ),
                )
                inserted = await cursor.fetchone()
                if inserted is not None:
                    await connection.execute(
                        """
                        UPDATE stations
                        SET status = %s, updated_at = %s
                        WHERE id = %s AND status IS DISTINCT FROM %s
                        """,
                        (status, measured_at, station_id, status),
                    )
                return inserted is not None

