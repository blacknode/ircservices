/* Time-delay routine include stuff.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <time.h>

/*************************************************************************/

/* Timeout type. */

typedef struct timeout_ Timeout;

struct timeout_ {
    void *data;                 /* Caller data; can be anything */
    time_t settime;             /* Time timer was set (from time()) */
    /* Remainder is PRIVATE DATA! */
#ifdef IN_TIMEOUT_C
    Timeout *next, *prev;
    uint32 timeout;             /* In milliseconds (time_msec()) */
    uint32 repeat;              /* Does this timeout repeat indefinitely?
                                 *    (if nonzero, new value of `timeout') */
    void (*code)(Timeout *);    /* This structure is passed to the code */
#endif
};

/*************************************************************************/

/* Check the timeout list for any pending actions. */
extern void check_timeouts(void);

/* Add a timeout to the list to be triggered in `delay' seconds (`delay'
 * may be zero).  Any timeout added from within a timeout routine will not
 * be checked during that run through the timeout list.  Always succeeds. */
extern Timeout *add_timeout(int delay, void (*code)(Timeout *), int repeat);

/* Add a timeout to the list to be triggered in `delay' milliseconds
 * (`delay' may be zero). */
extern Timeout *add_timeout_ms(uint32 delay, void (*code)(Timeout *),
                               int repeat);

/* Remove a timeout from the list (if it's there). */
extern void del_timeout(Timeout *t);

#ifdef DEBUG_COMMANDS
/* Send the list of timeouts to the given user. */
extern void send_timeout_list(User *u);
#endif

/*************************************************************************/

#endif  /* TIMEOUT_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
