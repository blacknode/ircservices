/* Linux epoll(7) event engine.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "engine.h"
#include "services.h"

#if ENGINE_EPOLL /* only built where available (cmake/ServicesPlatform.cmake) \
                  */

#include <sys/epoll.h>

/*************************************************************************/

#define EPOLL_BATCH 128 /* Events collected per epoll_wait() */

static int epoll_fd = -1;

/* Events of the current wait() call; remove() blanks out entries for a
 * descriptor that goes away before its events are delivered. */
static struct epoll_event batch[EPOLL_BATCH];
static int batch_used;

/*************************************************************************/

static uint32 epoll_mask(unsigned int interest)
{
    return ((interest & ENGINE_READ) ? EPOLLIN : 0) |
           ((interest & ENGINE_WRITE) ? EPOLLOUT : 0);
}

static int epoll_control(int op, int fd, unsigned int interest)
{
    struct epoll_event ev;

    memset(&ev, 0, sizeof(ev));
    ev.events = epoll_mask(interest);
    ev.data.fd = fd;
    return epoll_ctl(epoll_fd, op, fd, &ev);
}

/*************************************************************************/

static int epoll_engine_init(void)
{
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    return epoll_fd >= 0;
}

static void epoll_engine_shutdown(void)
{
    if (epoll_fd >= 0)
        close(epoll_fd);
    epoll_fd = -1;
    batch_used = 0;
}

static int epoll_engine_add(int fd, unsigned int interest)
{
    return epoll_control(EPOLL_CTL_ADD, fd, interest);
}

static int epoll_engine_update(int fd, unsigned int interest)
{
    return epoll_control(EPOLL_CTL_MOD, fd, interest);
}

static void epoll_engine_remove(int fd)
{
    int i;

    epoll_control(EPOLL_CTL_DEL, fd, 0);
    for (i = 0; i < batch_used; i++) {
        if (batch[i].data.fd == fd)
            batch[i].data.fd = -1;
    }
}

static int epoll_engine_wait(int timeout_ms, EngineHandler handler)
{
    int i, count;

    count = epoll_wait(epoll_fd, batch, EPOLL_BATCH, timeout_ms);
    if (count < 0)
        return errno == EINTR ? 0 : -1;

    batch_used = count;
    for (i = 0; i < count; i++) {
        const struct epoll_event* ev = &batch[i];
        unsigned int events = 0;

        if (ev->data.fd < 0)
            continue; /* removed by an earlier handler */
        if (ev->events & (EPOLLIN | EPOLLPRI))
            events |= ENGINE_READ;
        if (ev->events & EPOLLOUT)
            events |= ENGINE_WRITE;
        if (ev->events & (EPOLLERR | EPOLLHUP))
            events |= ENGINE_ERROR;
        handler(ev->data.fd, events);
    }
    batch_used = 0;
    return count;
}

/*************************************************************************/

const Engine engine_epoll = {
    .name = "epoll",
    .init = epoll_engine_init,
    .shutdown = epoll_engine_shutdown,
    .add = epoll_engine_add,
    .update = epoll_engine_update,
    .remove = epoll_engine_remove,
    .wait = epoll_engine_wait,
};

#endif /* ENGINE_EPOLL */
