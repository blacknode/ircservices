/* chanprot.c header file.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef CHANPROT_H
#define CHANPROT_H

#define init_chanprot RENAME_SYMBOL(init_chanprot)
#define exit_chanprot RENAME_SYMBOL(exit_chanprot)
extern int init_chanprot(void);
extern void exit_chanprot(void);

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
