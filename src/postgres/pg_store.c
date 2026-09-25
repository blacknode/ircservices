/* The table store: the Services tables, kept in PostgreSQL.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * This replaces the database/standard module (one .sdb file per table).
 * Services keep their data in memory, as always; a module describes each of
 * its tables with a DBTable (see databases.h), and this file is what reads
 * such a table from PostgreSQL when the module registers it, and writes it
 * back on every save.
 *
 * THE LAYOUT
 *
 * Every table lives in the configured schema (`schema' in the database
 * block, "services" by default), under its DBTable name with anything that
 * is not a letter, a digit or an underscore turned into an underscore
 * ("chan-access" becomes chan_access).  Its columns are the fields, named
 * the same way ("memos.memomax" becomes memos_memomax), plus "_seq", the
 * position of the record in the table, which is the primary key and the
 * order the records are loaded back in:
 *
 *     DBTYPE_INT8, UINT8, INT16   smallint
 *     DBTYPE_UINT16, INT32        integer
 *     DBTYPE_UINT32               bigint
 *     DBTYPE_TIME                 bigint (seconds since the epoch)
 *     DBTYPE_STRING               text (NULL for a NULL pointer)
 *     DBTYPE_BUFFER               text (up to the first NUL)
 *     DBTYPE_PASSWORD             bytea <name>, plus text <name>_cipher
 *
 * Strings are stored as UTF-8.  A string that is not valid UTF-8 (legacy
 * data in ISO-8859-1, typically) is converted from ISO-8859-1 on the way
 * in, and the conversion is logged.
 *
 * A table the database does not have is created; a field the database
 * does not have a column for is added (ALTER TABLE ... ADD COLUMN); a
 * column whose type no longer matches its field is converted.  Columns the
 * code no longer knows about are left alone.  load_only fields are read if
 * their column exists and never written.
 *
 * WHO OWNS THE DATA
 *
 * Services do.  The tables are a snapshot of what Services hold in memory,
 * rewritten on every save; a row changed in the database by anything else
 * while Services are running is overwritten by the next save.  Read the
 * tables freely; write to them only with Services stopped.
 *
 * Two copies of Services writing to the same schema would overwrite each
 * other in turn.  To prevent it, a copy claims the schema when it starts:
 * it takes a session-level advisory lock (so that a second copy started
 * while the first is running refuses to start) and writes a token of its
 * own to the `instance' table.  Every save checks the token inside its
 * transaction, so a copy that has lost its claim -- its connection
 * dropped, and another copy started in the meantime -- can no longer
 * write.
 *
 * SAVING
 *
 * A save snapshots every table in the main thread, into the COPY text
 * format, and hands the snapshots to a worker thread (pg_save.c), which
 * writes the tables that changed since the last successful save in one
 * transaction.  Snapshotting costs what the old file format cost to format
 * the records; the writing, which is what used to block, no longer does.
 */

/* System, libpq and jansson headers before services.h, whose memory.h may
 * redefine malloc() and free() as macros. */
#include "pg_save.h"

#include "databases.h"
#include "encrypt.h"
#include "pg_store.h"
#include "migration.h"
#include "services.h"

#include <netdb.h>

/*************************************************************************/

/* Longest SQL name (PostgreSQL's NAMEDATALEN - 1) and a buffer for one,
 * quoted. */
#define STORE_NAMELEN  63
#define STORE_QNAMELEN (STORE_NAMELEN * 2 + 3)

/* What the store remembers about each table it has seen, by name. */
typedef struct StoreTable_ StoreTable;
struct StoreTable_ {
    StoreTable* next;
    char name[STORE_NAMELEN + 1]; /* SQL name */
    uint64 hash;                  /* Of the last snapshot written */
    int hashed;                   /* `hash' is valid */
    unsigned int ensured;         /* Config generation it was created in */
};
static StoreTable* store_tables;

/* One column of a table. */
typedef struct {
    const DBField* field;
    char name[STORE_NAMELEN + 1]; /* SQL name, unquoted */
    const char* type;             /* SQL type */
    int cipher;                   /* The _cipher half of a password */
} StoreColumn;

