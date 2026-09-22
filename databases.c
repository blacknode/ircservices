/* Database core routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "databases.h"
#include "encrypt.h"
#include "modules.h"

/*************************************************************************/

/* List of registered tables */
typedef struct dbtablenode_ DBTableNode;
struct dbtablenode_ {
    DBTableNode *next, *prev;
    DBTable *table;
    const Module *owner;  /* Which module registered this table? */
    int loaded;           /* Has this table been loaded? */
};
static DBTableNode *tables = NULL;

/* Currently active database module */
static DBModule *dbmodule = NULL;


/* Local routines: */
static int do_unload_module(const Module *module);

/*************************************************************************/
/*************************************************************************/

/* Initialization/cleanup routines. */

int database_init(int ac, char **av)
{
    if (!add_callback(NULL, "unload module", do_unload_module)) {
        log("database_init: add_callback() failed");
        return 0;
    }
    return 1;
}

/************************************/

void database_cleanup(void)
{
    remove_callback(NULL, "unload module", do_unload_module);
}

/************************************/

/* Check for tables that the module forgot to unload. */

static int do_unload_module(const Module *module)
{
    DBTableNode *t, *t2;

    LIST_FOREACH_SAFE (t, tables, t2) {
        if (t->owner == module) {
            log("database: Module `%s' forgot to unregister table `%s'",
                get_module_name(module), t->table->name);
            unregister_dbtable(t->table);
        }
    }
    return 0;
}

/*************************************************************************/

/* Register a new database table.  Returns nonzero on success, zero on
 * error.
 */

int _register_dbtable(DBTable *table, const Module *caller)
{
    DBTableNode *t;

    /* Sanity checks on parameter */
    if (!table) {
        log("BUG: register_dbtable() with NULL table!");
        return 0;
    }
    if (!table->name) {
        log("BUG: register_dbtable(): table->name is NULL!");
        return 0;
    }
    if (!table->fields || !table->newrec || !table->freerec
     || !table->insert || !table->first || !table->next
    ) {
        log("BUG: register_dbtable(%s): table->%s is NULL!", table->name,
            !table->fields  ? "fields"  :
            !table->newrec  ? "newrec"  :
            !table->freerec ? "freerec" :
            !table->insert  ? "insert"  :
            !table->first   ? "first"   : 
            !table->next    ? "next"    :
                              "???");
        return 0;
    }

    /* Sanity check: make sure it's not already registered */
    LIST_FOREACH (t, tables) {
        if (t->table == table) {
            log("BUG: register_dbtable(%s): table already registered!",
                table->name);
            break;
        }
    }

    /* Allocate and append to list (make sure to preserve order, since
     * later tables may depend on earlier ones); if a database module is
     * available, load the table ammediately*/
    t = smalloc(sizeof(*t));
    t->table = table;
    t->owner = caller;
    t->loaded = 0;
    LIST_APPEND(t, tables);
    if (dbmodule)
        t->loaded = (*dbmodule->load_table)(t->table);

    return 1;
}

/*************************************************************************/

/* Unregister a database table.  Does nothing if the table was not
 * registered in the first place.
 */

void unregister_dbtable(DBTable *table)
{
    DBTableNode *t;

    if (!table) {
        log("BUG: unregister_dbtable() with NULL table!");
        return;
    }
    /* Sanity check: make sure it was registered first */
    LIST_FOREACH (t, tables) {
        if (t->table == table) {
            LIST_REMOVE(t, tables);
            free(t);
            break;
        }
    }
}

/*************************************************************************/

/* Save all registered database tables to permanent storage.  Returns 1 if
 * all tables were successfully saved or no tables are registered, 0 if
 * some tables were successfully saved (but some were not), or -1 if no
 * tables were successfully saved.
 */

int save_all_dbtables(void)
{
    DBTableNode *t;
    int some_saved = 0;
    int some_failed = 0;

    if (!tables)
        return 1;
    if (!dbmodule) {
        log("save_all_dbtables(): No database module registered!");
        return -1;
    }
    LIST_FOREACH (t, tables) {
        if ((*dbmodule->save_table)(t->table)) {
            some_saved = 1;
        } else {
            log("save_all_dbtables(): Failed to save table `%s'",
                t->table->name);
            some_failed = 1;
        }
    }
    return some_saved - some_failed;
}

