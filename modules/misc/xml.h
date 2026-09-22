/* Header for XML import/export modules.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef XML_H
#define XML_H

/*************************************************************************/

/* Type for write function passed to export routine. */
typedef int (*xml_writefunc_t)(void *data, const char *fmt, ...);

/* The export routine itself. */
extern int xml_export(xml_writefunc_t writefunc, void *data);

/*************************************************************************/

#endif  /* XML_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
