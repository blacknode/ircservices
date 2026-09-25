/* Worker threads and the queues that connect them to the main thread.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (ircd/worker.c).
 *
 * Three queues, all guarded by one mutex:
 *
 *   - wi_pending  tasks waiting for a pool thread to pick them up.
 *   - wi_running  tasks a pool thread is working on right now.  Kept as a
 *                 list rather than a counter because unloading a module
 *                 has to wait for exactly its own tasks.
 *   - wi_ready    tasks that are finished and waiting for the main thread.
 *
 * One mutex for all three is not a bottleneck: the critical sections are a
 * pointer swap each, and the interesting work happens outside them.  The
 * main thread never blocks on the mutex for longer than a list splice --
 * except when it is deliberately waiting, which is only ever on a module
 * unload or a shutdown.
 *
 * The main thread learns that wi_ready is non-empty through a self-pipe
 * watched by the socket layer (sock_watch_fd()).
 *
 * Functions here are labelled with the thread they run in.  The ones a
 * worker thread may call touch nothing but this file's own state and the
 * system allocator; see include/worker.h.
 */

/* The system allocator, whatever MEMCHECKS says: worker_alloc() is called
 * from worker threads, and the allocation tracker has no locking. */
#define NO_MEMREDEF

#include "services.h"
#include "modules.h"
#include "worker.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>

/* How long to wait between complaints while blocked on a stuck worker. */
#define WORKER_STALL_WARN 2

/* A thread in the pool. */
struct PoolThread {
    pthread_t pt_thread;   /* The thread itself. */
    unsigned int pt_index; /* Its slot; it exits when this is >= target. */
    int pt_live;           /* Nonzero between create and join. */
};

/* A thread with a loop of its own. */
struct Worker {
    struct Worker* w_next;           /* Next in wi_workers. */
    char w_name[WORKER_NAMELEN + 1]; /* For logs. */
    pthread_t w_thread;              /* The thread itself. */
    WorkerMainFn w_main;             /* Its body. */
    void* w_arg;                     /* Handed to w_main. */
    struct Module_* w_owner;         /* Module that spawned it, or NULL. */
    int w_stop;                      /* Stop requested.  Under the mutex. */
    int w_stop_r;                    /* Read end of the stop pipe. */
    int w_stop_w;                    /* Write end of the stop pipe. */
    int w_started;                   /* Thread created (vs. still held). */
};

/* Everything the worker subsystem owns. */
static struct {
    int wi_up;                 /* Queues, pipe and threads exist. */
    int wi_configured;         /* worker_init() has run. */
    unsigned int wi_queue_max; /* Most outstanding tasks. */

    pthread_mutex_t wi_mutex; /* Guards everything below. */
    pthread_cond_t wi_work;   /* Pool threads wait for a task. */
    pthread_cond_t wi_idle;   /* Main thread waits for tasks to end. */

    struct WorkTask* wi_pending;       /* Head of the pending queue. */
    struct WorkTask** wi_pending_tail; /* Where the next one goes. */
    unsigned int wi_pending_n;         /* Length of the pending queue. */

    struct WorkTask* wi_running; /* Tasks in a worker thread now. */
    unsigned int wi_running_n;   /* How many of those. */

    struct WorkTask* wi_ready;       /* Head of the reply queue. */
    struct WorkTask** wi_ready_tail; /* Where the next one goes. */
    unsigned int wi_ready_n;         /* Length of the reply queue. */

    unsigned int wi_outstanding; /* pending + running + ready. */

    struct PoolThread wi_pool[WORKER_MAX_THREADS]; /* The pool. */
    unsigned int wi_nthreads;                      /* Threads alive. */
    unsigned int wi_target;                        /* Threads wanted. */

    struct Worker* wi_workers; /* Dedicated workers. */
    unsigned int wi_nworkers;  /* How many of them. */

    int wi_wake_r;     /* Read end of the wake-up pipe. */
    int wi_wake_w;     /* Write end of the wake-up pipe. */
    int wi_wake_armed; /* A byte is already in the pipe. */

