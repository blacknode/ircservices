/* Socket event engine interface (core-private).
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * The design follows ircu's event engines (ircd/engine_*.c): an engine is
 * a table of operations over file descriptors, one backend per source file,
 * and the socket layer (sockets.c) never calls select()/poll()/epoll()
 * itself.  Every backend that the platform supports is compiled in (see
 * cmake/ServicesPlatform.cmake) and the first one that initializes, in the
 * order of engine_list[], is used:
 *
 *     engine_epoll.c    Linux epoll(7)                 O(ready)
 *     engine_kqueue.c   BSD / macOS kqueue(2)          O(ready)
 *     engine_poll.c     POSIX poll(2)                  O(registered)
 *     engine_select.c   select(2), always available    O(max fd)
 *
 * An engine only reports readiness; buffering, callbacks and connection
 * state belong to the socket layer.
 */

#ifndef ENGINE_H
#define ENGINE_H

#include "config.h" /* ENGINE_* availability */

/*************************************************************************/

/* Readiness flags, used both for interest (what the socket layer wants to
 * hear about) and for the events an engine reports. */
#define ENGINE_READ  0x01 /* Readable, or a listener has a client */
#define ENGINE_WRITE 0x02 /* Writable, or a connect() completed */
#define ENGINE_ERROR 0x04 /* Error or hang-up (reported only) */

/* Called by Engine.wait() once for each descriptor with pending events. */
typedef void (*EngineHandler)(int fd, unsigned int events);

typedef struct engine_ {
    const char* name;

    /* Prepare the engine; return nonzero on success.  An engine that
     * cannot initialize (e.g. epoll on an old kernel) is skipped. */
    int (*init)(void);
    /* Release everything init() acquired. */
    void (*shutdown)(void);

    /* Start watching `fd' for `interest' (ENGINE_READ|ENGINE_WRITE).
     * Return 0 on success, -1 on error (errno set). */
    int (*add)(int fd, unsigned int interest);
    /* Change the interest set of a watched descriptor. */
    int (*update)(int fd, unsigned int interest);
    /* Stop watching `fd'; must be called before the descriptor is closed.
     * Events already collected for `fd' but not yet delivered by wait()
     * are discarded, so a handler never sees a stale descriptor. */
    void (*remove)(int fd);

    /* Wait up to `timeout_ms' milliseconds (-1 = forever) and call
     * `handler' for each ready descriptor.  Return the number of
     * descriptors reported, 0 on timeout or signal, -1 on error. */
    int (*wait)(int timeout_ms, EngineHandler handler);
} Engine;

/*************************************************************************/

#if ENGINE_EPOLL
extern const Engine engine_epoll;
#endif
#if ENGINE_KQUEUE
extern const Engine engine_kqueue;
#endif
#if ENGINE_POLL
extern const Engine engine_poll;
#endif
extern const Engine engine_select;

/*************************************************************************/

#endif /* ENGINE_H */