/* The control connection: loads, and the claim on the schema.  Main
 * thread only. */
static PGconn* store_pg;
static unsigned int store_generation;      /* Config it was opened with */
static char store_qschema[STORE_QNAMELEN]; /* Quoted schema name */

/*************************************************************************/
/******************************* Helpers *********************************/
/*************************************************************************/

/* `in' as an SQL name: lower case, anything that is not a letter, a digit
 * or an underscore turned into an underscore. */
static void store_mangle(const char* in, char* out, size_t size)
{
    size_t n = 0;

    for (; *in && n + 1 < size; in++) {
        unsigned char c = (unsigned char)*in;
        if (c >= 'A' && c <= 'Z')
            c = c - 'A' + 'a';
        else if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')))
            c = '_';
        out[n++] = (char)c;
    }
    out[n] = '\0';
}

/* The SQL type of a field. */
static const char* store_type(const DBField* field)
{
    switch (field->type) {
        case DBTYPE_INT8:
        case DBTYPE_UINT8:
        case DBTYPE_INT16:
            return "smallint";
        case DBTYPE_UINT16:
        case DBTYPE_INT32:
            return "integer";
        case DBTYPE_UINT32:
        case DBTYPE_TIME:
            return "bigint";
        case DBTYPE_STRING:
        case DBTYPE_BUFFER:
            return "text";
        case DBTYPE_PASSWORD:
            return "bytea";
    }
    return NULL;
}

/* The columns of `table', into a new array (freed by the caller);
 * load_only fields only if `with_load_only'.  Returns the number of
 * columns, or -1 (logged) if the table cannot be stored. */
static int store_columns(const DBTable* table, int with_load_only,
                         StoreColumn** cols_ret)
{
    StoreColumn* cols;
    int nfields, ncols = 0, i, j;

    for (nfields = 0; table->fields[nfields].name; nfields++)
        ;
    cols = scalloc(nfields * 2 + 1, sizeof(*cols));
    for (i = 0; i < nfields; i++) {
        const DBField* field = &table->fields[i];
        if (field->load_only && !with_load_only)
            continue;
        if (!store_type(field)) {
            log("database: BUG: table %s: field %s has an invalid type (%d)",
                table->name, field->name, field->type);
            free(cols);
            return -1;
        }
        cols[ncols].field = field;
        store_mangle(field->name, cols[ncols].name, STORE_NAMELEN - 7);
        cols[ncols].type = store_type(field);
        ncols++;
        if (field->type == DBTYPE_PASSWORD) {
            cols[ncols].field = field;
            char base[STORE_NAMELEN + 1];
            strbcpy(base, cols[ncols - 1].name);
            snprintf(cols[ncols].name, sizeof(cols[ncols].name), "%.*s_cipher",
                     STORE_NAMELEN - 7, base);
            cols[ncols].type = "text";
            cols[ncols].cipher = 1;
            ncols++;
        }
    }
    /* Two fields that turn into the same SQL name would overwrite each
     * other; so would a field called _seq. */
    for (i = 0; i < ncols; i++) {
        if (strcmp(cols[i].name, "_seq") == 0) {
            log("database: BUG: table %s: field %s is reserved", table->name,
                cols[i].field->name);
            free(cols);
            return -1;
        }
        for (j = i + 1; j < ncols; j++) {
            if (strcmp(cols[i].name, cols[j].name) == 0 &&
                !(cols[i].field->load_only || cols[j].field->load_only)) {
                log("database: BUG: table %s: fields %s and %s both map to"
                    " column %s",
                    table->name, cols[i].field->name, cols[j].field->name,
                    cols[i].name);
                free(cols);
                return -1;
            }
        }
    }
    *cols_ret = cols;
    return ncols;
}

/* The state kept for the table with SQL name `name', created if new. */
static StoreTable* store_state(const char* name)
{
    StoreTable* st;

    for (st = store_tables; st; st = st->next) {
        if (strcmp(st->name, name) == 0)
            return st;
    }
    st = scalloc(1, sizeof(*st));
    strbcpy(st->name, name);
    st->next = store_tables;
    store_tables = st;
    return st;
}

