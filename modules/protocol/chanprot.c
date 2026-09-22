/* Channel protect/founder (+a/+q) related functions.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "language.h"

#include "chanprot.h"

/*************************************************************************/

static int chanprot_old_HELP_SOP_MID1 = -1;

/*************************************************************************/

int init_chanprot(void)
{
    protocol_features |= PF_CHANPROT;
    chanprot_old_HELP_SOP_MID1 =
        mapstring(CHAN_HELP_SOP_MID1, CHAN_HELP_SOP_MID1_CHANPROT);
    return 1;
}

/*************************************************************************/

void exit_chanprot(void)
{
    if (chanprot_old_HELP_SOP_MID1 >= 0)
        mapstring(CHAN_HELP_SOP_MID1, chanprot_old_HELP_SOP_MID1);
    chanprot_old_HELP_SOP_MID1 = -1;
    protocol_features &= ~PF_CHANPROT;
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
