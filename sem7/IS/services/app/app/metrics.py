from prometheus_client import Counter, Gauge, Histogram


GENERATOR_RUNNING = Gauge(
    "charging_generator_running",
    "Whether the charging-station load generator is currently running.",
)
REQUESTED_RATE = Gauge(
    "charging_generator_requested_readings_per_second",
    "Configured number of station readings requested per second.",
)
CONFIGURED_STATIONS = Gauge(
    "charging_generator_configured_stations",
    "Number of stations configured for the next generator tick.",
)
MAX_CONCURRENCY = Gauge(
    "charging_generator_max_concurrency",
    "Maximum number of concurrent Python write tasks.",
)

READINGS_INSERTED = Counter(
    "charging_readings_inserted_total",
    "Number of new reading rows confirmed by PostgreSQL.",
)
STATION_READINGS = Counter(
    "charging_station_readings_total",
    "Number of readings confirmed by PostgreSQL, grouped by station status.",
    ("status",),
)
WRITE_OPERATIONS = Counter(
    "charging_write_operations_total",
    "Final result of logical station write operations.",
    ("result",),
)
DB_WRITE_ATTEMPTS = Counter(
    "charging_db_write_attempts_total",
    "Result of individual PostgreSQL write attempts, including retries.",
    ("result",),
)
DB_WRITE_DURATION = Histogram(
    "charging_db_write_duration_seconds",
    "Duration of an individual PostgreSQL write attempt.",
    buckets=(0.001, 0.0025, 0.005, 0.01, 0.025, 0.05, 0.1, 0.25, 0.5, 1, 2.5, 5),
)


# Create bounded label series immediately, so zero values are visible before an error occurs.
for result in ("success", "deduplicated", "failed"):
    WRITE_OPERATIONS.labels(result=result).inc(0)
for result in ("success", "error"):
    DB_WRITE_ATTEMPTS.labels(result=result).inc(0)
for station_status in ("charging", "idle", "offline", "error"):
    STATION_READINGS.labels(status=station_status).inc(0)