    unsigned int wi_submitted; /* Tasks accepted, ever. */
    unsigned int wi_completed; /* Tasks delivered, ever. */
    unsigned int wi_rejected;  /* Submissions refused, ever. */
} wInfo;

static void* worker_pool_main(void* arg);
static void* worker_dedicated_main(void* arg);

/* One-time setup of the mutex, the condition variables and the queue
 * tails.  Through pthread_once() so that any entry point, from any thread,
 * finds them ready. */
static pthread_once_t wi_once = PTHREAD_ONCE_INIT;

static void worker_once(void)
{
    pthread_mutex_init(&wInfo.wi_mutex, NULL);
    pthread_cond_init(&wInfo.wi_work, NULL);
    pthread_cond_init(&wInfo.wi_idle, NULL);

    wInfo.wi_pending_tail = &wInfo.wi_pending;
    wInfo.wi_ready_tail = &wInfo.wi_ready;
    wInfo.wi_wake_r = wInfo.wi_wake_w = -1;
    wInfo.wi_queue_max = WORKER_DEFAULT_QUEUE_MAX;
}

#define worker_prepare() pthread_once(&wi_once, worker_once)

/* Create a thread with every signal blocked.  Main thread.
 *
 * A thread inherits the creator's signal mask, and the signal handlers of
 * Services siglongjmp() into the main loop: a signal delivered to a worker
 * thread would jump onto the main thread's stack from the wrong thread.
 * Blocked here, the kernel always picks the main thread. */
static int create_thread(pthread_t* thread, void* (*fn)(void*), void* arg)
{
    sigset_t all, old;
    int res;

    sigfillset(&all);
    pthread_sigmask(SIG_SETMASK, &all, &old);
    res = pthread_create(thread, NULL, fn, arg);
    pthread_sigmask(SIG_SETMASK, &old, NULL);
    return res;
}

/*************************************************************************/
/******************************** Memory *********************************/
/*************************************************************************/

/* Allocate zeroed memory.  Any thread. */
void* worker_alloc(size_t size)
{
    return calloc(1, size ? size : 1);
}

/* Release memory from worker_alloc().  Any thread. */
void worker_free(void* ptr)
{
    free(ptr);
}

/*************************************************************************/
/***************** Queue plumbing (all with wi_mutex held) ****************/
/*************************************************************************/

/* Append `task' to a singly-linked queue. */
static void queue_push(struct WorkTask*** tail_p, struct WorkTask* task)
{
    task->wt_next = NULL;
    **tail_p = task;
    *tail_p = &task->wt_next;
}

/* Take the head off the pending queue, or NULL. */
static struct WorkTask* pending_pop(void)
{
    struct WorkTask* task = wInfo.wi_pending;

    if (!task)
        return NULL;
    wInfo.wi_pending = task->wt_next;
    if (!wInfo.wi_pending)
        wInfo.wi_pending_tail = &wInfo.wi_pending;
    wInfo.wi_pending_n--;
    task->wt_next = NULL;
    return task;
}

/* Make the main thread's event engine notice the reply queue.  Coalesced:
 * one byte is enough however many tasks are waiting.  The byte is cleared
 * by the drain, not here. */
static void wake_main(void)
{
    unsigned char c = 0;

    if (wInfo.wi_wake_armed || wInfo.wi_wake_w < 0)
        return;
    /* A short write means the pipe is full, which means the main thread
     * has a wake-up coming already. */
    if (write(wInfo.wi_wake_w, &c, 1) == 1)
        wInfo.wi_wake_armed = 1;
}

/* Put a finished task on the reply queue and wake the main thread. */
static void ready_push(struct WorkTask* task)
{
    queue_push(&wInfo.wi_ready_tail, task);
    wInfo.wi_ready_n++;
    wake_main();
}

/* Unlink `task' from the running list. */
static void running_unlink(struct WorkTask* task)
{
    struct WorkTask** p;

    for (p = &wInfo.wi_running; *p; p = &(*p)->wt_next) {
        if (*p == task) {
            *p = task->wt_next;
            task->wt_next = NULL;
            wInfo.wi_running_n--;
            return;
        }
    }
}

