/* Header for autokill stuff.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef AKILL_H
#define AKILL_H

#include "services.h"

/* Events of operserv/akill (see include/events.h), for the protocol:
 *     akill.send_akill:     (const char *username, const char *host,
 *                           time_t expires, const char *who,
 *                           const char *reason)
 *     akill.cancel_akill:   (const char *username, const char *host)
 *     akill.send_exclude:   as send_akill, for an exclusion
 *     akill.cancel_exclude: as cancel_akill, for an exclusion */
#define AKILL_EVENT_SEND_AKILL     "akill.send_akill"
#define AKILL_EVENT_CANCEL_AKILL   "akill.cancel_akill"
#define AKILL_EVENT_SEND_EXCLUDE   "akill.send_exclude"
#define AKILL_EVENT_CANCEL_EXCLUDE "akill.cancel_exclude"

E void create_akill(char *mask, const char *reason, const char *who,
                    time_t expiry);

#endif  /* AKILL_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
