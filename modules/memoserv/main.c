/* MemoServ functions.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "conffile.h"
#include "language.h"
#include "commands.h"
#include "databases.h"
#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"
#include "modules/operserv/operserv.h"

#include "modules/memoserv/memoserv.h"

/*************************************************************************/

static Module *module_chanserv;   /* Optional; NULL if not loaded */

/* Imports */
static ChannelInfo *(*p_get_channelinfo)(const char *channel);
static ChannelInfo *(*p_put_channelinfo)(ChannelInfo *ci);
static int (*p_get_ci_level)(const ChannelInfo *ci, int what);
static int (*p_check_access)(const User *user, const ChannelInfo *ci, int what);

static Event* command_event;
static Event* receive_memo_event;
static Event* help_event;
static Event* help_cmds_event;
static Event* set_event;

       int32  MSMaxMemos;
static time_t MSExpire;
static time_t MSExpireDelay;
static time_t MSSendDelay;

/*************************************************************************/

/* Error codes for get_memoinfo(). */
#define GMI_NOTFOUND    -1
#define GMI_FORBIDDEN   -2
#define GMI_SUSPENDED   -3
#define GMI_INTERR      -99

/* Macro to return the real memo maximum for a `mi->memomax' value (i.e.
 * convert MEMOMAX_DEFAULT to MSMaxMemos). */
#define REALMAX(n) ((n)==MEMOMAX_DEFAULT ? MSMaxMemos : (n))

/*************************************************************************/

static void check_memos(User *u);
static void expire_memos(MemoInfo *mi);
static MemoInfo *get_memoinfo(const char *name, NickGroupInfo **owner_ret,
                              int *error_ret);
static int send_memo(const User *source, const char *target, const char *text,
                     const char *channel, int *errormsg_ret);
static int list_memo(User *u, int index, MemoInfo *mi, int *sent_header,
                     int new);
static int list_memo_callback(int num, va_list args);
static int read_memo(User *u, int index, MemoInfo *mi);
static int read_memo_callback(int num, va_list args);
static int del_memo(MemoInfo *mi, int num);
static int del_memo_callback(int num, va_list args);

static void do_help(User *u);
static void do_send(User *u);
static void do_list(User *u);
static void do_read(User *u);
static void do_save(User *u);
static void do_del(User *u);
static void do_renumber(User *u);
static void do_set(User *u);
static void do_set_notify(User *u, MemoInfo *mi, char *param);
static void do_set_limit(User *u, MemoInfo *mi, char *param);
static void do_info(User *u);

/*************************************************************************/

static Command cmds[] = {
    { "HELP",       do_help,     NULL,  -1,                      -1,-1 },
    { "SEND",       do_send,     NULL,  MEMO_HELP_SEND,          -1,-1 },
    { "LIST",       do_list,     NULL,  MEMO_HELP_LIST,          -1,-1 },
    { "READ",       do_read,     NULL,  MEMO_HELP_READ,          -1,-1 },
    { "SAVE",       do_save,     NULL,  MEMO_HELP_SAVE,          -1,-1 },
    { "DEL",        do_del,      NULL,  MEMO_HELP_DEL,           -1,-1 },
    { "RENUMBER",   do_renumber, NULL,  MEMO_HELP_RENUMBER,      -1,-1 },
    { "SET",        do_set,      NULL,  MEMO_HELP_SET,           -1,-1 },
    { "SET NOTIFY", NULL,        NULL,  MEMO_HELP_SET_NOTIFY,    -1,-1,
                "NickServ" },  /* actual nick retrieved on the fly */
    { "SET LIMIT",  NULL,        NULL,  -1,
                MEMO_HELP_SET_LIMIT, MEMO_OPER_HELP_SET_LIMIT },
    { "INFO",       do_info,     NULL,  -1,
                MEMO_HELP_INFO, MEMO_OPER_HELP_INFO },
    { NULL }
};

/* Command alias type and array */
typedef struct {
    char *alias, *command;
} Alias;
static Alias *aliases;
static int aliases_count;

/*************************************************************************/

/* The memos of a nickname group is part of the group's record, which
 * nickserv/main keeps in the database (see include/store.h): nothing to
 * load or save here. */

/*************************************************************************/
/***************************** Main routines *****************************/
/*************************************************************************/

/* memoserv:  Main MemoServ routine.
 *            Note that the User structure passed to the do_* routines will
 *            always be valid (non-NULL) and, except for the HELP command,
 *            will always have valid NickInfo and NickGroupInfo pointers in
 *            the `ni' and `ngi' fields.
 */

static void memoserv_message(struct Service *service, User *u, char *buf)
{
    char *cmd;

    cmd = strtok(buf, " ");
    if (cmd) {
        int i;
        ARRAY_FOREACH (i, aliases) {
            if (stricmp(cmd, aliases[i].alias) == 0) {
                cmd = aliases[i].command;
                break;
            }
        }
        if (!valid_ngi(u) && stricmp(cmd, "HELP") != 0)
            notice_lang(memoserv_service.nick, u, NICK_NOT_REGISTERED_HELP, nickserv_service.nick);
        else if (!user_identified(u) && stricmp(cmd, "HELP") != 0)
            notice_lang(memoserv_service.nick, u, NICK_IDENTIFY_REQUIRED, nickserv_service.nick);
        else if (event_emit(command_event, u, cmd) <= 0)
            run_cmd(memoserv_service.nick, u, THIS_MODULE, cmd);
    }
}

/*************************************************************************/

/* Handler for users connecting to the network. */

/* Handler for users whose nick NickServ has looked up: after connecting,
 * or after changing nicknames (in which case only a change of nick group
 * matters). */

static int do_user_validated(User *user, int nickchange, uint32 old_nickgroup)
{
    uint32 new_nickgroup;

    if (!nickchange) {
        if (user_recognized(user))
            check_memos(user);
        return 0;
    }
    new_nickgroup = user->ngi && user->ngi != NICKGROUPINFO_INVALID
                  ? user->ngi->id : 0;
    if (old_nickgroup != new_nickgroup)
        check_memos(user);
    return 0;
}

/*************************************************************************/

/* Callback to check for un-away. */

static int do_receive_message(const char *source, const char *cmd,
                              int ac, char **av)
{
    if (stricmp(cmd, "AWAY") == 0 && (ac == 0 || *av[0] == 0)) {
        User *u = get_user(source);
        if (u)
            check_memos(u);
    }
    return 0;
}

/*************************************************************************/

/* Handler for users identifying for nicks. */

static int do_nick_identified(User *user, int old_authstat)
{
    if (!(old_authstat & (NA_IDENTIFIED | NA_RECOGNIZED)))
        check_memos(user);
    return 0;
}

