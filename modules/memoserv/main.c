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

#include "memoserv.h"

/*************************************************************************/

static Module *module_nickserv;
static Module *module_chanserv;

/* Imports */
static ChannelInfo *(*p_get_channelinfo)(const char *channel);
static ChannelInfo *(*p_put_channelinfo)(ChannelInfo *ci);
static int (*p_get_ci_level)(const ChannelInfo *ci, int what);
static int (*p_check_access)(const User *user, const ChannelInfo *ci, int what);

static int cb_command      = -1;
static int cb_receive_memo = -1;
static int cb_help         = -1;
static int cb_help_cmds    = -1;
static int cb_set          = -1;

       char * s_MemoServ;
static char * desc_MemoServ;
       int32  MSMaxMemos;
static time_t MSExpire;
static time_t MSExpireDelay;
static time_t MSSendDelay;
EXPORT_VAR(int32,MSMaxMemos)

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
/**************************** Database stuff *****************************/
/*************************************************************************/

/* See nickserv/autojoin.c for why we don't create our own table. */

/* Temporary structure for loading/saving access records */
typedef struct {
    uint32 nickgroup;
    Memo memo;
} DBRecord;
static DBRecord dbrec_static;

/* Iterators for first/next routines */
static NickGroupInfo *db_ngi_iterator;
static int db_array_iterator;

/*************************************************************************/

/* Table access routines */

static void *new_memo(void)
{
    return memset(&dbrec_static, 0, sizeof(dbrec_static));
}

static void free_memo(void *record)
{
    free(((DBRecord *)record)->memo.channel);
    free(((DBRecord *)record)->memo.text);
}

static void insert_memo(void *record)
{
    DBRecord *dbrec = record;
    NickGroupInfo *ngi = get_nickgroupinfo(dbrec->nickgroup);
    if (!ngi) {
        module_log("Discarding memo for missing nickgroup %u: (%s) %s",
                   dbrec->nickgroup, dbrec->memo.sender, dbrec->memo.text);
        free_memo(record);
    } else {
        ARRAY_EXTEND(ngi->memos.memos);
        ngi->memos.memos[ngi->memos.memos_count-1] = dbrec->memo;
    }
}

static void *next_memo(void)
{
    while (db_ngi_iterator
        && db_array_iterator >= db_ngi_iterator->memos.memos_count
    ) {
        db_ngi_iterator = next_nickgroupinfo();
        db_array_iterator = 0;
    }
    if (db_ngi_iterator) {
        dbrec_static.nickgroup = db_ngi_iterator->id;
        dbrec_static.memo = db_ngi_iterator->memos.memos[db_array_iterator++];
        return &dbrec_static;
    } else {
        return NULL;
    }
}

static void *first_memo(void)
{
    db_ngi_iterator = first_nickgroupinfo();
    db_array_iterator = 0;
    return next_memo();
}

/*************************************************************************/

/* Database table definition */

