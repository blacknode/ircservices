/* POSIX poll(2) event engine.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "engine.h"
#include "services.h"

#if ENGINE_POLL /* only built where available (cmake/ServicesPlatform.cmake)  \
                 */

#include <poll.h>

/*************************************************************************/

/* Watched descriptors, packed at the front of pollfds[]; slot_of[fd] is
 * the index of `fd' in pollfds[], or -1. */
static struct pollfd* pollfds;
static int pollfds_used, pollfds_size;
static int* slot_of;
static int slot_of_size;

/*************************************************************************/

static short poll_mask(unsigned int interest)
{
    return ((interest & ENGINE_READ) ? POLLIN : 0) |
           ((interest & ENGINE_WRITE) ? POLLOUT : 0);
}

static int grow_slot_map(int fd)
{
    int newsize, i;
    int* p;

    if (fd < slot_of_size)
        return 0;
    newsize = fd + 64;
    p = realloc(slot_of, newsize * sizeof(*p));
    if (!p)
        return -1;
    for (i = slot_of_size; i < newsize; i++)
        p[i] = -1;
    slot_of = p;
    slot_of_size = newsize;
    return 0;
}

/*************************************************************************/

static int poll_engine_init(void)
{
    return 1;
}

static void poll_engine_shutdown(void)
{
    free(pollfds);
    free(slot_of);
    pollfds = NULL;
    slot_of = NULL;
    pollfds_used = pollfds_size = slot_of_size = 0;
}

static int poll_engine_add(int fd, unsigned int interest)
{
    if (grow_slot_map(fd) < 0)
        return -1;
    if (pollfds_used == pollfds_size) {
        int newsize = pollfds_size ? pollfds_size * 2 : 64;
        struct pollfd* p = realloc(pollfds, newsize * sizeof(*p));
        if (!p)
            return -1;
        pollfds = p;
        pollfds_size = newsize;
    }
    slot_of[fd] = pollfds_used;
    pollfds[pollfds_used].fd = fd;
    pollfds[pollfds_used].events = poll_mask(interest);
    pollfds[pollfds_used].revents = 0;
    pollfds_used++;
    return 0;
}

static int poll_engine_update(int fd, unsigned int interest)
{
    if (fd >= slot_of_size || slot_of[fd] < 0) {
        errno = EBADF;
        return -1;
    }
    pollfds[slot_of[fd]].events = poll_mask(interest);
    return 0;
}

static void poll_engine_remove(int fd)
{
    int slot, last;

    if (fd >= slot_of_size || (slot = slot_of[fd]) < 0)
        return;
    /* Fill the hole with the last entry to keep the array packed. */
    last = --pollfds_used;
    if (slot != last) {
        pollfds[slot] = pollfds[last];
        slot_of[pollfds[slot].fd] = slot;
    }
    slot_of[fd] = -1;
}

static int poll_engine_wait(int timeout_ms, EngineHandler handler)
{
    int i, count, reported = 0;

    count = poll(pollfds, pollfds_used, timeout_ms);
    if (count < 0)
        return errno == EINTR ? 0 : -1;

    /* Handlers may add or remove descriptors, which reorders pollfds[],
     * so walk it backwards and re-check each entry is still the one that
     * poll() filled in. */
    for (i = pollfds_used - 1; i >= 0 && reported < count; i--) {
        struct pollfd* pfd;
        unsigned int events = 0;
        int fd;

        if (i >= pollfds_used)
            continue;
        pfd = &pollfds[i];
        if (!pfd->revents)
            continue;
        fd = pfd->fd;
        if (pfd->revents & (POLLIN | POLLPRI))
            events |= ENGINE_READ;
        if (pfd->revents & POLLOUT)
            events |= ENGINE_WRITE;
        if (pfd->revents & (POLLERR | POLLHUP | POLLNVAL))
            events |= ENGINE_ERROR;
        pfd->revents = 0;
        reported++;
        handler(fd, events);
    }
    return reported;
}

/*************************************************************************/

const Engine engine_poll = {
    .name = "poll",
    .init = poll_engine_init,
    .shutdown = poll_engine_shutdown,
    .add = poll_engine_add,
    .update = poll_engine_update,
    .remove = poll_engine_remove,
    .wait = poll_engine_wait,
};

#endif /* ENGINE_POLL */
