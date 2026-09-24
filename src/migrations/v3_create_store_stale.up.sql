-- Redis keys that may hold an older copy of a record than PostgreSQL.
--
-- The store writes a record to PostgreSQL first and then to Redis.  If
-- Redis cannot be reached in between, it may keep the previous copy; the
-- key is recorded here, and deleted from Redis (so that the next read goes
-- to PostgreSQL) as soon as Redis answers again, and when Services start.
-- See src/store/store.c.

CREATE TABLE IF NOT EXISTS store_stale (
  redis_key  text        PRIMARY KEY,
  created_at timestamptz NOT NULL DEFAULT now()
);