/*************************************************************************/
/*************************************************************************/

/* Register a database module.  Returns nonzero on success, zero on error.
 * On success, all registered tables which have not already been loaded
 * will be loaded from permanent storage.  Only one database module can be
 * registered.
 */

int register_dbmodule(DBModule *module)
{
    DBTableNode *t;

    if (!module) {
        log("BUG: register_dbmodule() with NULL module!");
        return 0;
    }
    if (!module->load_table || !module->save_table) {
        log("BUG: register_dbmodule(): module->%s is NULL!",
            !module->load_table ? "load_table" : "save_table");
        return 0;
    }
    if (dbmodule) {
        if (module == dbmodule)
            log("BUG: register_dbmodule(): attempt to re-register module!");
        else
            log("register_dbmodule(): a database module is already registered");
        return 0;
    }

    dbmodule = module;
    LIST_FOREACH (t, tables) {
        if (!t->loaded)
            t->loaded = (*dbmodule->load_table)(t->table);
    }
    return 1;
}

/*************************************************************************/

/* Unregister a database module.  Does nothing if the module was not
 * registered in the first place.
 */

void unregister_dbmodule(DBModule *module)
{
    if (!module) {
        log("BUG: unregister_dbmodule() with NULL module!");
        return;
    }
    if (dbmodule == module)
        dbmodule = NULL;
}

/*************************************************************************/

/* Read a value from a database field.  The value buffer is assumed to be
 * large enough to hold the retrieved value.
 */

void get_dbfield(const void *record, const DBField *field, void *buffer)
{
    int size;

    if (!record || !field || !buffer) {
        log("BUG: get_dbfield(): %s is NULL!",
            !record ? "record" : !field ? "field" : "buffer");
        return;
    }
    if (field->get) {
        (*field->get)(record, buffer);
        return;
    }
    switch (field->type) {
        case DBTYPE_INT8:     size = 1;                break;
        case DBTYPE_UINT8:    size = 1;                break;
        case DBTYPE_INT16:    size = 2;                break;
        case DBTYPE_UINT16:   size = 2;                break;
        case DBTYPE_INT32:    size = 4;                break;
        case DBTYPE_UINT32:   size = 4;                break;
        case DBTYPE_TIME:     size = sizeof(time_t);   break;
        case DBTYPE_STRING:   size = sizeof(char *);   break;
        case DBTYPE_BUFFER:   size = field->length;    break;
        case DBTYPE_PASSWORD: size = sizeof(Password); break;
        default:
            log("BUG: bad field type %d in get_dbfield()", field->type);
            return;
    }
    if (!size) {
        return;
    }
    memcpy(buffer, (const uint8 *)record + field->offset, size);
}

/*************************************************************************/

/* Store a value to a database field. */

void put_dbfield(void *record, const DBField *field, const void *value)
{
    int size;

    if (!record || !field || !value) {
        log("BUG: get_dbfield(): %s is NULL!",
            !record ? "record" : !field ? "field" : "value");
        return;
    }
    if (field->put) {
        (*field->put)(record, value);
        return;
    }
    switch (field->type) {
        case DBTYPE_INT8:     size = 1;                break;
        case DBTYPE_UINT8:    size = 1;                break;
        case DBTYPE_INT16:    size = 2;                break;
        case DBTYPE_UINT16:   size = 2;                break;
        case DBTYPE_INT32:    size = 4;                break;
        case DBTYPE_UINT32:   size = 4;                break;
        case DBTYPE_TIME:     size = sizeof(time_t);   break;
        case DBTYPE_STRING:   size = sizeof(char *);   break;
        case DBTYPE_BUFFER:   size = field->length;    break;
        case DBTYPE_PASSWORD: size = sizeof(Password); break;
        default:
            log("BUG: bad field type %d in get_dbfield()", field->type);
            return;
    }
    if (!size) {
        return;
    }
    memcpy((uint8 *)record + field->offset, value, size);
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
