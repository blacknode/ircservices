/* Base routines for ChanServ access level handling.
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
#include "modules/nickserv/nickserv.h"
#include "modules/operserv/operserv.h"

#include "chanserv.h"
#include "access.h"
#include "cs-local.h"

/*************************************************************************/

/* Array of all access levels: */
EXPORT_ARRAY(levelinfo)
LevelInfo levelinfo[] = {
    { CA_AUTOPROTECT,   ACCLEV_SOP, "AUTOPROTECT", CHAN_LEVEL_AUTOPROTECT,
          CL_SET_MODE,   { .cumode = {"a",0} } },
    { CA_AUTOOP,        ACCLEV_AOP, "AUTOOP",      CHAN_LEVEL_AUTOOP,
          CL_SET_MODE,   { .cumode = {"o",1} } },
    { CA_AUTOHALFOP,    ACCLEV_HOP, "AUTOHALFOP",  CHAN_LEVEL_AUTOHALFOP,
          CL_SET_MODE,   { .cumode = {"h",1} } },
    { CA_AUTOVOICE,     ACCLEV_VOP, "AUTOVOICE",   CHAN_LEVEL_AUTOVOICE,
          CL_SET_MODE,   { .cumode = {"v",0} } },

    /* Internal use; not settable by users.  These two levels change (in
     * effect, not in the actual tables) based on the SECUREOPS and
     * RESTRICTED settings. */
    { CA_AUTODEOP,              -1, "",            -1,
          CL_CLEAR_MODE|CL_LESSEQUAL, { cumode: {"oha",0} } },
    { CA_NOJOIN,              -100, "",            -1,
          CL_OTHER|CL_LESSEQUAL },

    { CA_INVITE,        ACCLEV_AOP, "INVITE",      CHAN_LEVEL_INVITE,
          CL_ALLOW_CMD,  { .cmd = {"INVITE"} } },
    { CA_AKICK,         ACCLEV_SOP, "AKICK",       CHAN_LEVEL_AKICK,
          CL_ALLOW_CMD,  { .cmd = {"AKICK"} } },
    { CA_SET,       ACCLEV_FOUNDER, "SET",         CHAN_LEVEL_SET,
          CL_ALLOW_CMD,  { .cmd = {"SET"} } },
    { CA_CLEAR,         ACCLEV_SOP, "CLEAR",       CHAN_LEVEL_CLEAR,
          CL_ALLOW_CMD,  { .cmd = {"CLEAR"} } },
    { CA_UNBAN,         ACCLEV_AOP, "UNBAN",       CHAN_LEVEL_UNBAN,
          CL_ALLOW_CMD,  { .cmd = {"UNBAN"} } },
    { CA_ACCESS_LIST,            0, "ACC-LIST",    CHAN_LEVEL_ACCESS_LIST,
          CL_ALLOW_CMD,  { .cmd = {"ACCESS","LIST"} } },
    { CA_ACCESS_CHANGE, ACCLEV_HOP, "ACC-CHANGE",  CHAN_LEVEL_ACCESS_CHANGE,
          CL_ALLOW_CMD,  { .cmd = {"ACCESS"} } },
    { CA_MEMO,          ACCLEV_SOP, "MEMO",        CHAN_LEVEL_MEMO,
          CL_OTHER },
    { CA_OPDEOP,        ACCLEV_AOP, "OP-DEOP",     CHAN_LEVEL_OPDEOP,
          CL_ALLOW_CMD,  { .cmd = {"OP"} } },  /* also includes "DEOP" */
    { CA_VOICE,         ACCLEV_VOP, "VOICE",       CHAN_LEVEL_VOICE,
          CL_ALLOW_CMD,  { .cmd = {"VOICE"} } },
    { CA_HALFOP,        ACCLEV_HOP, "HALFOP",      CHAN_LEVEL_HALFOP,
          CL_ALLOW_CMD,  { .cmd = {"HALFOP"} } },
    { CA_PROTECT,       ACCLEV_SOP, "PROTECT",     CHAN_LEVEL_PROTECT,
          CL_ALLOW_CMD,  { .cmd = {"PROTECT"} } },
    { CA_KICK,          ACCLEV_AOP, "KICK",        CHAN_LEVEL_KICK,
          CL_ALLOW_CMD,  { .cmd = {"KICK"} } },
    { CA_TOPIC,         ACCLEV_AOP, "TOPIC",       CHAN_LEVEL_TOPIC,
          CL_ALLOW_CMD,  { .cmd = {"TOPIC"} } },
    { CA_STATUS,        ACCLEV_SOP, "STATUS",      CHAN_LEVEL_STATUS,
          CL_ALLOW_CMD,  { .cmd = {"STATUS"} } },

    { -1 }
};

