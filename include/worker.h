/* Worker threads: blocking work, off the main thread.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (include/worker.h).
 *
 * The Services core stays single-threaded.  Everything in this file exists
 * so that work which would otherwise stall it -- a database round trip, a
 * cache lookup, a table snapshot being written out -- can happen somewhere
 * else.
 *
 * There are two shapes:
 *
 *   - A task (WorkTask) is a one-shot unit of work handed to a pool of
 *     interchangeable threads.  Submit it with worker_submit(); its wt_work
 *     runs in whichever pool thread picks it up, and its wt_done runs
 *     afterwards in the main thread.
 *
 *   - A dedicated worker (Worker) is a thread of its own with its own loop,
 *     for something that is not a series of short tasks: a pooled database
 *     connection, a connection to a cache.  Start it with worker_spawn(); it
 *     hands results back with worker_post(), which puts a task straight onto
 *     the reply queue so its wt_done runs in the main thread just the same.
 *
 * Both shapes wake the main thread through a self-pipe that the socket
 * layer watches like any other descriptor (sock_watch_fd()), so no event
 * engine needed changing to support this.
 *
 * THE RULE
 *
 * A worker thread never touches core state.
 *
 * Not a User, not a Channel, not a NickInfo, not the hash tables, not a
 * global of any kind.  Not smalloc()/sstrdup(), and not malloc()/free() as
 * seen from a file that includes services.h: with MEMCHECKS, memory.h
 * redefines them to the allocation tracker, which has no locking.  Not
 * log(), not module_log(), not wallops() or any of the send_*() family.
 * Not the configuration, which the main thread replaces on REHASH.
 *
 * What a worker thread may call is short enough to list: worker_alloc(),
 * worker_free(), worker_log(), worker_stopping(), worker_stop_fd(),
 * worker_post(), worker_submit(), the system's own thread-safe library
 * functions, and pure functions that work on a context the caller
 * supplied.
 *
 * Everything a worker needs, it receives copied into the task.  Everything
 * it produces, it returns copied in the task.  That is the whole boundary,
 * and it is the only part of Services that has to be audited for thread
 * safety.  Source files whose code runs in a worker thread do not include
 * services.h at all.
 *
 * Worker threads are created with every signal blocked: Services' signal
 * handlers siglongjmp() into the main loop and must only ever run in the
 * main thread.
 *
 * See docs/readme.database.
 */

#ifndef WORKER_H
#define WORKER_H

#include <stddef.h> /* size_t */
#include <time.h>   /* time_t */

struct Module_;
struct WorkTask;
struct Worker;

/* Largest pool the configuration accepts.  Not a resource limit so much as
 * a sanity one: a typo of 100000 should be refused rather than obeyed. */
#define WORKER_MAX_THREADS 64

/* Pool size and queue depth when the `workers' block does not set them. */
#define WORKER_DEFAULT_THREADS   2
#define WORKER_DEFAULT_QUEUE_MAX 4096

/* Longest name a dedicated worker may have, without the NUL. */
#define WORKER_NAMELEN 31

/* The work itself.  Runs in a worker thread.  Reads wt_in, writes wt_out
 * and wt_status.  Must obey the rule above. */
typedef void (*WorkFn)(struct WorkTask* task);

/* What to do with the result.  Runs in the main thread, between two
 * passes of the event loop, so it may touch core state freely -- including
 * submitting another task.  It may not assume that the user who asked for
 * the work is still online: look them up again by nick. */
typedef void (*WorkDoneFn)(struct WorkTask* task);

/* Release a task's payload.  Runs in the main thread, on every path,
 * including when the task is cancelled without ever having been worked on.
 * Optional: NULL means worker_free() on wt_in and wt_out, which is right
 * for any payload allocated with worker_alloc(). */
typedef void (*WorkFreeFn)(struct WorkTask* task);

/* The body of a dedicated worker.  Runs in its own thread; returning ends
 * the thread.  A worker that means to stay alive loops until
 * worker_stopping() is true, and waits on worker_stop_fd() rather than
 * sleeping, so that worker_stop() does not have to wait out a sleep. */
typedef void (*WorkerMainFn)(struct Worker* worker, void* arg);

/* One unit of work, and its result.
 *
 * The submitter fills in the first group before calling worker_submit(),
 * the worker thread fills in the second, and the third belongs to the
 * core.  Ownership passes to the core on a successful worker_submit() or
 * worker_post(), and the core frees the task once wt_done has run.  A
 * submit that fails leaves the task with the caller, who is expected to
 * worker_task_free() it. */
struct WorkTask {
    /* --- set by the submitter, in the main thread --- */
    WorkFn wt_work;     /* The work.  Required, except for a post. */
    WorkDoneFn wt_done; /* The result.  May be NULL. */
    WorkFreeFn wt_free; /* Payload release.  May be NULL. */
    void* wt_arg;       /* Opaque to the core; the submitter's own. */

    void* wt_in;      /* Input, read by wt_work. */
    size_t wt_in_len; /* Length of wt_in. */

    /* --- set by wt_work, in a worker thread; read by wt_done --- */
    void* wt_out;      /* Output, read by wt_done. */
    size_t wt_out_len; /* Length of wt_out. */
    int wt_status;     /* Zero for success; the task's own meaning. */

    /* --- private to worker.c --- */
    struct WorkTask* wt_next; /* Next on whichever queue it is on. */
    struct Module_* wt_owner; /* Module that submitted it, or NULL. */
};

/*************************************************************************/

/* Lifecycle.  The core calls these; modules do not. */