#define FIELD(name,type,...) \
    { #name, type, offsetof(DBRecord,name) , ##__VA_ARGS__ }
#define FIELD_MEMO(name,type,...) \
    { #name, type, offsetof(DBRecord,memo)+offsetof(Memo,name) , \
      ##__VA_ARGS__ }

static DBField memo_dbfields[] = {
    FIELD(nickgroup,      DBTYPE_UINT32),
    FIELD_MEMO(number,    DBTYPE_UINT32),
    FIELD_MEMO(flags,     DBTYPE_INT16),
    FIELD_MEMO(time,      DBTYPE_TIME),
    FIELD_MEMO(firstread, DBTYPE_TIME),
    FIELD_MEMO(sender,    DBTYPE_BUFFER, NICKMAX),
    FIELD_MEMO(channel,   DBTYPE_STRING),
    FIELD_MEMO(text,      DBTYPE_STRING),
    { NULL }
};

static DBTable memo_dbtable = {
    .name    = "memo",
    .newrec  = new_memo,
    .freerec = free_memo,
    .insert  = insert_memo,
    .first   = first_memo,
    .next    = next_memo,
    .fields  = memo_dbfields,
};

#undef FIELD
#undef FIELD_MEMO

/*************************************************************************/
/***************************** Main routines *****************************/
/*************************************************************************/

/* Introduce the MemoServ pseudoclient. */

static int introduce_memoserv(const char *nick)
{
    if (!nick || irc_stricmp(nick, s_MemoServ) == 0) {
        send_pseudo_nick(s_MemoServ, desc_MemoServ, PSEUDO_OPER);
        if (nick)
            return 1;
    }
    return 0;
}

/*************************************************************************/

/* memoserv:  Main MemoServ routine.
 *            Note that the User structure passed to the do_* routines will
 *            always be valid (non-NULL) and, except for the HELP command,
 *            will always have valid NickInfo and NickGroupInfo pointers in
 *            the `ni' and `ngi' fields.
 */

static int memoserv(const char *source, const char *target, char *buf)
{
    char *cmd;
    User *u = get_user(source);

    if (irc_stricmp(target, s_MemoServ) != 0)
        return 0;

    if (!u) {
        module_log("user record for %s not found", source);
        notice(s_MemoServ, source, getstring(NULL,INTERNAL_ERROR));
        return 1;
    }

    cmd = strtok(buf, " ");
    if (!cmd) {
        return 1;
    } else if (stricmp(cmd, "\1PING") == 0) {
        const char *s;
        if (!(s = strtok_remaining()))
            s = "\1";
        notice(s_MemoServ, source, "\1PING %s", s);
    } else {
        int i;
        ARRAY_FOREACH (i, aliases) {
            if (stricmp(cmd, aliases[i].alias) == 0) {
                cmd = aliases[i].command;
                break;
            }
        }
        if (!valid_ngi(u) && stricmp(cmd, "HELP") != 0)
            notice_lang(s_MemoServ, u, NICK_NOT_REGISTERED_HELP, s_NickServ);
        else if (!user_identified(u) && stricmp(cmd, "HELP") != 0)
            notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
        else if (call_callback_2(cb_command, u, cmd) <= 0)
            run_cmd(s_MemoServ, u, THIS_MODULE, cmd);
    }
    return 1;
}

/*************************************************************************/

/* Return a /WHOIS response for MemoServ. */

static int memoserv_whois(const char *source, char *who, char *extra)
{
    if (irc_stricmp(who, s_MemoServ) != 0)
        return 0;
    send_cmd(ServerName, "311 %s %s %s %s * :%s", source, who,
             ServiceUser, ServiceHost, desc_MemoServ);
    send_cmd(ServerName, "312 %s %s %s :%s", source, who,
             ServerName, ServerDesc);
    send_cmd(ServerName, "318 %s %s End of /WHOIS response.", source, who);
    return 1;
}

/*************************************************************************/

/* Callback for users connecting to the network. */

static int do_user_create(User *user, int ac, char **av)
{
    if (user_recognized(user))
        check_memos(user);
    return 0;
}

/*************************************************************************/

/* Callback for users changing nicknames. */

static int do_user_nickchange(User *user, const char *oldnick)
{
    NickInfo *old_ni;
    uint32 old_nickgroup, new_nickgroup;

    /* user->{ni,ngi} are already changed, so look it up again */
    old_ni = get_nickinfo(oldnick);
    old_nickgroup = old_ni ? old_ni->nickgroup : 0;
    put_nickinfo(old_ni);
    new_nickgroup = user->ngi ? user->ngi->id : 0;
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

/* Callback for NickServ REGISTER/LINK check; we disallow
 * registration/linking of the MemoServ pseudoclient nickname.
 */

static int do_reglink_check(const User *u, const char *nick,
                            const char *pass, const char *email)
{
    return irc_stricmp(nick, s_MemoServ) == 0;
}

/*************************************************************************/

/* Callback for users identifying for nicks. */

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
        notice_lang(s_MemoServ, u,
                newcnt==1 ? MEMO_HAVE_NEW_MEMO : MEMO_HAVE_NEW_MEMOS, newcnt);
        if (newcnt == 1 && (ngi->memos.memos[i-1].flags & MF_UNREAD)) {
            notice_lang(s_MemoServ, u, MEMO_TYPE_READ_LAST, s_MemoServ);
        } else if (newcnt == 1) {
            ARRAY_FOREACH (i, ngi->memos.memos) {
                if (ngi->memos.memos[i].flags & MF_UNREAD)
                    break;
            }
            notice_lang(s_MemoServ, u, MEMO_TYPE_READ_NUM, s_MemoServ,
                        ngi->memos.memos[i].number);
        } else {
            notice_lang(s_MemoServ, u, MEMO_TYPE_LIST_NEW, s_MemoServ);
        }
    }
    max = REALMAX(ngi->memos.memomax);
    if (max > 0 && ngi->memos.memos_count >= max) {
        if (ngi->memos.memos_count > max)
            notice_lang(s_MemoServ, u, MEMO_OVER_LIMIT, max);
        else
            notice_lang(s_MemoServ, u, MEMO_AT_LIMIT, max);
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
        int res = call_callback_5(cb_receive_memo, source, target, ngi,
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
                            notice_lang(s_MemoServ, u2,
                                        MEMO_NEW_CHAN_MEMO_ARRIVED,
                                        source->nick, channel,
                                        s_MemoServ, m->number);
                        } else {
                            notice_lang(s_MemoServ, u2,
                                        MEMO_NEW_MEMO_ARRIVED,
                                        source->nick, s_MemoServ,
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
        notice_lang(s_MemoServ, u,
                    new ? MEMO_LIST_NEW_MEMOS : MEMO_LIST_MEMOS,
                    u->nick, s_MemoServ);
        notice_lang(s_MemoServ, u, MEMO_LIST_HEADER);
        *sent_header = 1;
    }
    m = &mi->memos[index];
    strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                  STRFTIME_DATE_TIME_FORMAT, m->time);
    timebuf[sizeof(timebuf)-1] = 0;     /* just in case */
    notice_lang(s_MemoServ, u, MEMO_LIST_FORMAT,
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
        notice_lang(s_MemoServ, u, MEMO_CHAN_HEADER, m->number,
                    m->sender, m->channel, timebuf, s_MemoServ, m->number);
    else
        notice_lang(s_MemoServ, u, MEMO_HEADER, m->number,
                    m->sender, timebuf, s_MemoServ, m->number);
    notice(s_MemoServ, u->nick, "%s", m->text);
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
        const char *def_s_ChanServ = "ChanServ";
        const char **p_s_ChanServ = NULL;
        const char *levstr;
        if (module_chanserv)
            p_s_ChanServ = get_module_symbol(module_chanserv, "s_ChanServ");
        if (!p_s_ChanServ)
            p_s_ChanServ = &def_s_ChanServ;
        if (find_module("chanserv/access-xop")) {
            if (find_module("chanserv/access-levels"))
                levstr = getstring(u->ngi, CHAN_HELP_REQSOP_LEVXOP);
            else
                levstr = getstring(u->ngi, CHAN_HELP_REQSOP_XOP);
        } else {
            levstr = getstring(u->ngi, CHAN_HELP_REQSOP_LEV);
        }
        notice_help(s_MemoServ, u, MEMO_HELP);
        if (MSExpire) {
            notice_help(s_MemoServ, u, MEMO_HELP_EXPIRES,
                        maketime(u->ngi,MSExpire,MT_DUALUNIT));
        }
        if (find_module("chanserv/access-levels")) {
            notice_help(s_MemoServ, u, MEMO_HELP_END_LEVELS, levstr,
                        *p_s_ChanServ);
        } else {
            notice_help(s_MemoServ, u, MEMO_HELP_END_XOP);
        }
    } else if (call_callback_2(cb_help, u, cmd) > 0) {
        return;
    } else if (stricmp(cmd, "COMMANDS") == 0) {
        notice_help(s_MemoServ, u, MEMO_HELP_COMMANDS);
        if (find_module("memoserv/forward"))
            notice_help(s_MemoServ, u, MEMO_HELP_COMMANDS_FORWARD);
        if (MSExpire)
            notice_help(s_MemoServ, u, MEMO_HELP_COMMANDS_SAVE);
        notice_help(s_MemoServ, u, MEMO_HELP_COMMANDS_DEL);
        if (find_module("memoserv/ignore"))
            notice_help(s_MemoServ, u, MEMO_HELP_COMMANDS_IGNORE);
        call_callback_2(cb_help_cmds, u, 0);
        if (is_oper(u)) {
            notice_help(s_MemoServ, u, MEMO_OPER_HELP_COMMANDS);
            call_callback_2(cb_help_cmds, u, 1);
        }
    } else if (stricmp(cmd, "SET") == 0) {
        notice_help(s_MemoServ, u, MEMO_HELP_SET);
        if (find_module("memoserv/forward"))
            notice_help(s_MemoServ, u, MEMO_HELP_SET_OPTION_FORWARD);
        notice_help(s_MemoServ, u, MEMO_HELP_SET_END);
    } else if (strnicmp(cmd, "SET", 3) == 0
               && isspace(cmd[3])
               && stricmp(cmd+4+strspn(cmd+4," \t"), "NOTIFY") == 0) {
        char **p_s_NickServ = NULL;
        if (module_nickserv)
            p_s_NickServ = get_module_symbol(module_nickserv, "s_NickServ");
        notice_help(s_MemoServ, u, MEMO_HELP_SET_NOTIFY,
                    p_s_NickServ ? *p_s_NickServ : "NickServ");
    } else {
        help_cmd(s_MemoServ, u, THIS_MODULE, cmd);
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
        notice_lang(s_MemoServ, u, MEMO_SEND_DISABLED);

    } else if (!target || !text) {
        syntax_error(s_MemoServ, u, "SEND", MEMO_SEND_SYNTAX);

    } else if (MSSendDelay > 0
               && (u && u->lastmemosend+MSSendDelay > now)
               && !is_services_admin(u)) {
        u->lastmemosend = now;
        notice_lang(s_MemoServ, u, MEMO_SEND_PLEASE_WAIT,
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
            notice_lang(s_MemoServ, u, MEMO_SENT, target);
            u->lastmemosend = now;
        } else {
            notice_lang(s_MemoServ, u, errormsg, target);
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
        syntax_error(s_MemoServ, u, "LIST", MEMO_LIST_SYNTAX);
    } else if (mi->memos_count == 0) {
        notice_lang(s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
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
                    notice_lang(s_MemoServ, u, MEMO_HAVE_NO_NEW_MEMOS);
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
        syntax_error(s_MemoServ, u, "READ", MEMO_READ_SYNTAX);
    } else if (mi->memos_count == 0) {
        notice_lang(s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
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
                notice_lang(s_MemoServ, u, MEMO_HAVE_NO_NEW_MEMOS);
        } else if (stricmp(numstr, "LAST") == 0) {
            read_memo(u, mi->memos_count-1, mi);
        } else {        /* number[s] */
            if (!process_numlist(numstr, &count, read_memo_callback, u, mi)) {
                if (count == 1)
                    notice_lang(s_MemoServ, u, MEMO_DOES_NOT_EXIST, num);
                else
                    notice_lang(s_MemoServ, u, MEMO_LIST_NOT_FOUND);
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
        syntax_error(s_MemoServ, u, "SAVE", MEMO_SAVE_SYNTAX);
    } else if (mi->memos_count == 0) {
        notice_lang(s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
    } else {
        int last = 0;
        int savecount =
            process_numlist(numstr, &count, save_memo_callback, u, mi, &last);
        if (savecount) {
            /* Some memos got saved. */
            if (savecount > 1)
                notice_lang(s_MemoServ, u, MEMO_SAVED_SEVERAL, savecount);
            else
                notice_lang(s_MemoServ, u, MEMO_SAVED_ONE, last);
        } else {
            /* No matching memos found. */
            if (count == 1)
                notice_lang(s_MemoServ, u, MEMO_DOES_NOT_EXIST, num);
            else
                notice_lang(s_MemoServ, u, MEMO_LIST_NOT_FOUND);
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
        syntax_error(s_MemoServ, u, "DEL", MEMO_DEL_SYNTAX);
    } else if (mi->memos_count == 0) {
        notice_lang(s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
    } else {
        if (isdigit(*numstr)) {
            /* Delete a specific memo or memos. */
            delcount = process_numlist(numstr, &count, del_memo_callback,
                                       u, mi, &last);
            if (delcount) {
                /* Some memos got deleted. */
                if (delcount > 1)
                    notice_lang(s_MemoServ, u, MEMO_DELETED_SEVERAL, delcount);
                else
                    notice_lang(s_MemoServ, u, MEMO_DELETED_ONE, last);
            } else {
                /* No memos were deleted. */
                if (count == 1)
                    notice_lang(s_MemoServ, u, MEMO_DOES_NOT_EXIST,
                                atoi(numstr));
                else
                    notice_lang(s_MemoServ, u, MEMO_DELETED_NONE);
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
            notice_lang(s_MemoServ, u, MEMO_DELETED_ALL);
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
            notice_lang(s_MemoServ, u, MEMO_RENUMBER_ONLY_YOU);
        else
            notice_lang(s_MemoServ, u, SYNTAX_ERROR, "RENUMBER");
        notice_lang(s_MemoServ, u, MORE_INFO, s_MemoServ, "RENUMBER");
        return;
    }
    ARRAY_FOREACH (i, u->ngi->memos.memos)
        u->ngi->memos.memos[i].number = i+1;
    notice_lang(s_MemoServ, u, MEMO_RENUMBER_DONE);
}

/*************************************************************************/

static void do_set(User *u)
{
    char *cmd    = strtok(NULL, " ");
    char *param  = strtok_remaining();
    MemoInfo *mi = &u->ngi->memos;

    if (readonly) {
        notice_lang(s_MemoServ, u, MEMO_SET_DISABLED);
        return;
    }
    if (!cmd || !param) {
        syntax_error(s_MemoServ, u, "SET", MEMO_SET_SYNTAX);
    } else if (!user_identified(u)) {
        notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
        return;
    } else if (call_callback_4(cb_set, u, mi, cmd, param) > 0) {
        return;
    } else if (stricmp(cmd, "NOTIFY") == 0) {
        do_set_notify(u, mi, param);
    } else if (stricmp(cmd, "LIMIT") == 0) {
        do_set_limit(u, mi, param);
    } else {
        notice_lang(s_MemoServ, u, MEMO_SET_UNKNOWN_OPTION, strupper(cmd));
        notice_lang(s_MemoServ, u, MORE_INFO, s_MemoServ, "SET");
    }
}

/*************************************************************************/

static void do_set_notify(User *u, MemoInfo *mi, char *param)
{
    if (stricmp(param, "ON") == 0) {
        u->ngi->flags |= NF_MEMO_SIGNON | NF_MEMO_RECEIVE;
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_ON, s_MemoServ);
    } else if (stricmp(param, "LOGON") == 0) {
        u->ngi->flags |= NF_MEMO_SIGNON;
        u->ngi->flags &= ~NF_MEMO_RECEIVE;
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_LOGON, s_MemoServ);
    } else if (stricmp(param, "NEW") == 0) {
        u->ngi->flags &= ~NF_MEMO_SIGNON;
        u->ngi->flags |= NF_MEMO_RECEIVE;
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_NEW, s_MemoServ);
    } else if (stricmp(param, "OFF") == 0) {
        u->ngi->flags &= ~(NF_MEMO_SIGNON | NF_MEMO_RECEIVE);
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_OFF, s_MemoServ);
    } else {
        syntax_error(s_MemoServ, u, "SET NOTIFY", MEMO_SET_NOTIFY_SYNTAX);
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

    ni->usecount++;
    ngi->usecount++;

    if (is_servadmin) {
        if (p2 && stricmp(p2, "HARD") != 0) {
            put_nickinfo(ni);
            put_nickgroupinfo(ngi);
            if (!(ni = get_nickinfo(p1))) {
                notice_lang(s_MemoServ, u, NICK_X_NOT_REGISTERED, p1);
                return;
            }
            if (!(ngi = get_ngi(ni))) {
                notice_lang(s_MemoServ, u, INTERNAL_ERROR);
                put_nickinfo(ni);
                return;
            }
            user = p1;
            mi = &ngi->memos;
            p1 = p2;
            p2 = strtok(NULL, " ");
        } else if (!p1) {
            syntax_error(s_MemoServ, u, "SET LIMIT",
                         MEMO_SET_LIMIT_OPER_SYNTAX);
            return;
        }
        if ((!isdigit(*p1) && stricmp(p1, "NONE") != 0
             && stricmp(p1, "DEFAULT") != 0)
            || (p2 && stricmp(p2, "HARD") != 0)
        ) {
            syntax_error(s_MemoServ, u, "SET LIMIT",
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
                syntax_error(s_MemoServ, u, "SET LIMIT",
                             MEMO_SET_LIMIT_OPER_SYNTAX);
                return;
            } else if (limit > MEMOMAX_MAX) {
                notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_OVERFLOW,
                            MEMOMAX_MAX);
                limit = MEMOMAX_MAX;
            }
        }
    } else {
        if (!p1 || p2 || !isdigit(*p1)) {
            syntax_error(s_MemoServ, u, "SET LIMIT", MEMO_SET_LIMIT_SYNTAX);
            return;
        }
        if (ngi->flags & NF_MEMO_HARDMAX) {
            notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT_FORBIDDEN);
            return;
        }
        limit = (int)atolsafe(p1, 0, INT_MAX);
        if (limit < 0) {
            syntax_error(s_MemoServ, u, "SET LIMIT", MEMO_SET_LIMIT_SYNTAX);
            return;
        } else if (MSMaxMemos > 0 && limit > MSMaxMemos) {
            notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT_TOO_HIGH,
                        MSMaxMemos);
            return;
        } else if (limit > MEMOMAX_MAX) {
            notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_OVERFLOW, MEMOMAX_MAX);
            limit = MEMOMAX_MAX;
        }
    }

    mi->memomax = limit;

    if (limit > 0) {
        if (ni == u->ni)
            notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT, limit);
        else
            notice_lang(s_MemoServ, u, MEMO_SET_LIMIT, user, limit);
    } else if (limit == 0) {
        if (ni == u->ni)
            notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT_ZERO);
        else
            notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_ZERO, user);
    } else if (limit == MEMOMAX_DEFAULT) {
        if (ni == u->ni)
            notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT_DEFAULT,
                        MSMaxMemos);
        else
            notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_DEFAULT, user,
                        MSMaxMemos);
    } else {
        if (ni == u->ni)
            notice_lang(s_MemoServ, u, MEMO_UNSET_YOUR_LIMIT);
        else
            notice_lang(s_MemoServ, u, MEMO_UNSET_LIMIT, user);
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
            notice_lang(s_MemoServ, u, NICK_X_NOT_REGISTERED, name);
            return;
        } else if (ni->status & NS_VERBOTEN) {
            notice_lang(s_MemoServ, u, NICK_X_FORBIDDEN, name);
            put_nickinfo(ni);
            return;
        }
        ngi = get_ngi(ni);
        if (!ngi) {
            notice_lang(s_MemoServ, u, INTERNAL_ERROR);
            put_nickinfo(ni);
            return;
        }
        mi = &ngi->memos;
        is_hardmax = ngi->flags & NF_MEMO_HARDMAX ? 1 : 0;
    } else { /* !name or !servadmin */
        if (!user_identified(u)) {
            notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
            return;
        }
        ni = u->ni;
        if (ni)
            ni->usecount++;
        ngi = u->ngi;
        if (ngi)
            ngi->usecount++;
        mi = &u->ngi->memos;
    }
    max = REALMAX(mi->memomax);

    if (ni != u->ni) {
        /* Report info for a nick other than the caller. */
        if (!mi->memos_count) {
            notice_lang(s_MemoServ, u, MEMO_INFO_X_NO_MEMOS, name);
        } else if (mi->memos_count == 1) {
            if (mi->memos[0].flags & MF_UNREAD)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMO_UNREAD, name);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMO, name);
        } else {
            int count = 0, i;
            ARRAY_FOREACH (i, mi->memos) {
                if (mi->memos[i].flags & MF_UNREAD)
                    count++;
            }
            if (count == mi->memos_count)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS_ALL_UNREAD,
                        name, count);
            else if (count == 0)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS,
                        name, mi->memos_count);
            else if (count == 0)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS_ONE_UNREAD,
                        name, mi->memos_count);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS_SOME_UNREAD,
                        name, mi->memos_count, count);
        }
        if (max >= 0) {
            if (is_hardmax)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_HARD_LIMIT, name, max);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_X_LIMIT, name, max);
        } else {
            notice_lang(s_MemoServ, u, MEMO_INFO_X_NO_LIMIT, name);
        }
        if ((ngi->flags & NF_MEMO_RECEIVE) && (ngi->flags & NF_MEMO_SIGNON)) {
            notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_ON, name);
        } else if (ngi->flags & NF_MEMO_RECEIVE) {
            notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_RECEIVE, name);
        } else if (ngi->flags & NF_MEMO_SIGNON) {
            notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_SIGNON, name);
        } else {
            notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_OFF, name);
        }

    } else { /* ni == u->ni */

        if (!mi->memos_count) {
            notice_lang(s_MemoServ, u, MEMO_INFO_NO_MEMOS);
        } else if (mi->memos_count == 1) {
            if (mi->memos[0].flags & MF_UNREAD)
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMO_UNREAD);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMO);
        } else {
            int count = 0, i;
            ARRAY_FOREACH (i, mi->memos) {
                if (mi->memos[i].flags & MF_UNREAD)
                    count++;
            }
            if (count == mi->memos_count)
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS_ALL_UNREAD, count);
            else if (count == 0)
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS, mi->memos_count);
            else if (count == 1)
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS_ONE_UNREAD,
                        mi->memos_count);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS_SOME_UNREAD,
                        mi->memos_count, count);
        }
        if (max == 0) {
            if (!is_servadmin && is_hardmax)
                notice_lang(s_MemoServ, u, MEMO_INFO_HARD_LIMIT_ZERO);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_LIMIT_ZERO);
        } else if (max > 0) {
            if (!is_servadmin && is_hardmax)
                notice_lang(s_MemoServ, u, MEMO_INFO_HARD_LIMIT, max);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_LIMIT, max);
        } else {
            notice_lang(s_MemoServ, u, MEMO_INFO_NO_LIMIT);
        }
        if ((ngi->flags & NF_MEMO_RECEIVE) && (ngi->flags & NF_MEMO_SIGNON)) {
            notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_ON);
        } else if (ngi->flags & NF_MEMO_RECEIVE) {
            notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_RECEIVE);
        } else if (ngi->flags & NF_MEMO_SIGNON) {
            notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_SIGNON);
        } else {
            notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_OFF);
        }

    } /* if (ni != u->ni) */

    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int do_MSAlias(const char *filename, int linenum, char *param);