/* The fully qualified, quoted name of the SQL table for `table'. */
static char* store_qualified(const DBTable* table, char* buf, size_t size)
{
    char name[STORE_NAMELEN + 1], quoted[STORE_QNAMELEN];

    store_mangle(table->name, name, sizeof(name));
    pg_quote_ident(name, quoted, sizeof(quoted));
    snprintf(buf, size, "%s.%s", store_qschema, quoted);
    return buf;
}

/* The script that creates `table' if it is missing and adds any column it
 * lacks.  Returns a string on the system allocator (NULL if out of
 * memory). */
static char* store_ddl(const DBTable* table, const StoreColumn* cols,
                       int ncols)
{
    char qtable[STORE_QNAMELEN * 2 + 2], qcol[STORE_QNAMELEN];
    struct PgBuf buf;
    int i;

    store_qualified(table, qtable, sizeof(qtable));
    pgbuf_init(&buf);
    pgbuf_printf(&buf,
                 "create table if not exists %s (\"_seq\" bigint"
                 " primary key",
                 qtable);
    for (i = 0; i < ncols; i++) {
        pg_quote_ident(cols[i].name, qcol, sizeof(qcol));
        pgbuf_printf(&buf, ", %s %s", qcol, cols[i].type);
    }
    pgbuf_puts(&buf, ");\n");
    if (ncols > 0) {
        pgbuf_printf(&buf, "alter table %s", qtable);
        for (i = 0; i < ncols; i++) {
            pg_quote_ident(cols[i].name, qcol, sizeof(qcol));
            pgbuf_printf(&buf, "%s add column if not exists %s %s",
                         i ? "," : "", qcol, cols[i].type);
        }
        pgbuf_puts(&buf, ";\n");
    }
    return pgbuf_steal(&buf);
}

/* FNV-1a, 64 bits. */
static uint64 store_hash(const char* data, size_t len)
{
    uint64 hash = 14695981039346656037ULL;
    size_t i;

    for (i = 0; i < len; i++) {
        hash ^= (unsigned char)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

/* Length of the valid UTF-8 sequence at `s', or 0 if there is none. */
static int utf8_seqlen(const unsigned char* s)
{
    if (s[0] < 0x80)
        return 1;
    if (s[0] >= 0xC2 && s[0] <= 0xDF)
        return (s[1] & 0xC0) == 0x80 ? 2 : 0;
    if (s[0] >= 0xE0 && s[0] <= 0xEF) {
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80)
            return 0;
        if (s[0] == 0xE0 && s[1] < 0xA0)
            return 0; /* overlong */
        if (s[0] == 0xED && s[1] >= 0xA0)
            return 0; /* surrogate */
        return 3;
    }
    if (s[0] >= 0xF0 && s[0] <= 0xF4) {
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 ||
            (s[3] & 0xC0) != 0x80)
            return 0;
        if (s[0] == 0xF0 && s[1] < 0x90)
            return 0; /* overlong */
        if (s[0] == 0xF4 && s[1] >= 0x90)
            return 0; /* past U+10FFFF */
        return 4;
    }
    return 0;
}

static int utf8_valid(const char* str, size_t len)
{
    const unsigned char* s = (const unsigned char*)str;
    size_t i = 0;

    while (i < len) {
        int n = utf8_seqlen(s + i);
        if (!n || i + n > len)
            return 0;
        i += n;
    }
    return 1;
}

/* Append one byte to a COPY text field, escaped. */
static void copy_byte(struct PgBuf* buf, unsigned char c)
{
    switch (c) {
        case '\\':
            pgbuf_append(buf, "\\\\", 2);
            break;
        case '\t':
            pgbuf_append(buf, "\\t", 2);
            break;
        case '\n':
            pgbuf_append(buf, "\\n", 2);
            break;
        case '\r':
            pgbuf_append(buf, "\\r", 2);
            break;
        default:
            pgbuf_append(buf, &c, 1);
            break;
    }
}

/* Append a string as a COPY text field: UTF-8, escaped; ISO-8859-1
 * converted to UTF-8 if it is not valid UTF-8 (counted in `*converted'). */