/*************************************************************************/
/*********************** MemoServ private routines ***********************/
/*************************************************************************/

/* check_memos:  See if the given user has any unread memos, and send a
 *               NOTICE to that user if so (and if the appropriate flag is
 *               set).
 */

static void check_memos(User *u)
{
    NickGroupInfo *ngi = u->ngi;
    int i, newcnt = 0, max;

    if (!ngi || !user_recognized(u) || !(ngi->flags & NF_MEMO_SIGNON))
        return;

    expire_memos(&ngi->memos);

    ARRAY_FOREACH (i, ngi->memos.memos) {
        if (ngi->memos.memos[i].flags & MF_UNREAD)
            newcnt++;
    }
    if (newcnt > 0) {
        notice_lang(memoserv_service.nick, u,
                newcnt==1 ? MEMO_HAVE_NEW_MEMO : MEMO_HAVE_NEW_MEMOS, newcnt);
        if (newcnt == 1 && (ngi->memos.memos[i-1].flags & MF_UNREAD)) {
            notice_lang(memoserv_service.nick, u, MEMO_TYPE_READ_LAST, memoserv_service.nick);
        } else if (newcnt == 1) {
            ARRAY_FOREACH (i, ngi->memos.memos) {
                if (ngi->memos.memos[i].flags & MF_UNREAD)
                    break;
            }
            notice_lang(memoserv_service.nick, u, MEMO_TYPE_READ_NUM, memoserv_service.nick,
                        ngi->memos.memos[i].number);
        } else {
            notice_lang(memoserv_service.nick, u, MEMO_TYPE_LIST_NEW, memoserv_service.nick);
        }
    }
    max = REALMAX(ngi->memos.memomax);
    if (max > 0 && ngi->memos.memos_count >= max) {
        if (ngi->memos.memos_count > max)
            notice_lang(memoserv_service.nick, u, MEMO_OVER_LIMIT, max);
        else
            notice_lang(memoserv_service.nick, u, MEMO_AT_LIMIT, max);
    }
}

/*************************************************************************/

/* Expire memos for the given MemoInfo. */

static void expire_memos(MemoInfo *mi)
{
    int i;
    time_t limit = time(NULL) - MSExpire;
    time_t readlimit = time(NULL) - MSExpireDelay;

    if (!MSExpire)
        return;
    ARRAY_FOREACH (i, mi->memos) {
        if ((mi->memos[i].flags & MF_EXPIREOK)
         && !(mi->memos[i].flags & MF_UNREAD)
         && mi->memos[i].time <= limit
         && mi->memos[i].firstread <= readlimit
        ) {
            free(mi->memos[i].channel);
            free(mi->memos[i].text);
            ARRAY_REMOVE(mi->memos, i);
            i--;
        }
    }
}

/*************************************************************************/

/* Return the MemoInfo corresponding to the given nickname.
 * Return in `owner' (which must not be NULL) the NickGroupInfo owning the
 * MemoInfo; the caller must call put_nickgroupinfo() on the nickgroup when
 * it is no longer needed.
 * Return in `error' a GMI_* error code if the return value is NULL.
 * Also set `error' to GMI_SUSPENDED if the nick is suspended, although a
 * valid MemoInfo will still be returned.
 *
 * Checks memos in the MemoInfo for expiration before returning.
 */

static MemoInfo *get_memoinfo(const char *name, NickGroupInfo **owner_ret,
                              int *error_ret)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    MemoInfo *mi = NULL;
    static int dummy_error;

    if (!owner_ret) {
        module_log("BUG: get_memoinfo() called with owner_ret==NULL");
        if (error_ret)
            *error_ret = GMI_INTERR;
        return NULL;
    }
    if (!error_ret)
        error_ret = &dummy_error;

    *error_ret = 0;
    ni = get_nickinfo(name);
    if (ni) {
        if (ni->status & NS_VERBOTEN) {
            *error_ret = GMI_FORBIDDEN;
            put_nickinfo(ni);
            return NULL;
        }
        ngi = get_ngi(ni);
        put_nickinfo(ni);
        if (!ngi) {
            *error_ret = GMI_INTERR;
            return NULL;
        }
        if (ngi->flags & NF_SUSPENDED)
            *error_ret = GMI_SUSPENDED;
        *owner_ret = ngi;
        mi = &ngi->memos;
    } else {
        *error_ret = GMI_NOTFOUND;
        return NULL;
    }

    if (!mi) {
        module_log("BUG: get_memoinfo(): mi==NULL after checks");
        *error_ret = GMI_INTERR;
        put_nickgroupinfo(ngi);
        return NULL;
    }
    expire_memos(mi);
    return mi;
}

/*************************************************************************/

/* Send a memo to a single user.  Returns 1 on success, 0 (and sets
 * `errormsg' to an appropriate message number) on failure.
 */

static int send_memo(const User *source, const char *target, const char *text,
                     const char *channel, int *errormsg_ret)
{
    NickGroupInfo *ngi = NULL;
    MemoInfo *mi;
    Memo *m;
    int error, dummy_errormsg;
    int is_servadmin = is_services_admin(source);
    int retval = 0;

    if (!errormsg_ret)
        errormsg_ret = &dummy_errormsg;

    if (!(mi = get_memoinfo(target, &ngi, &error))) {
        if (error == GMI_FORBIDDEN)
            *errormsg_ret = NICK_X_FORBIDDEN;
        else
            *errormsg_ret = NICK_X_NOT_REGISTERED;

    } else if (error == GMI_SUSPENDED) {
        *errormsg_ret = NICK_X_SUSPENDED_MEMOS;

    } else if (mi->memomax == 0 && !is_servadmin) {
        *errormsg_ret = MEMO_X_GETS_NO_MEMOS;

    } else if (mi->memomax != MEMOMAX_UNLIMITED
               && mi->memos_count >= REALMAX(mi->memomax)
               && !is_servadmin) {
        *errormsg_ret = MEMO_X_HAS_TOO_MANY_MEMOS;

    } else {
        int res = event_emit(receive_memo_event, source, target, ngi,
                                  channel, text);
        if (res > 1) {
            /* Callback reported an error */
            *errormsg_ret = res;
        } else if (res == 1) {
            /* Callback delivered the memo successfully */
            retval = 1;
        } else {
            /* Deliver the memo ourselves */
            ARRAY_EXTEND(mi->memos);
            m = &mi->memos[mi->memos_count-1];
            memset(m->sender, 0, NICKMAX);  // Avoid leaking random data
            strbcpy(m->sender, source->nick);
            if (mi->memos_count > 1) {
                m->number = m[-1].number + 1;
                if (m->number < 1) {
                    int i;
                    ARRAY_FOREACH (i, mi->memos)
                        mi->memos[i].number = i+1;
                }
            } else {
                m->number = 1;
            }
            m->time = time(NULL);
            m->firstread = 0;
            m->channel = channel ? sstrdup(channel) : NULL;
            m->text = sstrdup(text);
            m->flags = MF_UNREAD;
            if (MSExpire)
                m->flags |= MF_EXPIREOK;
            if (ngi && (ngi->flags & NF_MEMO_RECEIVE)) {
                int i;
                ARRAY_FOREACH (i, ngi->nicks) {
                    NickInfo *ni2 = get_nickinfo(ngi->nicks[i]);
                    User *u2 = ni2 ? ni2->user : NULL;
                    if (u2 && user_recognized(u2)) {
                        if (channel) {
                            notice_lang(memoserv_service.nick, u2,
                                        MEMO_NEW_CHAN_MEMO_ARRIVED,
                                        source->nick, channel,
                                        memoserv_service.nick, m->number);
                        } else {
                            notice_lang(memoserv_service.nick, u2,
                                        MEMO_NEW_MEMO_ARRIVED,
                                        source->nick, memoserv_service.nick,
                                        m->number);
                        }
                    }
                    put_nickinfo(ni2);
                }
            } /* if (flags & MEMO_RECEIVE) */
            retval = 1;
        } /* callback returned <=0 */
    }
    put_nickgroupinfo(ngi);
    return retval;
}

