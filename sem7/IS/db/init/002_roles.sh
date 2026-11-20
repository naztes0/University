#!/bin/sh
set -eu

: "${APP_DB_PASSWORD:?Set APP_DB_PASSWORD}"
: "${MONITOR_DB_PASSWORD:?Set MONITOR_DB_PASSWORD}"

psql -v ON_ERROR_STOP=1 \
  --username "$POSTGRES_USER" \
  --dbname "$POSTGRES_DB" \
  --set=db_name="$POSTGRES_DB" \
  --set=app_password="$APP_DB_PASSWORD" \
  --set=monitor_password="$MONITOR_DB_PASSWORD" <<'SQL'
CREATE ROLE charging_app LOGIN PASSWORD :'app_password';
CREATE ROLE charging_monitor LOGIN PASSWORD :'monitor_password';

GRANT CONNECT ON DATABASE :"db_name" TO charging_app, charging_monitor;
GRANT USAGE ON SCHEMA public TO charging_app;
GRANT SELECT, INSERT, UPDATE ON stations, load_runs, readings TO charging_app;
GRANT USAGE, SELECT ON SEQUENCE readings_id_seq TO charging_app;

GRANT pg_monitor TO charging_monitor;
SQL