/* Count a task in, if there is room. */
static int task_accept(void)
{
    if (wInfo.wi_outstanding >= wInfo.wi_queue_max) {
        wInfo.wi_rejected++;
        return 0;
    }
    wInfo.wi_outstanding++;
    wInfo.wi_submitted++;
    return 1;
}

/*************************************************************************/
/********************************* Tasks *********************************/
/*************************************************************************/

/* Allocate a zeroed task.  Any thread. */
struct WorkTask* worker_task_new(WorkFn work, WorkDoneFn done)
{
    struct WorkTask* task = worker_alloc(sizeof(*task));

    if (!task)
        return NULL;
    task->wt_work = work;
    task->wt_done = done;
    return task;
}

/* Release a task's payload and then the task.  Main thread.  wt_free
 * replaces the default release entirely. */
static void task_destroy(struct WorkTask* task)
{
    if (!task)
        return;
    if (task->wt_free) {
        (*task->wt_free)(task);
    }
    else {
        worker_free(task->wt_in);
        worker_free(task->wt_out);
    }
    worker_free(task);
}

/* Release a task the core does not own.  Main thread. */
void worker_task_free(struct WorkTask* task)
{
    task_destroy(task);
}

/* Hand a task to the pool, recording who owns it.  Any thread. */
int worker_submit_owned(struct Module_* mod, struct WorkTask* task)
{
    int accepted;

    if (!task || !task->wt_work)
        return 0;
    worker_prepare();

    pthread_mutex_lock(&wInfo.wi_mutex);
    /* Read inside the lock: a worker thread may be submitting at the same
     * moment the main thread is shutting the pool down.  Before
     * worker_init() -- a module's `init', before the fork -- the
     * task is queued, and the pool picks it up when it starts. */
    accepted = (wInfo.wi_up ? wInfo.wi_nthreads > 0 : !wInfo.wi_configured) &&
               task_accept();
    if (accepted) {
        task->wt_owner = mod;
        queue_push(&wInfo.wi_pending_tail, task);
        wInfo.wi_pending_n++;
        pthread_cond_signal(&wInfo.wi_work);
    }
    pthread_mutex_unlock(&wInfo.wi_mutex);
    return accepted;
}

int worker_submit(struct WorkTask* task)
{
    return worker_submit_owned(NULL, task);
}

/* Hand a result to the main thread.  Worker thread. */
int worker_post(struct Worker* worker, struct WorkTask* task)
{
    int accepted;

    if (!worker || !task)
        return 0;
    worker_prepare();

    pthread_mutex_lock(&wInfo.wi_mutex);
    accepted = wInfo.wi_up && task_accept();
    if (accepted) {
        task->wt_owner = worker->w_owner;
        ready_push(task);
    }
    pthread_mutex_unlock(&wInfo.wi_mutex);
    return accepted;
}

/*************************************************************************/
/*********************** Logging from a worker thread ********************/
/*************************************************************************/

/* Write the queued line to the log.  Main thread. */
static void worker_log_done(struct WorkTask* task)
{
    log("%s", (const char*)task->wt_in);
}

/* Queue a log line from a worker thread.  log() writes to shared state and
 * formats into shared buffers, so it cannot be called from there; the line
 * is formatted with the system's vsnprintf(), which is thread-safe, and
 * handed to the main thread like any other result. */
void worker_log(const char* fmt, ...)
{
    char buf[512];
    struct WorkTask* task;
    va_list args;
    size_t len;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    worker_prepare();
    /* No subsystem means no worker thread (none exist before worker_init()
     * or after worker_shutdown()), so the caller is the main thread and
     * may log directly. */
    pthread_mutex_lock(&wInfo.wi_mutex);
    if (!wInfo.wi_up) {
        pthread_mutex_unlock(&wInfo.wi_mutex);
        log("%s", buf);
        return;
    }
    pthread_mutex_unlock(&wInfo.wi_mutex);

    if (!(task = worker_task_new(NULL, worker_log_done)))
        return;
    len = strlen(buf) + 1;
    if (!(task->wt_in = worker_alloc(len))) {
        worker_free(task);
        return;
    }
    memcpy(task->wt_in, buf, len);
    task->wt_in_len = len;

    pthread_mutex_lock(&wInfo.wi_mutex);
    if (wInfo.wi_up && task_accept()) {
        ready_push(task);
        task = NULL;
    }
    pthread_mutex_unlock(&wInfo.wi_mutex);

    /* Dropped rather than blocking: a worker in a loop must not be able to
     * push Services into swap through the log. */
    if (task)
        task_destroy(task);
}