/*************************************************************************/

/* Send a memo to all appropriate users on a channel (the founder and any
 * users with access level at least `level').  Returns the number of users
 * to whom the memo was successfully sent.
 */

static int send_chan_memo(User *u, const ChannelInfo *ci, int level,
                          const char *text)
{
    NickGroupInfo *ngi;
    int delivered = 0;
    int i;

    ngi = get_ngi_id(ci->founder);
    if (ngi) {
        delivered += send_memo(u, ngi->nicks[ngi->mainnick], text,
                               ci->name, NULL);
        put_nickgroupinfo(ngi);
    }
    ARRAY_FOREACH (i, ci->access) {
        if (ci->access[i].nickgroup != ci->founder
         && ci->access[i].level >= level
        ) {
            ngi = get_ngi_id(ci->access[i].nickgroup);
            if (ngi) {
                delivered += send_memo(u, ngi->nicks[ngi->mainnick],
                                       text, ci->name, NULL);
                put_nickgroupinfo(ngi);
            }
        }
    }
    return delivered;
}

/*************************************************************************/

/* Display a single memo entry, possibly printing the header first. */

static int list_memo(User *u, int index, MemoInfo *mi, int *sent_header,
                     int new)
{
    Memo *m;
    char timebuf[64];

    if (index < 0 || index >= mi->memos_count)
        return 0;
    if (!*sent_header) {
        notice_lang(memoserv_service.nick, u,
                    new ? MEMO_LIST_NEW_MEMOS : MEMO_LIST_MEMOS,
                    u->nick, memoserv_service.nick);
        notice_lang(memoserv_service.nick, u, MEMO_LIST_HEADER);
        *sent_header = 1;
    }
    m = &mi->memos[index];
    strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                  STRFTIME_DATE_TIME_FORMAT, m->time);
    timebuf[sizeof(timebuf)-1] = 0;     /* just in case */
    notice_lang(memoserv_service.nick, u, MEMO_LIST_FORMAT,
                (m->flags & MF_UNREAD) ? '*' : ' ',
                m->channel ? '#' : ' ',
                (!MSExpire || (m->flags & MF_EXPIREOK)) ? ' ' : '+',
                m->number, m->sender, timebuf);
    return 1;
}

/* List callback. */

static int list_memo_callback(int num, va_list args)
{
    User *u = va_arg(args, User *);
    MemoInfo *mi = va_arg(args, MemoInfo *);
    int *sent_header = va_arg(args, int *);
    int i;

    ARRAY_FOREACH (i, mi->memos) {
        if (mi->memos[i].number == num)
            break;
    }
    /* Range checking done by list_memo() */
    return list_memo(u, i, mi, sent_header, 0);
}

/*************************************************************************/

/* Send a single memo to the given user. */

static int read_memo(User *u, int index, MemoInfo *mi)
{
    Memo *m;
    char timebuf[BUFSIZE];

    if (index < 0 || index >= mi->memos_count)
        return 0;
    m = &mi->memos[index];
    strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                  STRFTIME_DATE_TIME_FORMAT, m->time);
    timebuf[sizeof(timebuf)-1] = 0;
    if (m->channel)
        notice_lang(memoserv_service.nick, u, MEMO_CHAN_HEADER, m->number,
                    m->sender, m->channel, timebuf, memoserv_service.nick, m->number);
    else
        notice_lang(memoserv_service.nick, u, MEMO_HEADER, m->number,
                    m->sender, timebuf, memoserv_service.nick, m->number);
    notice(memoserv_service.nick, u->nick, "%s", m->text);
    m->flags &= ~MF_UNREAD;
    return 1;
}

/* Read callback. */

static int read_memo_callback(int num, va_list args)
{
    User *u = va_arg(args, User *);
    MemoInfo *mi = va_arg(args, MemoInfo *);
    int i;

    ARRAY_FOREACH (i, mi->memos) {
        if (mi->memos[i].number == num)
            break;
    }
    /* Range check done in read_memo */
    return read_memo(u, i, mi);
}

/*************************************************************************/

/* Mark a given memo as non-expiring. */

static int save_memo(User *u, int index, MemoInfo *mi)
{
    if (index < 0 || index >= mi->memos_count)
        return 0;
    mi->memos[index].flags &= ~MF_EXPIREOK;
    return 1;
}

/* Save callback. */

static int save_memo_callback(int num, va_list args)
{
    User *u = va_arg(args, User *);
    MemoInfo *mi = va_arg(args, MemoInfo *);
    int *last = va_arg(args, int *);
    int i;

    ARRAY_FOREACH (i, mi->memos) {
        if (mi->memos[i].number == num)
            break;
    }
    /* Range check done in save_memo */
    if (save_memo(u, i, mi)) {
        *last = num;
        return 1;
    } else {
        return 0;
    }
}

/*************************************************************************/

/* Delete a memo by number.  Return 1 if the memo was found, else 0. */

static int del_memo(MemoInfo *mi, int num)
{
    int i;

    ARRAY_FOREACH (i, mi->memos) {
        if (mi->memos[i].number == num)
            break;
    }
    if (i < mi->memos_count) {
        free(mi->memos[i].channel);
        free(mi->memos[i].text);
        ARRAY_REMOVE(mi->memos, i);
        return 1;
    } else {
        return 0;
    }
}

