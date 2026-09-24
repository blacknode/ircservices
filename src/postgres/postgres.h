/* Private interfaces of the PostgreSQL driver.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (modules/workers/postgres/postgres.h), where the driver
 * is a loadable module; here it is part of the core.
 *
 * Nothing here is visible to a consumer of the database API: a module that
 * wants to run a query includes db.h and calls db_query(), and never learns
 * that the answer came from libpq.  This header is for the driver's own
 * files, and it does not include services.h: most of what is declared here
 * runs in a worker thread.
 *
 * WHICH THREAD IS WHICH
 *
 *   - Main thread: the pools' bookkeeping, handing answers back to the
 *     caller, and the table store's load path (pg_store.c).
 *   - Connection thread: one dedicated worker per pooled connection.  It
 *     owns exactly one PGconn and obeys the rule in worker.h without
 *     exception: it allocates with worker_alloc(), logs with worker_log(),
 *     and hands its results to the main thread with worker_post().
 *   - Pool thread: migrations and table saves, which take a connection of
 *     their own for as long as they take (pg_migrate.c, pg_save.c).
 *
 * The main thread and the connection threads share only the pool: its
 * queue, its counters and its stop flag, all under PgPool.pgp_lock.
 * Everything else that crosses the boundary is copied.
 */

#ifndef POSTGRES_H
#define POSTGRES_H

#include "db.h"
#include "worker.h"

#include <jansson.h>
#include <libpq-fe.h>
#include <pthread.h>
#include <time.h>

/* Queued queries allowed per connection before the pool says it is busy.
 * The queue absorbs a burst; a query behind sixteen others has already
 * missed its timeout. */
#define PG_QUEUE_PER_CONN 16

/* Prepared statements one connection caches before it is recycled (closed
 * and reopened, which discards them all at once -- see pg_conn.c). */
#define PG_STMT_CACHE   128
#define PG_STMT_BUCKETS 64

/* How long a connection thread sleeps between checks for a stop request. */
#define PG_POLL_INTERVAL_MS 100

/* Grace period for a cancelled query to acknowledge before the connection
 * is thrown away and remade instead. */
#define PG_CANCEL_GRACE_MS 250

/* Seconds to wait before retrying a connection that failed. */
#define PG_RECONNECT_DELAY 2

/*************************************************************************/

/* pg_error.c -- what the database says, translated into enum DbError. */

/* Translate an SQLSTATE; DB_ERR_INTERNAL when there is nothing to go on. */
extern enum DbError pg_error_from_sqlstate(const char* sqlstate);

/* The message a consumer is allowed to see for `code': the driver's own
 * words, never libpq's. */
extern const char* pg_error_message(enum DbError code);

/* Record the database's own account of a failure in the log.  Any
 * thread. */
extern void pg_error_log(const char* where, const char* detail);

/*************************************************************************/

/* pg_types.c -- enum DbType on one side, PostgreSQL OIDs on the other. */

extern Oid pg_type_oid(enum DbType type);
/* Width of a binary value of `type', or -1 when it has no fixed width. */
extern int pg_type_binary_length(enum DbType type);

/*************************************************************************/

/* pg_json.c -- a PGresult becomes a json_t array of row objects. */

/* Convert the rows of `res' (text format).  Returns a new reference, or
 * NULL if there was no memory.  Any thread. */
extern json_t* pg_json_rows(const PGresult* res);

/* Readers for db_rows() and friends.  Main thread. */
extern unsigned int pg_json_count(json_t* data);
extern const char* pg_json_str(json_t* data, unsigned int row,
                               const char* column);
extern long long pg_json_int(json_t* data, unsigned int row,
                             const char* column);

/*************************************************************************/

/* pg_conn.c -- one pooled connection and the thread that owns it, and the
 * waiting primitives everything else is built on. */

struct PgPool;
struct PgConn;
struct PgRequest;

/* A deadline, on the monotonic clock. */
struct PgDeadline {
    struct timespec pgd_at;
};

/* Milliseconds on the monotonic clock; only good for differences. */
extern long pg_monotonic_ms(void);

/* Arm a deadline `ms' milliseconds from now.  Callers pass a value already
 * clamped to its own ceiling (DB_TIMEOUT_MAX_MS for a query, the migration
 * or save maximum for those); this clamps to the largest of them. */
