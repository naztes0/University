from pydantic import BaseModel, Field


class LoadConfig(BaseModel):
    station_count: int = Field(default=20, ge=1, le=200)
    interval_ms: int = Field(default=2_000, ge=1_000, le=10_000)
    duration_seconds: int = Field(default=60, ge=5, le=600)
    max_concurrency: int = Field(default=5, ge=1, le=10)

    @property
    def requested_readings_per_second(self) -> float:
        return self.station_count * 1_000 / self.interval_ms