/* Delete a single memo from a MemoInfo. */

static int del_memo_callback(int num, va_list args)
{
    /* User *u = */ (void) va_arg(args, User *);
    MemoInfo *mi = va_arg(args, MemoInfo *);
    int *last = va_arg(args, int *);

    if (del_memo(mi, num)) {
        *last = num;
        return 1;
    } else {
        return 0;
    }
}

/*************************************************************************/
/*********************** MemoServ command routines ***********************/
/*************************************************************************/

/* Return a help message. */

static void do_help(User *u)
{
    char *cmd = strtok_remaining();

    if (!cmd) {
        struct Service *chanserv = NULL;
        const char *levstr;
        if (module_chanserv)
            chanserv = module_symbol(module_chanserv, "chanserv_service");
        if (module_find("chanserv/access-xop")) {
            if (module_find("chanserv/access-levels"))
                levstr = getstring(u->ngi, CHAN_HELP_REQSOP_LEVXOP);
            else
                levstr = getstring(u->ngi, CHAN_HELP_REQSOP_XOP);
        } else {
            levstr = getstring(u->ngi, CHAN_HELP_REQSOP_LEV);
        }
        notice_help(memoserv_service.nick, u, MEMO_HELP);
        if (MSExpire) {
            notice_help(memoserv_service.nick, u, MEMO_HELP_EXPIRES,
                        maketime(u->ngi,MSExpire,MT_DUALUNIT));
        }
        if (module_find("chanserv/access-levels")) {
            notice_help(memoserv_service.nick, u, MEMO_HELP_END_LEVELS, levstr,
                        chanserv ? chanserv->nick : "ChanServ");
        } else {
            notice_help(memoserv_service.nick, u, MEMO_HELP_END_XOP);
        }
    } else if (event_emit(help_event, u, cmd) > 0) {
        return;
    } else if (stricmp(cmd, "COMMANDS") == 0) {
        notice_help(memoserv_service.nick, u, MEMO_HELP_COMMANDS);
        if (module_find("memoserv/forward"))
            notice_help(memoserv_service.nick, u, MEMO_HELP_COMMANDS_FORWARD);
        if (MSExpire)
            notice_help(memoserv_service.nick, u, MEMO_HELP_COMMANDS_SAVE);
        notice_help(memoserv_service.nick, u, MEMO_HELP_COMMANDS_DEL);
        if (module_find("memoserv/ignore"))
            notice_help(memoserv_service.nick, u, MEMO_HELP_COMMANDS_IGNORE);
        event_emit(help_cmds_event, u, 0);
        if (is_oper(u)) {
            notice_help(memoserv_service.nick, u, MEMO_OPER_HELP_COMMANDS);
            event_emit(help_cmds_event, u, 1);
        }
    } else if (stricmp(cmd, "SET") == 0) {
        notice_help(memoserv_service.nick, u, MEMO_HELP_SET);
        if (module_find("memoserv/forward"))
            notice_help(memoserv_service.nick, u, MEMO_HELP_SET_OPTION_FORWARD);
        notice_help(memoserv_service.nick, u, MEMO_HELP_SET_END);
    } else if (strnicmp(cmd, "SET", 3) == 0
               && isspace(cmd[3])
               && stricmp(cmd+4+strspn(cmd+4," \t"), "NOTIFY") == 0) {
        notice_help(memoserv_service.nick, u, MEMO_HELP_SET_NOTIFY,
                    nickserv_service.nick);
    } else {
        help_cmd(memoserv_service.nick, u, THIS_MODULE, cmd);
    }
}

/*************************************************************************/

/* Send a memo to a nick/channel. */

static void do_send(User *u)
{
    char *target = strtok(NULL, " ");
    char *text = strtok_remaining();
    time_t now = time(NULL);

    if (readonly) {
        notice_lang(memoserv_service.nick, u, MEMO_SEND_DISABLED);

    } else if (!target || !text) {
        syntax_error(memoserv_service.nick, u, "SEND", MEMO_SEND_SYNTAX);

    } else if (MSSendDelay > 0
               && (u && u->lastmemosend+MSSendDelay > now)
               && !is_services_admin(u)) {
        u->lastmemosend = now;
        notice_lang(memoserv_service.nick, u, MEMO_SEND_PLEASE_WAIT,
                    maketime(u->ngi,MSSendDelay,MT_SECONDS));

    } else {
        int delivered = 0, errormsg = INTERNAL_ERROR;

        if (*target == '#') {  /* channel name */
            if (p_get_channelinfo && p_get_ci_level && p_check_access) {
                ChannelInfo *ci = (*p_get_channelinfo)(target);
                if (!ci) {
                    errormsg = CHAN_X_NOT_REGISTERED;
                } else if (ci->levels[CA_MEMO] == ACCLEV_INVALID) {
                    errormsg = MEMO_X_GETS_NO_MEMOS;
                } else if ((ci->flags & CF_MEMO_RESTRICTED)
                        && !(*p_check_access)(u, ci, CA_MEMO)) {
                    errormsg = PERMISSION_DENIED;
                } else {
                    int level = (*p_get_ci_level)(ci, CA_MEMO);
                    errormsg = MEMO_SEND_FAILED;
                    delivered = send_chan_memo(u, ci, level, text);
                }
                (*p_put_channelinfo)(ci);
            } else {  /* ChanServ module not available */
                errormsg = MEMO_SEND_CHAN_NOT_AVAIL;
            }
        } else {  /* nickname */
            delivered = send_memo(u, target, text, NULL, &errormsg);
        }
        if (delivered) {
            notice_lang(memoserv_service.nick, u, MEMO_SENT, target);
            u->lastmemosend = now;
        } else {
            notice_lang(memoserv_service.nick, u, errormsg, target);
        }

    } /* if command is valid */
}

/*************************************************************************/

/* List memos for the source nick or given channel. */

static void do_list(User *u)
{
    MemoInfo *mi = &u->ngi->memos;
    char *param;
    int i;

    param = strtok(NULL, " ");
    mi = &u->ngi->memos;
    if (param && !isdigit(*param) && stricmp(param, "NEW") != 0) {
        syntax_error(memoserv_service.nick, u, "LIST", MEMO_LIST_SYNTAX);
    } else if (mi->memos_count == 0) {
        notice_lang(memoserv_service.nick, u, MEMO_HAVE_NO_MEMOS);
    } else {
        int sent_header = 0;
        if (param && isdigit(*param)) {
            process_numlist(param, NULL, list_memo_callback, u, mi,
                            &sent_header);
        } else {
            if (param) {
                ARRAY_FOREACH (i, mi->memos) {
                    if (mi->memos[i].flags & MF_UNREAD)
                        break;
                }
                if (i == mi->memos_count)
                    notice_lang(memoserv_service.nick, u, MEMO_HAVE_NO_NEW_MEMOS);
            }
            ARRAY_FOREACH (i, mi->memos) {
                if (param && !(mi->memos[i].flags & MF_UNREAD))
                    continue;
                list_memo(u, i, mi, &sent_header, param != NULL);
            }
        }
    }
}

