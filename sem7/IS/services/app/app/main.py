from contextlib import asynccontextmanager
from pathlib import Path

from fastapi import FastAPI, HTTPException, Response, status
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles

from .database import Database
from .generator import AlreadyRunningError, LoadGenerator
from .models import LoadConfig

database = Database()
generator = LoadGenerator(database)


@asynccontextmanager
async def lifespan(_: FastAPI):
    await database.open()
    try:
        try:
            await database.mark_orphaned_runs()
        except Exception:
            # Readiness reports the database failure; HTTP stays available.
            pass
        yield
    finally:
        await generator.stop(final_status="interrupted")
        await database.close()


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
