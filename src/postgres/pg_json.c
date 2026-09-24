/* Turning a PGresult into the JSON array db.h promises.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (modules/workers/postgres/pg_json.c).
 *
 * Results are fetched in text format on purpose.  Binary format would mean
 * decoding every type by hand, getting the endianness and the epoch right
 * for each one, and being wrong in a new way for every type PostgreSQL
 * gains; text format hands over exactly what the server would have printed,
 * and this file decides what that is worth as JSON.
 *
 * The mapping is deliberately conservative.  Numbers, booleans and JSON
 * documents become their JSON equivalents, NULL becomes null, and
 * everything else -- timestamps, UUIDs, arrays, the \x form of a bytea,
 * a type nobody here has heard of -- stays a string.  A string is never
 * wrong; guessing at a structure would be.
 *
 * Everything here runs in a connection thread, so the allocator is
 * jansson's (the system malloc, whatever MEMCHECKS says) and nothing touches
 * core state.
 */

#include "postgres.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* OIDs; see pg_types.c for why they are written out. */
#define PG_OID_BOOL    16
#define PG_OID_INT8    20
#define PG_OID_INT2    21
#define PG_OID_INT4    23
#define PG_OID_JSON    114
#define PG_OID_FLOAT4  700
#define PG_OID_FLOAT8  701
#define PG_OID_NUMERIC 1700
#define PG_OID_JSONB   3802

/* Convert one text-format field to JSON.
 *
 * @param[in] oid Type the column was declared as.
 * @param[in] text The server's rendering of the value.
 * @return A new reference, or NULL if there was no memory.
 */
static json_t* pg_json_value(Oid oid, const char* text)
{
    json_t* value;
    char* end;

    switch (oid) {
        case PG_OID_BOOL:
            /* The server prints exactly "t" or "f". */
            return json_boolean(text[0] == 't');

        case PG_OID_INT2:
        case PG_OID_INT4:
        case PG_OID_INT8: {
            long long number;

            errno = 0;
            number = strtoll(text, &end, 10);
            if (errno || end == text || *end)
                break; /* not what we were promised; keep the text */

            return json_integer((json_int_t)number);
        }

        case PG_OID_FLOAT4:
        case PG_OID_FLOAT8:
        case PG_OID_NUMERIC: {
            double number;

            /* "NaN" and "Infinity" are valid here and have no JSON spelling,
             * so they fall through to the string below rather than being
             * rounded into something that is not the value.
             */
            errno = 0;
            number = strtod(text, &end);
            if (errno || end == text || *end)
                break;

            return json_real(number);
        }

        case PG_OID_JSON:
        case PG_OID_JSONB:
            /* Embed the document rather than a string containing it: a column
             * of JSON is the one case where the server has already done the
             * work.
             */
            if ((value = json_loads(text, JSON_DECODE_ANY, 0)))
                return value;
            break; /* malformed: hand back the text as it came */

        default:
            break;
    }

    return json_string(text);
}

json_t* pg_json_rows(const PGresult* res)
{
    json_t* rows;
    int nrows;
    int ncols;
    int row;
    int col;

    if (!(rows = json_array()))
        return 0;

    nrows = PQntuples(res);
    ncols = PQnfields(res);

    for (row = 0; row < nrows; row++) {
        json_t* object = json_object();

        if (!object) {
            json_decref(rows);
            return 0;
        }

        for (col = 0; col < ncols; col++) {
            const char* name = PQfname(res, col);
            json_t* value;

            if (PQgetisnull(res, row, col))
                value = json_null();
            else
                value = pg_json_value(PQftype(res, col),
                                      PQgetvalue(res, row, col));

            if (!value ||
                json_object_set_new(object, name ? name : "", value)) {
                /* json_object_set_new() steals the reference even when it
                 * fails, so there is nothing left to release but the row
                 * itself.
                 */
                json_decref(object);
                json_decref(rows);
                return 0;
            }
        }

        if (json_array_append_new(rows, object)) {
            json_decref(rows);
            return 0;
        }
    }

    return rows;
}

/* ------------------------------------------------------------------------
 * Reading a result back, for a caller with no jansson.
 *
 * A module need not include jansson, so the
 * few things db.h offers for reading a row without it land here, where the
 * library already is.  Main thread: these are called from a callback, on
 * values the main thread owns.
 * ------------------------------------------------------------------------ */

/* Buffer behind pg_json_str(), which renders numbers and nested values. */
static char pg_json_scratch[1024];

/* The value at \a row / \a column, or NULL.
 * @param[in] data A JSON array of row objects, or NULL.
 * @param[in] row Row index, from zero.
 * @param[in] column Column name.
 */
static json_t* pg_json_cell(json_t* data, unsigned int row, const char* column)
{
    json_t* object;

    if (!data || !json_is_array(data) || !column)
        return 0;

    if (!(object = json_array_get(data, row)) || !json_is_object(object))
        return 0;

    return json_object_get(object, column);
}

unsigned int pg_json_count(json_t* data)
{
    if (!data || !json_is_array(data))
        return 0;

    return (unsigned int)json_array_size(data);
}

const char* pg_json_str(json_t* data, unsigned int row, const char* column)
{
    json_t* value = pg_json_cell(data, row, column);
    char* dumped;

    if (!value || json_is_null(value))
        return 0;

    /* A string is already text; everything else is rendered into the scratch
     * buffer, which is why the result is only good until the next call.
     */
    if (json_is_string(value))
        return json_string_value(value);

    if (json_is_integer(value)) {
        snprintf(pg_json_scratch, sizeof(pg_json_scratch), "%lld",
                 (long long)json_integer_value(value));
        return pg_json_scratch;
    }

    if (json_is_real(value)) {
        snprintf(pg_json_scratch, sizeof(pg_json_scratch), "%g",
                 json_real_value(value));
        return pg_json_scratch;
    }

    if (json_is_boolean(value))
        return json_is_true(value) ? "true" : "false";

    if ((dumped = json_dumps(value, JSON_COMPACT | JSON_ENCODE_ANY))) {
        strncpy(pg_json_scratch, dumped, sizeof(pg_json_scratch) - 1);
        pg_json_scratch[sizeof(pg_json_scratch) - 1] = '\0';
        free(dumped);
        return pg_json_scratch;
    }

    return 0;
}

long long pg_json_int(json_t* data, unsigned int row, const char* column)
{
    json_t* value = pg_json_cell(data, row, column);

    if (!value)
        return 0;

    if (json_is_integer(value))
        return (long long)json_integer_value(value);
    if (json_is_real(value))
        return (long long)json_real_value(value);
    if (json_is_boolean(value))
        return json_is_true(value) ? 1 : 0;

    /* A numeric column that came back as text -- an out-of-range value, say
     * -- is still worth reading as a number when it is one.
     */
    if (json_is_string(value))
        return strtoll(json_string_value(value), 0, 10);

    return 0;
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