static void copy_text(struct PgBuf* buf, const char* str, size_t len,
                      int* converted)
{
    size_t i;

    if (utf8_valid(str, len)) {
        for (i = 0; i < len; i++)
            copy_byte(buf, (unsigned char)str[i]);
        return;
    }
    (*converted)++;
    for (i = 0; i < len; i++) {
        unsigned char c = (unsigned char)str[i];
        if (c < 0x80) {
            copy_byte(buf, c);
        }
        else {
            unsigned char seq[2];
            seq[0] = (unsigned char)(0xC0 | (c >> 6));
            seq[1] = (unsigned char)(0x80 | (c & 0x3F));
            pgbuf_append(buf, seq, 2);
        }
    }
}

/* Append a byte string as a COPY text field of type bytea: the \x form,
 * with its backslash escaped for COPY. */
static void copy_bytea(struct PgBuf* buf, const unsigned char* data,
                       size_t len)
{
    static const char hex[] = "0123456789abcdef";
    size_t i;

    pgbuf_append(buf, "\\\\x", 3);
    for (i = 0; i < len; i++) {
        char pair[2];
        pair[0] = hex[data[i] >> 4];
        pair[1] = hex[data[i] & 15];
        pgbuf_append(buf, pair, 2);
    }
}

/* Decode a bytea in its \x text form into `out' (at most `size' bytes,
 * zero-filled).  Returns nonzero if it was well formed. */
static int store_unhex(const char* text, unsigned char* out, size_t size)
{
    size_t n = 0;

    memset(out, 0, size);
    if (text[0] != '\\' || text[1] != 'x')
        return 0;
    for (text += 2; text[0] && text[1]; text += 2) {
        int hi, lo;
        hi = isdigit((unsigned char)text[0]) ? text[0] - '0'
                                             : (tolower(text[0]) - 'a' + 10);
        lo = isdigit((unsigned char)text[1]) ? text[1] - '0'
                                             : (tolower(text[1]) - 'a' + 10);
        if (hi < 0 || hi > 15 || lo < 0 || lo > 15)
            return 0;
        if (n < size)
            out[n] = (unsigned char)(hi << 4 | lo);
        n++;
    }
    return *text == '\0';
}

/*************************************************************************/
/************************ The control connection *************************/
/*************************************************************************/

/* Deadline for work on the control connection: the save timeout, since a
 * load is the same amount of data travelling the other way. */
static void store_deadline(struct PgDeadline* deadline)
{
    pg_deadline_set(deadline, db_conf_save_timeout());
}

/* Make sure the main thread's connection is up for the current
 * configuration, and take the connection to use: that of pg_sync.c, which
 * also holds this copy's claim on the schema.  A new configuration (REHASH)
 * means perhaps another server or another schema: every table will be
 * created there if need be and written whole on the next save. */
static int store_connect(void)
{
    StoreTable* st;

    if (!(store_pg = pg_sync_conn()))
        return 0;
    if (store_generation != db_conf_generation()) {
        for (st = store_tables; st; st = st->next) {
            st->hashed = 0;
            st->ensured = 0;
        }
        if (!pg_quote_ident(db_conf_schema(), store_qschema,
                            sizeof(store_qschema)))
            return 0;
        store_generation = db_conf_generation();
    }
    return 1;
}

int pg_store_open(void)
{
    if (!db_conf()) {
        log("database: the database block is missing");
        return 0;
    }
    /* Connect (creating the schema), bring the core's own tables up to
     * date, and claim the schema for this copy -- unless it only reads. */
    if (!pg_sync_open())
        return 0;
    if (!migration_core_start())
        return 0;
    if (!readonly && !pg_sync_claim())
        return 0;
    return store_connect();
}

void pg_store_close(void)
{
    StoreTable* st;

    pg_sync_close();
    store_pg = NULL;
    while ((st = store_tables) != NULL) {
        store_tables = st->next;
        free(st);
    }
}

/*************************************************************************/
/******************************** Loading ********************************/
/*************************************************************************/

