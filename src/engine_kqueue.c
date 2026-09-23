/* BSD kqueue(2) event engine.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "engine.h"
#include "services.h"

#if ENGINE_KQUEUE /* only built where available                               \
                     (cmake/ServicesPlatform.cmake) */

#include <sys/event.h>

/*************************************************************************/

#define KQUEUE_BATCH 128 /* Events collected per kevent() */

static int kqueue_fd = -1;

/* kqueue filters are registered one by one, so the current interest set
 * of every descriptor is remembered to know which filters to change. */
static unsigned char* interests;
static int interests_size;

static struct kevent batch[KQUEUE_BATCH];
static int batch_used;

/*************************************************************************/

static int remember_interest(int fd, unsigned int interest)
{
    if (fd >= interests_size) {
        int newsize = fd + 64;
        unsigned char* p = realloc(interests, newsize);
        if (!p)
            return -1;
        memset(p + interests_size, 0, newsize - interests_size);
        interests = p;
        interests_size = newsize;
    }
    interests[fd] = (unsigned char)interest;
    return 0;
}

/* Bring the filters of `fd' from `old' to `new' interest. */
static int kqueue_apply(int fd, unsigned int old, unsigned int new)
{
    struct kevent changes[2];
    int n = 0;

    if ((old ^ new) & ENGINE_READ) {
        EV_SET(&changes[n++], fd, EVFILT_READ,
               (new & ENGINE_READ) ? EV_ADD | EV_ENABLE : EV_DELETE, 0, 0,
               NULL);
    }
    if ((old ^ new) & ENGINE_WRITE) {
        EV_SET(&changes[n++], fd, EVFILT_WRITE,
               (new & ENGINE_WRITE) ? EV_ADD | EV_ENABLE : EV_DELETE, 0, 0,
               NULL);
    }
    if (n && kevent(kqueue_fd, changes, n, NULL, 0, NULL) < 0)
        return -1;
    return remember_interest(fd, new);
}

/*************************************************************************/

static int kqueue_engine_init(void)
{
    kqueue_fd = kqueue();
    return kqueue_fd >= 0;
}

static void kqueue_engine_shutdown(void)
{
    if (kqueue_fd >= 0)
        close(kqueue_fd);
    kqueue_fd = -1;
    free(interests);
    interests = NULL;
    interests_size = 0;
    batch_used = 0;
}

static int kqueue_engine_add(int fd, unsigned int interest)
{
    return kqueue_apply(fd, 0, interest);
}

static int kqueue_engine_update(int fd, unsigned int interest)
{
    unsigned int old = fd < interests_size ? interests[fd] : 0;
    return kqueue_apply(fd, old, interest);
}

static void kqueue_engine_remove(int fd)
{
    int i;

    if (fd < interests_size) {
        kqueue_apply(fd, interests[fd], 0);
        interests[fd] = 0;
    }
    for (i = 0; i < batch_used; i++) {
        if ((int)batch[i].ident == fd)
            batch[i].ident = (uintptr_t)-1;
    }
}

static int kqueue_engine_wait(int timeout_ms, EngineHandler handler)
{
    struct timespec ts, *tsp = NULL;
    int i, count;

    if (timeout_ms >= 0) {
        ts.tv_sec = timeout_ms / 1000;
        ts.tv_nsec = (timeout_ms % 1000) * 1000000L;
        tsp = &ts;
    }
    count = kevent(kqueue_fd, NULL, 0, batch, KQUEUE_BATCH, tsp);
    if (count < 0)
        return errno == EINTR ? 0 : -1;

    batch_used = count;
    for (i = 0; i < count; i++) {
        const struct kevent* ev = &batch[i];
        unsigned int events;

        if (ev->ident == (uintptr_t)-1)
            continue; /* removed by an earlier handler */
        events = (ev->filter == EVFILT_READ) ? ENGINE_READ : ENGINE_WRITE;
        if (ev->flags & (EV_EOF | EV_ERROR))
            events |= ENGINE_ERROR;
        handler((int)ev->ident, events);
    }
    batch_used = 0;
    return count;
}

/*************************************************************************/

const Engine engine_kqueue = {
    .name = "kqueue",
    .init = kqueue_engine_init,
    .shutdown = kqueue_engine_shutdown,
    .add = kqueue_engine_add,
    .update = kqueue_engine_update,
    .remove = kqueue_engine_remove,
    .wait = kqueue_engine_wait,
};

#endif /* ENGINE_KQUEUE */
