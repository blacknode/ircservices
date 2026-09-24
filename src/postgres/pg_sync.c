/* The main thread's own connection: synchronous work, and the claim.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Most database work goes through the pools (pg_pool.c) and never makes
 * the main loop wait.  A few things cannot: the core's migrations and the
 * migrations of the modules Services need, applied at start-up before
 * anything else can run; the claim on the schema; and the reads a command
 * needs an answer to before it can reply (db_query_sync()), which the
 * cache in front of the database keeps rare.  They all go through this one
 * connection, owned by the main thread, with short deadlines: the sync
 * timeout (`sync_timeout', at most DB_TIMEOUT_MAX_MS) for queries, the
 * migration timeout for migrations.
 *
 * THE CLAIM
 *
 * Two copies of Services using one schema would undo each other's writes,
 * so a copy claims the schema when it starts: it takes a session-level
 * advisory lock on this connection, for as long as it runs (a second copy
 * started meanwhile refuses to start), and writes a token of its own to the
 * `instance' table, created by the core's migrations.  If this connection
 * drops and has to be reopened, the claim is only taken again if the token
 * is still this copy's: otherwise another copy has taken over, and this one
 * must not write.
 */

#include "postgres.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Every name this file puts in SQL is quoted with pg_quote_ident() and
 * comes from the configuration, never from a user. */
#define SYNC_QNAMELEN 130

static PGconn* sync_pg;               /* The connection, or NULL */
static unsigned int sync_generation;  /* Config it was opened with */
static char sync_qschema[SYNC_QNAMELEN]; /* Quoted schema name */
static char sync_token[128];          /* This copy's claim, "" if none */
static int sync_claimed;              /* The claim has been taken */
static int sync_lost;                 /* Another copy took the schema */

/*************************************************************************/

/* FNV-1a, 64 bits: the advisory lock key for the schema. */
static unsigned long long sync_hash(const char* s)
{
    unsigned long long hash = 14695981039346656037ULL;

    while (*s) {
        hash ^= (unsigned char)*s++;
        hash *= 1099511628211ULL;
    }
    return hash;
}

static void sync_deadline(struct PgDeadline* deadline, int migration)
{
    pg_deadline_set(deadline, migration ? db_conf_migration_timeout()
                                        : db_conf_sync_timeout());
}

static void sync_close(void)
{
    if (sync_pg) {
        PQfinish(sync_pg); /* releases the advisory lock */
        sync_pg = NULL;
    }
}

/* Take the claim: the advisory lock, and the token (unless `check' says
 * the token must still be ours, after a reconnection). */
static int sync_claim(const struct PgDeadline* deadline, int check)
{
    const char* values[3];
    char key[32], sql[512], host[256], pid[16], seed[SYNC_QNAMELEN + 16];
    enum DbError code;
    PGresult* res;
    int ok;

    snprintf(seed, sizeof(seed), "ircservices:%s", db_conf_schema());
    snprintf(key, sizeof(key), "%lld", (long long)sync_hash(seed));
    values[0] = key;
    if (!pg_run_params(sync_pg, deadline,
                       "select pg_try_advisory_lock($1::bigint)", 1, values,
                       "locking the schema", &res, &code))
        return 0;
    ok = PQntuples(res) == 1 && *PQgetvalue(res, 0, 0) == 't';
    PQclear(res);
    if (!ok) {
        worker_log("database: schema %s is in use by another copy of"
                   " Services (see its `instance' table); refusing to use it",
                   db_conf_schema());
        return 0;
    }

    if (check) {
        snprintf(sql, sizeof(sql), "select token from %s.instance where id = 1",
                 sync_qschema);
        if (!pg_run(sync_pg, deadline, sql, 0, "checking the instance", &res,
                    &code))
            return 0;
        ok = PQntuples(res) == 0 ||
             strcmp(PQgetvalue(res, 0, 0), sync_token) == 0;
        PQclear(res);
        if (!ok) {
            worker_log("database: another copy of Services has claimed schema"
                       " %s; this one will not write to it",
                       db_conf_schema());
            sync_lost = 1;
            return 0;
        }
    }

    if (gethostname(host, sizeof(host)) < 0)
        snprintf(host, sizeof(host), "unknown");
    host[sizeof(host) - 1] = 0;
    if (!*sync_token)
        snprintf(sync_token, sizeof(sync_token), "%.64s:%d:%ld:%08x", host,
                 (int)getpid(), (long)time(NULL), (unsigned)rand());
    snprintf(pid, sizeof(pid), "%d", (int)getpid());
    snprintf(sql, sizeof(sql),
             "insert into %s.instance (id, token, host, pid)"
             " values (1, $1, $2, $3::integer) on conflict (id) do update"
             " set token = excluded.token, host = excluded.host,"
             " pid = excluded.pid, started_at = now()",
             sync_qschema);
    values[0] = sync_token;
    values[1] = host;
    values[2] = pid;
    return pg_run_params(sync_pg, deadline, sql, 3, values,
                         "claiming the schema", NULL, &code);
}