extern void pg_deadline_set(struct PgDeadline* deadline, int ms);

/* Milliseconds left before `deadline', or zero once it has passed. */
extern int pg_deadline_left(const struct PgDeadline* deadline);

/* Wait for `fd' (readable, or writable if `forwrite'), for a stop request
 * on `stopfd' (-1 for none), or for the deadline.  The one place this
 * driver ever waits.  Returns 1 when `fd' is ready, 0 when the deadline
 * passed, -1 when the thread was asked to stop or the wait failed. */
extern int pg_socket_wait(int fd, int forwrite, int stopfd,
                          const struct PgDeadline* deadline);

/* Flush what was sent and wait for its result, draining the connection
 * completely.  `*out' receives the first result (the caller clears it).
 * Returns 0 when a result arrived, -1 on the deadline, -2 when the
 * connection is no longer usable. */
extern int pg_collect(PGconn* pg, int stopfd,
                      const struct PgDeadline* deadline, PGresult** out);

/* Open a non-blocking connection to `dsn' within `deadline', without a
 * stop descriptor.  Returns the connection, or NULL with `*code' set.  Used
 * by the migrations, the table saves and the store. */
extern PGconn* pg_open(const char* dsn, const struct PgDeadline* deadline,
                       enum DbError* code);

/* Send `sql' (no parameters) and wait for it.  With `simple' set it goes
 * through the simple protocol, which accepts several statements and DDL --
 * only ever for SQL built by Services itself from static names, never with
 * a value in it.  Otherwise it goes through the extended protocol, as one
 * statement.  `*res_out' (if not NULL) receives the result on success.
 * Returns nonzero on success; on failure `*code' says why and the
 * database's message has been logged under `what'. */
extern int pg_run(PGconn* pg, const struct PgDeadline* deadline,
                  const char* sql, int simple, const char* what,
                  PGresult** res_out, enum DbError* code);

/* Run `sql' with `nparams' text parameters through the extended protocol
 * (unnamed statement), and wait for it.  Same conventions as pg_run(). */
extern int pg_run_params(PGconn* pg, const struct PgDeadline* deadline,
                         const char* sql, int nparams,
                         const char* const* values, const char* what,
                         PGresult** res_out, enum DbError* code);

/* Set up a session the way every connection of Services is set up: UTF-8,
 * no chatter below a warning, `timeout_ms' as the server-side statement
 * timeout, and `search_path' (may be NULL) as the search path.  Failures
 * are logged and survived: the client-side deadline still holds. */
extern void pg_session_setup(PGconn* pg, const struct PgDeadline* deadline,
                             int timeout_ms, const char* search_path);

/* Create a connection for `pool'.  Main thread.  The socket is not opened
 * here; the connection thread does that the first time it has work. */
extern struct PgConn* pg_conn_new(struct PgPool* pool, unsigned int index);

/* Release a connection.  Main thread, after its worker has stopped. */
extern void pg_conn_free(struct PgConn* conn);

/* Name the connection's worker thread carries. */
extern const char* pg_conn_name(const struct PgConn* conn);

/* The body of a connection thread: takes requests off the pool's queue
 * until asked to stop, and posts an answer for every one of them. */
extern void pg_conn_main(struct Worker* worker, void* arg);

/*************************************************************************/

/* pg_migrate.c -- schema migrations, off the pool and in a transaction. */

/* Start one migration.  Main thread.  Returns DB_OK when it was queued. */
extern enum DbError pg_migrate_submit(unsigned long id,
                                      const struct DbMigration* migration);

/* Run one migration on `pg' (BEGIN, the script, the record, COMMIT, or a
 * ROLLBACK), within `deadline'.  Any thread that owns `pg'.  On success
 * `*rows' receives the recorded row. */
extern enum DbError pg_migrate_exec(PGconn* pg,
                                    const struct PgDeadline* deadline,
                                    const struct DbMigration* migration,
                                    json_t** rows);

/*************************************************************************/

/* pg_pool.c -- the queue, the connections, and the counters. */

/* One parameter, copied out of the caller's DbParam.  Worker memory. */
struct PgBound {
    Oid pgb_type;    /* OID to bind as, or 0 to let the server infer */
    char* pgb_value; /* The bytes, or NULL for SQL NULL */
    int pgb_length;  /* Length in bytes; only read in binary format */
    int pgb_format;  /* 0 text, 1 binary */
};