/* Default access levels (initialized at runtime): */
int16 def_levels[CA_SIZE];
/* Which levels are "maximums" (CL_LESSEQUAL): */
static int lev_is_max[CA_SIZE];

static int get_access_if_idented(const User *user, const ChannelInfo *ci);

/*************************************************************************/
/************************** Global-use routines **************************/
/*************************************************************************/

/* Return the channel's level for the given category, handling
 * ACCLEV_DEFAULT appropriately.  Returns ACCLEV_INVALID on invalid
 * parameters (`ci' NULL or `what' out of range).
 */

EXPORT_FUNC(get_ci_level)
int get_ci_level(const ChannelInfo *ci, int what)
{
    int level;

    if (!ci) {
        module_log("get_ci_level() called with NULL ChannelInfo!");
        return ACCLEV_INVALID;
    } else if (what < 0 || what >= CA_SIZE) {
        module_log("get_ci_level() called with invalid `what'!");
        return ACCLEV_INVALID;
    }
    level = ci->levels[what];
    if (level == ACCLEV_DEFAULT)
        level = def_levels[what];
    return level;
}

/*************************************************************************/

/* Return 1 if the user's access level on the given channel falls into the
 * given category, 0 otherwise.  Note that this may seem slightly confusing
 * in some cases: for example, check_access(..., CA_NOJOIN) returns true if
 * the user does _not_ have access to the channel (i.e. matches the NOJOIN
 * criterion).
 */

EXPORT_FUNC(check_access)
int check_access(const User *user, const ChannelInfo *ci, int what)
{
    int level = get_access(user, ci);
    int limit;

    if (level == ACCLEV_FOUNDER)
        return lev_is_max[what] ? 0 : 1;
    limit = get_ci_level(ci, what);
    /* Hacks to make flags work */
    if (what == CA_AUTODEOP && (ci->flags & CF_SECUREOPS))
        limit = 0;
    if (what == CA_NOJOIN && (ci->flags & CF_RESTRICTED))
        limit = 0;
    if (limit == ACCLEV_INVALID)
        return 0;
    if (lev_is_max[what])
        return level <= limit;
    else
        return level >= limit;
}

/*************************************************************************/

/* Do like check_access(), but return whether the user would match the
 * given category if they had identified for their nick.  If the nick is
 * not registered, returns the same value as check_access().
 */

EXPORT_FUNC(check_access_if_idented)
int check_access_if_idented(const User *user, const ChannelInfo *ci, int what)
{
    int level = get_access_if_idented(user, ci);
    int limit;

    if (level == ACCLEV_FOUNDER)
        return lev_is_max[what] ? 0 : 1;
    limit = get_ci_level(ci, what);
    /* Hacks to make flags work */
    if (what == CA_AUTODEOP && (ci->flags & CF_SECUREOPS))
        limit = 0;
    if (what == CA_NOJOIN && (ci->flags & CF_RESTRICTED))
        limit = 0;
    if (limit == ACCLEV_INVALID)
        return 0;
    if (lev_is_max[what])
        return level <= limit;
    else
        return level >= limit;
}

/*************************************************************************/

/* Return positive if the user is permitted access to the given command for
 * the given channel, zero otherwise.  If no level is found that
 * corresponds to the given command, return -1.
 */

EXPORT_FUNC(check_access_cmd)
int check_access_cmd(const User *user, const ChannelInfo *ci,
                     const char *command, const char *subcommand)
{
    int i;

    /* If we have a subcommand, first check for an exact match */
    if (subcommand) {
        for (i = 0; levelinfo[i].what >= 0; i++) {
            if ((levelinfo[i].action & CL_TYPEMASK) == CL_ALLOW_CMD
             && levelinfo[i].target.cmd.sub
             && stricmp(command, levelinfo[i].target.cmd.main) == 0
             && stricmp(subcommand, levelinfo[i].target.cmd.sub) == 0
            ) {
                return check_access(user, ci, levelinfo[i].what);
            }
        }
    }

    /* No subcommand or no exact match, so match on command name;
     * explicitly skip entries with subcommands (because they didn't
     * match before) */
    for (i = 0; levelinfo[i].what >= 0; i++) {
        if ((levelinfo[i].action & CL_TYPEMASK) == CL_ALLOW_CMD
         && !levelinfo[i].target.cmd.sub
         && stricmp(command, levelinfo[i].target.cmd.main) == 0
        ) {
            return check_access(user, ci, levelinfo[i].what);
        }
    }

    /* No level found */
    return -1;
}