/*************************************************************************/
/******************************* The pool ********************************/
/*************************************************************************/

/* Body of a pool thread.  Worker thread.  Waits for a task, runs it, puts
 * it on the reply queue, repeats.  Exits when its slot is above the target
 * size, which is how the pool shrinks. */
static void* worker_pool_main(void* arg)
{
    struct PoolThread* self = arg;
    struct WorkTask* task;

    pthread_mutex_lock(&wInfo.wi_mutex);
    for (;;) {
        while (!wInfo.wi_pending && self->pt_index < wInfo.wi_target)
            pthread_cond_wait(&wInfo.wi_work, &wInfo.wi_mutex);
        if (self->pt_index >= wInfo.wi_target)
            break;
        if (!(task = pending_pop()))
            continue;

        /* On the running list before the mutex is dropped, so that a
         * module unload that starts now sees this task and waits for it. */
        task->wt_next = wInfo.wi_running;
        wInfo.wi_running = task;
        wInfo.wi_running_n++;
        pthread_mutex_unlock(&wInfo.wi_mutex);

        (*task->wt_work)(task);

        pthread_mutex_lock(&wInfo.wi_mutex);
        running_unlink(task);
        ready_push(task);
        /* Somebody may be waiting for exactly this task to be over. */
        pthread_cond_broadcast(&wInfo.wi_idle);
    }
    pthread_mutex_unlock(&wInfo.wi_mutex);
    return NULL;
}

/* Grow or shrink the pool.  Main thread.  Shrinking lowers the target,
 * wakes everybody so the surplus threads notice, and joins them -- which
 * waits for whatever they were in the middle of. */
static void pool_resize(unsigned int target)
{
    unsigned int i, old;

    if (target > WORKER_MAX_THREADS)
        target = WORKER_MAX_THREADS;

    pthread_mutex_lock(&wInfo.wi_mutex);
    old = wInfo.wi_nthreads;
    wInfo.wi_target = target;
    pthread_cond_broadcast(&wInfo.wi_work);
    pthread_mutex_unlock(&wInfo.wi_mutex);

    for (i = target; i < old; i++) {
        if (!wInfo.wi_pool[i].pt_live)
            continue;
        pthread_join(wInfo.wi_pool[i].pt_thread, NULL);
        wInfo.wi_pool[i].pt_live = 0;
    }

    for (i = old; i < target; i++) {
        wInfo.wi_pool[i].pt_index = i;
        if (create_thread(&wInfo.wi_pool[i].pt_thread, worker_pool_main,
                          &wInfo.wi_pool[i]) != 0) {
            log("worker: could not create pool thread %u: %s", i,
                strerror(errno));
            /* Settle for the threads that did start. */
            pthread_mutex_lock(&wInfo.wi_mutex);
            wInfo.wi_target = i;
            pthread_mutex_unlock(&wInfo.wi_mutex);
            target = i;
            break;
        }
        wInfo.wi_pool[i].pt_live = 1;
    }

    pthread_mutex_lock(&wInfo.wi_mutex);
    wInfo.wi_nthreads = target;
    pthread_mutex_unlock(&wInfo.wi_mutex);
}

/*************************************************************************/
/*************************** Dedicated workers ***************************/
/*************************************************************************/

/* Trampoline into the worker's own loop.  Worker thread. */
static void* worker_dedicated_main(void* arg)
{
    struct Worker* worker = arg;

    (*worker->w_main)(worker, worker->w_arg);
    return NULL;
}

int worker_stopping(const struct Worker* worker)
{
    int stop;

    pthread_mutex_lock(&wInfo.wi_mutex);
    stop = worker->w_stop;
    pthread_mutex_unlock(&wInfo.wi_mutex);
    return stop;
}