/*************************************************************************/

/* Read memos. */

static void do_read(User *u)
{
    MemoInfo *mi = &u->ngi->memos;
    char *numstr;
    int num, count;

    numstr = strtok(NULL, " ");
    num = numstr ? atoi(numstr) : -1;
    if (!numstr || (stricmp(numstr,"LAST") != 0 && stricmp(numstr,"NEW") != 0
                    && num <= 0)) {
        syntax_error(memoserv_service.nick, u, "READ", MEMO_READ_SYNTAX);
    } else if (mi->memos_count == 0) {
        notice_lang(memoserv_service.nick, u, MEMO_HAVE_NO_MEMOS);
    } else {
        int i;

        if (stricmp(numstr, "NEW") == 0) {
            int readcount = 0;
            ARRAY_FOREACH (i, mi->memos) {
                if (mi->memos[i].flags & MF_UNREAD) {
                    read_memo(u, i, mi);
                    readcount++;
                }
            }
            if (!readcount)
                notice_lang(memoserv_service.nick, u, MEMO_HAVE_NO_NEW_MEMOS);
        } else if (stricmp(numstr, "LAST") == 0) {
            read_memo(u, mi->memos_count-1, mi);
        } else {        /* number[s] */
            if (!process_numlist(numstr, &count, read_memo_callback, u, mi)) {
                if (count == 1)
                    notice_lang(memoserv_service.nick, u, MEMO_DOES_NOT_EXIST, num);
                else
                    notice_lang(memoserv_service.nick, u, MEMO_LIST_NOT_FOUND);
            }
        }
    }
}

/*************************************************************************/

/* Save memos (mark them as non-expiring). */

static void do_save(User *u)
{
    MemoInfo *mi = &u->ngi->memos;
    char *numstr;
    int num, count;

    numstr = strtok(NULL, " ");
    num = numstr ? atoi(numstr) : -1;
    if (!numstr || num <= 0) {
        syntax_error(memoserv_service.nick, u, "SAVE", MEMO_SAVE_SYNTAX);
    } else if (mi->memos_count == 0) {
        notice_lang(memoserv_service.nick, u, MEMO_HAVE_NO_MEMOS);
    } else {
        int last = 0;
        int savecount =
            process_numlist(numstr, &count, save_memo_callback, u, mi, &last);
        if (savecount) {
            /* Some memos got saved. */
            if (savecount > 1)
                notice_lang(memoserv_service.nick, u, MEMO_SAVED_SEVERAL, savecount);
            else
                notice_lang(memoserv_service.nick, u, MEMO_SAVED_ONE, last);
        } else {
            /* No matching memos found. */
            if (count == 1)
                notice_lang(memoserv_service.nick, u, MEMO_DOES_NOT_EXIST, num);
            else
                notice_lang(memoserv_service.nick, u, MEMO_LIST_NOT_FOUND);
        }
    }
}

/*************************************************************************/

/* Delete memos. */

static void do_del(User *u)
{
    MemoInfo *mi = &u->ngi->memos;
    char *numstr;
    int last, i;
    int delcount, count;

    numstr = strtok(NULL, " ");
    if (!numstr || (!isdigit(*numstr) && stricmp(numstr, "ALL") != 0)) {
        syntax_error(memoserv_service.nick, u, "DEL", MEMO_DEL_SYNTAX);
    } else if (mi->memos_count == 0) {
        notice_lang(memoserv_service.nick, u, MEMO_HAVE_NO_MEMOS);
    } else {
        if (isdigit(*numstr)) {
            /* Delete a specific memo or memos. */
            delcount = process_numlist(numstr, &count, del_memo_callback,
                                       u, mi, &last);
            if (delcount) {
                /* Some memos got deleted. */
                if (delcount > 1)
                    notice_lang(memoserv_service.nick, u, MEMO_DELETED_SEVERAL, delcount);
                else
                    notice_lang(memoserv_service.nick, u, MEMO_DELETED_ONE, last);
            } else {
                /* No memos were deleted. */
                if (count == 1)
                    notice_lang(memoserv_service.nick, u, MEMO_DOES_NOT_EXIST,
                                atoi(numstr));
                else
                    notice_lang(memoserv_service.nick, u, MEMO_DELETED_NONE);
            }
        } else {
            /* Delete all memos. */
            ARRAY_FOREACH (i, mi->memos) {
                free(mi->memos[i].channel);
                free(mi->memos[i].text);
            }
            free(mi->memos);
            mi->memos = NULL;
            mi->memos_count = 0;
            notice_lang(memoserv_service.nick, u, MEMO_DELETED_ALL);
        }
    }
}

/*************************************************************************/

static void do_renumber(User *u)
{
    char *s;
    int i;

    if ((s = strtok_remaining()) != NULL) {
        if (is_services_admin(u))
            notice_lang(memoserv_service.nick, u, MEMO_RENUMBER_ONLY_YOU);
        else
            notice_lang(memoserv_service.nick, u, SYNTAX_ERROR, "RENUMBER");
        notice_lang(memoserv_service.nick, u, MORE_INFO, memoserv_service.nick, "RENUMBER");
        return;
    }
    ARRAY_FOREACH (i, u->ngi->memos.memos)
        u->ngi->memos.memos[i].number = i+1;
    notice_lang(memoserv_service.nick, u, MEMO_RENUMBER_DONE);
}

/*************************************************************************/

static void do_set(User *u)
{
    char *cmd    = strtok(NULL, " ");
    char *param  = strtok_remaining();
    MemoInfo *mi = &u->ngi->memos;

    if (readonly) {
        notice_lang(memoserv_service.nick, u, MEMO_SET_DISABLED);
        return;
    }
    if (!cmd || !param) {
        syntax_error(memoserv_service.nick, u, "SET", MEMO_SET_SYNTAX);
    } else if (!user_identified(u)) {
        notice_lang(memoserv_service.nick, u, NICK_IDENTIFY_REQUIRED, nickserv_service.nick);
        return;
    } else if (event_emit(set_event, u, mi, cmd, param) > 0) {
        return;
    } else if (stricmp(cmd, "NOTIFY") == 0) {
        do_set_notify(u, mi, param);
    } else if (stricmp(cmd, "LIMIT") == 0) {
        do_set_limit(u, mi, param);
    } else {
        notice_lang(memoserv_service.nick, u, MEMO_SET_UNKNOWN_OPTION, strupper(cmd));
        notice_lang(memoserv_service.nick, u, MORE_INFO, memoserv_service.nick, "SET");
    }
}

