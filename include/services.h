/* Main header for Services.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef SERVICES_H
#define SERVICES_H

/*************************************************************************/

/* System configuration information (from "configure"): */
#include "config.h"

/* User configuration and basic constants, macros, and includes: */
#include "defs.h"

/*************************************************************************/
/*************************************************************************/

/* Types corresponding to various structures.  These have to come first
 * because some structures reference each other circularly. */

typedef struct user_ User;
typedef struct channel_ Channel;
typedef struct server_ Server;
typedef struct serverstats_ ServerStats;

typedef struct nickinfo_ NickInfo;
typedef struct nickgroupinfo_ NickGroupInfo;
typedef struct channelinfo_ ChannelInfo;
typedef struct memoinfo_ MemoInfo;


/* Types for various name buffers, so we can make arrays of them. */

typedef char nickname_t[NICKMAX];
typedef char channame_t[CHANMAX];

/*************************************************************************/

/* Suspension info structure. */

typedef struct suspendinfo_ SuspendInfo;
struct suspendinfo_ {
    char who[NICKMAX];  /* who added this suspension */
    char *reason;
    time_t suspended;
    time_t expires;     /* 0 for no expiry */
};

/*************************************************************************/

/* Constants for "what" parameter to clear_channel(). */

#define CLEAR_MODES     0x0001  /* Binary modes */
#define CLEAR_BANS      0x0002  /* Bans */
#define CLEAR_EXCEPTS   0x0004  /* Ban exceptions (no-op if not supported) */
#define CLEAR_INVITES   0x0008  /* Invite masks (no-op if not supported) */
#define CLEAR_UMODES    0x0010  /* User modes (+v, +o) */

#define CLEAR_USERS     0x8000  /* Kick all users and empty the channel */

/* All channel modes: */
#define CLEAR_CMODES    (CLEAR_MODES|CLEAR_BANS|CLEAR_EXCEPTS|CLEAR_INVITES)

/*************************************************************************/
/*************************************************************************/

/* All other "main" include files. */

#include "memory.h"
#include "list-array.h"
#include "log.h"
#include "sockets.h"
#include "send.h"
#include "modes.h"
#include "users.h"
#include "channels.h"
#include "servers.h"

#include "extern.h"

/*************************************************************************/

#endif  /* SERVICES_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
