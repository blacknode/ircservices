/* The table store: the Services tables, kept in PostgreSQL.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Core-private: databases.c is the only caller.  See pg_store.c for how
 * the tables are laid out and written.
 */

#ifndef PG_STORE_H
#define PG_STORE_H

#include "databases.h"

/* Open the store: connect, create the schema and the bookkeeping tables
 * if they are missing, and claim the schema for this copy of Services
 * (unless running read-only).  Blocks the main thread; called once from
 * init(), before any module is loaded.  Returns nonzero on success; on
 * failure the reason has been logged. */
extern int pg_store_open(void);

/* Close the store, releasing the claim.  At exit, after the last save. */
extern void pg_store_close(void);

/* Load `table' from its SQL table (creating or extending the SQL table
 * first if needed): every row, in the order it was saved, goes through
 * newrec(), the fields, and insert(); then postload() runs.  Blocks the
 * main thread (a table is loaded when its module is).  Returns nonzero on
 * success. */
extern int pg_store_load(DBTable* table);

/* Called with the outcome of a save: 1 if every table was written (or
 * nothing had changed), 0 if nothing was. */
typedef void (*PgStoreSaveDone)(int ok);

/* Save `tables': snapshot them now, in the main thread, and write every
 * one that changed since the last successful save in one transaction, in
 * a worker thread.  `done' is called in the main thread when the save is
 * over -- possibly before this returns, when there was nothing to write or
 * no worker to write it (the save then runs here and blocks).  Returns
 * nonzero if `done' has been or will be called. */
extern int pg_store_save(DBTable** tables, int ntables, PgStoreSaveDone done);

#endif /* PG_STORE_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