int worker_stop_fd(const struct Worker* worker)
{
    return worker->w_stop_r;
}

const char* worker_name(const struct Worker* worker)
{
    return worker->w_name;
}

/* Start a thread with a loop of its own.  Main thread.
 *
 * Services load their modules before they fork into the background, and a
 * thread does not survive a fork: the subsystem only comes up afterwards
 * (worker_init()).  A module that asks for a worker from its `init' --
 * directly, or by sending the first query to a connection pool -- gets one
 * all the same: the worker is recorded here and its thread started by
 * worker_init(), so that the caller does not have to know about the
 * fork. */
struct Worker* worker_spawn_owned(struct Module_* mod, const char* name,
                                  WorkerMainFn fn, void* arg)
{
    struct Worker* worker;
    int p[2];

    worker_prepare();
    /* Before worker_init(): held.  After worker_shutdown(): refused. */
    if (!fn || (!wInfo.wi_up && wInfo.wi_configured))
        return NULL;
    if (!(worker = worker_alloc(sizeof(*worker))))
        return NULL;
    if (pipe(p) < 0) {
        log("worker: no stop pipe for %s: %s", name ? name : "?",
            strerror(errno));
        worker_free(worker);
        return NULL;
    }
    worker->w_stop_r = p[0];
    worker->w_stop_w = p[1];
    worker->w_main = fn;
    worker->w_arg = arg;
    worker->w_owner = mod;
    snprintf(worker->w_name, sizeof(worker->w_name), "%s",
             name ? name : "worker");

    if (wInfo.wi_up) {
        if (create_thread(&worker->w_thread, worker_dedicated_main, worker)) {
            log("worker: could not start %s: %s", worker->w_name,
                strerror(errno));
            close(worker->w_stop_r);
            close(worker->w_stop_w);
            worker_free(worker);
            return NULL;
        }
        worker->w_started = 1;
    }
    worker->w_next = wInfo.wi_workers;
    wInfo.wi_workers = worker;
    wInfo.wi_nworkers++;
    return worker;
}

struct Worker* worker_spawn(const char* name, WorkerMainFn fn, void* arg)
{
    return worker_spawn_owned(NULL, name, fn, arg);
}

/* Start the workers that were spawned before the subsystem came up. */
static void worker_start_pending(void)
{
    struct Worker **p = &wInfo.wi_workers, *worker;

    while ((worker = *p) != NULL) {
        if (worker->w_started) {
            p = &worker->w_next;
            continue;
        }
        if (create_thread(&worker->w_thread, worker_dedicated_main, worker)) {
            log("worker: could not start %s: %s", worker->w_name,
                strerror(errno));
            *p = worker->w_next;
            wInfo.wi_nworkers--;
            close(worker->w_stop_r);
            close(worker->w_stop_w);
            worker_free(worker);
            continue;
        }
        worker->w_started = 1;
        p = &worker->w_next;
    }
}

/* Nonzero if `worker' is on the list of dedicated workers.  Compares
 * addresses and never looks inside, because the pointer may be to memory
 * that has been freed already. */
static int worker_live(const struct Worker* worker)
{
    const struct Worker* w;

    for (w = wInfo.wi_workers; w; w = w->w_next) {
        if (w == worker)
            return 1;
    }
    return 0;
}

/* Ask a dedicated worker to stop, and wait for it.  Main thread. */
void worker_stop(struct Worker* worker)
{
    struct Worker** p;
    unsigned char c = 0;

    worker_prepare();
    if (!worker || !worker_live(worker))
        return;

    pthread_mutex_lock(&wInfo.wi_mutex);
    worker->w_stop = 1;
    pthread_mutex_unlock(&wInfo.wi_mutex);

    /* Both halves matter: the flag for a worker that polls it, the byte
     * for one parked in poll() on the descriptor. */
    if (write(worker->w_stop_w, &c, 1) != 1)
        log_debug(1, "worker: stop pipe write for %s failed", worker->w_name);
    if (worker->w_started)
        pthread_join(worker->w_thread, NULL);

    for (p = &wInfo.wi_workers; *p; p = &(*p)->w_next) {
        if (*p == worker) {
            *p = worker->w_next;
            wInfo.wi_nworkers--;
            break;
        }
    }
    close(worker->w_stop_r);
    close(worker->w_stop_w);
    worker_free(worker);
}

