/* select(2) event engine: the portable fallback.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "engine.h"
#include "services.h"

#include <sys/select.h>

/*************************************************************************/

static fd_set read_set, write_set;
static int max_fd = -1; /* Highest descriptor watched */

/*************************************************************************/

static int select_engine_init(void)
{
    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
    max_fd = -1;
    return 1;
}

static void select_engine_shutdown(void)
{
    select_engine_init();
}

static int select_engine_update(int fd, unsigned int interest)
{
    if (fd < 0 || fd >= FD_SETSIZE) {
        errno = EMFILE;
        return -1;
    }
    if (interest & ENGINE_READ)
        FD_SET(fd, &read_set);
    else
        FD_CLR(fd, &read_set);
    if (interest & ENGINE_WRITE)
        FD_SET(fd, &write_set);
    else
        FD_CLR(fd, &write_set);
    if (fd > max_fd)
        max_fd = fd;
    return 0;
}

static int select_engine_add(int fd, unsigned int interest)
{
    return select_engine_update(fd, interest);
}

static void select_engine_remove(int fd)
{
    if (fd < 0 || fd >= FD_SETSIZE)
        return;
    FD_CLR(fd, &read_set);
    FD_CLR(fd, &write_set);
    while (max_fd >= 0 && !FD_ISSET(max_fd, &read_set) &&
           !FD_ISSET(max_fd, &write_set))
        max_fd--;
}

static int select_engine_wait(int timeout_ms, EngineHandler handler)
{
    fd_set rfds = read_set, wfds = write_set;
    struct timeval tv, *tvp = NULL;
    int fd, count, reported = 0;

    if (timeout_ms >= 0) {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        tvp = &tv;
    }
    count = select(max_fd + 1, &rfds, &wfds, NULL, tvp);
    if (count < 0)
        return errno == EINTR ? 0 : -1;

    for (fd = 0; fd <= max_fd && reported < count; fd++) {
        unsigned int events = 0;

        /* A handler may have stopped watching a later descriptor. */
        if (FD_ISSET(fd, &rfds) && FD_ISSET(fd, &read_set))
            events |= ENGINE_READ;
        if (FD_ISSET(fd, &wfds) && FD_ISSET(fd, &write_set))
            events |= ENGINE_WRITE;
        if (events) {
            reported++;
            handler(fd, events);
        }
    }
    return reported;
}

/*************************************************************************/

const Engine engine_select = {
    .name = "select",
    .init = select_engine_init,
    .shutdown = select_engine_shutdown,
    .add = select_engine_add,
    .update = select_engine_update,
    .remove = select_engine_remove,
    .wait = select_engine_wait,
};
