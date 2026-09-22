/* Code for servers supporting the ircd.dal SVSNICK command.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"

#include "svsnick.h"

/* Command name for SVSNICK */
static const char *cmd_SVSNICK = "SVSNICK";

/*************************************************************************/
/*************************************************************************/

/* Send a command to change the nickname of a user on another server. */

static void svsnick_send_nickchange_remote(const char *nick,
                                           const char *newnick)
{
    send_cmd(ServerName, "%s %s %s :%ld", cmd_SVSNICK, nick, newnick,
             (long)time(NULL));
}

/*************************************************************************/
/*************************************************************************/

int init_svsnick(const char *cmdname)
{
    cmd_SVSNICK = cmdname;
    send_nickchange_remote = svsnick_send_nickchange_remote;
    protocol_features |= PF_CHANGENICK;
    return 1;
}

/*************************************************************************/

void exit_svsnick(void)
{
    protocol_features &= ~PF_CHANGENICK;
    send_nickchange_remote = NULL;
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
