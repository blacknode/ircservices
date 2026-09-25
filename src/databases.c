/* Database core routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "databases.h"
#include "db.h"
#include "encrypt.h"
#include "modules.h"
#include "services.h"
#include "worker.h"

#include "postgres/pg_store.h"

/*************************************************************************/

/* The tables live in PostgreSQL (src/postgres/pg_store.c): each one is
 * loaded when its module registers it, and every table is written back on
 * every save.  This file is the registry in front of that: which tables
 * exist, in which order, and which save is running. */

/* List of registered tables */
typedef struct dbtablenode_ DBTableNode;
struct dbtablenode_ {
    DBTableNode *next, *prev;
    DBTable* table;
    const Module* owner; /* Which module registered this table? */
};
static DBTableNode* tables = NULL;

/* Saves: one running at a time, and at most one more queued behind it
 * (a save snapshots every table, so two queued saves are one save). */
static int save_running = 0;
static int save_queued = 0;

/* Local routines: */
static int do_unload_module(const Module* module);
static void save_done(int ok);

/*************************************************************************/
/*************************************************************************/

/* Initialization/cleanup routines. */

int database_init(int ac, char** av)
{
    if (!event_attach(NULL, EVENT_MODULE_UNLOADED, do_unload_module)) {
        log("database_init: event_attach() failed");
        return 0;
    }
    if (!pg_store_open()) {
        log("database: cannot open the database; check the database block"
            " of %s", IRCSERVICES_CONF);
        return 0;
    }
    return 1;
}

/************************************/

void database_cleanup(void)
{
    event_detach(NULL, EVENT_MODULE_UNLOADED, do_unload_module);
    pg_store_close();
}

/************************************/

/* Check for tables that the module forgot to unload. */

static int do_unload_module(const Module* module)
{
    DBTableNode *t, *t2;

    LIST_FOREACH_SAFE(t, tables, t2)
    {
        if (t->owner == module) {
            log("database: Module `%s' forgot to unregister table `%s'",
                module_name(module), t->table->name);
            unregister_dbtable(t->table);
        }
    }
    return 0;
}

/*************************************************************************/

/* Register a new database table and load it.  Returns nonzero on success,
 * zero on error.
 */

int _register_dbtable(DBTable* table, const Module* caller)
{
    DBTableNode* t;

    /* Sanity checks on parameter */
    if (!table) {
        log("BUG: register_dbtable() with NULL table!");
        return 0;
    }
    if (!table->name) {
        log("BUG: register_dbtable(): table->name is NULL!");
        return 0;
    }
    if (!table->fields || !table->newrec || !table->freerec ||
        !table->insert || !table->first || !table->next) {
        log("BUG: register_dbtable(%s): table->%s is NULL!", table->name,
            !table->fields    ? "fields"
            : !table->newrec  ? "newrec"
            : !table->freerec ? "freerec"
            : !table->insert  ? "insert"
            : !table->first   ? "first"
            : !table->next    ? "next"
                              : "???");
        return 0;
    }

    /* Sanity check: make sure it's not already registered */
    LIST_FOREACH(t, tables)
    {
        if (t->table == table) {
            log("BUG: register_dbtable(%s): table already registered!",
                table->name);
            return 0;
        }
    }

    /* Load it before registering it: a table that could not be loaded is
     * not saved either, because saving it would replace the data in the
     * database with an empty table. */
    if (!pg_store_load(table)) {
        log("database: cannot load table `%s'", table->name);
        return 0;
    }

    /* Append to list (preserving order, since later tables may depend on
     * earlier ones). */
    t = smalloc(sizeof(*t));
    t->table = table;
    t->owner = caller;
    LIST_APPEND(t, tables);
    return 1;
}

/*************************************************************************/

/* Unregister a database table.  Does nothing if the table was not
 * registered in the first place.
 */

void unregister_dbtable(DBTable* table)
{
    DBTableNode* t;

    if (!table) {
        log("BUG: unregister_dbtable() with NULL table!");
        return;
    }
    LIST_FOREACH(t, tables)
    {
        if (t->table == table) {
            LIST_REMOVE(t, tables);
            free(t);
            break;
        }
    }
}

/*************************************************************************/

/* Save all registered database tables.  Returns 1 if the save was started
 * or queued, 0 if it could not be; the outcome is reported through the
 * "core.save_complete" event (EVENT_SAVE_COMPLETE).
 */