/* Convert every column whose type does not match its field. */
static int store_fix_types(const DBTable* table, const StoreColumn* cols,
                           int ncols, const struct PgDeadline* deadline)
{
    char name[STORE_NAMELEN + 1], qtable[STORE_QNAMELEN * 2 + 2];
    char qcol[STORE_QNAMELEN], sql[1024];
    const char* values[2];
    enum DbError code;
    PGresult* res;
    int i, row, ok = 1;

    store_mangle(table->name, name, sizeof(name));
    values[0] = db_conf_schema();
    values[1] = name;
    if (!pg_run_params(store_pg, deadline,
                       "select column_name, data_type from"
                       " information_schema.columns where table_schema = $1"
                       " and table_name = $2",
                       2, values, "reading a table's columns", &res, &code))
        return 0;
    store_qualified(table, qtable, sizeof(qtable));
    for (i = 0; i < ncols && ok; i++) {
        for (row = 0; row < PQntuples(res); row++) {
            if (strcmp(PQgetvalue(res, row, 0), cols[i].name) == 0)
                break;
        }
        if (row >= PQntuples(res) ||
            strcmp(PQgetvalue(res, row, 1), cols[i].type) == 0)
            continue;
        log("database: table %s: converting column %s from %s to %s", name,
            cols[i].name, PQgetvalue(res, row, 1), cols[i].type);
        pg_quote_ident(cols[i].name, qcol, sizeof(qcol));
        snprintf(sql, sizeof(sql),
                 "alter table %s alter column %s type %s using %s::%s", qtable,
                 qcol, cols[i].type, qcol, cols[i].type);
        ok = pg_run(store_pg, deadline, sql, 1, "converting a column", NULL,
                    &code);
    }
    PQclear(res);
    return ok;
}

/* Store the text `value' of column `col' into `record'. */
static void store_put(void* record, const StoreColumn* col, const char* value,
                      const char* cipher)
{
    const DBField* field = col->field;
    union {
        int8 i8;
        uint8 u8;
        int16 i16;
        uint16 u16;
        int32 i32;
        uint32 u32;
        time_t t;
        char* s;
        Password pass;
    } v;
    char* buf;

    switch (field->type) {
        case DBTYPE_INT8:
            v.i8 = (int8)strtol(value, NULL, 10);
            break;
        case DBTYPE_UINT8:
            v.u8 = (uint8)strtoul(value, NULL, 10);
            break;
        case DBTYPE_INT16:
            v.i16 = (int16)strtol(value, NULL, 10);
            break;
        case DBTYPE_UINT16:
            v.u16 = (uint16)strtoul(value, NULL, 10);
            break;
        case DBTYPE_INT32:
            v.i32 = (int32)strtol(value, NULL, 10);
            break;
        case DBTYPE_UINT32:
            v.u32 = (uint32)strtoull(value, NULL, 10);
            break;
        case DBTYPE_TIME:
            v.t = (time_t)strtoll(value, NULL, 10);
            break;
        case DBTYPE_STRING:
            /* The record (or the field's put routine) takes the string. */
            v.s = value ? sstrdup(value) : NULL;
            break;
        case DBTYPE_BUFFER:
            buf = scalloc(field->length > 0 ? field->length : 1, 1);
            if (field->length > 0)
                strscpy(buf, value, field->length);
            put_dbfield(record, field, buf);
            free(buf);
            return;
        case DBTYPE_PASSWORD: {
            char password[PASSHASHMAX];
            if (!store_unhex(value, (unsigned char*)password,
                             sizeof(password)))
                memset(password, 0, sizeof(password));
            init_password(&v.pass);
            set_password(&v.pass, password, cipher);
            break;
        }
    }
    put_dbfield(record, field, &v);
}

