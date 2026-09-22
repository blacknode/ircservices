/* SVSNICK support external declarations.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef SVSNICK_H
#define SVSNICK_H

#define init_svsnick RENAME_SYMBOL(init_svsnick)
#define exit_svsnick RENAME_SYMBOL(exit_svsnick)
extern int init_svsnick(const char *cmdname);
extern void exit_svsnick(void);

#endif  /* SVSNICK_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
