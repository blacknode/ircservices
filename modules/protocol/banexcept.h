/* banexcept.c header file.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef BANEXCEPT_H
#define BANEXCEPT_H

/* Note that we need to rename all of our exported symbols to avoid
 * conflicts when the .c file is included in multiple modules */
#define init_banexcept RENAME_SYMBOL(init_banexcept)
#define exit_banexcept RENAME_SYMBOL(exit_banexcept)

extern int init_banexcept(void);
extern void exit_banexcept(void);

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
