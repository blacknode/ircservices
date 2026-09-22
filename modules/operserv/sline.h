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

/*************************************************************************/

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