int worker_stop_owned(struct Module_* mod, struct Worker* worker)
{
    worker_prepare();
    if (!worker || !worker_live(worker) || worker->w_owner != mod)
        return 0;
    worker_stop(worker);
    return 1;
}

/*************************************************************************/
/************************** Delivering results ***************************/
/*************************************************************************/

/* Deliver everything the workers have finished.  Main thread.  The reply
 * queue is detached under the mutex and walked without it, because a
 * wt_done may submit more work. */
unsigned int worker_drain(void)
{
    struct WorkTask *list, *task;
    unsigned int delivered = 0;

    if (!wInfo.wi_up)
        return 0;

    pthread_mutex_lock(&wInfo.wi_mutex);
    list = wInfo.wi_ready;
    wInfo.wi_ready = NULL;
    wInfo.wi_ready_tail = &wInfo.wi_ready;
    wInfo.wi_ready_n = 0;
    pthread_mutex_unlock(&wInfo.wi_mutex);

    while ((task = list) != NULL) {
        list = task->wt_next;
        task->wt_next = NULL;
        if (task->wt_done)
            (*task->wt_done)(task);
        task_destroy(task);
        delivered++;
    }

    if (delivered) {
        pthread_mutex_lock(&wInfo.wi_mutex);
        wInfo.wi_outstanding -= delivered;
        wInfo.wi_completed += delivered;
        pthread_mutex_unlock(&wInfo.wi_mutex);
    }
    return delivered;
}

/* Empty the wake-up pipe and clear the armed flag.  Main thread.  The
 * pipe is emptied before the flag is cleared, so that a byte written
 * between the two is not lost. */
static void wake_clear(void)
{
    unsigned char buf[64];

    while (read(wInfo.wi_wake_r, buf, sizeof(buf)) == (ssize_t)sizeof(buf))
        ;
    pthread_mutex_lock(&wInfo.wi_mutex);
    wInfo.wi_wake_armed = 0;
    pthread_mutex_unlock(&wInfo.wi_mutex);
}

/* Wake-up pipe became readable.  Main thread, from check_sockets(). */
static void worker_wake_callback(int fd, void* arg_unused)
{
    wake_clear();
    worker_drain();
}

unsigned int worker_wait(int ms)
{
    struct pollfd pfd;

    if (!wInfo.wi_up)
        return 0;
    pfd.fd = wInfo.wi_wake_r;
    pfd.events = POLLIN;
    pfd.revents = 0;
    if (poll(&pfd, 1, ms < 0 ? 0 : ms) > 0)
        wake_clear();
    return worker_drain();
}

/*************************************************************************/
/********** Ownership: keeping a module's code alive while it runs *******/
/*************************************************************************/

/* Unlink every task `mod' owns from a queue, onto `dropped'.  Mutex held.
 * Returns how many were removed. */
static unsigned int queue_filter_owner(struct WorkTask** head_p,
                                       struct WorkTask*** tail_p,
                                       unsigned int* count_p,
                                       const struct Module_* mod,
                                       struct WorkTask** dropped)
{
    struct WorkTask** p = head_p;
    struct WorkTask* task;
    unsigned int removed = 0;

    while ((task = *p) != NULL) {
        if (task->wt_owner != mod) {
            p = &task->wt_next;
            continue;
        }
        if (!(*p = task->wt_next))
            *tail_p = p;
        (*count_p)--;
        wInfo.wi_outstanding--;
        task->wt_next = *dropped;
        *dropped = task;
        removed++;
    }
    return removed;
}

/* Nonzero if a task of `mod' is in a worker right now.  Mutex held. */
static int module_has_running(const struct Module_* mod)
{
    const struct WorkTask* task;

    for (task = wInfo.wi_running; task; task = task->wt_next) {
        if (task->wt_owner == mod)
            return 1;
    }
    return 0;
}

