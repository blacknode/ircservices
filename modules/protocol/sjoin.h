/* sjoin.c header file.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef SJOIN_H
#define SJOIN_H

#define do_sjoin RENAME_SYMBOL(do_sjoin)
#define init_sjoin RENAME_SYMBOL(init_sjoin)
#define exit_sjoin RENAME_SYMBOL(exit_sjoin)

#define SJOIN_CONFIG \
    { "CSSetChannelTime", { { CD_SET, 0, &CSSetChannelTime } } }

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
