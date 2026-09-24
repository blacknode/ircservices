/* Database structures and declarations.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "services.h"

#ifndef MODULES_H
#include "modules.h"
#endif

/*************************************************************************/

/* Type constants for DBField.type */
typedef enum {
    DBTYPE_INT8,
    DBTYPE_UINT8,
    DBTYPE_INT16,
    DBTYPE_UINT16,
    DBTYPE_INT32,
    DBTYPE_UINT32,
    DBTYPE_TIME,
    DBTYPE_STRING,
    DBTYPE_BUFFER, /* Buffer size in DBField.length */
    DBTYPE_PASSWORD,
} DBType;

/* Structure that describes a field of a database table */
typedef struct dbfield_ {
    const char* name; /* Field name */
    DBType type;      /* Field type */
    int offset;       /* Offset to field from start of structure
                       *    (use standard `offsetof' macro) */
    int length;       /* Length of DBTYPE_BUFFER fields */
    int load_only;    /* If nonzero, field is not saved (use for reading
                       *    obsolete fields) */
    void (*get)(const void* record, void** value_ret);
    /* Function to retrieve the field's value when
     *    saving; `value_ret' points to a variable of
     *    the appropriate type (for DBTYPE_STRING, the
     *    string should be malloc'd if not NULL) */
    void (*put)(void* record, const void* value);
    /* Function to set the field's value on load;
     *    `value' points to a variable of the
     *    appropriate type */
} DBField;

/* Structure that describes a database table */
typedef struct dbtable_ {
    const char* name; /* Table name */
    DBField* fields;  /* Array of fields, terminated with name==NULL */
    void* (*newrec)(void);
    /* Routine to allocate a new record */
    void (*freerec)(void* record);
    /* Routine to free an allocated record (that has
     *    not been inserted into the table) */
    void (*insert)(void* record);
    /* Routine to insert a record (used when loading,
     *    returns nonzero for success or 0 for failure) */
    void* (*first)(void), *(*next)(void);
    /* Routines to iterate through all records (used
     *    when saving) */
    int (*postload)(void);
    /* Routine called after all records have been loaded;
     *    returns nonzero for success or 0 for failure */
} DBTable;

/*************************************************************************/

/* Macro to return a pointer to a field in a record */
#define DB_FIELDPTR(record, field) ((int8*)(record) + (field)->offset)

/*************************************************************************/

/* Initialization/cleanup routines.  database_init() opens the table store
 * (PostgreSQL, see the `database' block of ircservices.conf) and fails if
 * the database cannot be reached: Services do not run without their data. */
extern int database_init(int ac, char** av);
extern void database_cleanup(void);

/* Register a new database table, and load it from the database.  Returns
 * nonzero on success, zero on error (the table could not be loaded: it is
 * then not registered, and the module should fail to initialize).  Tables
 * are saved in the order they were registered, and loaded as they are
 * registered, so register a table after the tables it depends on. */
#define register_dbtable(table) _register_dbtable((table), THIS_MODULE)
extern int _register_dbtable(DBTable* table, const Module* caller);

/* Unregister a database table.  Does nothing if the table was not
 * registered in the first place. */
extern void unregister_dbtable(DBTable* table);

/* Save all registered database tables.  The tables are snapshotted now and
 * written to the database in the background, every one that changed in one
 * transaction; the "save data complete" callback is called with 1 or 0
 * when the save is over (possibly before this returns).  A save requested
 * while one is running is done when that one ends.  Returns 1 if the save
 * was started or queued, 0 if it could not be. */
extern int save_all_dbtables(void);

/* Nonzero while a save is running or queued. */
extern int database_saving(void);

/* Wait (at most the save timeout) for the running and queued saves to end.
 * Called before Services exit, while the tables are still registered. */
extern void database_flush(void);

/* Read a value from a database field.  The value buffer is assumed to be
 * large enough to hold the retrieved value. */
extern void get_dbfield(const void* record, const DBField* field,
                        void* buffer);

/* Store a value to a database field. */
extern void put_dbfield(void* record, const DBField* field, const void* value);

/*************************************************************************/

#endif /* DATABASE_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
