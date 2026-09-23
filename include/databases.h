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

#ifndef MODULES_H
# include "modules.h"
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
    DBTYPE_BUFFER,      /* Buffer size in DBField.length */
    DBTYPE_PASSWORD,
} DBType;

/* Structure that describes a field of a database table */
typedef struct dbfield_ {
    const char *name;   /* Field name */
    DBType type;        /* Field type */
    int offset;         /* Offset to field from start of structure
                         *    (use standard `offsetof' macro) */
    int length;         /* Length of DBTYPE_BUFFER fields */
    int load_only;      /* If nonzero, field is not saved (use for reading
                         *    obsolete fields) */
    void (*get)(const void *record, void **value_ret);
                        /* Function to retrieve the field's value when
                         *    saving; `value_ret' points to a variable of
                         *    the appropriate type (for DBTYPE_STRING, the
                         *    string should be malloc'd if not NULL) */
    void (*put)(void *record, const void *value);
                        /* Function to set the field's value on load;
                         *    `value' points to a variable of the
                         *    appropriate type */
} DBField;

/* Structure that describes a database table */
typedef struct dbtable_ {
    const char *name;   /* Table name */
    DBField *fields;    /* Array of fields, terminated with name==NULL */
    void *(*newrec)(void);
                        /* Routine to allocate a new record */
    void (*freerec)(void *record);
                        /* Routine to free an allocated record (that has
                         *    not been inserted into the table) */
    void (*insert)(void *record);
                        /* Routine to insert a record (used when loading,
                         *    returns nonzero for success or 0 for failure) */
    void *(*first)(void), *(*next)(void);
                        /* Routines to iterate through all records (used
                         *    when saving) */
    int (*postload)(void);
                        /* Routine called after all records have been loaded;
                         *    returns nonzero for success or 0 for failure */
} DBTable;

/* Container for module-implemented database functions */
typedef struct dbmodule_ {
    /* Load the given table from permanent storage, returning nonzero for
     * success, zero for failure. */
    int (*load_table)(DBTable *table);
    /* Save the given table to permanent storage, returning nonzero for
     * success, zero for failure */
    int (*save_table)(DBTable *table);
} DBModule;

/*************************************************************************/

/* Macro to return a pointer to a field in a record */
#define DB_FIELDPTR(record,field) ((int8 *)(record) + (field)->offset)

/*************************************************************************/

/* Initialization/cleanup routines. */
extern int database_init(int ac, char **av);
extern void database_cleanup(void);

/* Register a new database table.  Returns nonzero on success, zero on
 * error. */
#define register_dbtable(table) _register_dbtable((table), THIS_MODULE)
extern int _register_dbtable(DBTable *table, const Module *caller);

/* Unregister a database table.  Does nothing if the table was not
 * registered in the first place. */
extern void unregister_dbtable(DBTable *table);

/* Save all registered database tables to permanent storage.  Returns 1 if
 * all tables were successfully saved or no tables are registered, 0 if
 * some tables were successfully saved (but some were not), or -1 if no
 * tables were successfully saved. */
extern int save_all_dbtables(void);

/* Register a database module.  Returns nonzero on success, zero on error.
 * On success, all registered tables which have not already been loaded
 * will be loaded from permanent storage.  Only one database module can be
 * registered. */
extern int register_dbmodule(DBModule *module);

/* Unregister a database module.  Does nothing if the module was not
 * registered in the first place. */
extern void unregister_dbmodule(DBModule *module);

/* Read a value from a database field.  The value buffer is assumed to be
 * large enough to hold the retrieved value. */
extern void get_dbfield(const void *record, const DBField *field,
                        void *buffer);

/* Store a value to a database field. */
extern void put_dbfield(void *record, const DBField *field, const void *value);

/*************************************************************************/

#endif  /* DATABASE_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