/*************************************************************************/
/************************** Local-use routines ***************************/
/*************************************************************************/

/* Return the access level the given user has on the channel.  If the
 * channel doesn't exist, the user isn't on the access list, or the channel
 * is CS_SECURE and the user hasn't IDENTIFY'd with NickServ, return 0.
 * The only external use is by the STATUS command.
 */

int get_access(const User *user, const ChannelInfo *ci)
{
    if (is_founder(user, ci))
        return ACCLEV_FOUNDER;
    if (!ci || !valid_ngi(user) || (ci->flags & (CF_VERBOTEN | CF_SUSPENDED)))
        return 0;
    if (user_identified(user)
     || (user_recognized(user) && !(ci->flags & CF_SECURE))
    ) {
        int32 id = user->ngi->id;
        int i;
        ARRAY_FOREACH (i, ci->access) {
            if (ci->access[i].nickgroup == id)
                return ci->access[i].level;
        }
    }
    return 0;
}

/*************************************************************************/

/* Return the access level the given user has on the channel, supposing the
 * user was identified for their current nick (if it's registered).
 */

static int get_access_if_idented(const User *user, const ChannelInfo *ci)
{
    int i;
    int32 id;

    if (is_identified(user, ci))
        return ACCLEV_FOUNDER;
    if (!ci || !valid_ngi(user) || (ci->flags & (CF_VERBOTEN | CF_SUSPENDED)))
        return 0;
    if (user->ngi->id == ci->founder)
        return ACCLEV_FOUNDER;
    id = user->ngi->id;
    ARRAY_FOREACH (i, ci->access) {
        if (ci->access[i].nickgroup == id)
            return ci->access[i].level;
    }
    return 0;
}

/*************************************************************************/

/* Check a channel user mode change, and return the mask of mode flags
 * which should be changed (i.e. should be the reverse of the new flags
 * given).  The `changemask' parameter indicates which flags are changing,
 * and `newmodes' indicates the value of the changing flags; a new user on
 * a channel, e.g. from JOIN, should have a `changemask' with all bits set
 * (because all bits are changing from "undefined" to either on or off).
 * The returned value will always be a subset of `changemask'.
 *
 * Example: autoprotect/autoop user joins channel
 *     check_access_cumode(..., 0, ~0) -> CUMODE_a | CUMODE_o
 * Example: an autodeop user gets +o'd by somebody
 *     check_access_cumode(..., CUMODE_o, CUMODE_o) -> CUMODE_o
 *         (i.e. "setting CUMODE_o is incorrect" -> send a MODE -o)
 */

int check_access_cumode(const User *user, const ChannelInfo *ci,
                        int32 newmodes, int32 changemask)
{
    int i;
    int32 result = 0;

    for (i = 0; levelinfo[i].what >= 0; i++) {
        int type = levelinfo[i].action & CL_TYPEMASK;
        int32 flags = levelinfo[i].target.cumode.flags;
        int clevel = get_ci_level(ci, levelinfo[i].what);
        if ((type == CL_SET_MODE || type == CL_CLEAR_MODE)
         && clevel != ACCLEV_INVALID
         && (changemask & flags)
         && check_access(user, ci, levelinfo[i].what)
        ) {
            if (type == CL_SET_MODE && (~newmodes & flags))
                result |= ~newmodes & flags;
            else if (type == CL_CLEAR_MODE && (newmodes & flags))
                result |= newmodes & flags;
            while (levelinfo[i].target.cumode.cont)
                i++;
        }
    }
    return result;
}

/*************************************************************************/

/* Add an entry `nick' (at `level') to the access list by a user with
 * access `uacc'.
 */