ConfigDirective module_config[] = {
    { "MemoServName",     { { CD_STRING, CF_DIRREQ, &s_MemoServ },
                            { CD_STRING, 0, &desc_MemoServ } } },
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

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "nickserv/main") == 0) {
        module_nickserv = mod;
        use_module(mod);
        if (!add_callback(module_nickserv, "REGISTER/LINK check",
                          do_reglink_check))
            module_log("Unable to register NickServ REGISTER/LINK callback");
        if (!add_callback(mod, "identified", do_nick_identified))
            module_log("Unable to register NickServ IDENTIFY callback");
    } else if (strcmp(modname, "chanserv/main") == 0) {
        module_chanserv = mod;
        p_get_channelinfo = get_module_symbol(NULL, "get_channelinfo");
        p_put_channelinfo = get_module_symbol(NULL, "put_channelinfo");
        p_get_ci_level = get_module_symbol(NULL, "get_ci_level");
        p_check_access = get_module_symbol(NULL, "check_access");
        if (!p_get_channelinfo || !p_put_channelinfo || !p_get_ci_level
         || !p_check_access
        ) {
            module_log("ChanServ symbol(s) not found, channel memos will"
                       " not be available");
        }
    }
    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_nickserv) {
        remove_callback(module_nickserv, "identified", do_nick_identified);
        remove_callback(module_nickserv, "REGISTER/LINK check",
                        do_reglink_check);
        unuse_module(module_nickserv);
        module_nickserv = NULL;
    } else if (mod == module_chanserv) {
        p_get_channelinfo = NULL;
        p_put_channelinfo = NULL;
        p_get_ci_level = NULL;
        module_chanserv = NULL;
    }
    return 0;
}

