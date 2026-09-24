-- Which copy of Services owns this schema.
--
-- A copy claims the schema when it starts: it holds an advisory lock for as
-- long as it runs and writes a token of its own here, and every write it
-- makes checks that the token is still its own.  Two copies of Services
-- sharing a schema would otherwise undo each other's writes.  See
-- src/postgres/pg_sync.c.

CREATE TABLE IF NOT EXISTS instance (
  id         integer     PRIMARY KEY DEFAULT 1 CHECK (id = 1),
  token      text        NOT NULL,
  host       text,
  pid        integer,
  started_at timestamptz NOT NULL DEFAULT now()
);
