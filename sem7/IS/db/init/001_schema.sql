CREATE TABLE stations (
    id smallint PRIMARY KEY CHECK (id BETWEEN 1 AND 200),
    code text NOT NULL UNIQUE,
    status text NOT NULL DEFAULT 'idle'
        CHECK (status IN ('idle', 'charging', 'offline', 'error')),
    updated_at timestamptz NOT NULL DEFAULT now()
);

CREATE TABLE load_runs (
    id uuid PRIMARY KEY,
    started_at timestamptz NOT NULL DEFAULT now(),
    finished_at timestamptz,
    station_count smallint NOT NULL CHECK (station_count BETWEEN 1 AND 200),
    interval_ms integer NOT NULL CHECK (interval_ms BETWEEN 1000 AND 10000),
    status text NOT NULL DEFAULT 'running'
        CHECK (status IN ('running', 'completed', 'stopped', 'failed', 'interrupted')),
    CHECK (finished_at IS NULL OR finished_at >= started_at)
);

CREATE TABLE readings (
    id bigint GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    station_id smallint NOT NULL REFERENCES stations(id),
    run_id uuid NOT NULL REFERENCES load_runs(id),
    measured_at timestamptz NOT NULL,
    power_kw double precision NOT NULL CHECK (power_kw BETWEEN 0 AND 350),
    voltage_v double precision NOT NULL CHECK (voltage_v BETWEEN 0 AND 1000),
    current_a double precision NOT NULL CHECK (current_a BETWEEN 0 AND 500),
    energy_delta_kwh double precision NOT NULL CHECK (energy_delta_kwh >= 0),
    temperature_c double precision NOT NULL CHECK (temperature_c BETWEEN -40 AND 100),
    status text NOT NULL CHECK (status IN ('idle', 'charging', 'offline', 'error')),
    CHECK (status = 'charging' OR power_kw = 0)
);

CREATE INDEX readings_station_time_idx ON readings (station_id, measured_at DESC);
CREATE INDEX readings_run_id_idx ON readings (run_id);

CREATE TABLE schema_migrations (
    version text PRIMARY KEY,
    applied_at timestamptz NOT NULL DEFAULT now()
);

INSERT INTO schema_migrations (version) VALUES ('001_initial');

INSERT INTO stations (id, code)
SELECT number, 'ST-' || lpad(number::text, 3, '0')
FROM generate_series(1, 200) AS number;
