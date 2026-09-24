/* SQLSTATE in, DbError out.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (modules/workers/postgres/pg_error.c).
 *
 * Two rules shape this file, and both come from db.h.
 *
 * The code a consumer sees is one of the DbError values and never an
 * SQLSTATE, an errno or anything else with a PostgreSQL flavour.  The
 * translation is by SQLSTATE class -- the first two characters -- with
 * the handful of individual codes that deserve their own answer listed
 * first.  A class nobody has thought about yet lands on DB_ERR_INTERNAL
 * rather than being mistaken for something more specific.
 *
 * The message a consumer sees is this file's own wording, not the
 * database's.  A caller is entitled to put it in a notice to a user, and a
 * PostgreSQL error string names tables, columns, constraints and often a
 * fragment of the statement.  The database's own account is not thrown away
 * -- pg_error_log() writes it to the server log, where the operator can
 * read it and nobody else can.
 */

#include "postgres.h"

#include <string.h>

enum DbError pg_error_from_sqlstate(const char* sqlstate)
{
    if (!sqlstate || strlen(sqlstate) < 2)
        return DB_ERR_INTERNAL;

    /* The specific codes worth separating from their class. */
    if (!strcmp(sqlstate, "23505"))
        return DB_ERR_UNIQUE;
    if (!strcmp(sqlstate, "23503"))
        return DB_ERR_FOREIGN_KEY;
    if (!strcmp(sqlstate, "23502"))
        return DB_ERR_NOT_NULL;
    if (!strcmp(sqlstate, "40001") || !strcmp(sqlstate, "40P01"))
        return DB_ERR_RETRY; /* serialization failure, deadlock */
    if (!strcmp(sqlstate, "57014"))
        return DB_ERR_TIMEOUT; /* query cancelled, ours or theirs */
    if (!strcmp(sqlstate, "25006") || !strcmp(sqlstate, "25V01"))
        return DB_ERR_READONLY;
    if (!strcmp(sqlstate, "53300") || !strcmp(sqlstate, "53200"))
        return DB_ERR_RESOURCE; /* too many clients, out of memory */

    /* Otherwise the class is enough.  These are the classes from the
     * PostgreSQL error-code table that mean something different to a caller.
     */
    switch ((sqlstate[0] << 8) | sqlstate[1]) {
        case ('0' << 8) | '8': /* connection exception */
            return DB_ERR_CONNECT;
        case ('2' << 8) | '2': /* data exception */
            return DB_ERR_DATA;
        case ('2' << 8) | '3': /* integrity constraint violation */
            return DB_ERR_CONSTRAINT;
        case ('2' << 8) | '5': /* invalid transaction state */
            return DB_ERR_SYNTAX;
        case ('4' << 8) | '0': /* transaction rollback */
            return DB_ERR_RETRY;
        case ('4' << 8) | '2': /* syntax error or access rule violation */
            /* 42501 is "insufficient privilege"; the rest of the class is the
             * statement being wrong about what exists.
             */
            if (!strcmp(sqlstate, "42501"))
                return DB_ERR_PERMISSION;
            if (!strcmp(sqlstate, "42P01")     /* undefined_table */
                || !strcmp(sqlstate, "42P02")  /* undefined_parameter */
                || !strcmp(sqlstate, "42703")  /* undefined_column */
                || !strcmp(sqlstate, "42883")  /* undefined_function */
                || !strcmp(sqlstate, "42704")) /* undefined_object */
                return DB_ERR_UNDEFINED;
            return DB_ERR_SYNTAX;
        case ('2' << 8) | '8': /* invalid authorization specification */
            return DB_ERR_PERMISSION;
        case ('5' << 8) | '3': /* insufficient resources */
        case ('5' << 8) | '4': /* program limit exceeded */
            return DB_ERR_RESOURCE;
        case ('5' << 8) | '7': /* operator intervention */
        case ('5' << 8) | '8': /* system error */
            return DB_ERR_CONNECT;
        case ('0' << 8) | 'A': /* feature not supported */
            return DB_ERR_SYNTAX;
        default:
            return DB_ERR_INTERNAL;
    }
}

const char* pg_error_message(enum DbError code)
{
    switch (code) {
        case DB_OK:
            return "no error";
        case DB_ERR_UNAVAILABLE:
            return "the database driver is not running";
        case DB_ERR_CONFIG:
            return "the database block is missing or unusable";
        case DB_ERR_CONNECT:
            return "the database is unreachable";
        case DB_ERR_TIMEOUT:
            return "the query took too long and was cancelled";
        case DB_ERR_BUSY:
            return "every database connection is busy";
        case DB_ERR_PARAM:
            return "the query or one of its parameters is malformed";
        case DB_ERR_SYNTAX:
            return "the database rejected the statement";
        case DB_ERR_UNDEFINED:
            return "the statement names something that does not exist";
        case DB_ERR_PERMISSION:
            return "the database refused the operation";
        case DB_ERR_UNIQUE:
            return "a record with that value already exists";
        case DB_ERR_FOREIGN_KEY:
            return "a referenced record does not exist";
        case DB_ERR_NOT_NULL:
            return "a required value was missing";
        case DB_ERR_CONSTRAINT:
            return "the database refused the value";
        case DB_ERR_DATA:
            return "a value was not acceptable to the database";
        case DB_ERR_READONLY:
            return "this connection cannot write";
        case DB_ERR_RETRY:
            return "the transaction was rolled back and may be retried";
        case DB_ERR_RESOURCE:
            return "the database is out of resources";
        case DB_ERR_INTERNAL:
        default:
            return "the database driver failed";
    }
}

void pg_error_log(const char* where, const char* detail)
{
    char text[512];
    size_t len;

    if (!detail || !*detail)
        return;

    /* libpq terminates its messages with a newline, and a multi-line one
     * would turn a log line into several.  Copy, trim, flatten.
     */
    strncpy(text, detail, sizeof(text) - 1);
    text[sizeof(text) - 1] = '\0';

    for (len = 0; text[len]; len++)
        if (text[len] == '\n' || text[len] == '\r')
            text[len] = ' ';

    while (len > 0 && text[len - 1] == ' ')
        text[--len] = '\0';

    if (len)
        worker_log("database: %s: %s", where, text);
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
