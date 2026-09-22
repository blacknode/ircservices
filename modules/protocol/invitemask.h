/* invitemask.c header file.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef INVITEMASK_H
#define INVITEMASK_H

#define init_invitemask RENAME_SYMBOL(init_invitemask)
#define exit_invitemask RENAME_SYMBOL(exit_invitemask)
extern int init_invitemask(void);
extern void exit_invitemask(void);

#endif

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
