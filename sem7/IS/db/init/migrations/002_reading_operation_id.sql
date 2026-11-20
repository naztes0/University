ALTER TABLE readings
    ADD COLUMN IF NOT EXISTS operation_id uuid;

UPDATE readings
SET operation_id = gen_random_uuid()
WHERE operation_id IS NULL;

ALTER TABLE readings
    ALTER COLUMN operation_id SET NOT NULL;

CREATE UNIQUE INDEX IF NOT EXISTS readings_operation_id_uidx
    ON readings (operation_id);