int save_all_dbtables(void)
{
    DBTableNode* t;
    DBTable** list;
    int count = 0, ok;

    if (save_running) {
        save_queued = 1;
        return 1;
    }
    LIST_FOREACH(t, tables)
    count++;
    list = smalloc(sizeof(*list) * (count + 1));
    count = 0;
    LIST_FOREACH(t, tables)
    list[count++] = t->table;

    save_running = 1;
    ok = pg_store_save(list, count, save_done);
    free(list);
    if (!ok) {
        save_running = 0;
        wallops(NULL, "\2Warning:\2 Databases could not be saved; see the"
                      " log for details.");
        event_emit(save_complete_event, 0);
        return 0;
    }
    return 1;
}

/* A save is over (possibly from within save_all_dbtables()). */
static void save_done(int ok)
{
    save_running = 0;
    if (!ok)
        wallops(NULL, "\2Warning:\2 Databases could not be saved; see the"
                      " log for details.");
    event_emit(save_complete_event, ok);
    if (save_queued) {
        save_queued = 0;
        save_all_dbtables();
    }
}

/*************************************************************************/

int database_saving(void)
{
    return save_running || save_queued;
}

/************************************/

void database_flush(void)
{
    time_t deadline = time(NULL) + db_conf_save_timeout() / 1000 + 5;

    while (database_saving() && time(NULL) < deadline)
        worker_wait(100);
    if (database_saving())
        log("database: gave up waiting for the last save");
}

/*************************************************************************/

/* Read a value from a database field.  The value buffer is assumed to be
 * large enough to hold the retrieved value.
 */

void get_dbfield(const void* record, const DBField* field, void* buffer)
{
    int size;

    if (!record || !field || !buffer) {
        log("BUG: get_dbfield(): %s is NULL!", !record  ? "record"
                                               : !field ? "field"
                                                        : "buffer");
        return;
    }
    if (field->get) {
        (*field->get)(record, buffer);
        return;
    }
    switch (field->type) {
        case DBTYPE_INT8:
            size = 1;
            break;
        case DBTYPE_UINT8:
            size = 1;
            break;
        case DBTYPE_INT16:
            size = 2;
            break;
        case DBTYPE_UINT16:
            size = 2;
            break;
        case DBTYPE_INT32:
            size = 4;
            break;
        case DBTYPE_UINT32:
            size = 4;
            break;
        case DBTYPE_TIME:
            size = sizeof(time_t);
            break;
        case DBTYPE_STRING:
            size = sizeof(char*);
            break;
        case DBTYPE_BUFFER:
            size = field->length;
            break;
        case DBTYPE_PASSWORD:
            size = sizeof(Password);
            break;
        default:
            log("BUG: bad field type %d in get_dbfield()", field->type);
            return;
    }
    if (!size) {
        return;
    }
    memcpy(buffer, (const uint8*)record + field->offset, size);
}

/*************************************************************************/

/* Store a value to a database field. */

void put_dbfield(void* record, const DBField* field, const void* value)
{
    int size;

    if (!record || !field || !value) {
        log("BUG: get_dbfield(): %s is NULL!", !record  ? "record"
                                               : !field ? "field"
                                                        : "value");
        return;
    }
    if (field->put) {
        (*field->put)(record, value);
        return;
    }
    switch (field->type) {
        case DBTYPE_INT8:
            size = 1;
            break;
        case DBTYPE_UINT8:
            size = 1;
            break;
        case DBTYPE_INT16:
            size = 2;
            break;
        case DBTYPE_UINT16:
            size = 2;
            break;
        case DBTYPE_INT32:
            size = 4;
            break;
        case DBTYPE_UINT32:
            size = 4;
            break;
        case DBTYPE_TIME:
            size = sizeof(time_t);
            break;
        case DBTYPE_STRING:
            size = sizeof(char*);
            break;
        case DBTYPE_BUFFER:
            size = field->length;
            break;
        case DBTYPE_PASSWORD:
            size = sizeof(Password);
            break;
        default:
            log("BUG: bad field type %d in get_dbfield()", field->type);
            return;
    }
    if (!size) {
        return;
    }
    memcpy((uint8*)record + field->offset, value, size);
}

/*************************************************************************/

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
