-- The table every migration is recorded in, including this one.
--
-- Applied automatically when Services start; see docs/readme.migrations.
-- Core migrations only ever create or extend this table.  They never drop
-- anything: the record of what a network has applied is not something an
-- upgrade of Services is allowed to throw away.

CREATE TABLE IF NOT EXISTS migrations (
  id                    bigint GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  version               integer     NOT NULL,
  module_name           text        NOT NULL,
  module_migration_name text        NOT NULL,
  exec_duration         text        NOT NULL,
  exec_by               text        NOT NULL,
  created_at            timestamptz NOT NULL DEFAULT now()
);

-- One row per version per module.  Not decoration: it is what stops two
-- operators from applying the same migration at the same
-- moment and each believing they were first.
CREATE UNIQUE INDEX IF NOT EXISTS migrations_module_version_key
  ON migrations (module_name, version);