/* Drop everything a module owns, so it can be unloaded.  Main thread.
 *
 * A task of the module's still waiting to run is taken off the queue and
 * thrown away.  A task that a worker is executing right now can only be
 * waited for, because the alternative is dlclose() on the function on a
 * live stack.  So the main thread blocks here, and that is logged so an
 * operator can see why Services went quiet.  Results are discarded rather
 * than delivered: wt_done belongs to the module too. */
void worker_cancel_module(struct Module_* mod)
{
    struct WorkTask *dropped = NULL, *task;
    struct Worker *worker, *next;
    unsigned int cancelled = 0;
    time_t complained = 0;

    worker_prepare();
    if (!mod)
        return;

    /* Its threads first, and joined before anything else happens: a
     * dedicated worker still running could post a result whose wt_done is
     * this module's code. */
    for (worker = wInfo.wi_workers; worker; worker = next) {
        next = worker->w_next;
        if (worker->w_owner == mod)
            worker_stop(worker);
    }
    if (!wInfo.wi_up)
        return;

    pthread_mutex_lock(&wInfo.wi_mutex);
    cancelled += queue_filter_owner(&wInfo.wi_pending, &wInfo.wi_pending_tail,
                                    &wInfo.wi_pending_n, mod, &dropped);
    while (module_has_running(mod)) {
        struct timespec deadline;

        if (!complained) {
            complained = time(NULL);
            log("worker: waiting for work in flight before unloading"
                " module `%s'",
                module_name((const Module*)mod));
        }
        deadline.tv_sec = time(NULL) + WORKER_STALL_WARN;
        deadline.tv_nsec = 0;
        if (pthread_cond_timedwait(&wInfo.wi_idle, &wInfo.wi_mutex,
                                   &deadline) == ETIMEDOUT &&
            module_has_running(mod))
            log("worker: still waiting after %ld seconds; Services are"
                " blocked until a worker returns",
                (long)(time(NULL) - complained));
    }
    /* Pending again: a task that was running may have submitted more. */
    cancelled += queue_filter_owner(&wInfo.wi_pending, &wInfo.wi_pending_tail,
                                    &wInfo.wi_pending_n, mod, &dropped);
    cancelled += queue_filter_owner(&wInfo.wi_ready, &wInfo.wi_ready_tail,
                                    &wInfo.wi_ready_n, mod, &dropped);
    pthread_mutex_unlock(&wInfo.wi_mutex);

    /* Outside the mutex: wt_free is the module's code and may do anything.
     * It still runs, because the module is still mapped. */
    while ((task = dropped) != NULL) {
        dropped = task->wt_next;
        task->wt_next = NULL;
        task_destroy(task);
    }
    if (cancelled)
        log("worker: discarded %u task%s belonging to module `%s'", cancelled,
            cancelled == 1 ? "" : "s", module_name((const Module*)mod));
}

/*************************************************************************/
/******************************* Lifecycle *******************************/
/*************************************************************************/

int worker_init(int nthreads, int queue_max)
{
    int p[2];

    worker_prepare();
    if (wInfo.wi_up) {
        worker_configure(nthreads, queue_max);
        return 1;
    }
    wInfo.wi_configured = 1;
    if (pipe(p) < 0) {
        log("worker: could not open wake pipe: %s", strerror(errno));
        return 0;
    }
    /* Non-blocking on both ends: the write happens under the mutex from a
     * worker thread, and the drain reads until the pipe is empty. */
    fcntl(p[0], F_SETFL, fcntl(p[0], F_GETFL) | O_NONBLOCK);
    fcntl(p[1], F_SETFL, fcntl(p[1], F_GETFL) | O_NONBLOCK);
    if (sock_watch_fd(p[0], worker_wake_callback, NULL) < 0) {
        log_perror("worker: could not watch the wake pipe");
        close(p[0]);
        close(p[1]);
        return 0;
    }
    wInfo.wi_wake_r = p[0];
    wInfo.wi_wake_w = p[1];

    pthread_mutex_lock(&wInfo.wi_mutex);
    wInfo.wi_queue_max = queue_max > 0 ? queue_max : WORKER_DEFAULT_QUEUE_MAX;
    wInfo.wi_up = 1;
    pthread_mutex_unlock(&wInfo.wi_mutex);

    pool_resize(nthreads > 0 ? (unsigned int)nthreads : 1);
    worker_start_pending();
    log_debug(1, "worker: pool running with %u thread%s", wInfo.wi_nthreads,
              wInfo.wi_nthreads == 1 ? "" : "s");
    return wInfo.wi_nthreads > 0;
}

