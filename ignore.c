/* Ignore handling.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"

/*************************************************************************/

/* ignore_init: Initialize ignore-related fields of a new User structure. */

void ignore_init(User *u)
{
    u->ignore = 0;
    u->lastcmd = time_msec();
    u->lastcmd_s = time(NULL);
}

/*************************************************************************/

/* ignore_update: Update the user's ignore level.  The "ignore level" is a
 *                measure of how much time Services has spent processing
 *                this user's commands, and is calculated as a decaying
 *                average of the fraction of time spent on the user's
 *                commands over the total time Services has been running--
 *                more specifically, the average over time of a function
 *                whose value is 1 when Services is executing a command
 *                from the user and 0 at all other times.  The rate of
 *                decay of the average, and hence the rate of response to
 *                new commands, is dependent on the IgnoreDecay
 *                configuration setting: the average decays by half every
 *                IgnoreDecay milliseconds (note that the value is given as
 *                seconds in the configuration file).
 *                     The `msec' parameter to this function is the length
 *                of time taken by the most recent command (which caused
 *                this update).  `msec' may be specified as zero to update
 *                the user's ignore level at any time.
 */

void ignore_update(User *u, uint32 msec)
{
    time_t now;
    uint32 now_msec;

    if (!IgnoreDecay) {
        /* Ignore code disabled, just return */
        return;
    }
    now = time(NULL);
    now_msec = time_msec();
    if (now - u->lastcmd_s > 1000000) {
        /* Millisecond counter may have overflowed, just reset to 0 */
        u->ignore = 0;
    } else {
        double zerolen = (now_msec - msec) - u->lastcmd;
        if (zerolen > 0)
            u->ignore *= pow(2, -(zerolen / IgnoreDecay));
    }
    if (msec) {
        double factor;
        while (msec > IgnoreDecay) {
            u->ignore = u->ignore*0.5 + 0.5;
            msec -= IgnoreDecay;
        }
        factor = pow(2, -((double)msec / IgnoreDecay));
        u->ignore = u->ignore*factor + (1-factor);
    }
    u->lastcmd = now_msec;
    u->lastcmd_s = now;
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
