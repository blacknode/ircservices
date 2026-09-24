/* The table store's save job: what the main thread hands a worker.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * A save is built in the main thread (pg_store.c, which knows about
 * DBTables and records) and run in a pool thread (pg_save.c, which knows
 * about nothing but PostgreSQL).  This header is what they share, and like
 * postgres.h it does not include services.h: everything here is allocated
 * with the system allocator, because the worker thread frees and reallocs
 * none of it but reads all of it, and the main thread frees it afterwards.
 */

#ifndef PG_SAVE_H
#define PG_SAVE_H

#include "postgres.h"

#include <stddef.h>

/*************************************************************************/

/* A growable byte buffer on the system allocator.  An allocation failure
 * is sticky: every later append is ignored and pgb_failed stays set, so a
 * caller checks once at the end. */
struct PgBuf {
    char* pgb_data;
    size_t pgb_len;
    size_t pgb_size;
    int pgb_failed;
};

extern void pgbuf_init(struct PgBuf* buf);
extern void pgbuf_free(struct PgBuf* buf);
extern void pgbuf_append(struct PgBuf* buf, const void* data, size_t len);
extern void pgbuf_puts(struct PgBuf* buf, const char* str);
extern void pgbuf_printf(struct PgBuf* buf, const char* fmt, ...)
#ifdef __GNUC__
    __attribute__((format(printf, 2, 3)))
#endif
    ;
/* Take the contents as a NUL-terminated string (the buffer is emptied);
 * NULL if an allocation failed. */
extern char* pgbuf_steal(struct PgBuf* buf);

/* strdup() on the system allocator, for the strings of a job built in a
 * file that includes services.h (where strdup() may be the allocation
 * tracker's).  NULL in, NULL out. */
extern char* pg_save_strdup(const char* str);

/* free() on the system allocator, for what pgbuf_steal() and
 * pg_save_strdup() returned to such a file. */
extern void pg_save_free(void* ptr);

/*************************************************************************/

/* One table of a save. */
struct PgSaveTable {
    char* pst_name;         /* SQL name of the table, for the log */
    char* pst_ddl;          /* Script creating/extending it, or NULL */
    char* pst_truncate;     /* "truncate table <t>" */
    char* pst_copy;         /* "copy <t> (<columns>) from stdin" */
    struct PgBuf pst_data;  /* The rows, in COPY text format */
    unsigned long pst_rows; /* How many */
};

/* One save: every table that changed, in one transaction. */
struct PgSaveJob {
    /* --- set by the main thread --- */
    char* psj_dsn;         /* Where to connect (the write side) */
    char* psj_search_path; /* The configured schema, then public */
    char* psj_check;       /* Query returning the owning instance token */
    char* psj_token;       /* This instance's token, or NULL for none */
    int psj_timeout_ms;    /* Deadline for the whole save */
    int psj_ntables;
    struct PgSaveTable* psj_tables;

    /* --- set by the worker thread --- */
    enum DbError psj_code; /* How it ended */
    int psj_stolen;        /* Another instance owns the schema now */
    long psj_elapsed_ms;   /* How long it took, connect included */
    char psj_failed[64];   /* Table being written when it failed */
};

/* Called in the main thread when a save has finished (successfully or
 * not).  The job is freed after this returns. */
typedef void (*PgSaveDoneFn)(struct PgSaveJob* job, void* arg);

/* Allocate an empty job with room for `ntables' tables. */
extern struct PgSaveJob* pg_save_job_new(int ntables);

/* Free a job and everything in it. */
extern void pg_save_job_free(struct PgSaveJob* job);

/* Hand `job' to the worker pool.  On success the job belongs to the save
 * and `done' will be called; on failure (the pool is not running, or its
 * queue is full) it is still the caller's. */
extern int pg_save_submit(struct PgSaveJob* job, PgSaveDoneFn done, void* arg);

/* Run `job' right here, in the calling thread, and return.  For the last
 * save before Services exit, when there may be no worker left to run it. */
extern void pg_save_run(struct PgSaveJob* job);

/*************************************************************************/

#endif /* PG_SAVE_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
