#!/bin/sh
set -eu

# Subsequent numbered SQL files go in db/init/migrations/.
# Run again with: docker compose exec postgres sh /docker-entrypoint-initdb.d/003_migrate.sh
for migration in /docker-entrypoint-initdb.d/migrations/*.sql; do
    [ -f "$migration" ] || continue
    version=${migration##*/}
    version=${version%.sql}

    case "$version" in
        [0-9][0-9][0-9]_*) ;;
        *) echo "Invalid migration filename: $version" >&2; exit 1 ;;
    esac
    case "$version" in
        *[!a-z0-9_]*) echo "Invalid migration filename: $version" >&2; exit 1 ;;
    esac

    applied=$(psql -v ON_ERROR_STOP=1 -At \
        --username "$POSTGRES_USER" --dbname "$POSTGRES_DB" \
        -c "SELECT EXISTS (SELECT 1 FROM schema_migrations WHERE version = '$version')")
    [ "$applied" = f ] || continue

    psql -v ON_ERROR_STOP=1 -1 \
        --username "$POSTGRES_USER" --dbname "$POSTGRES_DB" \
        --file "$migration" \
        -c "INSERT INTO schema_migrations (version) VALUES ('$version')"
done
