import json
import logging
import os
from datetime import UTC, datetime
from logging.handlers import RotatingFileHandler
from pathlib import Path

from fastapi import FastAPI
from opentelemetry import trace
from opentelemetry.exporter.otlp.proto.http.trace_exporter import OTLPSpanExporter
from opentelemetry.instrumentation.fastapi import FastAPIInstrumentor
from opentelemetry.sdk.resources import SERVICE_NAME, SERVICE_VERSION, Resource
from opentelemetry.sdk.trace import TracerProvider
from opentelemetry.sdk.trace.export import BatchSpanProcessor
from opentelemetry.sdk.trace.sampling import (
    Decision,
    ParentBased,
    Sampler,
    SamplingResult,
    TraceIdRatioBased,
)


SAFE_FIELDS = {
    "event",
    "operation",
    "run_id",
    "operation_id",
    "station_id",
    "duration_ms",
    "status",
    "station_count",
    "interval_ms",
    "duration_seconds",
    "max_concurrency",
    "requested_rate",
    "successful_readings",
    "failed_operations",
    "deduplicated_operations",
    "error_type",
    "error",
    "sqlstate",
    "reason",
    "trace_id",
    "span_id",
    "sampled",
}


class DiagnosticSampler(Sampler):
    """Sample diagnostic traces always and regular root traces by ratio."""

    def __init__(self, ratio: float) -> None:
        self._delegate = ParentBased(root=TraceIdRatioBased(ratio))

    def should_sample(
        self,
        parent_context,
        trace_id,
        name,
        kind=None,
        attributes=None,
        links=None,
        trace_state=None,
    ) -> SamplingResult:
        if attributes and attributes.get("diagnostic.force_sample") is True:
            return SamplingResult(
                Decision.RECORD_AND_SAMPLE,
                attributes=attributes,
                trace_state=trace_state,
            )
        return self._delegate.should_sample(
            parent_context,
            trace_id,
            name,
            kind,
            attributes,
            links,
            trace_state,
        )

    def get_description(self) -> str:
        return f"DiagnosticSampler({self._delegate.get_description()})"


class JsonFormatter(logging.Formatter):
    def format(self, record: logging.LogRecord) -> str:
        document = {
            "timestamp": datetime.fromtimestamp(record.created, UTC).isoformat(
                timespec="milliseconds"
            ),
            "service": "charging-app",
            "level": record.levelname.lower(),
            "event": getattr(record, "event", "application_log"),
            "message": record.getMessage(),
        }
        for field in SAFE_FIELDS:
            value = getattr(record, field, None)
            if value is not None:
                document[field] = value
        return json.dumps(document, ensure_ascii=False, separators=(",", ":"))


def configure_logging() -> logging.Logger:
    logger = logging.getLogger("charging")
    if logger.handlers:
        return logger

    log_path = Path(os.getenv("APP_LOG_PATH", "/var/log/charging/app.jsonl"))
    log_path.parent.mkdir(parents=True, exist_ok=True)
    max_bytes = int(os.getenv("APP_LOG_MAX_BYTES", str(5 * 1024 * 1024)))
    backup_count = int(os.getenv("APP_LOG_BACKUP_COUNT", "3"))

    handler = RotatingFileHandler(
        log_path,
        maxBytes=max_bytes,
        backupCount=backup_count,
        encoding="utf-8",
    )
    handler.setFormatter(JsonFormatter())
    logger.addHandler(handler)
    logger.setLevel(logging.INFO)
    logger.propagate = False
    return logger


def configure_tracing(app: FastAPI) -> TracerProvider:
    ratio = float(os.getenv("OTEL_TRACE_SAMPLE_RATIO", "0.1"))
    if not 0 <= ratio <= 1:
        raise ValueError("OTEL_TRACE_SAMPLE_RATIO must be between 0 and 1")

    provider = TracerProvider(
        resource=Resource.create(
            {
                SERVICE_NAME: "charging-app",
                SERVICE_VERSION: "0.1.0",
                "deployment.environment.name": "local",
            }
        ),
        sampler=DiagnosticSampler(ratio),
    )
    exporter = OTLPSpanExporter(
        endpoint=os.getenv(
            "OTEL_EXPORTER_OTLP_TRACES_ENDPOINT",
            "http://alloy:4318/v1/traces",
        ),
        timeout=5,
    )
    provider.add_span_processor(
        BatchSpanProcessor(
            exporter,
            max_queue_size=2048,
            max_export_batch_size=512,
            schedule_delay_millis=1000,
            export_timeout_millis=5000,
        )
    )
    trace.set_tracer_provider(provider)
    FastAPIInstrumentor.instrument_app(
        app,
        tracer_provider=provider,
        excluded_urls=".*/healthz,.*/readyz,.*/metrics",
    )
    return provider


def log_event(
    logger: logging.Logger,
    level: int,
    event: str,
    message: str,
    **fields: object,
) -> None:
    safe_fields = {key: value for key, value in fields.items() if key in SAFE_FIELDS}
    span_context = trace.get_current_span().get_span_context()
    if span_context.is_valid:
        safe_fields.setdefault("trace_id", format(span_context.trace_id, "032x"))
        safe_fields.setdefault("span_id", format(span_context.span_id, "016x"))
    logger.log(level, message, extra={"event": event, **safe_fields})