/*************************************************************************/

static void do_set_notify(User *u, MemoInfo *mi, char *param)
{
    if (stricmp(param, "ON") == 0) {
        u->ngi->flags |= NF_MEMO_SIGNON | NF_MEMO_RECEIVE;
        notice_lang(memoserv_service.nick, u, MEMO_SET_NOTIFY_ON, memoserv_service.nick);
    } else if (stricmp(param, "LOGON") == 0) {
        u->ngi->flags |= NF_MEMO_SIGNON;
        u->ngi->flags &= ~NF_MEMO_RECEIVE;
        notice_lang(memoserv_service.nick, u, MEMO_SET_NOTIFY_LOGON, memoserv_service.nick);
    } else if (stricmp(param, "NEW") == 0) {
        u->ngi->flags &= ~NF_MEMO_SIGNON;
        u->ngi->flags |= NF_MEMO_RECEIVE;
        notice_lang(memoserv_service.nick, u, MEMO_SET_NOTIFY_NEW, memoserv_service.nick);
    } else if (stricmp(param, "OFF") == 0) {
        u->ngi->flags &= ~(NF_MEMO_SIGNON | NF_MEMO_RECEIVE);
        notice_lang(memoserv_service.nick, u, MEMO_SET_NOTIFY_OFF, memoserv_service.nick);
    } else {
        syntax_error(memoserv_service.nick, u, "SET NOTIFY", MEMO_SET_NOTIFY_SYNTAX);
        return;
    }
}

/*************************************************************************/

/* Regular user parameters: number
 * Services admin parameters: [nick] {number|NONE|DEFAULT} [HARD]
 */