/*************************************************************************/

static int do_reconfigure(int after_configure)
{
    static char old_s_MemoServ[NICKMAX];
    static char *old_desc_MemoServ = NULL;

    if (!after_configure) {
        /* Before reconfiguration: save old values. */
        strbcpy(old_s_MemoServ, s_MemoServ);
        old_desc_MemoServ = strdup(desc_MemoServ);
        if (old_HELP_LIST >= 0) {
            mapstring(MEMO_HELP_LIST, old_HELP_LIST);
            old_HELP_LIST = -1;
        }
    } else {
        /* After reconfiguration: handle value changes. */
        if (strcmp(old_s_MemoServ, s_MemoServ) != 0)
            send_nickchange(old_s_MemoServ, s_MemoServ);
        if (!old_desc_MemoServ || strcmp(old_desc_MemoServ,desc_MemoServ) != 0)
            send_namechange(s_MemoServ, desc_MemoServ);
        free(old_desc_MemoServ);
        if (MSExpire)
            old_HELP_LIST = mapstring(MEMO_HELP_LIST, MEMO_HELP_LIST_EXPIRE);
    }  /* if (!after_configure) */
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    Command *cmd;
    Module *tmpmod;


    if (!new_commandlist(THIS_MODULE)
     || !register_commands(THIS_MODULE, cmds)
    ) {
        module_log("Unable to register commands");
        exit_module(0);
        return 0;
    }
    if (MSExpire) {
        old_HELP_LIST = mapstring(MEMO_HELP_LIST, MEMO_HELP_LIST_EXPIRE);
    } else {
        /* Disable SAVE command if no expiration */
        cmd_SAVE = lookup_cmd(THIS_MODULE, "SAVE");
        if (cmd_SAVE)
            cmd_SAVE->name = "";
    }

    cb_command      = register_callback("command");
    cb_receive_memo = register_callback("receive memo");
    cb_help         = register_callback("HELP");
    cb_help_cmds    = register_callback("HELP COMMANDS");
    cb_set          = register_callback("SET");
    if (cb_command < 0 || cb_receive_memo < 0 || cb_help < 0
     || cb_help_cmds < 0 || cb_set < 0) {
        module_log("Unable to register callbacks");
        exit_module(0);
        return 0;
    }

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "reconfigure", do_reconfigure)
     || !add_callback(NULL, "introduce_user", introduce_memoserv)
     || !add_callback(NULL, "m_privmsg", memoserv)
     || !add_callback(NULL, "m_whois", memoserv_whois)
     || !add_callback(NULL, "receive message", do_receive_message)
     || !add_callback(NULL, "user create", do_user_create)
     || !add_callback(NULL, "user nickchange (after)", do_user_nickchange)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    if (!register_dbtable(&memo_dbtable)) {
        module_log("Unable to register database table");
        exit_module(0);
        return 0;
    }

    tmpmod = find_module("nickserv/main");
    if (tmpmod)
        do_load_module(tmpmod, "nickserv/main");
    tmpmod = find_module("chanserv/main");
    if (tmpmod)
        do_load_module(tmpmod, "chanserv/main");

    cmd = lookup_cmd(THIS_MODULE, "SET NOTIFY");
    if (cmd)
        cmd->help_param1 = s_NickServ;
    cmd = lookup_cmd(THIS_MODULE, "SET LIMIT");
    if (cmd) {
        cmd->help_param1 = (char *)(long)MSMaxMemos;
        cmd->help_param2 = (char *)(long)MSMaxMemos;
    }

    if (linked)
        introduce_memoserv(NULL);

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (linked)
        send_cmd(s_MemoServ, "QUIT :");

    if (module_chanserv)
        do_unload_module(module_chanserv);
    if (module_nickserv)
        do_unload_module(module_nickserv);

    unregister_dbtable(&memo_dbtable);

    remove_callback(NULL, "user nickchange (after)", do_user_nickchange);
    remove_callback(NULL, "user create", do_user_create);
    remove_callback(NULL, "receive message", do_receive_message);
    remove_callback(NULL, "m_whois", memoserv_whois);
    remove_callback(NULL, "m_privmsg", memoserv);
    remove_callback(NULL, "introduce_user", introduce_memoserv);
    remove_callback(NULL, "reconfigure", do_reconfigure);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

    unregister_callback(cb_set);
    unregister_callback(cb_help_cmds);
    unregister_callback(cb_help);
    unregister_callback(cb_receive_memo);
    unregister_callback(cb_command);

    if (cmd_SAVE) {
        cmd_SAVE->name = "SAVE";
        cmd_SAVE = NULL;
    }
    if (old_HELP_LIST >= 0) {
        mapstring(MEMO_HELP_LIST, old_HELP_LIST);
        old_HELP_LIST = -1;
    }
    unregister_commands(THIS_MODULE, cmds);
    del_commandlist(THIS_MODULE);

    return 1;
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