/* Bring the worker subsystem up with `nthreads' pool threads and at most
 * `queue_max' outstanding tasks.  Called once from init(), after the
 * configuration has been read and before any module is loaded.  Returns
 * nonzero on success. */
extern int worker_init(int nthreads, int queue_max);

/* Apply a changed `workers' block (REHASH): resize the pool, change the
 * queue limit.  Shrinking waits for the threads it removes to finish what
 * they are doing. */
extern void worker_configure(int nthreads, int queue_max);

/* Stop every worker and release the subsystem.  Waits for work in flight;
 * results that arrive during the wait are discarded rather than delivered,
 * because the core they would run against is being taken apart. */
extern void worker_shutdown(void);

/* Deliver everything the workers have finished: run each finished task's
 * wt_done, then free it.  The main thread calls this when the wake-up pipe
 * is readable.  Returns the number of tasks delivered. */
extern unsigned int worker_drain(void);

/* Block the main thread for up to `ms' milliseconds waiting for a result,
 * then deliver whatever has arrived.  For the few places that must wait
 * for an answer before going on -- the last save before shutting down.
 * Returns the number of tasks delivered. */
extern unsigned int worker_wait(int ms);

/*************************************************************************/

/* Submitting work. */

/* Allocate a zeroed task.  `work' runs in a worker thread (required except
 * for a post); `done' runs in the main thread afterwards, or is NULL.
 * Returns a task the caller owns until worker_submit() accepts it, or NULL
 * if there was no memory. */
extern struct WorkTask* worker_task_new(WorkFn work, WorkDoneFn done);

/* Release a task the core does not own: only for a task worker_submit()
 * refused.  Runs wt_free (or the default payload release) first. */
extern void worker_task_free(struct WorkTask* task);

/* Hand a task to the pool, on behalf of the core.  Safe to call from a
 * worker thread as well as from the main thread.  Returns nonzero on
 * success; zero if the pool is not running or the queue is full, in which
 * case the task is still the caller's. */
extern int worker_submit(struct WorkTask* task);

/* The same, on behalf of module `mod' (THIS_MODULE): the task is dropped,
 * and a running one waited for, if the module is unloaded. */
extern int worker_submit_owned(struct Module_* mod, struct WorkTask* task);

/*************************************************************************/

/* Dedicated workers. */

/* Start a thread with a loop of its own.  `name' is for logs.  Returns the
 * worker, or NULL if the subsystem is not running or the thread could not
 * be created.  The core owns it; stop it with worker_stop(). */
extern struct Worker* worker_spawn(const char* name, WorkerMainFn fn,
                                   void* arg);

/* The same, owned by module `mod': stopped when the module is unloaded. */
extern struct Worker* worker_spawn_owned(struct Module_* mod, const char* name,
                                         WorkerMainFn fn, void* arg);

/* Ask a dedicated worker to stop, and wait for it.  Main thread only.  A
 * worker the core has already stopped is gone, and stopping it again does
 * nothing: the pointer is checked against the list of live workers before
 * it is read. */
extern void worker_stop(struct Worker* worker);

/* Stop `worker' if `mod' owns it.  Returns nonzero if it did. */
extern int worker_stop_owned(struct Module_* mod, struct Worker* worker);

/* Nonzero once somebody has asked this worker to stop.  Safe to call from
 * the worker thread. */
extern int worker_stopping(const struct Worker* worker);

/* A descriptor that becomes readable when the worker should stop.  Put it
 * in the worker's own poll() set so that a stop does not have to wait for a
 * timeout.  Never read from it. */
extern int worker_stop_fd(const struct Worker* worker);

/* Name a dedicated worker was given. */
extern const char* worker_name(const struct Worker* worker);

/* Hand a result to the main thread.  Called from a dedicated worker.  The
 * task's wt_work is never called; its wt_done runs in the main thread on
 * the next drain.  Returns nonzero on success; zero if the queue is full,
 * leaving the task with the caller. */
extern int worker_post(struct Worker* worker, struct WorkTask* task);

/*************************************************************************/

/* Things a worker thread may call. */

/* Allocate zeroed memory a worker thread may touch: the system allocator,
 * whatever MEMCHECKS says.  Freed with worker_free(). */
extern void* worker_alloc(size_t size);

/* Release memory from worker_alloc().  NULL is accepted. */
extern void worker_free(void* ptr);

/* Write a line to the log, from a worker thread.  The line is formatted
 * here and queued, and the main thread logs it on the next drain: it
 * appears slightly late, and is dropped rather than blocking if the queue
 * is full. */
extern void worker_log(const char* fmt, ...)
#ifdef __GNUC__
    __attribute__((format(printf, 1, 2)))
#endif
    ;

/*************************************************************************/

/* Instrumentation. */

/* Nonzero when the subsystem is running. */
extern int worker_enabled(void);
/* Threads currently in the pool. */
extern unsigned int worker_thread_count(void);
/* Dedicated workers currently running. */
extern unsigned int worker_dedicated_count(void);
/* Tasks submitted and not yet delivered: queued + running + undelivered. */
extern unsigned int worker_outstanding(void);
/* Tasks accepted, delivered, and refused for a full queue, ever. */
extern unsigned int worker_submitted(void);
extern unsigned int worker_completed(void);
extern unsigned int worker_rejected(void);

/*************************************************************************/

/* Drop everything `mod' owns so that it can be unloaded: stop its
 * dedicated workers, discard its queued tasks and results, and wait for
 * its running tasks (the only case in which the main thread blocks on a
 * worker).  Called by the module loader before the module is unmapped. */
extern void worker_cancel_module(struct Module_* mod);

/*************************************************************************/

#endif /* WORKER_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
