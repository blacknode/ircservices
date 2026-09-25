/* S-line data structure and interface.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef SLINE_H
#define SLINE_H

#include "services.h"

/*************************************************************************/

/* Events of operserv/sline (see include/events.h), for the protocol:
 *     sline.send_sgline:   (const char *mask, time_t expires,
 *                          const char *who, const char *reason)
 *     sline.cancel_sgline: (const char *mask)
 * and the same for SQLINE (sqline) and SZLINE (szline). */
#define SLINE_EVENT_SEND_SGLINE   "sline.send_sgline"
#define SLINE_EVENT_CANCEL_SGLINE "sline.cancel_sgline"
#define SLINE_EVENT_SEND_SQLINE   "sline.send_sqline"
#define SLINE_EVENT_CANCEL_SQLINE "sline.cancel_sqline"
#define SLINE_EVENT_SEND_SZLINE   "sline.send_szline"
#define SLINE_EVENT_CANCEL_SZLINE "sline.cancel_szline"

E void create_sline(uint8 type, char *mask, const char *reason,
                    const char *who, time_t expiry);

/*************************************************************************/

#endif  /* SLINE_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