/* One query on its way to a connection.  Worker memory throughout:
 * everything the connection thread needs is in here, copied. */
struct PgRequest {
    struct PgRequest* pgr_next; /* Next in the pool's queue */
    unsigned long pgr_id;       /* Handle for db_complete() */
    int pgr_timeout_ms;         /* Deadline for the whole round trip */
    char* pgr_sql;              /* The statement */
    unsigned int pgr_nparams;   /* Number of parameters */
    struct PgBound* pgr_params; /* pgr_nparams parameters, or NULL */
};

/* Release a request and everything in it.  Either thread. */
extern void pg_request_free(struct PgRequest* req);

/* Take the next request off `pool', waiting a little for one.  Connection
 * thread.  Returns NULL when the thread should look at worker_stopping()
 * again. */
extern struct PgRequest* pg_pool_take(struct PgPool* pool,
                                      struct Worker* worker);

/* Hand an answer back to the main thread.  Connection thread.  A JSON
 * value handed over here belongs to the main thread afterwards. */
extern void pg_pool_answer(struct Worker* worker, unsigned long id,
                           json_t* data, unsigned int rows, enum DbError code);

/* The pool's own copies of its configuration, readable without a lock by
 * its connection threads. */
extern const char* pg_pool_dsn(const struct PgPool* pool);
extern const char* pg_pool_label(const struct PgPool* pool);
extern int pg_pool_timeout(const struct PgPool* pool);
extern const char* pg_pool_search_path(const struct PgPool* pool);

/* Count outcomes against a pool.  Connection thread. */
extern void pg_pool_count(struct PgPool* pool, enum DbError code);
extern void pg_pool_count_connect(struct PgPool* pool, int ok);

/* The pools themselves.  Main thread only. */

/* Tear the pools down if the configuration changed since they were built;
 * the next query builds them again.  Cheap and idempotent. */
extern void pg_pools_sync(void);

/* The pool for `role', started if it was not running; NULL with `*err'
 * set on failure. */
extern struct PgPool* pg_pools_get(enum DbRole role, enum DbError* err);

/* Queue `query' on `pool' (copying it).  Returns DB_OK when queued. */
extern enum DbError pg_pools_submit(struct PgPool* pool, unsigned long id,
                                    const struct DbQuery* query);

/* Stop every pool, failing whatever was queued with DB_ERR_UNAVAILABLE.
 * Waits for the connection threads, at most one query timeout. */
extern void pg_pools_stop(void);

/* Write a line per pool to the log. */
extern void pg_pools_report(void);

/*************************************************************************/

/* Helpers shared by the driver and the store. */

/* `name' as a quoted SQL identifier ("na""me"), into `buf'.  Only ever
 * used on names Services itself chose (schema, table and column names),
 * never on a value.  Returns `buf', or NULL if it did not fit. */
extern char* pg_quote_ident(const char* name, char* buf, size_t size);

/* The search path every connection is given: the configured schema first,
 * then public.  Into `buf'; returns `buf'. */
extern char* pg_search_path(const char* schema, char* buf, size_t size);

/*************************************************************************/

/* pg_sync.c -- the main thread's own connection.  Main thread only. */

/* Connect (creating the schema if needed).  Nonzero on success. */
extern int pg_sync_open(void);
/* Claim the schema for this copy of Services (see pg_sync.c).  Needs the
 * instance table, which the core's migrations create. */
extern int pg_sync_claim(void);
/* This copy's claim token, or NULL if it has none (read-only, or lost). */
extern const char* pg_sync_token(void);
/* The connection, reconnected for the current configuration if needed;
 * NULL if the database cannot be reached. */
extern PGconn* pg_sync_conn(void);
/* Close it, releasing the claim. */
extern void pg_sync_close(void);
/* One statement, with the sync timeout; `*rows' as db.h describes. */
extern enum DbError pg_sync_query(const struct DbQuery* query, json_t** rows,
                                  unsigned int* nrows);
/* One migration, with the migration timeout. */
extern enum DbError pg_sync_migrate(const struct DbMigration* migration,
                                    json_t** rows);

/*************************************************************************/

/* postgres.c -- the driver the core registers. */

/* Register the PostgreSQL driver with db.c.  Main thread, at start-up. */
extern int pg_driver_init(void);

/*************************************************************************/

#endif /* POSTGRES_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