int pg_store_load(DBTable* table)
{
    char qtable[STORE_QNAMELEN * 2 + 2], name[STORE_NAMELEN + 1];
    char sql[STORE_QNAMELEN * 2 + 64];
    struct PgDeadline deadline;
    StoreColumn* cols;
    int* colnum;
    enum DbError code;
    PGresult* res;
    char* ddl;
    int ncols, nall, i, row, nrows, ok;
    StoreColumn* all;

    if (!store_connect())
        return 0;
    store_mangle(table->name, name, sizeof(name));

    /* Create or extend the SQL table, and fix column types, first. */
    if ((ncols = store_columns(table, 0, &cols)) < 0)
        return 0;
    store_deadline(&deadline);
    if (!(ddl = store_ddl(table, cols, ncols))) {
        free(cols);
        return 0;
    }
    ok =
        pg_run(store_pg, &deadline, ddl, 1, "creating a table", NULL, &code) &&
        store_fix_types(table, cols, ncols, &deadline);
    pg_save_free(ddl);
    free(cols);
    if (!ok) {
        log("database: cannot create table %s: %s", name,
            pg_error_message(code));
        return 0;
    }

    /* Then read it: every column, load_only fields included. */
    if ((nall = store_columns(table, 1, &all)) < 0)
        return 0;
    store_qualified(table, qtable, sizeof(qtable));
    snprintf(sql, sizeof(sql), "select * from %s order by \"_seq\"", qtable);
    if (!pg_run(store_pg, &deadline, sql, 0, "reading a table", &res, &code)) {
        log("database: cannot read table %s: %s", name,
            pg_error_message(code));
        free(all);
        return 0;
    }
    colnum = smalloc(sizeof(*colnum) * (nall + 1));
    for (i = 0; i < nall; i++)
        colnum[i] = PQfnumber(res, all[i].name);

    nrows = PQntuples(res);
    for (row = 0; row < nrows; row++) {
        void* record = table->newrec();
        if (!record) {
            log("database: table %s: newrec() failed for row %d", name, row);
            break;
        }
        for (i = 0; i < nall; i++) {
            const char* value;
            const char* cipher = NULL;
            if (all[i].cipher || colnum[i] < 0)
                continue;
            value = PQgetisnull(res, row, colnum[i])
                        ? NULL
                        : PQgetvalue(res, row, colnum[i]);
            if (all[i].field->type == DBTYPE_PASSWORD) {
                /* The cipher is the next column. */
                if (i + 1 < nall && all[i + 1].cipher && colnum[i + 1] >= 0 &&
                    !PQgetisnull(res, row, colnum[i + 1]))
                    cipher = PQgetvalue(res, row, colnum[i + 1]);
                if (!value)
                    continue;
            }
            else if (!value && all[i].field->type != DBTYPE_STRING) {
                continue; /* SQL NULL: leave newrec()'s default */
            }
            store_put(record, &all[i], value, cipher);
        }
        table->insert(record);
    }
    PQclear(res);
    free(colnum);
    free(all);

    if (table->postload && !(*table->postload)()) {
        log("database: table %s: postload routine failed", name);
        return 0;
    }
    store_state(name)->ensured = store_generation;
    log_debug(1, "database: loaded %d record%s from %s", nrows,
              nrows == 1 ? "" : "s", name);
    return 1;
}

/*************************************************************************/
/********************************* Saving ********************************/
/*************************************************************************/

/* Snapshot one table into `t', in COPY text format.  Returns nonzero on
 * success. */
