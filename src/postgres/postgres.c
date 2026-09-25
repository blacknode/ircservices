/* The PostgreSQL driver: what db.c sees of it.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (modules/workers/postgres/postgres.c).  In ircu2 this
 * is a module that registers itself as the database driver from its
 * mi_init; in Services the driver is part of the core, and init() registers
 * it before any module is loaded, so that db_query() works from every
 * module's `init' on.
 *
 * The files beside this one are the parts: pg_pool.c is the queue and the
 * copying, pg_conn.c is one connection and its deadline, pg_json.c turns a
 * result into JSON, pg_error.c turns a failure into a DbError, pg_types.c
 * is the type map, pg_migrate.c runs migrations, and pg_store.c/pg_save.c
 * keep the Services tables (see databases.h).  postgres.h says which
 * thread each of them runs in.
 */

#include "postgres.h"

/* Start one query.  Main thread. */
static enum DbError pg_driver_submit(unsigned long id,
                                     const struct DbQuery* query,
                                     enum DbRole role)
{
    enum DbError err = DB_ERR_INTERNAL;
    struct PgPool* pool;

    if (!(pool = pg_pools_get(role, &err)))
        return err;
    return pg_pools_submit(pool, id, query);
}

/* Release a result the driver produced.  Main thread. */
static void pg_driver_release(struct json_t* data)
{
    if (data)
        json_decref((json_t*)data);
}

/* Notice a changed database block.  Cheap when nothing moved; when
 * something did, the pools are stopped so the next query builds them
 * against the new configuration. */
static void pg_driver_reconfigure(void)
{
    pg_pools_sync();
}

/* Readers, adapted to db.h's struct json_t. */
static unsigned int pg_driver_rows(struct json_t* data)
{
    return pg_json_count((json_t*)data);
}

static const char* pg_driver_row_str(struct json_t* data, unsigned int row,
                                     const char* column)
{
    return pg_json_str((json_t*)data, row, column);
}

static long long pg_driver_row_int(struct json_t* data, unsigned int row,
                                   const char* column)
{
    return pg_json_int((json_t*)data, row, column);
}

static enum DbError pg_driver_query_sync(const struct DbQuery* query,
                                         struct json_t** data,
                                         unsigned int* rows)
{
    json_t* result = NULL;
    enum DbError code = pg_sync_query(query, &result, rows);

    *data = (struct json_t*)result;
    return code;
}

static enum DbError pg_driver_migrate_sync(const struct DbMigration* migration,
                                           struct json_t** data)
{
    json_t* result = NULL;
    enum DbError code = pg_sync_migrate(migration, &result);

    *data = (struct json_t*)result;
    return code;
}

static const struct DbDriver pg_driver = {
    .dbdrv_name = "postgres",
    .dbdrv_submit = pg_driver_submit,
    .dbdrv_release = pg_driver_release,
    .dbdrv_migrate = pg_migrate_submit,
    .dbdrv_rows = pg_driver_rows,
    .dbdrv_row_str = pg_driver_row_str,
    .dbdrv_row_int = pg_driver_row_int,
    .dbdrv_query_sync = pg_driver_query_sync,
    .dbdrv_migrate_sync = pg_driver_migrate_sync,
    .dbdrv_reconfigure = pg_driver_reconfigure,
    .dbdrv_report = pg_pools_report,
    .dbdrv_shutdown = pg_pools_stop,
};

int pg_driver_init(void)
{
    if (!db_register_driver(&pg_driver))
        return 0;
    worker_log("database: PostgreSQL driver ready (libpq %d, jansson %s)",
               PQlibVersion(), JANSSON_VERSION);
    return 1;
}

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