/* Make sure the connection is up, for the current configuration. */
static PGconn* sync_get(enum DbError* code)
{
    struct PgDeadline deadline;
    char search_path[SYNC_QNAMELEN + 16], sql[SYNC_QNAMELEN + 64];
    const char* dsn = db_conf_dsn(DB_ROLE_WRITE);
    int reopened;

    if (sync_pg && PQstatus(sync_pg) == CONNECTION_OK &&
        sync_generation == db_conf_generation())
        return sync_pg;
    if (!dsn) {
        *code = DB_ERR_CONFIG;
        return NULL;
    }
    /* Reopened for the same configuration (the connection dropped), as
     * opposed to opened for a new one (REHASH): only the former has to
     * make sure nobody else claimed the schema meanwhile. */
    reopened = sync_pg != NULL && sync_generation == db_conf_generation();
    sync_close();
    if (!pg_quote_ident(db_conf_schema(), sync_qschema,
                        sizeof(sync_qschema))) {
        worker_log("database: schema name too long: %s", db_conf_schema());
        *code = DB_ERR_CONFIG;
        return NULL;
    }

    sync_deadline(&deadline, 0);
    if (!(sync_pg = pg_open(dsn, &deadline, code))) {
        worker_log("database: cannot connect to the database: %s",
                   pg_error_message(*code));
        return NULL;
    }
    pg_session_setup(sync_pg, &deadline, db_conf_migration_timeout(),
                     pg_search_path(db_conf_schema(), search_path,
                                    sizeof(search_path)));
    snprintf(sql, sizeof(sql), "create schema if not exists %s", sync_qschema);
    if (!pg_run(sync_pg, &deadline, sql, 1, "creating the schema", NULL,
                code)) {
        sync_close();
        return NULL;
    }
    if (sync_claimed && !sync_claim(&deadline, reopened)) {
        sync_close();
        *code = DB_ERR_PERMISSION;
        return NULL;
    }
    sync_generation = db_conf_generation();
    return sync_pg;
}

/*************************************************************************/
/******************************* Interface *******************************/
/*************************************************************************/

int pg_sync_open(void)
{
    enum DbError code;

    return sync_get(&code) != NULL;
}

int pg_sync_claim(void)
{
    struct PgDeadline deadline;
    enum DbError code;

    if (!sync_get(&code))
        return 0;
    sync_deadline(&deadline, 0);
    if (!sync_claim(&deadline, 0))
        return 0;
    sync_claimed = 1;
    return 1;
}

const char* pg_sync_token(void)
{
    return sync_claimed && !sync_lost ? sync_token : NULL;
}

PGconn* pg_sync_conn(void)
{
    enum DbError code;

    return sync_get(&code);
}

void pg_sync_close(void)
{
    sync_close();
    sync_claimed = 0;
}

enum DbError pg_sync_query(const struct DbQuery* query, json_t** rows,
                           unsigned int* nrows)
{
    const char* values[DB_MAX_PARAMS];
    Oid types[DB_MAX_PARAMS];
    int lengths[DB_MAX_PARAMS], formats[DB_MAX_PARAMS];
    struct PgDeadline deadline;
    enum DbError code;
    PGresult* res;
    PGconn* pg;
    int n = 0, collected;

    *rows = NULL;
    *nrows = 0;
    if (!(pg = sync_get(&code)))
        return code;
    if (query->params) {
        for (; query->params[n]; n++) {
            const struct DbParam* p = query->params[n];
            if (n >= DB_MAX_PARAMS)
                return DB_ERR_PARAM;
            types[n] = pg_type_oid(p->type);
            values[n] = p->type == DB_TYPE_NULL ? NULL : p->value;
            formats[n] = p->format == DB_FORMAT_BINARY ? 1 : 0;
            if (formats[n]) {
                if ((lengths[n] = pg_type_binary_length(p->type)) < 0)
                    return DB_ERR_PARAM;
            }
            else {
                lengths[n] = 0;
            }
        }
    }

    sync_deadline(&deadline, 0);
    /* The unnamed statement: prepared and executed in one round trip, with
     * the values bound, never pasted. */
    if (!PQsendQueryParams(pg, query->sql, n, n ? types : NULL,
                           n ? values : NULL, n ? lengths : NULL,
                           n ? formats : NULL, 0)) {
        pg_error_log("running a statement", PQerrorMessage(pg));
        sync_close();
        return DB_ERR_CONNECT;
    }
    collected = pg_collect(pg, -1, &deadline, &res);
    if (collected == -1) {
        /* Past the deadline: the connection is in an unknown state, and a
         * cancel would be one more wait.  Start again with a fresh one. */
        sync_close();
        return DB_ERR_TIMEOUT;
    }
    if (collected < 0 || !res) {
        sync_close();
        return DB_ERR_CONNECT;
    }
    switch (PQresultStatus(res)) {
    case PGRES_TUPLES_OK:
        *rows = pg_json_rows(res);
        *nrows = (unsigned int)PQntuples(res);
        code = *rows ? DB_OK : DB_ERR_RESOURCE;
        break;
    case PGRES_COMMAND_OK: {
        const char* affected = PQcmdTuples(res);
        *rows = json_array();
        *nrows = (affected && *affected) ? (unsigned int)atoi(affected) : 0;
        code = *rows ? DB_OK : DB_ERR_RESOURCE;
        break;
    }
    default:
        code = pg_error_from_sqlstate(
            PQresultErrorField(res, PG_DIAG_SQLSTATE));
        pg_error_log("running a statement", PQresultErrorMessage(res));
        break;
    }
    PQclear(res);
    return code;
}

enum DbError pg_sync_migrate(const struct DbMigration* migration,
                             json_t** rows)
{
    struct PgDeadline deadline;
    enum DbError code;
    PGconn* pg;

    *rows = NULL;
    if (!(pg = sync_get(&code)))
        return code;
    sync_deadline(&deadline, 1);
    code = pg_migrate_exec(pg, &deadline, migration, rows);
    if (code == DB_ERR_TIMEOUT || code == DB_ERR_CONNECT)
        sync_close();
    return code;
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