static int store_snapshot(const DBTable* table, const StoreColumn* cols,
                          int ncols, struct PgSaveTable* t)
{
    struct PgBuf* buf = &t->pst_data;
    unsigned long seq = 0;
    size_t bufsize = 16;
    int converted = 0, i;
    char* fieldbuf;
    void* record;

    for (i = 0; i < ncols; i++) {
        if (cols[i].field->type == DBTYPE_BUFFER &&
            (size_t)cols[i].field->length + 1 > bufsize)
            bufsize = cols[i].field->length + 1;
    }
    fieldbuf = smalloc(bufsize);

    for (record = table->first(); record; record = table->next()) {
        pgbuf_printf(buf, "%lu", ++seq);
        for (i = 0; i < ncols; i++) {
            const DBField* field = cols[i].field;
            union {
                int8 i8;
                uint8 u8;
                int16 i16;
                uint16 u16;
                int32 i32;
                uint32 u32;
                time_t t;
                const char* s;
                Password pass;
            } v;

            pgbuf_append(buf, "\t", 1);
            if (field->type == DBTYPE_BUFFER) {
                memset(fieldbuf, 0, bufsize);
                get_dbfield(record, field, fieldbuf);
                copy_text(
                    buf, fieldbuf,
                    strnlen(fieldbuf, field->length > 0 ? field->length : 0),
                    &converted);
                continue;
            }
            memset(&v, 0, sizeof(v));
            get_dbfield(record, field, &v);
            switch (field->type) {
                case DBTYPE_INT8:
                    pgbuf_printf(buf, "%d", (int)v.i8);
                    break;
                case DBTYPE_UINT8:
                    pgbuf_printf(buf, "%u", (unsigned)v.u8);
                    break;
                case DBTYPE_INT16:
                    pgbuf_printf(buf, "%d", (int)v.i16);
                    break;
                case DBTYPE_UINT16:
                    pgbuf_printf(buf, "%u", (unsigned)v.u16);
                    break;
                case DBTYPE_INT32:
                    pgbuf_printf(buf, "%ld", (long)v.i32);
                    break;
                case DBTYPE_UINT32:
                    pgbuf_printf(buf, "%lu", (unsigned long)v.u32);
                    break;
                case DBTYPE_TIME:
                    pgbuf_printf(buf, "%lld", (long long)v.t);
                    break;
                case DBTYPE_STRING:
                    if (v.s)
                        copy_text(buf, v.s, strlen(v.s), &converted);
                    else
                        pgbuf_append(buf, "\\N", 2);
                    break;
                case DBTYPE_PASSWORD:
                    if (cols[i].cipher) {
                        if (v.pass.cipher)
                            copy_text(buf, v.pass.cipher,
                                      strlen(v.pass.cipher), &converted);
                        else
                            pgbuf_append(buf, "\\N", 2);
                    }
                    else {
                        size_t len = sizeof(v.pass.password);
                        /* Trailing NULs are restored on load. */
                        while (len > 0 && !v.pass.password[len - 1])
                            len--;
                        copy_bytea(buf, (const unsigned char*)v.pass.password,
                                   len);
                    }
                    break;
                case DBTYPE_BUFFER:
                    break; /* handled above */
            }
        }
        pgbuf_append(buf, "\n", 1);
    }
    free(fieldbuf);
    t->pst_rows = seq;
    if (converted)
        log("database: table %s: %d string%s not valid UTF-8, saved as"
            " ISO-8859-1",
            t->pst_name, converted, converted == 1 ? " was" : "s were");
    return !buf->pgb_failed;
}

/* What a save remembers until it is over. */
typedef struct {
    PgStoreSaveDone done;
    int ntables;
    StoreTable** states;
    uint64* hashes;
    unsigned int generation;
} StoreSave;

static void store_save_free(StoreSave* save)
{
    free(save->states);
    free(save->hashes);
    free(save);
}

/* The save is over.  Main thread. */
static void store_save_done(struct PgSaveJob* job, void* arg)
{
    StoreSave* save = arg;
    PgStoreSaveDone done = save->done;
    unsigned long rows = 0;
    int i;

    for (i = 0; i < job->psj_ntables; i++)
        rows += job->psj_tables[i].pst_rows;
    if (job->psj_code == DB_OK) {
        /* Only now are the snapshots known to be in the database. */
        for (i = 0; i < save->ntables; i++) {
            save->states[i]->hash = save->hashes[i];
            save->states[i]->hashed = 1;
            save->states[i]->ensured = save->generation;
        }
        log_debug(1, "database: saved %d table%s (%lu record%s) in %ldms",
                  job->psj_ntables, job->psj_ntables == 1 ? "" : "s", rows,
                  rows == 1 ? "" : "s", job->psj_elapsed_ms);
    }
    else if (job->psj_stolen) {
        log("database: save refused: another copy of Services has claimed"
            " schema %s",
            db_conf_schema());
    }
    else {
        log("database: save failed after %ldms%s%s: %s", job->psj_elapsed_ms,
            *job->psj_failed ? " writing table " : "", job->psj_failed,
            pg_error_message(job->psj_code));
    }
    store_save_free(save);
    if (done)
        (*done)(job->psj_code == DB_OK);
}