void worker_configure(int nthreads, int queue_max)
{
    if (!wInfo.wi_up)
        return;
    if (nthreads < 1)
        nthreads = 1;
    pthread_mutex_lock(&wInfo.wi_mutex);
    wInfo.wi_queue_max = queue_max > 0 ? queue_max : WORKER_DEFAULT_QUEUE_MAX;
    pthread_mutex_unlock(&wInfo.wi_mutex);
    if ((unsigned int)nthreads != wInfo.wi_nthreads) {
        pool_resize((unsigned int)nthreads);
        log("worker: pool resized to %u thread%s", wInfo.wi_nthreads,
            wInfo.wi_nthreads == 1 ? "" : "s");
    }
}

void worker_shutdown(void)
{
    struct WorkTask* task;

    worker_prepare();
    /* Dedicated workers first -- including any still held from before
     * worker_init(), which have a stop pipe but no thread. */
    while (wInfo.wi_workers)
        worker_stop(wInfo.wi_workers);
    wInfo.wi_configured = 1;
    if (!wInfo.wi_up) {
        /* Tasks queued before a worker_init() that never came. */
        while ((task = pending_pop()) != NULL) {
            wInfo.wi_outstanding--;
            task_destroy(task);
        }
        return;
    }
    pool_resize(0);

    /* Whatever came back is dropped: the core it would run against is
     * already being taken apart. */
    pthread_mutex_lock(&wInfo.wi_mutex);
    task = wInfo.wi_ready;
    wInfo.wi_ready = NULL;
    wInfo.wi_ready_tail = &wInfo.wi_ready;
    wInfo.wi_ready_n = 0;
    while (wInfo.wi_pending) {
        struct WorkTask* pending = pending_pop();
        pending->wt_next = task;
        task = pending;
    }
    wInfo.wi_outstanding = 0;
    wInfo.wi_up = 0;
    pthread_mutex_unlock(&wInfo.wi_mutex);

    while (task) {
        struct WorkTask* next = task->wt_next;
        task->wt_next = NULL;
        task_destroy(task);
        task = next;
    }

    sock_unwatch_fd(wInfo.wi_wake_r);
    close(wInfo.wi_wake_r);
    close(wInfo.wi_wake_w);
    wInfo.wi_wake_r = wInfo.wi_wake_w = -1;
    wInfo.wi_wake_armed = 0;
}

/*************************************************************************/
/**************************** Instrumentation ****************************/
/*************************************************************************/

/* Read one counter under the lock: they are written by worker threads. */
static unsigned int worker_read(const unsigned int* field)
{
    unsigned int value;

    worker_prepare();
    pthread_mutex_lock(&wInfo.wi_mutex);
    value = *field;
    pthread_mutex_unlock(&wInfo.wi_mutex);
    return value;
}

int worker_enabled(void)
{
    int up;

    worker_prepare();
    pthread_mutex_lock(&wInfo.wi_mutex);
    up = wInfo.wi_up && wInfo.wi_nthreads > 0;
    pthread_mutex_unlock(&wInfo.wi_mutex);
    return up;
}

unsigned int worker_thread_count(void)
{
    return worker_read(&wInfo.wi_nthreads);
}

unsigned int worker_dedicated_count(void)
{
    return wInfo.wi_nworkers;
}

unsigned int worker_outstanding(void)
{
    return worker_read(&wInfo.wi_outstanding);
}

unsigned int worker_submitted(void)
{
    return worker_read(&wInfo.wi_submitted);
}

unsigned int worker_completed(void)
{
    return worker_read(&wInfo.wi_completed);
}

unsigned int worker_rejected(void)
{
    return worker_read(&wInfo.wi_rejected);
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