static void do_set_limit(User *u, MemoInfo *mi, char *param)
{
    char *p1 = strtok(param, " ");
    char *p2 = strtok(NULL, " ");
    char *user = NULL;
    int limit;
    NickInfo *ni = u->ni;
    NickGroupInfo *ngi = u->ngi;
    int is_servadmin = is_services_admin(u);

    hold_nickinfo(ni);
    hold_nickgroupinfo(ngi);

    if (is_servadmin) {
        if (p2 && stricmp(p2, "HARD") != 0) {
            put_nickinfo(ni);
            put_nickgroupinfo(ngi);
            if (!(ni = get_nickinfo(p1))) {
                notice_lang(memoserv_service.nick, u, NICK_X_NOT_REGISTERED, p1);
                return;
            }
            if (!(ngi = get_ngi(ni))) {
                notice_lang(memoserv_service.nick, u, INTERNAL_ERROR);
                put_nickinfo(ni);
                return;
            }
            user = p1;
            mi = &ngi->memos;
            p1 = p2;
            p2 = strtok(NULL, " ");
        } else if (!p1) {
            syntax_error(memoserv_service.nick, u, "SET LIMIT",
                         MEMO_SET_LIMIT_OPER_SYNTAX);
            return;
        }
        if ((!isdigit(*p1) && stricmp(p1, "NONE") != 0
             && stricmp(p1, "DEFAULT") != 0)
            || (p2 && stricmp(p2, "HARD") != 0)
        ) {
            syntax_error(memoserv_service.nick, u, "SET LIMIT",
                         MEMO_SET_LIMIT_OPER_SYNTAX);
            return;
        }
        if (p2)
            ngi->flags |= NF_MEMO_HARDMAX;
        else
            ngi->flags &= ~NF_MEMO_HARDMAX;
        if (stricmp(p1, "NONE") == 0) {
            limit = MEMOMAX_UNLIMITED;
        } else if (stricmp(p1, "DEFAULT") == 0) {
            limit = MEMOMAX_DEFAULT;
        } else {
            limit = (int)atolsafe(p1, 0, INT_MAX);
            if (limit < 0) {
                syntax_error(memoserv_service.nick, u, "SET LIMIT",
                             MEMO_SET_LIMIT_OPER_SYNTAX);
                return;
            } else if (limit > MEMOMAX_MAX) {
                notice_lang(memoserv_service.nick, u, MEMO_SET_LIMIT_OVERFLOW,
                            MEMOMAX_MAX);
                limit = MEMOMAX_MAX;
            }
        }
    } else {
        if (!p1 || p2 || !isdigit(*p1)) {
            syntax_error(memoserv_service.nick, u, "SET LIMIT", MEMO_SET_LIMIT_SYNTAX);
            return;
        }
        if (ngi->flags & NF_MEMO_HARDMAX) {
            notice_lang(memoserv_service.nick, u, MEMO_SET_YOUR_LIMIT_FORBIDDEN);
            return;
        }
        limit = (int)atolsafe(p1, 0, INT_MAX);
        if (limit < 0) {
            syntax_error(memoserv_service.nick, u, "SET LIMIT", MEMO_SET_LIMIT_SYNTAX);
            return;
        } else if (MSMaxMemos > 0 && limit > MSMaxMemos) {
            notice_lang(memoserv_service.nick, u, MEMO_SET_YOUR_LIMIT_TOO_HIGH,
                        MSMaxMemos);
            return;
        } else if (limit > MEMOMAX_MAX) {
            notice_lang(memoserv_service.nick, u, MEMO_SET_LIMIT_OVERFLOW, MEMOMAX_MAX);
            limit = MEMOMAX_MAX;
        }
    }

    mi->memomax = limit;

    if (limit > 0) {
        if (ni == u->ni)
            notice_lang(memoserv_service.nick, u, MEMO_SET_YOUR_LIMIT, limit);
        else
            notice_lang(memoserv_service.nick, u, MEMO_SET_LIMIT, user, limit);
    } else if (limit == 0) {
        if (ni == u->ni)
            notice_lang(memoserv_service.nick, u, MEMO_SET_YOUR_LIMIT_ZERO);
        else
            notice_lang(memoserv_service.nick, u, MEMO_SET_LIMIT_ZERO, user);
    } else if (limit == MEMOMAX_DEFAULT) {
        if (ni == u->ni)
            notice_lang(memoserv_service.nick, u, MEMO_SET_YOUR_LIMIT_DEFAULT,
                        MSMaxMemos);
        else
            notice_lang(memoserv_service.nick, u, MEMO_SET_LIMIT_DEFAULT, user,
                        MSMaxMemos);
    } else {
        if (ni == u->ni)
            notice_lang(memoserv_service.nick, u, MEMO_UNSET_YOUR_LIMIT);
        else
            notice_lang(memoserv_service.nick, u, MEMO_UNSET_LIMIT, user);
    }

    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

static void do_info(User *u)
{
    MemoInfo *mi;
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;
    char *name = strtok(NULL, " ");
    int is_servadmin = is_services_admin(u);
    int max = 0;
    int is_hardmax = 0;

    if (is_servadmin && name) {
        ni = get_nickinfo(name);
        if (!ni) {
            notice_lang(memoserv_service.nick, u, NICK_X_NOT_REGISTERED, name);
            return;
        } else if (ni->status & NS_VERBOTEN) {
            notice_lang(memoserv_service.nick, u, NICK_X_FORBIDDEN, name);
            put_nickinfo(ni);
            return;
        }
        ngi = get_ngi(ni);
        if (!ngi) {
            notice_lang(memoserv_service.nick, u, INTERNAL_ERROR);
            put_nickinfo(ni);
            return;
        }
        mi = &ngi->memos;
        is_hardmax = ngi->flags & NF_MEMO_HARDMAX ? 1 : 0;
    } else { /* !name or !servadmin */
        if (!user_identified(u)) {
            notice_lang(memoserv_service.nick, u, NICK_IDENTIFY_REQUIRED, nickserv_service.nick);
            return;
        }
        ni = u->ni;
        if (ni)
            hold_nickinfo(ni);
        ngi = u->ngi;
        if (ngi)
            hold_nickgroupinfo(ngi);
        mi = &u->ngi->memos;
    }
    max = REALMAX(mi->memomax);

    if (ni != u->ni) {
        /* Report info for a nick other than the caller. */
        if (!mi->memos_count) {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_X_NO_MEMOS, name);
        } else if (mi->memos_count == 1) {
            if (mi->memos[0].flags & MF_UNREAD)
                notice_lang(memoserv_service.nick, u, MEMO_INFO_X_MEMO_UNREAD, name);
            else
                notice_lang(memoserv_service.nick, u, MEMO_INFO_X_MEMO, name);
        } else {
            int count = 0, i;
            ARRAY_FOREACH (i, mi->memos) {
                if (mi->memos[i].flags & MF_UNREAD)
                    count++;
            }
            if (count == mi->memos_count)
                notice_lang(memoserv_service.nick, u, MEMO_INFO_X_MEMOS_ALL_UNREAD,
                        name, count);
            else if (count == 0)
                notice_lang(memoserv_service.nick, u, MEMO_INFO_X_MEMOS,
                        name, mi->memos_count);
            else if (count == 0)
                notice_lang(memoserv_service.nick, u, MEMO_INFO_X_MEMOS_ONE_UNREAD,
                        name, mi->memos_count);
            else
                notice_lang(memoserv_service.nick, u, MEMO_INFO_X_MEMOS_SOME_UNREAD,
                        name, mi->memos_count, count);
        }
        if (max >= 0) {
            if (is_hardmax)
                notice_lang(memoserv_service.nick, u, MEMO_INFO_X_HARD_LIMIT, name, max);
            else
                notice_lang(memoserv_service.nick, u, MEMO_INFO_X_LIMIT, name, max);
        } else {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_X_NO_LIMIT, name);
        }
        if ((ngi->flags & NF_MEMO_RECEIVE) && (ngi->flags & NF_MEMO_SIGNON)) {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_X_NOTIFY_ON, name);
        } else if (ngi->flags & NF_MEMO_RECEIVE) {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_X_NOTIFY_RECEIVE, name);
        } else if (ngi->flags & NF_MEMO_SIGNON) {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_X_NOTIFY_SIGNON, name);
        } else {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_X_NOTIFY_OFF, name);
        }

    } else { /* ni == u->ni */

        if (!mi->memos_count) {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_NO_MEMOS);
        } else if (mi->memos_count == 1) {
            if (mi->memos[0].flags & MF_UNREAD)
                notice_lang(memoserv_service.nick, u, MEMO_INFO_MEMO_UNREAD);
            else
                notice_lang(memoserv_service.nick, u, MEMO_INFO_MEMO);
        } else {
            int count = 0, i;
            ARRAY_FOREACH (i, mi->memos) {
                if (mi->memos[i].flags & MF_UNREAD)
                    count++;
            }
            if (count == mi->memos_count)
                notice_lang(memoserv_service.nick, u, MEMO_INFO_MEMOS_ALL_UNREAD, count);
            else if (count == 0)
                notice_lang(memoserv_service.nick, u, MEMO_INFO_MEMOS, mi->memos_count);
            else if (count == 1)
                notice_lang(memoserv_service.nick, u, MEMO_INFO_MEMOS_ONE_UNREAD,
                        mi->memos_count);
            else
                notice_lang(memoserv_service.nick, u, MEMO_INFO_MEMOS_SOME_UNREAD,
                        mi->memos_count, count);
        }
        if (max == 0) {
            if (!is_servadmin && is_hardmax)
                notice_lang(memoserv_service.nick, u, MEMO_INFO_HARD_LIMIT_ZERO);
            else
                notice_lang(memoserv_service.nick, u, MEMO_INFO_LIMIT_ZERO);
        } else if (max > 0) {
            if (!is_servadmin && is_hardmax)
                notice_lang(memoserv_service.nick, u, MEMO_INFO_HARD_LIMIT, max);
            else
                notice_lang(memoserv_service.nick, u, MEMO_INFO_LIMIT, max);
        } else {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_NO_LIMIT);
        }
        if ((ngi->flags & NF_MEMO_RECEIVE) && (ngi->flags & NF_MEMO_SIGNON)) {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_NOTIFY_ON);
        } else if (ngi->flags & NF_MEMO_RECEIVE) {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_NOTIFY_RECEIVE);
        } else if (ngi->flags & NF_MEMO_SIGNON) {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_NOTIFY_SIGNON);
        } else {
            notice_lang(memoserv_service.nick, u, MEMO_INFO_NOTIFY_OFF);
        }

    } /* if (ni != u->ni) */

    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int do_MSAlias(const char *filename, int linenum, char *param);

static ConfigDirective memoserv_config[] = {
    { "MSAlias",          { { CD_FUNC, 0, do_MSAlias } } },
    { "MSExpire",         { { CD_TIME, 0, &MSExpire } } },
    { "MSExpireDelay",    { { CD_TIME, 0, &MSExpireDelay } } },
    { "MSMaxMemos",       { { CD_POSINT, 0, &MSMaxMemos } } },
    { "MSSendDelay",      { { CD_TIME, 0, &MSSendDelay } } },
    { NULL }
};

static Command *cmd_SAVE = NULL;  /* For restoring if !MSExpire */
static int old_HELP_LIST = -1;    /* For restoring if MSExpire */