int pg_store_save(DBTable** tables, int ntables, PgStoreSaveDone done)
{
    char qtable[STORE_QNAMELEN * 2 + 2], qcol[STORE_QNAMELEN];
    char name[STORE_NAMELEN + 1], search_path[STORE_QNAMELEN + 16];
    char check[STORE_QNAMELEN + 128];
    struct PgSaveJob* job;
    StoreSave* save;
    int i, n = 0;

    /* A new configuration is picked up here: connect to it (and claim it)
     * before writing anything there. */
    if (!store_connect()) {
        log("database: save failed: the database is not available");
        return 0;
    }
    if (!readonly && !pg_sync_token()) {
        log("database: save refused: this copy of Services no longer owns"
            " schema %s", db_conf_schema());
        return 0;
    }
    if (!(job = pg_save_job_new(ntables)))
        return 0;
    save = scalloc(1, sizeof(*save));
    save->done = done;
    save->states = scalloc(ntables + 1, sizeof(*save->states));
    save->hashes = scalloc(ntables + 1, sizeof(*save->hashes));
    save->generation = store_generation;

    for (i = 0; i < ntables; i++) {
        struct PgSaveTable* t = &job->psj_tables[n];
        StoreColumn* cols;
        StoreTable* st;
        struct PgBuf sql;
        uint64 hash;
        int ncols, c;

        if ((ncols = store_columns(tables[i], 0, &cols)) < 0)
            continue;
        store_mangle(tables[i]->name, name, sizeof(name));
        store_qualified(tables[i], qtable, sizeof(qtable));
        st = store_state(name);
        t->pst_name = pg_save_strdup(name);
        if (!store_snapshot(tables[i], cols, ncols, t)) {
            log("database: out of memory saving table %s", name);
            free(cols);
            pg_save_free(t->pst_name);
            t->pst_name = NULL;
            pgbuf_free(&t->pst_data);
            continue;
        }
        hash = store_hash(t->pst_data.pgb_data ? t->pst_data.pgb_data : "",
                          t->pst_data.pgb_len);
        if (st->hashed && st->hash == hash &&
            st->ensured == store_generation) {
            /* Unchanged since the last save: nothing to write. */
            free(cols);
            pg_save_free(t->pst_name);
            t->pst_name = NULL;
            pgbuf_free(&t->pst_data);
            continue;
        }
        if (st->ensured != store_generation)
            t->pst_ddl = store_ddl(tables[i], cols, ncols);
        pgbuf_init(&sql);
        pgbuf_printf(&sql, "truncate table %s", qtable);
        t->pst_truncate = pgbuf_steal(&sql);
        pgbuf_printf(&sql, "copy %s (\"_seq\"", qtable);
        for (c = 0; c < ncols; c++) {
            pg_quote_ident(cols[c].name, qcol, sizeof(qcol));
            pgbuf_printf(&sql, ", %s", qcol);
        }
        pgbuf_puts(&sql, ") from stdin");
        t->pst_copy = pgbuf_steal(&sql);
        free(cols);
        save->states[n] = st;
        save->hashes[n] = hash;
        n++;
    }
    job->psj_ntables = n;
    save->ntables = n;

    if (n == 0) {
        /* Nothing changed. */
        pg_save_job_free(job);
        store_save_free(save);
        if (done)
            (*done)(1);
        return 1;
    }

    snprintf(check, sizeof(check),
             "select token from %s.instance where id = 1 for update",
             store_qschema);
    job->psj_dsn = pg_save_strdup(db_conf_dsn(DB_ROLE_WRITE));
    job->psj_search_path = pg_save_strdup(
        pg_search_path(db_conf_schema(), search_path, sizeof(search_path)));
    job->psj_check = pg_save_strdup(check);
    job->psj_token = readonly ? NULL : pg_save_strdup(pg_sync_token());
    job->psj_timeout_ms = db_conf_save_timeout();

    if (worker_enabled() && pg_save_submit(job, store_save_done, save))
        return 1;

    /* No worker to run it -- Services have not forked yet, or are going
     * away -- so it runs here, and the main loop waits. */
    pg_save_run(job);
    store_save_done(job, save);
    pg_save_job_free(job);
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