int access_add(ChannelInfo *ci, const char *nick, int level, int uacc)
{
    int i;
    NickInfo *ni;
    NickGroupInfo *ngi;

    if (level >= uacc)
        return RET_PERMISSION;
    ni = get_nickinfo(nick);
    if (!ni) {
        return RET_NOSUCHNICK;
    } else if (ni->status & NS_VERBOTEN) {
        put_nickinfo(ni);
        return RET_NICKFORBID;
    } else if (!(ngi = get_ngi(ni))) {
        put_nickinfo(ni);
        return RET_INTERR;
    } else if ((ngi->flags & NF_NOOP) && level > 0) {
        put_nickgroupinfo(ngi);
        put_nickinfo(ni);
        return RET_NOOP;
    }
    put_nickgroupinfo(ngi);
    ARRAY_FOREACH (i, ci->access) {
        if (ci->access[i].nickgroup == ni->nickgroup) {
            put_nickinfo(ni);
            /* Don't allow lowering from a level >= uacc */
            if (ci->access[i].level >= uacc)
                return RET_PERMISSION;
            if (ci->access[i].level == level)
                return RET_UNCHANGED;
            ci->access[i].level = level;
            return RET_CHANGED;
        }
    }
    ARRAY_SEARCH_SCALAR(ci->access, nickgroup, 0, i);
    if (i == ci->access_count) {
        if (i < CSAccessMax) {
            ARRAY_EXTEND(ci->access);
        } else {
            put_nickinfo(ni);
            return RET_LISTFULL;
        }
    }
    ci->access[i].channel = ci;
    ci->access[i].nickgroup = ni->nickgroup;
    ci->access[i].level = level;
    put_nickinfo(ni);
    return RET_ADDED;
}

/*************************************************************************/

int access_del(ChannelInfo *ci, const char *nick, int uacc)
{
    NickInfo *ni;
    int i;

    ni = get_nickinfo(nick);
    if (!ni) {
        return RET_NOSUCHNICK;
    } else if (ni->status & NS_VERBOTEN) {
        put_nickinfo(ni);
        return RET_NICKFORBID;
    } else if (!check_ngi(ni)) {
        put_nickinfo(ni);
        return RET_INTERR;
    }
    ARRAY_SEARCH_SCALAR(ci->access, nickgroup, ni->nickgroup, i);
    put_nickinfo(ni);
    if (i == ci->access_count)
        return RET_NOENTRY;
    if (uacc <= ci->access[i].level)
        return RET_PERMISSION;
    ci->access[i].nickgroup = 0;
    return RET_DELETED;
}

/*************************************************************************/
/*************************************************************************/

int init_access(void)
{
    int i;

    /* Initialize def_levels[] and lev_is_max[], and convert mode letters
     * to flags */
    for (i = 0; levelinfo[i].what >= 0; i++) {
        int type = levelinfo[i].action & CL_TYPEMASK;
        if (type == CL_SET_MODE || type == CL_CLEAR_MODE) {
            /* Use MODE_NOERROR to deal with protocols that don't
             * support some modes (e.g. +h in AUTODEOP) */
            levelinfo[i].target.cumode.flags =
                mode_string_to_flags(levelinfo[i].target.cumode.modes,
                                     MODE_CHANUSER | MODE_NOERROR);
        }
        def_levels[levelinfo[i].what] = levelinfo[i].defval;
        lev_is_max[levelinfo[i].what] = levelinfo[i].action & CL_LESSEQUAL;
    }

    /* Delete any levels for features not supported by protocol */
    if (!(protocol_features & PF_HALFOP)) {
        int offset = 0;
        for (i = 0; i == 0 || levelinfo[i-1].what >= 0; i++) {
            if (levelinfo[i].what == CA_AUTOHALFOP
             || levelinfo[i].what == CA_HALFOP
            ) {
                offset++;
            } else if (offset) {
                memcpy(&levelinfo[i-offset], &levelinfo[i],
                       sizeof(*levelinfo));
            }
        }
    }
    if (!(protocol_features & PF_CHANPROT)) {
        int offset = 0;
        for (i = 0; i == 0 || levelinfo[i-1].what >= 0; i++) {
            if (levelinfo[i].what == CA_AUTOPROTECT
             || levelinfo[i].what == CA_PROTECT
            ) {
                offset++;
            } else if (offset) {
                memcpy(&levelinfo[i-offset], &levelinfo[i],
                       sizeof(*levelinfo));
            }
        }
    }

    return 1;
}

/*************************************************************************/

void exit_access(void)
{
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