/*************************************************************************/

static int do_MSAlias(const char *filename, int linenum, char *param)
{
    static Alias *new_aliases = NULL;
    static int new_aliases_count = 0;
    int i;
    char *s;

    if (!filename) {
        switch (linenum) {
          case CDFUNC_INIT:
            /* Prepare for reading config file: clear out "new" array */
            ARRAY_FOREACH (i, new_aliases) {
                free(new_aliases[i].alias);
                free(new_aliases[i].command);
            }
            free(new_aliases);
            new_aliases = NULL;
            new_aliases_count = 0;
            break;
          case CDFUNC_SET:
            /* Copy data to config variables */
            ARRAY_FOREACH (i, aliases) {
                free(aliases[i].alias);
                free(aliases[i].command);
            }
            free(aliases);
            aliases = new_aliases;
            aliases_count = new_aliases_count;
            new_aliases = NULL;
            new_aliases_count = 0;
            break;
          case CDFUNC_DECONFIG:
            /* Clear out config variables */
            ARRAY_FOREACH (i, aliases) {
                free(aliases[i].alias);
                free(aliases[i].command);
            }
            free(aliases);
            aliases = NULL;
            aliases_count = 0;
            break;
        }
        return 1;
    } /* if (!filename) */

    s = strchr(param, '=');
    if (!s) {
        config_error(filename, linenum, "Missing = in MSAlias parameter");
        return 0;
    }
    *s++ = 0;
    ARRAY_EXTEND(new_aliases);
    new_aliases[new_aliases_count-1].alias = sstrdup(param);
    new_aliases[new_aliases_count-1].command = sstrdup(s);
    return 1;
}

/*************************************************************************/

/* ChanServ is optional: channel memos use its functions while it is
 * loaded. */

static int do_module_loaded(Module *mod, const char *modname)
{
    if (strcmp(modname, "chanserv/main") == 0) {
        module_chanserv = mod;
        p_get_channelinfo = module_symbol(mod, "get_channelinfo");
        p_put_channelinfo = module_symbol(mod, "put_channelinfo");
        p_get_ci_level = module_symbol(mod, "get_ci_level");
        p_check_access = module_symbol(mod, "check_access");
        if (!p_get_channelinfo || !p_put_channelinfo || !p_get_ci_level
         || !p_check_access
        ) {
            module_log("ChanServ symbol(s) not found, channel memos will"
                       " not be available");
        }
    }
    return 0;
}

static int do_module_unloaded(Module *mod)
{
    if (mod == module_chanserv) {
        p_get_channelinfo = NULL;
        p_put_channelinfo = NULL;
        p_get_ci_level = NULL;
        p_check_access = NULL;
        module_chanserv = NULL;
    }
    return 0;
}

/*************************************************************************/

static void memoserv_rehash(Module *module)
{
    if (old_HELP_LIST >= 0) {
        mapstring(MEMO_HELP_LIST, old_HELP_LIST);
        old_HELP_LIST = -1;
    }
    if (MSExpire)
        old_HELP_LIST = mapstring(MEMO_HELP_LIST, MEMO_HELP_LIST_EXPIRE);
}

/*************************************************************************/

/* EVENT_SERVER_EOB_ACK: the uplink's burst is over, so join the
 * serverinfo channel (see service.h). */
static int do_eob_ack(void)
{
    service_join_channel(&memoserv_service);
    return 0;
}

static int memoserv_init(Module *module)
{
    Command *cmd;
    Module *chanserv;

    if (!new_commandlist(module) || !register_commands(module, cmds)) {
        module_log("Unable to register commands");
        return 0;
    }
    if (MSExpire) {
        old_HELP_LIST = mapstring(MEMO_HELP_LIST, MEMO_HELP_LIST_EXPIRE);
    } else {
        /* Disable SAVE command if no expiration */
        cmd_SAVE = lookup_cmd(module, "SAVE");
        if (cmd_SAVE)
            cmd_SAVE->name = "";
    }

    command_event = event_declare(module, MEMOSERV_EVENT_COMMAND);
    receive_memo_event = event_declare(module, MEMOSERV_EVENT_RECEIVE_MEMO);
    help_event = event_declare(module, MEMOSERV_EVENT_HELP);
    help_cmds_event = event_declare(module, MEMOSERV_EVENT_HELP_COMMANDS);
    set_event = event_declare(module, MEMOSERV_EVENT_SET);
    if (!command_event || !receive_memo_event || !help_event
     || !help_cmds_event || !set_event) {
        module_log("Unable to declare events");
        return 0;
    }

    if (!event_attach(module, EVENT_MODULE_LOADED, do_module_loaded)
     || !event_attach(module, EVENT_MODULE_UNLOADED, do_module_unloaded)
     || !event_attach(module, EVENT_SERVER_EOB_ACK, do_eob_ack)
     || !event_attach(module, EVENT_MESSAGE_RECEIVE, do_receive_message)
     || !event_attach(module, NICKSERV_EVENT_IDENTIFIED, do_nick_identified)
     || !event_attach(module, NICKSERV_EVENT_USER_VALIDATED,
                      do_user_validated)
    ) {
        module_log("Unable to attach event handlers");
        return 0;
    }

    if ((chanserv = module_find("chanserv/main")) != NULL)
        do_module_loaded(chanserv, "chanserv/main");

    cmd = lookup_cmd(module, "SET NOTIFY");
    if (cmd)
        cmd->help_param1 = nickserv_service.nick;
    cmd = lookup_cmd(module, "SET LIMIT");
    if (cmd) {
        cmd->help_param1 = (char *)(long)MSMaxMemos;
        cmd->help_param2 = (char *)(long)MSMaxMemos;
    }

    return 1;
}

/*************************************************************************/

static int memoserv_fini(Module *module, int shutdown)
{
    if (cmd_SAVE) {
        cmd_SAVE->name = "SAVE";
        cmd_SAVE = NULL;
    }
    if (old_HELP_LIST >= 0) {
        mapstring(MEMO_HELP_LIST, old_HELP_LIST);
        old_HELP_LIST = -1;
    }
    unregister_commands(module, cmds);
    del_commandlist(module);
    return 1;
}

/*************************************************************************/

/* MemoServName = <nick>, <description>; in the module block. */
struct Service memoserv_service = {
    .directive = "MemoServName",
    .flags = SERVICE_OPER,
    .on_message = memoserv_message,
};

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "MemoServ: memos between users and to channels",
    .requires = MODULE_REQUIRES("nickserv/main"),
    .config = memoserv_config,
    .services = MODULE_SERVICES(&memoserv_service),
    .init = memoserv_init,
    .fini = memoserv_fini,
    .rehash = memoserv_rehash,
};

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
