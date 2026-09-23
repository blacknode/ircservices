/* Routines for time-delayed actions.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#define IN_TIMEOUT_C
#include "timeout.h"

/*************************************************************************/

static Timeout *timeouts = NULL;
static int checking_timeouts = 0;

/*************************************************************************/

#ifdef DEBUG_COMMANDS

/* Send the timeout list to the given user. */

void send_timeout_list(User *u)
{
    Timeout *to;
    uint32 now = time_msec();

    notice(ServerName, u->nick, "Now: %u.%03u", now/1000, now%1000);
    LIST_FOREACH (to, timeouts) {
        notice(ServerName, u->nick, "%p: %u.%03u: %p (%p)",
               to, to->timeout/1000, to->timeout%1000, to->code, to->data);
    }
}

#endif  /* DEBUG_COMMANDS */

/*************************************************************************/

/* Check the timeout list for any pending actions. */

void check_timeouts(void)
{
    Timeout *to, *to2;
    uint32 now = time_msec();

    if (checking_timeouts)
        fatal("check_timeouts() called recursively!");
    checking_timeouts = 1;
    log_debug(2, "Checking timeouts at time_msec = %u.%03u",
              now/1000, now%1000);

    LIST_FOREACH_SAFE (to, timeouts, to2) {
        if (to->timeout) {
            if ((int32)(to->timeout - now) > 0)
                continue;
            log_debug(3, "Running timeout %p (code=%p repeat=%d)",
                      to, to->code, to->repeat);
            to->code(to);
            if (to->repeat) {
                to->timeout = now + to->repeat;
                if (!to->timeout)  /* watch out for zero! */
                    to->timeout++;
                continue;
            }
        }
        LIST_REMOVE(to, timeouts);
        free(to);
    }

    log_debug(2, "Finished timeout list");
    checking_timeouts = 0;
}

/*************************************************************************/

/* Add a timeout to the list to be triggered in `delay' seconds.  If
 * `repeat' is nonzero, do not delete the timeout after it is triggered.
 * This must maintain the property that timeouts added from within a
 * timeout routine do not get checked during that run of the timeout list.
 */

Timeout *add_timeout(int delay, void (*code)(Timeout *), int repeat)
{
    if (delay < 0) {
        log("add_timeout(): called with a negative delay! (%d)", delay);
        return NULL;
    }
    if (!code) {
        log("add_timeout(): called with code==NULL!");
        return NULL;
    }
    if (delay > 2147483) {  /* 2^31/1000 (watch out for difference overflow) */
        log("add_timeout(): delay (%ds) too long, shortening to 2147483s",
            delay);
        delay = 2147483;
    }
    return add_timeout_ms((uint32)delay*1000, code, repeat);
}

/*************************************************************************/

/* The same thing, but using milliseconds instead of seconds. */

Timeout *add_timeout_ms(uint32 delay, void (*code)(Timeout *), int repeat)
{
    Timeout *t;

    if (!code) {
        log("add_timeout_ms(): called with code==NULL!");
        return NULL;
    }
    if (delay > 2147483647) {
        log("add_timeout_ms(): delay (%dms) too long, shortening to"
            " 2147483647ms", delay);
        delay = 2147483647;
    }
    t = malloc(sizeof(Timeout));
    if (!t)
        return NULL;
    t->settime = time(NULL);
    t->timeout = time_msec() + delay;
    /* t->timeout==0 is used to signal that the timeout should be deleted;
     * if the timeout value just happens to wrap around to 0, lengthen it
     * by a millisecond. */
    if (!t->timeout)
        t->timeout++;
    t->code = code;
    t->data = NULL;
    t->repeat = repeat ? delay : 0;
    LIST_INSERT(t, timeouts);
    return t;
}

/*************************************************************************/

/* Remove a timeout from the list (if it's there). */

void del_timeout(Timeout *t)
{
    Timeout *ptr;

    if (!t) {
        log("del_timeout(): called with t==NULL!");
        return;
    }
    LIST_FOREACH (ptr, timeouts) {
        if (ptr == t)
            break;
    }
    if (!ptr) {
        log("del_timeout(): attempted to remove timeout %p (not on list)", t);
        return;
    }
    if (checking_timeouts) {
        t->timeout = 0;  /* delete it when we hit it in the list */
        return;
    }
    LIST_REMOVE(t, timeouts);
    free(t);
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
