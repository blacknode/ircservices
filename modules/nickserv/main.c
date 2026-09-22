/* Main NickServ module.
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
#include "commands.h"
#include "databases.h"
#include "encrypt.h"
#include "language.h"
#include "modules/operserv/operserv.h"

#include "nickserv.h"
#include "ns-local.h"

/*************************************************************************/

static Module *module_operserv;
static const char **p_ServicesRoot;

static int cb_check_expire  = -1;
static int cb_command       = -1;
static int cb_help          = -1;
static int cb_help_cmds     = -1;
       int cb_reglink_check = -1;  /* called from util.c */
static int cb_registered    = -1;
static int cb_id_check      = -1;
static int cb_identified    = -1;

       char *s_NickServ;
static char *desc_NickServ;
EXPORT_VAR(char *,s_NickServ)

       int32  NSRegEmailMax;
       int    NSRequireEmail;
       int    NSRegDenyIfSuspended;
       time_t NSRegDelay;
       time_t NSInitialRegDelay;
       time_t NSSetEmailDelay;
       int32  NSDefFlags;
       time_t NSExpire;
       time_t NSExpireWarning;
       int    NSShowPassword;
       char * NSEnforcerUser;
       char * NSEnforcerHost;
       int    NSForceNickChange;
       time_t NSReleaseTimeout;
       int    NSAllowKillImmed;
       int    NSListOpersOnly;
       int    NSSecureAdmins;
       time_t NSSuspendExpire;
       time_t NSSuspendGrace;
static int    NSHelpWarning;
static int    NSEnableDropEmail;
static time_t NSDropEmailExpire;

/*************************************************************************/

static void do_help(User *u);
static void do_register(User *u);
static void do_identify(User *u);
static void do_drop(User *u);
static void do_dropnick(User *u);
static void do_dropemail(User *u);
static void do_dropemail_confirm(User *u);
static void do_info(User *u);
static void do_listchans(User *u);
static void do_list(User *u);
static void do_listemail(User *u);
static void do_recover(User *u);
static void do_release(User *u);
static void do_ghost(User *u);
static void do_status(User *u);
static void do_getpass(User *u);
static void do_forbid(User *u);
static void do_suspend(User *u);
static void do_unsuspend(User *u);
#ifdef DEBUG_COMMANDS
static void do_listnick(User *u);
#endif

/*************************************************************************/

static Command cmds[] = {
    { "HELP",     do_help,     NULL,  -1,                     -1,-1 },
    { "REGISTER", do_register, NULL,  NICK_HELP_REGISTER,     -1,-1 },
    { "IDENTIFY", do_identify, NULL,  NICK_HELP_IDENTIFY,     -1,-1 },
    { "SIDENTIFY",do_identify, NULL,  -1,                     -1,-1 },
    { "DROP",     do_drop,     NULL,  NICK_HELP_DROP,         -1,-1 },
    { "SET",      do_set,      NULL,  NICK_HELP_SET,
                -1, NICK_OPER_HELP_SET },
    { "SET PASSWORD", NULL,    NULL,  NICK_HELP_SET_PASSWORD, -1,-1 },
    { "SET URL",      NULL,    NULL,  NICK_HELP_SET_URL,      -1,-1 },
    { "SET EMAIL",    NULL,    NULL,  NICK_HELP_SET_EMAIL,    -1,-1 },
    { "SET INFO",     NULL,    NULL,  NICK_HELP_SET_INFO,     -1,-1 },
    { "SET KILL",     NULL,    NULL,  NICK_HELP_SET_KILL,     -1,-1 },
    { "SET SECURE",   NULL,    NULL,  NICK_HELP_SET_SECURE,   -1,-1 },
    { "SET PRIVATE",  NULL,    NULL,  NICK_HELP_SET_PRIVATE,  -1,-1 },
    { "SET NOOP",     NULL,    NULL,  NICK_HELP_SET_NOOP,     -1,-1 },
    { "SET HIDE",     NULL,    NULL,  NICK_HELP_SET_HIDE,     -1,-1 },
    { "SET TIMEZONE", NULL,    NULL,  NICK_HELP_SET_TIMEZONE, -1,-1 },
    { "SET NOEXPIRE", NULL,    NULL,  -1, -1,
                NICK_OPER_HELP_SET_NOEXPIRE },
    { "UNSET",    do_unset,    NULL,  NICK_HELP_UNSET,
                -1, NICK_OPER_HELP_UNSET },
    { "RECOVER",  do_recover,  NULL,  NICK_HELP_RECOVER,      -1,-1 },
    { "RELEASE",  do_release,  NULL,  NICK_HELP_RELEASE,      -1,-1 },
    { "GHOST",    do_ghost,    NULL,  NICK_HELP_GHOST,        -1,-1 },
    { "INFO",     do_info,     NULL,  NICK_HELP_INFO,
                -1, NICK_OPER_HELP_INFO },
    { "LIST",     do_list,     NULL,  -1,
                NICK_HELP_LIST, NICK_OPER_HELP_LIST },
    { "LISTEMAIL",do_listemail,NULL,  NICK_HELP_LISTEMAIL,    -1,-1 },
    { "STATUS",   do_status,   NULL,  NICK_HELP_STATUS,       -1,-1 },
    { "LISTCHANS",do_listchans,NULL,  NICK_HELP_LISTCHANS,
                -1, NICK_OPER_HELP_LISTCHANS },

    { "DROPNICK", do_dropnick, is_services_admin, -1,
                -1, NICK_OPER_HELP_DROPNICK },
    { "DROPEMAIL",do_dropemail,is_services_admin, -1,
                -1, NICK_OPER_HELP_DROPEMAIL },
    { "DROPEMAIL-CONFIRM", do_dropemail_confirm, is_services_admin, -1,
                -1, NICK_OPER_HELP_DROPEMAIL },
    { "GETPASS",  do_getpass,  is_services_admin, -1,
                -1, NICK_OPER_HELP_GETPASS },
    { "FORBID",   do_forbid,   is_services_admin, -1,
                -1, NICK_OPER_HELP_FORBID },
    { "SUSPEND",  do_suspend,  is_services_admin, -1,
                -1, NICK_OPER_HELP_SUSPEND },
    { "UNSUSPEND",do_unsuspend,is_services_admin, -1,
                -1, NICK_OPER_HELP_UNSUSPEND },
#ifdef DEBUG_COMMANDS
    { "LISTNICK", do_listnick, is_services_root, -1, -1, -1 },
#endif
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

/* Check whether the given nickname (or its suspension) should be expired,
 * and do the expiration if so.  Return 1 if the nickname was deleted, else
 * 0.  Note that we do last-seen-time updates here as well.
 */

static int check_expire_nick(NickInfo *ni)
{
    User *u = ni->user;
    NickGroupInfo *ngi;
    time_t now = time(NULL);

    if (u && user_id_or_rec(u)) {
        module_log_debug(2, "updating last seen time for %s", u->nick);
        ni->last_seen = time(NULL);
    }
    ngi = ni->nickgroup ? get_ngi_id(ni->nickgroup) : NULL;
    if (!*p_ServicesRoot || irc_stricmp(ni->nick, *p_ServicesRoot) != 0) {
        if (call_callback_2(cb_check_expire, ni, ngi) > 0) {
            put_nickgroupinfo(ngi);
            if (u)
                notice_lang(s_NickServ, u, NICK_EXPIRED);
            delnick(ni);
            return 1;
        }
        if (NSExpire
         && now >= ni->last_seen + NSExpire
         && !(ni->status & (NS_VERBOTEN | NS_NOEXPIRE))
         && (!ngi || !(ngi->flags & NF_SUSPENDED))
        ) {
            put_nickgroupinfo(ngi);
            module_log("Expiring nickname %s", ni->nick);
            if (u)
                notice_lang(s_NickServ, u, NICK_EXPIRED);
            delnick(ni);
            return 1;
        }
    }
    if (ngi && (ngi->flags & NF_SUSPENDED)
     && ngi->suspend_expires
     && now >= ngi->suspend_expires
    ) {
        module_log("Expiring suspension for %s (nick group %u)",
                   ni->nick, ngi->id);
        unsuspend_nick(ngi, 1);
    }
    put_nickgroupinfo(ngi);
    return 0;
}

/*************************************************************************/

#include "hash.h"

#undef HASH_STATIC
#define HASH_STATIC static
#undef EXPIRE_CHECK
#define EXPIRE_CHECK(node) check_expire_nick(node)
DEFINE_HASH(nickinfo_, NickInfo, nick)

EXPORT_FUNC(add_nickinfo)
NickInfo *add_nickinfo(NickInfo *ni)
{
    add_nickinfo_(ni);
    ni->usecount = 1;
    return ni;
}

EXPORT_FUNC(del_nickinfo)
void del_nickinfo(NickInfo *ni)
{
    del_nickinfo_(ni);
    free_nickinfo(ni);
}

EXPORT_FUNC(get_nickinfo)
NickInfo *get_nickinfo(const char *nick)
{
    NickInfo *ni;
    if ((ni = get_nickinfo_(nick)) != NULL)
        ni->usecount++;
    return ni;
}

EXPORT_FUNC(put_nickinfo)
NickInfo *put_nickinfo(NickInfo *ni)
{
    if (ni) {
        if (ni->usecount > 0)
            ni->usecount--;
        else
            module_log_debug(1, "BUG: put_nickinfo(%s) with usecount==0",
                             ni->nick);
    }
    return ni;
}

EXPORT_FUNC(first_nickinfo)
NickInfo *first_nickinfo(void)
{
    return first_nickinfo_();
}

EXPORT_FUNC(next_nickinfo)
NickInfo *next_nickinfo(void)
{
    return next_nickinfo_();
}

/*************************************************************************/

#undef HASH_STATIC
#define HASH_STATIC static
#undef HASHFUNC
#define HASHFUNC(key) (((uint32)(key)*31) % HASHSIZE)
#undef EXPIRE_CHECK
#define EXPIRE_CHECK(node) 0
DEFINE_HASH_SCALAR(nickgroupinfo_, NickGroupInfo, id, uint32);

EXPORT_FUNC(add_nickgroupinfo)
NickGroupInfo *add_nickgroupinfo(NickGroupInfo *ngi)
{
    add_nickgroupinfo_(ngi);
    ngi->usecount = 1;
    return ngi;
}

EXPORT_FUNC(del_nickgroupinfo)
void del_nickgroupinfo(NickGroupInfo *ngi)
{
    del_nickgroupinfo_(ngi);
    free_nickgroupinfo(ngi);
}

EXPORT_FUNC(get_nickgroupinfo)
NickGroupInfo *get_nickgroupinfo(uint32 id)
{
    NickGroupInfo *ngi;
    if ((ngi = get_nickgroupinfo_(id)) != NULL)
        ngi->usecount++;
    return ngi;
}

EXPORT_FUNC(put_nickgroupinfo)
NickGroupInfo *put_nickgroupinfo(NickGroupInfo *ngi)
{
    if (ngi) {
        if (ngi->usecount > 0)
            ngi->usecount--;
        else
            module_log_debug(1, "BUG: put_nickgroupinfo(%u) with usecount==0",
                             ngi->id);
    }
    return ngi;
}

EXPORT_FUNC(first_nickgroupinfo)
NickGroupInfo *first_nickgroupinfo(void)
{
    return first_nickgroupinfo_();
}

EXPORT_FUNC(next_nickgroupinfo)
NickGroupInfo *next_nickgroupinfo(void)
{
    return next_nickgroupinfo_();
}

/*************************************************************************/

/* Free all memory used by database tables. */

static void clean_dbtables(void)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    int save_noexpire = noexpire;

    noexpire = 1;
    for (ni = first_nickinfo(); ni; ni = next_nickinfo())
        free_nickinfo(ni);
    for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo())
        free_nickgroupinfo(ngi);
    noexpire = save_noexpire;
}

/*************************************************************************/

/* Database load/save helpers */

/************************************/

static void db_get_mainnick(const void *record, void **value_ret)
{
    NickGroupInfo *ngi = (NickGroupInfo *)record;
    memset((char *)value_ret, 0, NICKMAX);
    strscpy((char *)value_ret, ngi->nicks[ngi->mainnick], NICKMAX);
}

static void db_put_mainnick(void *record, const void *value)
{
    NickGroupInfo *ngi = (NickGroupInfo *)record;
    int i;
    /* ngi->nicks_count is 0 here, but just to be safe... */
    ARRAY_FOREACH (i, ngi->nicks) {
        if (irc_stricmp((const char *)value, ngi->nicks[i]) == 0) {
            ngi->mainnick = i;
            return;
        }
    }
    ARRAY_EXTEND(ngi->nicks);
    ngi->mainnick = ngi->nicks_count - 1;
    strbcpy(ngi->nicks[ngi->mainnick], (const char *)value);
}

static void *db_new_nickgroup(void)
{
    return new_nickgroupinfo(NULL);
}

static void insert_nickgroup(void *record)
{
    NickGroupInfo *ngi = add_nickgroupinfo(record);
    put_nickgroupinfo(ngi);
}

/************************************/

static void insert_nick(void *record)
{
    NickInfo *ni = add_nickinfo(record);
    if (ni->nickgroup) {
        NickGroupInfo *ngi = get_nickgroupinfo(ni->nickgroup);
        if (ngi) {
            int i;
            /* Don't re-add the main nick */
            ARRAY_SEARCH_PLAIN(ngi->nicks, ni->nick, irc_stricmp, i);
            if (i >= ngi->nicks_count) {
                ARRAY_EXTEND(ngi->nicks);
                strbcpy(ngi->nicks[ngi->nicks_count-1], ni->nick);
            }
        }
    }
    put_nickinfo(ni);
}

static int db_postload_nick(void)
{
    /* Check that main nicknames loaded from the nickgroup table actually
     * exist (disable expiration while we do this) */
    int saved_noexpire = noexpire;
    noexpire = 1;
    NickGroupInfo *ngi;
    for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {
        if (ngi->nicks_count == 0) {
            module_log("Nickgroup %u has no nicks, deleting", ngi->id);
            delgroup(ngi);
        } else {
            NickInfo *ni;
            if (ngi->mainnick >= ngi->nicks_count) {
                module_log("Nickgroup %u has invalid main nick index %u,"
                           " resetting to 0 (%s)", ngi->id, ngi->mainnick,
                           ngi->nicks[0]);
                ngi->mainnick = 0;
            }
            ni = get_nickinfo(ngi->nicks[ngi->mainnick]);
            if (!ni || ni->nickgroup != ngi->id) {
                module_log("main nick %s in nickgroup %u %s, clearing",
                           ngi->nicks[ngi->mainnick], ngi->id,
                           !ni ? "does not exist" : "has wrong nickgroup ID");
                ARRAY_REMOVE(ngi->nicks, ngi->mainnick);
                if (ngi->nicks_count == 0) {
                    module_log("... nickgroup %u is now empty, dropping",
                               ngi->id);
                    delgroup(ngi);
                } else {
                    module_log("... resetting main nick to 0 (%s)",
                               ngi->nicks[0]);
                    ngi->mainnick = 0;
                }
            }
            put_nickinfo(ni);
        }
    }
    noexpire = saved_noexpire;

    /* Return success */
    return 1;
}

/*************************************************************************/

/* Nickgroup database table info */

#define FIELD(name,type,...) \
    { #name, type, offsetof(NickGroupInfo,name) , ##__VA_ARGS__ }
static DBField nickgroup_dbfields[] = {
    FIELD(id,              DBTYPE_UINT32),
    FIELD(mainnick,        DBTYPE_BUFFER, NICKMAX,
          .get = db_get_mainnick, .put = db_put_mainnick),
    FIELD(pass,            DBTYPE_PASSWORD),
    FIELD(url,             DBTYPE_STRING),
    FIELD(email,           DBTYPE_STRING),
    FIELD(last_email,      DBTYPE_STRING),
    FIELD(info,            DBTYPE_STRING),
    FIELD(flags,           DBTYPE_INT32),
    FIELD(os_priv,         DBTYPE_INT16),
    FIELD(authcode,        DBTYPE_INT32),
    FIELD(authset,         DBTYPE_TIME),
    FIELD(authreason,      DBTYPE_INT16),
    FIELD(suspend_who,     DBTYPE_BUFFER, NICKMAX),
    FIELD(suspend_reason,  DBTYPE_STRING),
    FIELD(suspend_time,    DBTYPE_TIME),
    FIELD(suspend_expires, DBTYPE_TIME),
    FIELD(language,        DBTYPE_INT16),
    FIELD(timezone,        DBTYPE_INT16),
    FIELD(channelmax,      DBTYPE_INT16),
    { "memos.memomax",     DBTYPE_INT16,
      offsetof(NickGroupInfo,memos) + offsetof(MemoInfo,memomax) },

    { NULL }
};
static DBTable nickgroup_dbtable = {
    .name    = "nickgroup",
    .newrec  = db_new_nickgroup,
    .freerec = (void *)free_nickgroupinfo,
    .insert  = insert_nickgroup,
    .first   = (void *)first_nickgroupinfo,
    .next    = (void *)next_nickgroupinfo,
    .fields  = nickgroup_dbfields,
};
#undef FIELD
#undef FIELD_SUSPINFO

/* Nickname database table info */

#define FIELD(name,type,...) \
    { #name, type, offsetof(NickInfo,name) , ##__VA_ARGS__ }
static DBField nick_dbfields[] = {
    FIELD(nick,            DBTYPE_BUFFER, NICKMAX),
    FIELD(status,          DBTYPE_INT16),
    FIELD(last_usermask,   DBTYPE_STRING),
    FIELD(last_realmask,   DBTYPE_STRING),
    FIELD(last_realname,   DBTYPE_STRING),
    FIELD(last_quit,       DBTYPE_STRING),
    FIELD(time_registered, DBTYPE_TIME),
    FIELD(last_seen,       DBTYPE_TIME),
    FIELD(nickgroup,       DBTYPE_UINT32),
    FIELD(id_stamp,        DBTYPE_UINT32),
    { NULL }
};
static DBTable nick_dbtable = {
    .name    = "nick",
    .newrec  = (void *)new_nickinfo,
    .freerec = (void *)free_nickinfo,
    .insert  = insert_nick,
    .first   = (void *)first_nickinfo,
    .next    = (void *)next_nickinfo,
    .fields  = nick_dbfields,
    .postload= db_postload_nick,
};
#undef FIELD

/*************************************************************************/
/************************ Main NickServ routines *************************/
/*************************************************************************/

/* Introduce the NickServ pseudoclient. */

static int introduce_nickserv(const char *nick)
{
    if (!nick || irc_stricmp(nick, s_NickServ) == 0) {
        send_pseudo_nick(s_NickServ, desc_NickServ, PSEUDO_OPER);
        if (nick)
            return 1;
    }
    return 0;
}

/*************************************************************************/

/* Main NickServ routine. */

static int nickserv(const char *source, const char *target, char *buf)
{
    char *cmd;
    User *u = get_user(source);

    if (irc_stricmp(target, s_NickServ) != 0)
        return 0;

    if (!u) {
        module_log("user record for %s not found", source);
        notice(s_NickServ, source,
                getstring(NULL, INTERNAL_ERROR));
        return 1;
    }

    cmd = strtok(buf, " ");

    if (!cmd) {
        return 1;
    } else if (stricmp(cmd, "\1PING") == 0) {
        const char *s;
        if (!(s = strtok_remaining()))
            s = "\1";
        notice(s_NickServ, source, "\1PING %s", s);
    } else {
        int i;
        ARRAY_FOREACH (i, aliases) {
            if (stricmp(cmd, aliases[i].alias) == 0) {
                cmd = aliases[i].command;
                break;
            }
        }
        if (call_callback_2(cb_command, u, cmd) <= 0)
            run_cmd(s_NickServ, u, THIS_MODULE, cmd);
    }
    return 1;

}

/*************************************************************************/

/* Return a /WHOIS response for NickServ. */

static int nickserv_whois(const char *source, char *who, char *extra)
{
    if (irc_stricmp(who, s_NickServ) != 0)
        return 0;
    send_cmd(ServerName, "311 %s %s %s %s * :%s", source, who,
             ServiceUser, ServiceHost, desc_NickServ);
    send_cmd(ServerName, "312 %s %s %s :%s", source, who,
             ServerName, ServerDesc);
    send_cmd(ServerName, "313 %s %s :is a network service", source, who);
    send_cmd(ServerName, "318 %s %s End of /WHOIS response.", source, who);
    return 1;
}

/*************************************************************************/
/*************************************************************************/

/* Callback for users connecting to the network. */

static int do_user_create(User *user, int ac, char **av)
{
    validate_user(user);
    return 0;
}

/*************************************************************************/

/* Callbacks for users changing nicknames (before and after). */

static int do_user_nickchange_before(User *user, const char *newnick)
{
    /* Changing nickname case isn't a real change; pop out immediately
     * in that case. */
    if (irc_stricmp(newnick, user->nick) == 0)
        return 0;

    cancel_user(user);
    return 0;
}

static int do_user_nickchange_after(User *user, const char *oldnick)
{
    /* Changing nickname case isn't a real change; pop out immediately
     * in that case. */
    if (irc_stricmp(oldnick, user->nick) == 0)
        return 0;

    user->my_signon = time(NULL);
    validate_user(user);
    if (usermode_reg) {
        if (user_identified(user)) {
            send_cmd(s_NickServ, "SVSMODE %s :+%s", user->nick,
                     mode_flags_to_string(usermode_reg, MODE_USER));
            user->mode |= usermode_reg;
        } else {
            send_cmd(s_NickServ, "SVSMODE %s :-%s", user->nick,
                     mode_flags_to_string(usermode_reg, MODE_USER));
            user->mode &= ~usermode_reg;
        }
    }
    return 0;
}

/*************************************************************************/

/* Callback for users disconnecting from the network. */

static int do_user_delete(User *user, const char *reason)
{
    NickInfo *ni = user->ni;
    int i, j;

    if (user_recognized(user)) {
        free(ni->last_quit);
        ni->last_quit = *reason ? sstrdup(reason) : NULL;
    }
    ARRAY_FOREACH (i, user->id_nicks) {
        NickGroupInfo *ngi = get_ngi_id(user->id_nicks[i]);
        if (!ngi)
            continue;
        ARRAY_SEARCH_PLAIN_SCALAR(ngi->id_users, user, j);
        if (j < ngi->id_users_count) {
            ARRAY_REMOVE(ngi->id_users, j);
        } else {
            module_log("BUG: do_user_delete(): nickgroup %u listed in"
                       " id_nicks for user %p (%s), but user not in"
                       " id_users!", ngi->id, user, user->nick);
        }
        put_nickgroupinfo(ngi);
    }
    cancel_user(user);
    return 0;
}

/*************************************************************************/

/* Callback for REGISTER/LINK check; we disallow registration/linking of
 * the NickServ pseudoclient nickname or guest nicks.  This is done here
 * instead of in the routines themselves to avoid duplication of code at an
 * insignificant performance cost.
 */

static int do_reglink_check(const User *u, const char *nick,
                            const char *pass, const char *email)
{
    if ((protocol_features & PF_CHANGENICK) && is_guest_nick(nick)) {
        /* Don't allow guest nicks to be registered or linked.  This check
         * has to be done regardless of the state of NSForceNickChange
         * because other modules might take advantage of forced nick
         * changing. */
        return 1;
    }
    return irc_stricmp(nick, s_NickServ) == 0;
}

/*************************************************************************/

/* Callback for OperServ STATS ALL. */

static int do_stats_all(User *user, const char *s_OperServ)
{
    int32 count, mem;
    int i;
    NickGroupInfo *ngi;
    NickInfo *ni;

    count = mem = 0;
    for (ni = first_nickinfo(); ni; ni = next_nickinfo()) {
        count++;
        mem += sizeof(*ni);
        if (ni->last_usermask)
            mem += strlen(ni->last_usermask)+1;
        if (ni->last_realmask)
            mem += strlen(ni->last_realmask)+1;
        if (ni->last_realname)
            mem += strlen(ni->last_realname)+1;
        if (ni->last_quit)
            mem += strlen(ni->last_quit)+1;
    }
    notice_lang(s_OperServ, user, OPER_STATS_ALL_NICKINFO_MEM,
                count, (mem+512) / 1024);

    count = mem = 0;
    for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {
        count++;
        mem += sizeof(*ngi);
        if (ngi->url)
            mem += strlen(ngi->url)+1;
        if (ngi->email)
            mem += strlen(ngi->email)+1;
        if (ngi->last_email)
            mem += strlen(ngi->last_email)+1;
        if (ngi->info)
            mem += strlen(ngi->info)+1;
        if (ngi->suspend_reason)
            mem += strlen(ngi->suspend_reason)+1;
        mem += sizeof(*ngi->nicks) * ngi->nicks_count;
        mem += sizeof(*ngi->channels) * ngi->channels_count;
        mem += sizeof(*ngi->access) * ngi->access_count;
        ARRAY_FOREACH (i, ngi->access) {
            if (ngi->access[i])
                mem += strlen(ngi->access[i])+1;
        }
        mem += sizeof(*ngi->ajoin) * ngi->ajoin_count;
        ARRAY_FOREACH (i, ngi->ajoin) {
            if (ngi->ajoin[i])
                mem += strlen(ngi->ajoin[i])+1;
        }
        mem += sizeof(*ngi->memos.memos) * ngi->memos.memos_count;
        ARRAY_FOREACH (i, ngi->memos.memos) {
            if (ngi->memos.memos[i].text)
                mem += strlen(ngi->memos.memos[i].text)+1;
        }
        mem += sizeof(*ngi->ignore) * ngi->ignore_count;
        ARRAY_FOREACH (i, ngi->ignore) {
            if (ngi->ignore[i])
                mem += strlen(ngi->ignore[i])+1;
        }
    }
    notice_lang(s_OperServ, user, OPER_STATS_ALL_NICKGROUPINFO_MEM,
                count, (mem+512) / 1024);

    return 0;
}

/*************************************************************************/
/*********************** NickServ command routines ***********************/
/*************************************************************************/

/* Return a help message. */

static void do_help(User *u)
{
    char *cmd = strtok_remaining();

    if (!cmd) {
        notice_help(s_NickServ, u, NICK_HELP);
        if (NSExpire)
            notice_help(s_NickServ, u, NICK_HELP_EXPIRES,
                        maketime(u->ngi,NSExpire,0));
        if (NSHelpWarning)
            notice_help(s_NickServ, u, NICK_HELP_WARNING);
    } else if (call_callback_2(cb_help, u, cmd) > 0) {
        return;
    } else if (stricmp(cmd, "COMMANDS") == 0) {
        notice_help(s_NickServ, u, NICK_HELP_COMMANDS);
        if (find_module("nickserv/mail-auth"))
            notice_help(s_NickServ, u, NICK_HELP_COMMANDS_AUTH);
        if (find_module("nickserv/link"))
            notice_help(s_NickServ, u, NICK_HELP_COMMANDS_LINK);
        if (find_module("nickserv/access"))
            notice_help(s_NickServ, u, NICK_HELP_COMMANDS_ACCESS);
        if (find_module("nickserv/autojoin"))
            notice_help(s_NickServ, u, NICK_HELP_COMMANDS_AJOIN);
        notice_help(s_NickServ, u, NICK_HELP_COMMANDS_SET);
        if (!NSListOpersOnly)
            notice_help(s_NickServ, u, NICK_HELP_COMMANDS_LIST);
        notice_help(s_NickServ, u, NICK_HELP_COMMANDS_LISTCHANS);
        call_callback_2(cb_help_cmds, u, 0);
        if (is_oper(u)) {
            notice_help(s_NickServ, u, NICK_OPER_HELP_COMMANDS);
            if (NSEnableDropEmail)
                notice_help(s_NickServ, u, NICK_OPER_HELP_COMMANDS_DROPEMAIL);
            if (EnableGetpass)
                notice_help(s_NickServ, u, NICK_OPER_HELP_COMMANDS_GETPASS);
            notice_help(s_NickServ, u, NICK_OPER_HELP_COMMANDS_FORBID);
            if (NSListOpersOnly)
                notice_help(s_NickServ, u, NICK_HELP_COMMANDS_LIST);
            if (find_module("nickserv/mail-auth"))
                notice_help(s_NickServ, u, NICK_OPER_HELP_COMMANDS_SETAUTH);
            call_callback_2(cb_help_cmds, u, 1);
            notice_help(s_NickServ, u, NICK_OPER_HELP_COMMANDS_END);
        }
    } else if (stricmp(cmd, "REGISTER") == 0) {
        notice_help(s_NickServ, u, NICK_HELP_REGISTER,
                    getstring(u->ngi,NICK_REGISTER_SYNTAX));
        notice_help(s_NickServ, u, NICK_HELP_REGISTER_EMAIL);
        notice_help(s_NickServ, u, NICK_HELP_REGISTER_END);
    } else if (stricmp(cmd, "DROP") == 0) {
        notice_help(s_NickServ, u, NICK_HELP_DROP);
        if (find_module("nickserv/link"))
            notice_help(s_NickServ, u, NICK_HELP_DROP_LINK);
        notice_help(s_NickServ, u, NICK_HELP_DROP_END);
    } else if ((stricmp(cmd, "DROPEMAIL") == 0
                || stricmp(cmd, "DROPEMAIL-CONFIRM") == 0)
               && NSEnableDropEmail
               && is_oper(u)
    ) {
        notice_help(s_NickServ, u, NICK_OPER_HELP_DROPEMAIL,
                    maketime(u->ngi,NSDropEmailExpire,0));
    } else if (stricmp(cmd, "SET") == 0) {
        notice_help(s_NickServ, u, NICK_HELP_SET);
        if (find_module("nickserv/link"))
            notice_help(s_NickServ, u, NICK_HELP_SET_OPTION_MAINNICK);
        notice_help(s_NickServ, u, NICK_HELP_SET_END);
        if (is_oper(u))
            notice_help(s_NickServ, u, NICK_OPER_HELP_SET);
    } else if (strnicmp(cmd, "SET", 3) == 0
               && isspace(cmd[3])
               && stricmp(cmd+4+strspn(cmd+4," \t"), "LANGUAGE") == 0) {
        int i;
        notice_help(s_NickServ, u, NICK_HELP_SET_LANGUAGE);
        for (i = 0; i < NUM_LANGS && langlist[i] >= 0; i++) {
            notice(s_NickServ, u->nick, "    %2d) %s",
                   i+1, getstring_lang(langlist[i],LANG_NAME));
        }
    } else if (stricmp(cmd, "INFO") == 0) {
        notice_help(s_NickServ, u, NICK_HELP_INFO);
        if (find_module("nickserv/mail-auth"))
            notice_help(s_NickServ, u, NICK_HELP_INFO_AUTH);
        if (is_oper(u))
            notice_help(s_NickServ, u, NICK_OPER_HELP_INFO);
    } else if (stricmp(cmd, "LIST") == 0) {
        if (is_oper(u)) {
            notice_help(s_NickServ, u, NICK_OPER_HELP_LIST);
            notice_help(s_NickServ, u, NICK_OPER_HELP_LIST_END);
        } else {
            notice_help(s_NickServ, u, NICK_HELP_LIST);
        }
        if (NSListOpersOnly)
            notice_help(s_NickServ, u, NICK_HELP_LIST_OPERSONLY);
    } else if (stricmp(cmd, "LISTEMAIL") == 0) {
        char buf[BUFSIZE];
        int msg = is_oper(u) ? NICK_LIST_OPER_SYNTAX : NICK_LIST_SYNTAX;
        snprintf(buf, sizeof(buf), getstring(u->ngi,msg), "LISTEMAIL");
        notice_help(s_NickServ, u, NICK_HELP_LISTEMAIL, buf);
        if (NSListOpersOnly)
            notice_help(s_NickServ, u, NICK_HELP_LIST_OPERSONLY);
    } else if (stricmp(cmd, "RECOVER") == 0) {
        notice_help(s_NickServ, u, NICK_HELP_RECOVER,
                    maketime(u->ngi,NSReleaseTimeout,MT_SECONDS));
    } else if (stricmp(cmd, "RELEASE") == 0) {
        notice_help(s_NickServ, u, NICK_HELP_RELEASE,
                    maketime(u->ngi,NSReleaseTimeout,MT_SECONDS));
    } else if (stricmp(cmd, "SUSPEND") == 0 && is_oper(u)) {
        notice_help(s_NickServ, u, NICK_OPER_HELP_SUSPEND, s_OperServ);
    } else {
        help_cmd(s_NickServ, u, THIS_MODULE, cmd);
    }
}

/*************************************************************************/

/* Register a nick. */

static void do_register(User *u)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    char *pass = strtok(NULL, " ");
    char *email = strtok(NULL, " ");
    int n;
    time_t now = time(NULL);

    if (readonly) {
        notice_lang(s_NickServ, u, NICK_REGISTRATION_DISABLED);
        return;
    }

    if (now < u->lastnickreg + NSRegDelay) {
        time_t left = (u->lastnickreg + NSRegDelay) - now;
        notice_lang(s_NickServ, u, NICK_REG_PLEASE_WAIT,
                    maketime(u->ngi, left, MT_SECONDS));

    } else if (time(NULL) < u->my_signon + NSInitialRegDelay) {
        time_t left = (u->my_signon + NSInitialRegDelay) - now;
        notice_lang(s_NickServ, u, NICK_REG_PLEASE_WAIT_FIRST,
                    maketime(u->ngi, left, MT_SECONDS));

    } else if (!pass || (NSRequireEmail && !email)
               || (stricmp(pass, u->nick) == 0
                   && (strtok(NULL, "")
                       || (email && (!strchr(email,'@')
                                     || !strchr(email,'.')))))
    ) {
        /* No password/email, or they (apparently) tried to include the nick
         * in the command. */
        syntax_error(s_NickServ, u, "REGISTER", NICK_REGISTER_SYNTAX);

    } else if (!reglink_check(u, u->nick, pass, email)) {
        /* Denied by the callback. */
        notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick);
        return;

    } else if (u->ni) {  /* i.e. there's already such a nick regged */
        if (u->ni->status & NS_VERBOTEN) {
            module_log("%s@%s tried to register forbidden nick %s",
                       u->username, u->host, u->nick);
            notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick);
        } else {
            if (u->ngi->flags & NF_SUSPENDED)
                module_log("%s@%s tried to register suspended nick %s",
                           u->username, u->host, u->nick);
            notice_lang(s_NickServ, u, NICK_X_ALREADY_REGISTERED, u->nick);
        }

    } else if (u->ngi == NICKGROUPINFO_INVALID) {
        module_log("%s@%s tried to register nick %s with missing nick group",
                   u->username, u->host, u->nick);
        notice_lang(s_NickServ, u, NICK_REGISTRATION_FAILED);

    } else if (put_nickinfo(get_nickinfo(u->nick))) {
        /* Theoretically impossible if the previous tests were false, but
         * just in case */
        module_log("REGISTER %s: u->ni is NULL but nick is registered!",
                   u->nick);
        notice_lang(s_NickServ, u, INTERNAL_ERROR);

    } else if (stricmp(pass, u->nick) == 0
               || (StrictPasswords && strlen(pass) < 5)
    ) {
        notice_lang(s_NickServ, u, MORE_OBSCURE_PASSWORD);

    } else if (email && !valid_email(email)) {
        /* Display the syntax as well in case the user just got E-mail and
         * password backwards.  Don't use syntax_error(), because that also
         * prints a "for more help" message which might just confuse the
         * user more. */
        char buf[BUFSIZE];
        snprintf(buf, sizeof(buf), getstring(u->ngi,NICK_REGISTER_SYNTAX),
                 "REGISTER");
        notice_lang(s_NickServ, u, SYNTAX_ERROR, buf);
        notice_lang(s_NickServ, u, BAD_EMAIL);

    } else if (email && rejected_email(email)) {
        notice_lang(s_NickServ, u, REJECTED_EMAIL);
        return;

    } else if (NSRegEmailMax && email && !is_services_admin(u)
               && ((n = count_nicks_with_email(email)) < 0
                   || n >= NSRegEmailMax)) {
        if (n < 0) {
            notice_lang(s_NickServ, u, NICK_REGISTER_EMAIL_UNAUTHED);
        } else {
            notice_lang(s_NickServ, u, NICK_REGISTER_TOO_MANY_NICKS, n,
                        NSRegEmailMax);
        }

    } else {
        int replied = 0;
        Password passbuf;

        /* Check for E-mail addresses used in suspended nicks, if requested */
        if (NSRegDenyIfSuspended && email) {
            NickGroupInfo *ngi_iter;
            for (ngi_iter = first_nickgroupinfo(); ngi_iter;
                 ngi_iter = next_nickgroupinfo()
            ) {
                if ((ngi_iter->flags & NF_SUSPENDED)
                 && ngi_iter->email
                 && stricmp(ngi_iter->email, email) == 0
                ) {
                    module_log("REGISTER from %s!%s@%s denied because E-mail"
                               " address %s is used by suspended nick %s",
                               u->nick, u->username, u->host, email,
                               ngi_iter->email);
                    notice_lang(s_NickServ, u, PERMISSION_DENIED);
                    return;
                }
            }
        }

        /* Make sure the password can be encrypted first */
        init_password(&passbuf);
        if (encrypt_password(pass, strlen(pass), &passbuf) != 0) {
            clear_password(&passbuf);
            memset(pass, 0, strlen(pass));
            module_log("Failed to encrypt password for %s (register)",
                       u->nick);
            notice_lang(s_NickServ, u, NICK_REGISTRATION_FAILED);
            return;
        }
        /* Do nick setup stuff */
        ni = makenick(u->nick, &ngi);
        if (!ni) {
            clear_password(&passbuf);
            module_log("makenick(%s) failed", u->nick);
            notice_lang(s_NickServ, u, NICK_REGISTRATION_FAILED);
            return;
        }
        copy_password(&ngi->pass, &passbuf);
        clear_password(&passbuf);
        ni->time_registered = ni->last_seen = time(NULL);
        ni->authstat = NA_IDENTIFIED | NA_RECOGNIZED;
        if (email)
            ngi->email = sstrdup(email);
        ngi->flags = NSDefFlags;
        ngi->memos.memomax = MEMOMAX_DEFAULT;
        ngi->channelmax = CHANMAX_DEFAULT;
        ngi->language = LANG_DEFAULT;
        ngi->timezone = TIMEZONE_DEFAULT;
        call_callback_4(cb_registered, u, ni, ngi, &replied);
        /* If the IDENTIFIED flag is still set (a module might have
         * cleared it, e.g. mail-auth), record the ID stamp */
        if (nick_identified(ni))
            ni->id_stamp = u->servicestamp;
        /* Link back and forth to user record and store modified data */
        u->ni = ni;
        u->ngi = ngi;
        ni->user = u;
        update_userinfo(u);
        /* Tell people about it */
        if (email) {
            module_log("%s registered by %s@%s (%s)",
                       u->nick, u->username, u->host, email);
        } else {
            module_log("%s registered by %s@%s",
                       u->nick, u->username, u->host);
        }
        if (!replied)
            notice_lang(s_NickServ, u, NICK_REGISTERED, u->nick);
        if (NSShowPassword)
            notice_lang(s_NickServ, u, NICK_PASSWORD_IS, pass);
        /* Clear password from memory and other last-minute things */
        memset(pass, 0, strlen(pass));
        /* Note time REGISTER command was used */
        u->lastnickreg = time(NULL);
        /* Set +r (or other registered-nick mode) if IDENTIFIED is still
         * set. */
        if (nick_identified(ni) && usermode_reg) {
            send_cmd(s_NickServ, "SVSMODE %s :+%s", u->nick,
                     mode_flags_to_string(usermode_reg, MODE_USER));
        }

    }

}

/*************************************************************************/

static void do_identify(User *u)
{
    char *pass = strtok(NULL, " ");
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;

    if (!pass || strtok_remaining()) {
        syntax_error(s_NickServ, u, "IDENTIFY", NICK_IDENTIFY_SYNTAX);

    } else if (!(ni = u->ni)) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);

    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, u->nick);

    } else if (!(ngi = u->ngi) || ngi == NICKGROUPINFO_INVALID) {
        module_log("IDENTIFY: missing NickGroupInfo for %s", u->nick);
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);

    } else if (ngi->flags & NF_SUSPENDED) {
        notice_lang(s_NickServ, u, NICK_X_SUSPENDED, u->nick);

    } else if (!nick_check_password(u, u->ni, pass, "IDENTIFY",
                                    NICK_IDENTIFY_FAILED)) {
        /* nothing */

    } else if (NSRequireEmail && !ngi->email) {
        ni->authstat |= NA_IDENT_NOMAIL;
        notice_lang(s_NickServ, u, NICK_IDENTIFY_EMAIL_MISSING, s_NickServ);

    } else if (call_callback_2(cb_id_check, u, pass) <= 0) {
        int old_authstat = ni->authstat;
        set_identified(u);
        if (!(old_authstat & NA_IDENTIFIED)) {
            /* Only log if the user wasn't previously identified */
            module_log("%s!%s@%s identified for nick %s",
                       u->nick, u->username, u->host, u->nick);
        }
        notice_lang(s_NickServ, u, NICK_IDENTIFY_SUCCEEDED);
        call_callback_2(cb_identified, u, old_authstat);
    }
}

/*************************************************************************/

static void do_drop(User *u)
{
    char *pass = strtok(NULL, " ");
    NickInfo *ni = u->ni;
    NickGroupInfo *ngi = (u->ngi==NICKGROUPINFO_INVALID ? NULL : u->ngi);

    if (readonly && !is_services_admin(u)) {
        notice_lang(s_NickServ, u, NICK_DROP_DISABLED);
        return;
    }

    if (!pass || strtok_remaining()) {
        syntax_error(s_NickServ, u, "DROP", NICK_DROP_SYNTAX);
        if (find_module("nickserv/link"))
            notice_lang(s_NickServ, u, NICK_DROP_WARNING);
    } else if (!ni || !ngi) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
    } else if (ngi->flags & NF_SUSPENDED) {
        notice_lang(s_NickServ, u, NICK_X_SUSPENDED, u->nick);
    } else if (!nick_check_password(u, u->ni, pass, "DROP",
                                    NICK_DROP_FAILED)) {
        /* nothing */
    } else {
        if (readonly)  /* they must be a servadmin in this case */
            notice_lang(s_NickServ, u, READ_ONLY_MODE);
        drop_nickgroup(ngi, u, NULL);
        notice_lang(s_NickServ, u, NICK_DROPPED);
    }
}

/*************************************************************************/

/* Services admin function to drop another user's nickname.  Privileges
 * assumed to be pre-checked.
 */

static void do_dropnick(User *u)
{
    char *nick = strtok(NULL, " ");
    NickInfo *ni;
    NickGroupInfo *ngi = NULL;

    if (!nick) {
        syntax_error(s_NickServ, u, "DROPNICK", NICK_DROPNICK_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->nickgroup && !(ngi = get_ngi(ni))) {
        notice_lang(s_NickServ, u, INTERNAL_ERROR);
        put_nickinfo(ni);
    } else if (NSSecureAdmins && nick_is_services_admin(ni)
               && !is_services_root(u)
    ) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
        put_nickinfo(ni);
        put_nickgroupinfo(ngi);
    } else {
        if (WallAdminPrivs) {
            wallops(s_NickServ, "\2%s\2 used DROPNICK on \2%s\2",
                    u->nick, ni->nick);
        }
        if (readonly)
            notice_lang(s_NickServ, u, READ_ONLY_MODE);
        if (ngi) {
            drop_nickgroup(ngi, u, PTR_INVALID);
        } else {
            module_log("%s!%s@%s dropped forbidden nick %s",
                       u->nick, u->username, u->host, ni->nick);
            delnick(ni);
        }
        notice_lang(s_NickServ, u, NICK_X_DROPPED, nick);
    }
}

/*************************************************************************/

/* Services admin function to drop all nicknames whose E-mail address
 * matches the given mask.  Privileges assumed to be pre-checked.
 */

/* List of recent DROPEMAILs for CONFIRM */
static struct {
    char sender[NICKMAX];       /* Who sent the command (empty = no entry) */
    char mask[BUFSIZE];         /* What mask was used */
    int count;
    time_t sent;                /* When the command was sent */
} dropemail_buffer[DROPEMAIL_BUFSIZE];

static void do_dropemail(User *u)
{
    char *mask = strtok(NULL, " ");
    NickGroupInfo *ngi;
    int count, i, found;

    /* Parameter check */
    if (!mask || strtok_remaining()) {
        syntax_error(s_NickServ, u, "DROPEMAIL", NICK_DROPEMAIL_SYNTAX);
        return;
    }
    if (strlen(mask) > sizeof(dropemail_buffer[0].mask)-1) {
        notice_lang(s_NickServ, u, NICK_DROPEMAIL_PATTERN_TOO_LONG,
                    sizeof(dropemail_buffer[0].mask)-1);
        return;
    }

    /* Count nicks matching this mask; exit if none found */
    if (strcmp(mask,"-") == 0)
        mask = NULL;
    count = 0;
    for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {
        if ((mask && ngi->email && match_wild_nocase(mask,ngi->email))
         || (!mask && !ngi->email)
        ) {
            count += ngi->nicks_count;
        }
    }
    if (!count) {
        notice_lang(s_NickServ, u, NICK_DROPEMAIL_NONE);
        return;
    }
    if (mask == NULL)
        mask = "-";

    /* Clear out any previous entries for this sender/mask */
    for (i = 0; i < DROPEMAIL_BUFSIZE; i++) {
        if (irc_stricmp(u->nick, dropemail_buffer[i].sender) == 0
         && stricmp(mask, dropemail_buffer[i].mask) == 0
        ) {
            memset(&dropemail_buffer[i], 0, sizeof(dropemail_buffer[i]));
        }
    }

    /* Register command in buffer */
    found = -1;
    for (i = 0; i < DROPEMAIL_BUFSIZE; i++) {
        if (!*dropemail_buffer[i].sender) {
            found = i;
            break;
        }
    }
    if (found < 0) {
        found = 0;
        for (i = 1; i < DROPEMAIL_BUFSIZE; i++) {
            if (dropemail_buffer[i].sent < dropemail_buffer[found].sent)
                found = i;
        }
    }
    memset(&dropemail_buffer[found], 0, sizeof(dropemail_buffer[found]));
    strbcpy(dropemail_buffer[found].sender, u->nick);
    strbcpy(dropemail_buffer[found].mask, mask);
    dropemail_buffer[found].sent = time(NULL);
    dropemail_buffer[found].count = count;

    /* Send count and prompt for confirmation */
    notice_lang(s_NickServ, u, NICK_DROPEMAIL_COUNT, count, s_NickServ, mask);
}


static void do_dropemail_confirm(User *u)
{
    char *mask = strtok(NULL, " ");
    NickGroupInfo *ngi;
    int i;

    /* Parameter check */
    if (!mask || strtok_remaining()) {
        syntax_error(s_NickServ, u, "DROPEMAIL-CONFIRM",
                     NICK_DROPEMAIL_CONFIRM_SYNTAX);
        return;
    }

    /* Make sure this is a DROPEMAIL that (1) we've seen and (2) hasn't
     * expired */
    for (i = 0; i < DROPEMAIL_BUFSIZE; i++) {
        if (irc_stricmp(u->nick, dropemail_buffer[i].sender) == 0
         && stricmp(mask, dropemail_buffer[i].mask) == 0
         && time(NULL) - dropemail_buffer[i].sent < NSDropEmailExpire
        ) {
            break;
        }
    }
    if (i >= DROPEMAIL_BUFSIZE) {
        notice_lang(s_NickServ, u, NICK_DROPEMAIL_CONFIRM_UNKNOWN);
        return;
    }

    /* Okay, go ahead and delete */
    notice_lang(s_NickServ, u, NICK_DROPEMAIL_CONFIRM_DROPPING,
                dropemail_buffer[i].count);
    if (readonly)
        notice_lang(s_NickServ, u, READ_ONLY_MODE);
    *dropemail_buffer[i].mask = 0;  /* clear out the entry */
    if (strcmp(mask,"-") == 0)
        mask = NULL;
    for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {
        if ((mask && ngi->email && match_wild_nocase(mask,ngi->email))
         || (!mask && !ngi->email)
        ) {
            drop_nickgroup(ngi, u, mask ? mask : "-");
        }
    }
    notice_lang(s_NickServ, u, NICK_DROPEMAIL_CONFIRM_DROPPED);
    if (WallAdminPrivs) {
        wallops(s_NickServ, "\2%s\2 used DROPEMAIL for \2%s\2 (%d nicks"
                " dropped)", u->nick, mask, dropemail_buffer[i].count);
    }
}

/*************************************************************************/

/* Show hidden info to nick owners and sadmins when the "ALL" parameter is
 * supplied. If a nick is online, the "Last seen address" changes to "Is
 * online from".
 * Syntax: INFO <nick> {ALL}
 * -TheShadow (13 Mar 1999)
 */

/* Check the status of show_all and make a note of having done so.  This is
 * used at the end, to see whether we should print a "use ALL for more info"
 * message.  Note that this should be the last test in a boolean expression,
 * to ensure that used_all isn't set inappropriately. */
#define CHECK_SHOW_ALL (used_all = 1, show_all)

static void do_info(User *u)
{
    char *nick = strtok(NULL, " ");
    char *param = strtok(NULL, " ");
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;

    if (!nick) {
        syntax_error(s_NickServ, u, "INFO", NICK_INFO_SYNTAX);

    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);

    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);

    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(s_NickServ, u, INTERNAL_ERROR);

    } else {
        char buf[BUFSIZE], *end;
        const char *commastr = getstring(u->ngi, COMMA_SPACE);
        int need_comma = 0;
        int nick_online = 0;
        const char *linked_nick_online = NULL;
        int can_show_all = 0, show_all = 0, used_all = 0;
        int i;

        /* Is the real owner of the nick we're looking up online? */
        if (ni->user && nick_id_or_rec(ni))
            nick_online = 1;

        /* Is any nick in the group in use?  Give preference to the main
         * nickname if multiple nicks are in use. */
        ARRAY_FOREACH (i, ngi->nicks) {
            NickInfo *ni2 = get_nickinfo(ngi->nicks[i]);
            if (!ni2 || ni2->nickgroup != ngi->id) {
                module_log("nick %s in nickgroup %u %s, clearing",
                           ngi->nicks[i], ngi->id,
                           !ni2 ? "does not exist" : "has wrong nickgroup ID");
                ARRAY_REMOVE(ngi->nicks, i);
                i--;
                if (ngi->nicks_count == 0) {  /* Should be impossible */
                    module_log("... nickgroup %u is now empty, dropping",
                               ngi->id);
                    notice_lang(s_NickServ, u, INTERNAL_ERROR);
                    put_nickgroupinfo(ngi);
                    put_nickinfo(ni);
                    delgroup(ngi);
                    return;
                }
            } else if (ni2->user && nick_id_or_rec(ni2)) {
                if (!linked_nick_online || i == ngi->mainnick)
                    linked_nick_online = ngi->nicks[i];
            }
            put_nickinfo(ni2);
        }

        /* Only show hidden fields to owner and sadmins and only when the ALL
         * parameter is used. */
        can_show_all = ((u==ni->user && nick_online) || is_services_admin(u));

        if (can_show_all && (param && stricmp(param, "ALL") == 0))
            show_all = 1;

        notice_lang(s_NickServ, u, NICK_INFO_REALNAME,
                    nick, ni->last_realname);

        /* Ignore HIDE and show the real hostmask to anyone who can use
         * INFO ALL. */
        if (nick_online) {
            if (!(ngi->flags & NF_HIDE_MASK) || can_show_all)
                notice_lang(s_NickServ, u, NICK_INFO_ADDRESS_ONLINE,
                        can_show_all ? ni->last_realmask : ni->last_usermask);
            else
                notice_lang(s_NickServ, u, NICK_INFO_ADDRESS_ONLINE_NOHOST,
                            ni->nick);
        } else {
            if (linked_nick_online
             && (!(ngi->flags & NF_PRIVATE) || can_show_all)
            ) {
                notice_lang(s_NickServ, u, NICK_INFO_ADDRESS_OTHER_NICK,
                            linked_nick_online);
            }
            if (!(ngi->flags & NF_HIDE_MASK) || can_show_all)
                notice_lang(s_NickServ, u, NICK_INFO_ADDRESS,
                        can_show_all ? ni->last_realmask : ni->last_usermask);
            strftime_lang(buf, sizeof(buf), u->ngi,
                          STRFTIME_DATE_TIME_FORMAT, ni->last_seen);
            notice_lang(s_NickServ, u, NICK_INFO_LAST_SEEN, buf);
        }

        strftime_lang(buf, sizeof(buf), u->ngi, STRFTIME_DATE_TIME_FORMAT,
                      ni->time_registered);
        notice_lang(s_NickServ, u, NICK_INFO_TIME_REGGED, buf);
        if (ni->last_quit && (!(ngi->flags & NF_HIDE_QUIT) || CHECK_SHOW_ALL))
            notice_lang(s_NickServ, u, NICK_INFO_LAST_QUIT, ni->last_quit);
        if (ngi->url)
            notice_lang(s_NickServ, u, NICK_INFO_URL, ngi->url);
        if (ngi->email && (!(ngi->flags & NF_HIDE_EMAIL) || CHECK_SHOW_ALL)) {
            if (ngi_unauthed(ngi)) {
                if (can_show_all) {
                    notice_lang(s_NickServ, u, NICK_INFO_EMAIL_UNAUTHED,
                                ngi->email);
                }
            } else {
                notice_lang(s_NickServ, u, NICK_INFO_EMAIL, ngi->email);
            }
        }
        if (ngi->info)
            notice_lang(s_NickServ, u, NICK_INFO_INFO, ngi->info);
        *buf = 0;
        end = buf;
        if (ngi->flags & NF_KILLPROTECT) {
            end += snprintf(end, sizeof(buf)-(end-buf), "%s",
                            getstring(u->ngi, NICK_INFO_OPT_KILL));
            need_comma = 1;
        }
        if (ngi->flags & NF_SECURE) {
            end += snprintf(end, sizeof(buf)-(end-buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->ngi, NICK_INFO_OPT_SECURE));
            need_comma = 1;
        }
        if (ngi->flags & NF_PRIVATE) {
            end += snprintf(end, sizeof(buf)-(end-buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->ngi, NICK_INFO_OPT_PRIVATE));
            need_comma = 1;
        }
        if (ngi->flags & NF_NOOP) {
            end += snprintf(end, sizeof(buf)-(end-buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->ngi, NICK_INFO_OPT_NOOP));
            need_comma = 1;
        }
        notice_lang(s_NickServ, u, NICK_INFO_OPTIONS,
                    *buf ? buf : getstring(u->ngi, NICK_INFO_OPT_NONE));

        if ((ni->status & NS_NOEXPIRE) && CHECK_SHOW_ALL)
            notice_lang(s_NickServ, u, NICK_INFO_NO_EXPIRE);

        if (ngi->flags & NF_SUSPENDED) {
            notice_lang(s_NickServ, u, NICK_X_SUSPENDED, nick);
            if (CHECK_SHOW_ALL) {
                char timebuf[BUFSIZE], expirebuf[BUFSIZE];
                strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                              STRFTIME_DATE_TIME_FORMAT, ngi->suspend_time);
                expires_in_lang(expirebuf, sizeof(expirebuf), u->ngi,
                                ngi->suspend_expires);
                notice_lang(s_NickServ, u, NICK_INFO_SUSPEND_DETAILS,
                            ngi->suspend_who, timebuf, expirebuf);
                notice_lang(s_NickServ, u, NICK_INFO_SUSPEND_REASON,
                            ngi->suspend_reason);
            }
        }

        if (can_show_all && !show_all && used_all)
            notice_lang(s_NickServ, u, NICK_INFO_SHOW_ALL, s_NickServ,
                        ni->nick);
    }

    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

static void do_listchans(User *u)
{
    NickInfo *ni = u->ni;
    NickGroupInfo *ngi = NULL;

    if (ni)
        ni->usecount++;
    if (is_oper(u)) {
        char *nick = strtok(NULL, " ");
        if (nick) {
            NickInfo *ni2 = get_nickinfo(nick);
            if (!ni2) {
                notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
                return;
            } else if (ni2 == ni) {
                /* Let the command through even for non-servadmins if they
                 * gave their own nick; it's less confusing than a
                 * "Permission denied" error */
            } else if (!is_services_admin(u)) {
                notice_lang(s_NickServ, u, PERMISSION_DENIED);
                put_nickinfo(ni2);
                return;
            } else {
                put_nickinfo(ni);
                ni = ni2;
            }
        }
    } else if (strtok_remaining()) {
        syntax_error(s_NickServ, u, "LISTCHANS", NICK_LISTCHANS_SYNTAX);
        return;
    }
    if (!ni) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
        return;
    }
    if (ni->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, ni->nick);
    } else if (!user_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(s_NickServ, u, INTERNAL_ERROR);
    } else if (!ngi->channels_count) {
        notice_lang(s_NickServ, u, NICK_LISTCHANS_NONE, ni->nick);
    } else {
        int i;
        notice_lang(s_NickServ, u, NICK_LISTCHANS_HEADER, ni->nick);
        ARRAY_FOREACH (i, ngi->channels)
            notice(s_NickServ, u->nick, "    %s", ngi->channels[i]);
        notice_lang(s_NickServ, u, NICK_LISTCHANS_END, ngi->channels_count);
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

static void do_list(User *u)
{
    char *pattern = strtok(NULL, " ");
    char *keyword;
    NickInfo *ni;
    NickGroupInfo *ngi;
    int nnicks;
    char buf[BUFSIZE];
    int is_servadmin = is_services_admin(u);
    int16 match_NS = 0;  /* NS_ flags a nick must match one of to qualify */
    int32 match_NF = 0;  /* NF_ flags a nick must match one of to qualify */
    int match_auth = 0;  /* 1 if we match on nicks with auth codes */
    int have_auth_module = 0;  /* so we don't show no-auth char if no module */
    int skip = 0;        /* number of records to skip before displaying */


    if (NSListOpersOnly && !is_oper(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
        return;
    }

    have_auth_module = (find_module("nickserv/mail-auth") != NULL);

    if (pattern && *pattern == '+') {
        skip = (int)atolsafe(pattern+1, 0, INT_MAX);
        if (skip < 0) {
            syntax_error(s_NickServ, u, "LIST",
                         is_oper(u)? NICK_LIST_OPER_SYNTAX: NICK_LIST_SYNTAX);
            return;
        }
        pattern = strtok(NULL, " ");
    }

    if (!pattern) {
        syntax_error(s_NickServ, u, "LIST",
                     is_oper(u) ? NICK_LIST_OPER_SYNTAX : NICK_LIST_SYNTAX);
    } else {
        int mask_has_at = (strchr(pattern,'@') != 0);

        nnicks = 0;

        while (is_servadmin && (keyword = strtok(NULL, " "))) {
            if (stricmp(keyword, "FORBIDDEN") == 0) {
                match_NS |= NS_VERBOTEN;
            } else if (stricmp(keyword, "NOEXPIRE") == 0) {
                match_NS |= NS_NOEXPIRE;
            } else if (stricmp(keyword, "SUSPENDED") == 0) {
                match_NF |= NF_SUSPENDED;
            } else if (stricmp(keyword, "NOAUTH") == 0 && have_auth_module) {
                match_auth = 1;
            } else {
                syntax_error(s_NickServ, u, "LIST",
                     is_oper(u) ? NICK_LIST_OPER_SYNTAX : NICK_LIST_SYNTAX);
            }
        }

        for (ni = first_nickinfo(); ni; ni = next_nickinfo()) {
            int can_see_usermask = 0;  /* Does user get to see the usermask? */
            const char *mask;          /* Which mask to show? (fake or real) */

            if (u == ni->user || is_services_admin(u))
                mask = ni->last_realmask;
            else
                mask = ni->last_usermask;
            ngi = get_nickgroupinfo(ni->nickgroup);
            if (!is_servadmin && ((ngi && (ngi->flags & NF_PRIVATE))
                                  || (ni->status & NS_VERBOTEN))) {
                put_nickgroupinfo(ngi);
                continue;
            }
            if (match_NS || match_NF || match_auth) {
                /* We have flags, now see if they match */
                if (!((ni->status & match_NS)
                   || (ngi && (ngi->flags & match_NF))
                   || (ngi && ngi_unauthed(ngi) && match_auth)
                )) {
                    put_nickgroupinfo(ngi);
                    continue;
                }
            }
            if (!is_servadmin && (ngi->flags & NF_HIDE_MASK)) {
                snprintf(buf, sizeof(buf), "%-20s  [Hidden]", ni->nick);
            } else if (ni->status & NS_VERBOTEN) {
                snprintf(buf, sizeof(buf), "%-20s  [Forbidden]", ni->nick);
            } else {
                can_see_usermask = 1;
                snprintf(buf, sizeof(buf), "%-20s  %s", ni->nick,
                         mask ? mask : "[Never online]");
            }
            if ((!mask_has_at && match_wild_nocase(pattern, ni->nick))
             || (mask_has_at && can_see_usermask && mask
                 && match_wild_nocase(pattern, mask))
            ) {
                nnicks++;
                if (nnicks > skip && nnicks <= skip+ListMax) {
                    char suspended_char = ' ';
                    char noexpire_char = ' ';
                    const char *auth_char = have_auth_module ? " " : "";
                    if (is_servadmin) {
                        if (ngi && (ngi->flags & NF_SUSPENDED))
                            suspended_char = '*';
                        if (ni->status & NS_NOEXPIRE)
                            noexpire_char = '!';
                        if (have_auth_module && ngi && ngi_unauthed(ngi))
                            auth_char = "?";
                    }
                    if (nnicks == 1)  /* display header before first result */
                        notice_lang(s_NickServ, u, NICK_LIST_HEADER, pattern);
                    notice(s_NickServ, u->nick, "   %c%c%s %s",
                           suspended_char, noexpire_char, auth_char, buf);
                }
            }
            put_nickgroupinfo(ngi);
        }  /* for each nick */
        if (nnicks) {
            int count = nnicks - skip;
            if (count < 0)
                count = 0;
            else if (count > ListMax)
                count = ListMax;
            notice_lang(s_NickServ, u, LIST_RESULTS, count, nnicks);
        } else {
            notice_lang(s_NickServ, u, NICK_LIST_NO_MATCH);
        }
    }
}

/*************************************************************************/

static void do_listemail(User *u)
{
    char *pattern = strtok(NULL, " ");
    char *keyword;
    NickInfo *ni;
    NickGroupInfo *ngi;
    int nnicks;
    char buf[BUFSIZE];
    int is_servadmin = is_services_admin(u);
    int16 match_NS = 0;  /* NS_ flags a nick must match one of to qualify */
    int32 match_NF = 0;  /* NF_ flags a nick must match one of to qualify */
    int match_auth = 0;  /* 1 if we match on nicks with auth codes */
    int have_auth_module = 0;  /* so we don't show no-auth char if no module */
    int skip = 0;        /* number of records to skip before displaying */


    if (NSListOpersOnly && !is_oper(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
        return;
    }

    have_auth_module = (find_module("nickserv/mail-auth") != NULL);

    if (pattern && *pattern == '+') {
        skip = (int)atolsafe(pattern+1, 0, INT_MAX);
        if (skip < 0) {
            syntax_error(s_NickServ, u, "LISTEMAIL",
                         is_oper(u)? NICK_LIST_OPER_SYNTAX: NICK_LIST_SYNTAX);
            return;
        }
        pattern = strtok(NULL, " ");
    }

    if (!pattern) {
        syntax_error(s_NickServ, u, "LISTEMAIL",
                     is_oper(u) ? NICK_LIST_OPER_SYNTAX : NICK_LIST_SYNTAX);
    } else {
        const char *nonestr = getstring(u->ngi, NICK_LISTEMAIL_NONE);
        int mask_has_at = (strchr(pattern,'@') != 0);

        nnicks = 0;

        while (is_servadmin && (keyword = strtok(NULL, " "))) {
            if (stricmp(keyword, "FORBIDDEN") == 0) {
                match_NS |= NS_VERBOTEN;
            } else if (stricmp(keyword, "NOEXPIRE") == 0) {
                match_NS |= NS_NOEXPIRE;
            } else if (stricmp(keyword, "SUSPENDED") == 0) {
                match_NF |= NF_SUSPENDED;
            } else if (stricmp(keyword, "NOAUTH") == 0 && have_auth_module) {
                match_auth = 1;
            } else {
                syntax_error(s_NickServ, u, "LISTEMAIL",
                     is_oper(u) ? NICK_LIST_OPER_SYNTAX : NICK_LIST_SYNTAX);
            }
        }

        for (ni = first_nickinfo(); ni; ni = next_nickinfo()) {
            int can_see_email = 0;  /* Does user get to see the address? */

            ngi = get_nickgroupinfo(ni->nickgroup);
            if (!is_servadmin && ((ngi && (ngi->flags & NF_PRIVATE))
                                  || (ni->status & NS_VERBOTEN))) {
                put_nickgroupinfo(ngi);
                continue;
            }
            if (match_NS || match_NF || match_auth) {
                /* We have flags, now see if they match */
                if (!((ni->status & match_NS)
                   || (ngi && (ngi->flags & match_NF))
                   || (ngi && ngi_unauthed(ngi) && match_auth)
                )) {
                    put_nickgroupinfo(ngi);
                    continue;
                }
            }
            if (!is_servadmin && (ngi->flags & NF_HIDE_EMAIL)
             && (!valid_ngi(u) || ngi->id!=u->ngi->id || !user_identified(u))){
                snprintf(buf, sizeof(buf), "%-20s  [Hidden]", ni->nick);
            } else if (ni->status & NS_VERBOTEN) {
                snprintf(buf, sizeof(buf), "%-20s  [Forbidden]", ni->nick);
            } else {
                can_see_email = 1;
                snprintf(buf, sizeof(buf), "%-20s  %s", ni->nick,
                         ngi->email ? ngi->email : nonestr);
            }
            if ((!mask_has_at && match_wild_nocase(pattern, ni->nick))
             || (mask_has_at && can_see_email && ngi->email
                 && match_wild_nocase(pattern, ngi->email))
            ) {
                nnicks++;
                if (nnicks > skip && nnicks <= skip+ListMax) {
                    char suspended_char = ' ';
                    char noexpire_char = ' ';
                    const char *auth_char = have_auth_module ? " " : "";
                    if (is_servadmin) {
                        if (ngi && (ngi->flags & NF_SUSPENDED))
                            suspended_char = '*';
                        if (ni->status & NS_NOEXPIRE)
                            noexpire_char = '!';
                        if (have_auth_module && ngi && ngi_unauthed(ngi))
                            auth_char = "?";
                    }
                    if (nnicks == 1)  /* display header before first result */
                        notice_lang(s_NickServ, u, NICK_LIST_HEADER, pattern);
                    notice(s_NickServ, u->nick, "   %c%c%s %s",
                           suspended_char, noexpire_char, auth_char, buf);
                }
            }
            put_nickgroupinfo(ngi);
        }  /* for each nick */
        if (nnicks) {
            int count = nnicks - skip;
            if (count < 0)
                count = 0;
            else if (count > ListMax)
                count = ListMax;
            notice_lang(s_NickServ, u, LIST_RESULTS, count, nnicks);
        } else {
            notice_lang(s_NickServ, u, NICK_LIST_NO_MATCH);
        }
    }
}

/*************************************************************************/

static void do_recover(User *u)
{
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    NickInfo *ni;
    User *u2;

    if (!nick || strtok_remaining()) {
        syntax_error(s_NickServ, u, "RECOVER", NICK_RECOVER_SYNTAX);
    } else if (!(u2 = get_user(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (!(ni = u2->ni)) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_GUESTED) {
        notice_lang(s_NickServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
    } else if (irc_stricmp(nick, u->nick) == 0) {
        notice_lang(s_NickServ, u, NICK_NO_RECOVER_SELF);
    } else {
        if (pass) {
            if (!nick_check_password(u, ni, pass, "RECOVER", ACCESS_DENIED))
                return;
        } else if (!has_identified_nick(u, ni->nickgroup)) {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
            return;
        }
        collide_nick(ni, 0);
        notice_lang(s_NickServ, u, NICK_RECOVERED, s_NickServ, nick);
    }
}

/*************************************************************************/

static void do_release(User *u)
{
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    NickInfo *ni = NULL;

    if (!nick || strtok_remaining()) {
        syntax_error(s_NickServ, u, "RELEASE", NICK_RELEASE_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
    } else if (!(ni->status & NS_KILL_HELD)) {
        notice_lang(s_NickServ, u, NICK_RELEASE_NOT_HELD, nick);
    } else {
        if (pass) {
            if (!nick_check_password(u, ni, pass, "RELEASE", ACCESS_DENIED))
                return;
        } else if (!has_identified_nick(u, ni->nickgroup)) {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
            return;
        }
        release_nick(ni, 0);
        notice_lang(s_NickServ, u, NICK_RELEASED);
    }
    put_nickinfo(ni);
}

/*************************************************************************/

static void do_ghost(User *u)
{
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    NickInfo *ni;
    User *u2;

    if (!nick || strtok_remaining()) {
        syntax_error(s_NickServ, u, "GHOST", NICK_GHOST_SYNTAX);
    } else if (!(u2 = get_user(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (!(ni = u2->ni)) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_GUESTED) {
        notice_lang(s_NickServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
    } else if (irc_stricmp(nick, u->nick) == 0) {
        notice_lang(s_NickServ, u, NICK_NO_GHOST_SELF);
    } else {
        char buf[NICKMAX+32];
        if (pass) {
            if (!nick_check_password(u, ni, pass, "GHOST", ACCESS_DENIED))
                return;
        } else if (!has_identified_nick(u, ni->nickgroup)) {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
            return;
        }
        snprintf(buf, sizeof(buf), "GHOST command used by %s", u->nick);
        kill_user(s_NickServ, nick, buf);
        notice_lang(s_NickServ, u, NICK_GHOST_KILLED);
    }
}

/*************************************************************************/

static void do_status(User *u)
{
    char *nick;
    User *u2;
    int i = 0;

    while ((nick = strtok(NULL, " ")) && (i++ < 16)) {
        if (!(u2 = get_user(nick)) || !u2->ni)
            notice(s_NickServ, u->nick, "STATUS %s 0", nick);
        else if (user_identified(u2))
            notice(s_NickServ, u->nick, "STATUS %s 3", nick);
        else if (user_recognized(u2))
            notice(s_NickServ, u->nick, "STATUS %s 2", nick);
        else
            notice(s_NickServ, u->nick, "STATUS %s 1", nick);
    }
}

/*************************************************************************/

static void do_getpass(User *u)
{
    char *nick = strtok(NULL, " ");
    char pass[PASSMAX];
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;
    int i;

    /* Assumes that permission checking has already been done. */
    if (!nick) {
        syntax_error(s_NickServ, u, "GETPASS", NICK_GETPASS_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(s_NickServ, u, INTERNAL_ERROR);
    } else if (NSSecureAdmins && nick_is_services_admin(ni)
               && !is_services_root(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else if ((i = decrypt_password(&ngi->pass, pass, PASSMAX)) == -2) {
        notice_lang(s_NickServ, u, NICK_GETPASS_UNAVAILABLE, nick);
    } else if (i != 0) {
        module_log("decrypt_password() failed for GETPASS on %s", nick);
        notice_lang(s_NickServ, u, INTERNAL_ERROR);
    } else {
        module_log("%s!%s@%s used GETPASS on %s",
                   u->nick, u->username, u->host, ni->nick);
        if (WallAdminPrivs) {
            wallops(s_NickServ, "\2%s\2 used GETPASS on \2%s\2",
                    u->nick, ni->nick);
        }
        notice_lang(s_NickServ, u, NICK_GETPASS_PASSWORD_IS, nick, pass);
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

static void do_forbid(User *u)
{
    NickInfo *ni;
    char *nick = strtok(NULL, " ");
    User *u2;

    /* Assumes that permission checking has already been done. */
    if (!nick) {
        syntax_error(s_NickServ, u, "FORBID", NICK_FORBID_SYNTAX);
        return;
    }
    u2 = get_user(nick);
    if ((ni = get_nickinfo(nick)) != NULL) {
        if (NSSecureAdmins && nick_is_services_admin(ni)
            && !is_services_root(u)
        ) {
            notice_lang(s_NickServ, u, PERMISSION_DENIED);
            return;
        }
        if (u2) {
            put_nickinfo(u2->ni);
            put_nickgroupinfo(u2->ngi);
            u2->ni = NULL;
            u2->ngi = NULL;
        }
        delnick(ni);
    }

    if (readonly)
        notice_lang(s_NickServ, u, READ_ONLY_MODE);
    ni = makenick(nick, NULL);
    if (ni) {
        ni->status |= NS_VERBOTEN;
        ni->time_registered = time(NULL);
        put_nickinfo(ni);
        module_log("%s!%s@%s set FORBID for nick %s",
                   u->nick, u->username, u->host, nick);
        notice_lang(s_NickServ, u, NICK_FORBID_SUCCEEDED, nick);
        if (WallAdminPrivs)
            wallops(s_NickServ, "\2%s\2 used FORBID on \2%s\2", u->nick, nick);
        /* If someone is using the nick, make them stop */
        if (u2)
            validate_user(u2);
    } else {
        module_log("Valid FORBID for %s by %s!%s@%s failed",
                   nick, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, NICK_FORBID_FAILED, nick);
    }
}

/*************************************************************************/

static void do_suspend(User *u)
{
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;
    char *expiry, *nick, *reason;
    time_t expires;

    nick = strtok(NULL, " ");
    if (nick && *nick == '+') {
        expiry = nick+1;
        nick = strtok(NULL, " ");
    } else {
        expiry = NULL;
    }
    reason = strtok_remaining();

    if (!nick || !reason) {
        syntax_error(s_NickServ, u, "SUSPEND", NICK_SUSPEND_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(s_NickServ, u, INTERNAL_ERROR);
    } else if (ngi->flags & NF_SUSPENDED) {
        notice_lang(s_NickServ, u, NICK_SUSPEND_ALREADY_SUSPENDED, nick);
    } else if (NSSecureAdmins && nick_is_services_admin(ni)
               && !is_services_root(u)
    ) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else {
        if (expiry)
            expires = dotime(expiry);
        else
            expires = NSSuspendExpire;
        if (expires < 0) {
            notice_lang(s_NickServ, u, BAD_EXPIRY_TIME);
            return;
        } else if (expires > 0) {
            expires += time(NULL);      /* Set an absolute time */
        }
        module_log("%s!%s@%s suspended %s",
                   u->nick, u->username, u->host, ni->nick);
        suspend_nick(ngi, reason, u->nick, expires);
        notice_lang(s_NickServ, u, NICK_SUSPEND_SUCCEEDED, nick);
        if (readonly)
            notice_lang(s_NickServ, u, READ_ONLY_MODE);
        if (WallAdminPrivs) {
            wallops(s_NickServ, "\2%s\2 used SUSPEND on \2%s\2",
                    u->nick, ni->nick);
        }
        /* If someone is using the nick, make them stop */
        if (ni->user)
            validate_user(ni->user);
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

static void do_unsuspend(User *u)
{
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;
    char *nick = strtok(NULL, " ");

    if (!nick) {
        syntax_error(s_NickServ, u, "UNSUSPEND", NICK_UNSUSPEND_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(s_NickServ, u, INTERNAL_ERROR);
    } else if (!(ngi->flags & NF_SUSPENDED)) {
        notice_lang(s_NickServ, u, NICK_UNSUSPEND_NOT_SUSPENDED, nick);
    } else {
        module_log("%s!%s@%s unsuspended %s",
                   u->nick, u->username, u->host, ni->nick);
        unsuspend_nick(ngi, 1);
        notice_lang(s_NickServ, u, NICK_UNSUSPEND_SUCCEEDED, nick);
        if (readonly)
            notice_lang(s_NickServ, u, READ_ONLY_MODE);
        if (WallAdminPrivs) {
            wallops(s_NickServ, "\2%s\2 used UNSUSPEND on \2%s\2",
                    u->nick, ni->nick);
        }
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

#ifdef DEBUG_COMMANDS

/* Return all the fields in the NickInfo structure. */

static void do_listnick(User *u)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    char *nick = strtok(NULL, " ");
    char buf1[BUFSIZE], buf2[BUFSIZE];
    char *s;
    int i;

    if (!nick)
        return;
    ni = get_nickinfo(nick);
    if (!ni) {
        notice(s_NickServ, u->nick, "%s", nick);
        notice(s_NickServ, u->nick, ":");
        return;
    }
    ngi = get_nickgroupinfo(ni->nickgroup);
    notice(s_NickServ, u->nick, "%s group:%u usermask:%s realmask:%s"
           " reg:%d seen:%d stat:%04X auth:%04X idstamp:%d badpass:%d :%s;%s",
           ni->nick, (int)ni->nickgroup, ni->last_usermask, ni->last_realmask,
           (int)ni->time_registered, (int)ni->last_seen, ni->status & 0xFFFF,
           ni->authstat & 0xFFFF, ni->id_stamp, ni->bad_passwords,
           ni->last_realname, (ni->last_quit ? ni->last_quit : "-"));
    if (ngi) {
        if (ngi->authcode) {
            snprintf(buf1, sizeof(buf1), "%d.%d",
                     (int)ngi->authcode, (int)ngi->authset);
        } else {
            *buf1 = 0;
        }
        if (ngi->flags & NF_SUSPENDED) {
            snprintf(buf2, sizeof(buf2), "%s.%d.%d.%s",
                     ngi->suspend_who, (int)ngi->suspend_time,
                     (int)ngi->suspend_expires,
                     ngi->suspend_reason ? ngi->suspend_reason : "-");
            strnrepl(buf2, sizeof(buf2), " ", "_");
        } else {
            *buf2 = 0;
        }
        notice(s_NickServ, u->nick, "+ flags:%08X ospriv:%04X authcode:%s"
               " susp:%s chancnt:%d chanmax:%d lang:%d tz:%d acccnt:%d"
               " ajoincnt:%d memocnt:%d memomax:%d igncnt:%d",
               ngi->flags, ngi->os_priv, buf1, buf2, ngi->channels_count,
               ngi->channelmax, ngi->language, ngi->timezone,
               ngi->access_count, ngi->ajoin_count, ngi->memos.memos_count,
               ngi->memos.memomax, ngi->ignore_count);
        notice(s_NickServ, u->nick, "+ url:%s", ngi->url ? ngi->url : "");
        notice(s_NickServ, u->nick, "+ email:%s", ngi->email?ngi->email:"");
        notice(s_NickServ, u->nick, "+ info:%s", ngi->info ? ngi->info : "");
        s = buf1;
        *buf1 = 0;
        ARRAY_FOREACH (i, ngi->access)
            s += snprintf(s, sizeof(buf1)-(s-buf1), "%s%s",
                          *buf1 ? "," : "", ngi->access[i]);
        strnrepl(buf1, sizeof(buf1), " ", "_");
        notice(s_NickServ, u->nick, "+ acc:%s", buf1);
        s = buf1;
        *buf1 = 0;
        ARRAY_FOREACH (i, ngi->ajoin)
            s += snprintf(s, sizeof(buf1)-(s-buf1), "%s%s",
                          *buf1 ? "," : "", ngi->ajoin[i]);
        strnrepl(buf1, sizeof(buf1), " ", "_");
        notice(s_NickServ, u->nick, "+ ajoin:%s", buf1);
        s = buf1;
        *buf1 = 0;
        ARRAY_FOREACH (i, ngi->ignore)
            s += snprintf(s, sizeof(buf1)-(s-buf1), "%s%s",
                          *buf1 ? "," : "", ngi->ignore[i]);
        strnrepl(buf1, sizeof(buf1), " ", "_");
        notice(s_NickServ, u->nick, "+ ign:%s", buf1);
    } else {
        notice(s_NickServ, u->nick, ":");
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

#endif /* DEBUG_COMMANDS */

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int NSDefKill;
static int NSDefKillQuick;
static int NSDefSecure;
static int NSDefPrivate;
static int NSDefNoOp;
static int NSDefHideEmail;
static int NSDefHideUsermask;
static int NSDefHideQuit;
static int NSDefMemoSignon;
static int NSDefMemoReceive;
static int NSEnableRegister;
static char *temp_nsuserhost;

static int do_NSAlias(const char *filename, int linenum, char *param);

ConfigDirective module_config[] = {
    { "NickServName",     { { CD_STRING, CF_DIRREQ, &s_NickServ },
                            { CD_STRING, 0, &desc_NickServ } } },
    { "NSAlias",          { { CD_FUNC, 0, do_NSAlias } } },
    { "NSAllowKillImmed", { { CD_SET, 0, &NSAllowKillImmed } } },
    { "NSDefHideEmail",   { { CD_SET, 0, &NSDefHideEmail } } },
    { "NSDefHideQuit",    { { CD_SET, 0, &NSDefHideQuit } } },
    { "NSDefHideUsermask",{ { CD_SET, 0, &NSDefHideUsermask } } },
    { "NSDefKill",        { { CD_SET, 0, &NSDefKill } } },
    { "NSDefKillQuick",   { { CD_SET, 0, &NSDefKillQuick } } },
    { "NSDefMemoReceive", { { CD_SET, 0, &NSDefMemoReceive } } },
    { "NSDefMemoSignon",  { { CD_SET, 0, &NSDefMemoSignon } } },
    { "NSDefNoOp",        { { CD_SET, 0, &NSDefNoOp } } },
    { "NSDefPrivate",     { { CD_SET, 0, &NSDefPrivate } } },
    { "NSDefSecure",      { { CD_SET, 0, &NSDefSecure } } },
    { "NSDropEmailExpire",{ { CD_TIME, CF_DIRREQ, &NSDropEmailExpire } } },
    { "NSEnableDropEmail",{ { CD_SET, 0, &NSEnableDropEmail } } },
    { "NSEnableRegister", { { CD_SET, 0, &NSEnableRegister } } },
    { "NSEnforcerUser",   { { CD_STRING, CF_DIRREQ, &temp_nsuserhost } } },
    { "NSExpire",         { { CD_TIME, 0, &NSExpire } } },
    { "NSExpireWarning",  { { CD_TIME, 0, &NSExpireWarning } } },
    { "NSForceNickChange",{ { CD_SET, 0, &NSForceNickChange } } },
    { "NSHelpWarning",    { { CD_SET, 0, &NSHelpWarning } } },
    { "NSInitialRegDelay",{ { CD_TIME, 0, &NSInitialRegDelay } } },
    { "NSListOpersOnly",  { { CD_SET, 0, &NSListOpersOnly } } },
    { "NSRegDelay",       { { CD_TIME, 0, &NSRegDelay } } },
    { "NSRegDenyIfSuspended",{{CD_SET, 0, &NSRegDenyIfSuspended } } },
    { "NSRegEmailMax",    { { CD_POSINT, 0, &NSRegEmailMax } } },
    { "NSRequireEmail",   { { CD_SET, 0, &NSRequireEmail } } },
    { "NSReleaseTimeout", { { CD_TIME, CF_DIRREQ, &NSReleaseTimeout } } },
    { "NSSecureAdmins",   { { CD_SET, 0, &NSSecureAdmins } } },
    { "NSSetEmailDelay",  { { CD_TIME, 0, &NSSetEmailDelay } } },
    { "NSShowPassword",   { { CD_SET, 0, &NSShowPassword } } },
    { "NSSuspendExpire",  { { CD_TIME, 0 , &NSSuspendExpire },
                            { CD_TIME, 0 , &NSSuspendGrace } } },
    { NULL }
};

/* Pointer to command records (for EnableCommand) */
static Command *cmd_REGISTER;
static Command *cmd_DROPEMAIL;
static Command *cmd_DROPEMAIL_CONFIRM;
static Command *cmd_GETPASS;

/* Old message numbers */
static int old_REGISTER_SYNTAX          = -1;
static int old_HELP_REGISTER_EMAIL      = -1;
static int old_HELP_UNSET               = -1;
static int old_DISCONNECT_IN_1_MINUTE   = -1;
static int old_DISCONNECT_IN_20_SECONDS = -1;

/*************************************************************************/

/* NSAlias handling (array maintenance) */

static int do_NSAlias(const char *filename, int linenum, char *param)
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
        config_error(filename, linenum, "Missing = in NSAlias parameter");
        return 0;
    }
    *s++ = 0;
    ARRAY_EXTEND(new_aliases);
    new_aliases[new_aliases_count-1].alias = sstrdup(param);
    new_aliases[new_aliases_count-1].command = sstrdup(s);
    return 1;
}

/*************************************************************************/

static void handle_config(void)
{
    char *s;

    if (temp_nsuserhost) {
        NSEnforcerUser = temp_nsuserhost;
        if (!(s = strchr(temp_nsuserhost, '@'))) {
            NSEnforcerHost = ServiceHost;
        } else {
            *s++ = 0;
            NSEnforcerHost = s;
        }
    }

    NSDefFlags = 0;
    if (NSDefKill)
        NSDefFlags |= NF_KILLPROTECT;
    if (NSDefKillQuick)
        NSDefFlags |= NF_KILL_QUICK;
    if (NSDefSecure)
        NSDefFlags |= NF_SECURE;
    if (NSDefPrivate)
        NSDefFlags |= NF_PRIVATE;
    if (NSDefNoOp)
        NSDefFlags |= NF_NOOP;
    if (NSDefHideEmail)
        NSDefFlags |= NF_HIDE_EMAIL;
    if (NSDefHideUsermask)
        NSDefFlags |= NF_HIDE_MASK;
    if (NSDefHideQuit)
        NSDefFlags |= NF_HIDE_QUIT;
    if (NSDefMemoSignon)
        NSDefFlags |= NF_MEMO_SIGNON;
    if (NSDefMemoReceive)
        NSDefFlags |= NF_MEMO_RECEIVE;

    if (NSForceNickChange && !(protocol_features & PF_CHANGENICK)) {
        module_log("warning: forced nick changing not supported by IRC"
                   " server, disabling NSForceNickChange");
        NSForceNickChange = 0;
    }
}

/*************************************************************************/

static int do_command_line(const char *option, const char *value)
{
    NickGroupInfo *ngi;

    if (!option || strcmp(option, "clear-nick-email") != 0)
        return 0;
    if (value) {
        fprintf(stderr, "-clear-nick-email takes no options\n");
        return 2;
    }
    module_log("Clearing all E-mail addresses (-clear-nick-email specified"
               " on command line)");
    for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {
        free(ngi->email);
        ngi->email = NULL;
    }
    return 1;
}

/*************************************************************************/

static int do_reconfigure(int after_configure)
{
    static char old_s_NickServ[NICKMAX];
    static char *old_desc_NickServ = NULL;

    if (!after_configure) {
        /* Before reconfiguration: save old values. */
        strbcpy(old_s_NickServ, s_NickServ);
        old_desc_NickServ = strdup(desc_NickServ);
    } else {
        /* After reconfiguration: handle value changes. */
        handle_config();
        if (strcmp(old_s_NickServ,s_NickServ) != 0)
            send_nickchange(old_s_NickServ, s_NickServ);
        if (!old_desc_NickServ || strcmp(old_desc_NickServ,desc_NickServ) != 0)
            send_namechange(s_NickServ, desc_NickServ);
        free(old_desc_NickServ);
        if (NSEnableRegister)
            cmd_REGISTER->name = "REGISTER";
        else
            cmd_REGISTER->name = "";
        if (NSEnableDropEmail) {
            cmd_DROPEMAIL->name = "DROPEMAIL";
            cmd_DROPEMAIL_CONFIRM->name = "DROPEMAIL-CONFIRM";
        } else {
            cmd_DROPEMAIL->name = "";
            cmd_DROPEMAIL_CONFIRM->name = "";
        }
        if (EnableGetpass)
            cmd_GETPASS->name = "GETPASS";
        else
            cmd_GETPASS->name = "";
        if (NSRequireEmail) {
            mapstring(NICK_REGISTER_SYNTAX, NICK_REGISTER_REQ_EMAIL_SYNTAX);
            mapstring(NICK_HELP_REGISTER_EMAIL, NICK_HELP_REGISTER_EMAIL_REQ);
            mapstring(NICK_HELP_UNSET, NICK_HELP_UNSET_REQ_EMAIL);
        } else {
            mapstring(NICK_REGISTER_SYNTAX, old_REGISTER_SYNTAX);
            mapstring(NICK_HELP_REGISTER_EMAIL, old_HELP_REGISTER_EMAIL);
            mapstring(NICK_HELP_UNSET, old_HELP_UNSET);
        }
        if (NSForceNickChange) {
            mapstring(DISCONNECT_IN_1_MINUTE, FORCENICKCHANGE_IN_1_MINUTE);
            mapstring(DISCONNECT_IN_20_SECONDS, FORCENICKCHANGE_IN_20_SECONDS);
        } else {
            mapstring(DISCONNECT_IN_1_MINUTE, old_DISCONNECT_IN_1_MINUTE);
            mapstring(DISCONNECT_IN_20_SECONDS, old_DISCONNECT_IN_20_SECONDS);
        }
    }  /* if (!after_configure) */
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    handle_config();

    module_operserv = find_module("operserv/main");
    if (!module_operserv) {
        module_log("OperServ main module not loaded");
        exit_module(0);
        return 0;
    }
    use_module(module_operserv);
    p_ServicesRoot = get_module_symbol(module_operserv, "ServicesRoot");
    if (!p_ServicesRoot) {
        exit_module(0);
        return 0;
    }

    if (!new_commandlist(THIS_MODULE)
     || !register_commands(THIS_MODULE, cmds)
    ) {
        module_log("Unable to register commands");
        exit_module(0);
        return 0;
    }
    cmd_REGISTER = lookup_cmd(THIS_MODULE, "REGISTER");
    if (!cmd_REGISTER) {
        module_log("BUG: unable to find REGISTER command entry");
        exit_module(0);
        return 0;
    }
    cmd_DROPEMAIL = lookup_cmd(THIS_MODULE, "DROPEMAIL");
    if (!cmd_DROPEMAIL) {
        module_log("BUG: unable to find DROPEMAIL command entry");
        exit_module(0);
        return 0;
    }
    cmd_DROPEMAIL_CONFIRM = lookup_cmd(THIS_MODULE, "DROPEMAIL-CONFIRM");
    if (!cmd_DROPEMAIL_CONFIRM) {
        module_log("BUG: unable to find DROPEMAIL-CONFIRM command entry");
        exit_module(0);
        return 0;
    }
    cmd_GETPASS = lookup_cmd(THIS_MODULE, "GETPASS");
    if (!cmd_GETPASS) {
        module_log("BUG: unable to find GETPASS command entry");
        exit_module(0);
        return 0;
    }
    if (!NSEnableRegister)
        cmd_REGISTER->name = "";
    if (!NSEnableDropEmail) {
        cmd_DROPEMAIL->name = "";
        cmd_DROPEMAIL_CONFIRM->name = "";
    }
    if (!EnableGetpass)
        cmd_GETPASS->name = "";

    cb_check_expire  = register_callback("check_expire");
    cb_command       = register_callback("command");
    cb_help          = register_callback("HELP");
    cb_help_cmds     = register_callback("HELP COMMANDS");
    cb_reglink_check = register_callback("REGISTER/LINK check");
    cb_registered    = register_callback("registered");
    cb_id_check      = register_callback("IDENTIFY check");
    cb_identified    = register_callback("identified");
    if (cb_check_expire < 0 || cb_command < 0 || cb_help < 0
     || cb_help_cmds < 0 || cb_reglink_check < 0 || cb_registered < 0
     || cb_id_check < 0 || cb_identified < 0
    ) {
        module_log("Unable to register callbacks");
        exit_module(0);
        return 0;
    }

    if (!add_callback(NULL, "command line", do_command_line)
     || !add_callback(NULL, "reconfigure", do_reconfigure)
     || !add_callback(NULL, "introduce_user", introduce_nickserv)
     || !add_callback(NULL, "m_privmsg", nickserv)
     || !add_callback(NULL, "m_whois", nickserv_whois)
     || !add_callback(NULL, "user create", do_user_create)
     || !add_callback(NULL, "user nickchange (before)",
                      do_user_nickchange_before)
     || !add_callback(NULL, "user nickchange (after)",
                      do_user_nickchange_after)
     || !add_callback(NULL, "user delete", do_user_delete)
     || !add_callback(module_operserv, "STATS ALL", do_stats_all)
     || !add_callback(THIS_MODULE, "REGISTER/LINK check", do_reglink_check)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    if (!register_dbtable(&nickgroup_dbtable)
     || !register_dbtable(&nick_dbtable)
    ) {
        module_log("Unable to register database tables");
        exit_module(0);
        return 0;
    }

    if (!init_collide() || !init_set() || !init_util()) {
        exit_module(0);
        return 0;
    }

    if (encrypt_all) {
        NickGroupInfo *ngi;
        int done = 0, already = 0, failed = 0;
        module_log("Re-encrypting passwords...");
        for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {
            if ((EncryptionType && ngi->pass.cipher
                 && strcmp(ngi->pass.cipher, EncryptionType) == 0)
             || (!EncryptionType && !ngi->pass.cipher)
            ) {
                already++;
            } else {
                char plainbuf[PASSMAX];
                Password newpass;
                int res;

                init_password(&newpass);
                res = decrypt_password(&ngi->pass, plainbuf, sizeof(plainbuf));
                if (res != 0) {
                    failed++;
                } else {
                    res = encrypt_password(plainbuf,strlen(plainbuf),&newpass);
                    memset(plainbuf, 0, sizeof(plainbuf));
                    if (res != 0) {
                        failed++;
                    } else {
                        copy_password(&ngi->pass, &newpass);
                        clear_password(&newpass);
                        done++;
                    }
                }
            }
        }
        module_log("%d passwords re-encrypted, %d already encrypted, %d"
                   " failed", done, already, failed);
    } /* if (encrypt_all) */

    old_REGISTER_SYNTAX =
        mapstring(NICK_REGISTER_SYNTAX, NICK_REGISTER_SYNTAX);
    old_HELP_REGISTER_EMAIL =
        mapstring(NICK_HELP_REGISTER_EMAIL, NICK_HELP_REGISTER_EMAIL);
    old_HELP_UNSET =
        mapstring(NICK_HELP_UNSET, NICK_HELP_UNSET);
    old_DISCONNECT_IN_1_MINUTE =
        mapstring(DISCONNECT_IN_1_MINUTE, DISCONNECT_IN_1_MINUTE);
    old_DISCONNECT_IN_20_SECONDS =
        mapstring(DISCONNECT_IN_20_SECONDS, DISCONNECT_IN_20_SECONDS);
    if (NSRequireEmail) {
        mapstring(NICK_REGISTER_SYNTAX, NICK_REGISTER_REQ_EMAIL_SYNTAX);
        mapstring(NICK_HELP_REGISTER_EMAIL, NICK_HELP_REGISTER_EMAIL_REQ);
        mapstring(NICK_HELP_UNSET, NICK_HELP_UNSET_REQ_EMAIL);
    }
    if (NSForceNickChange) {
        mapstring(DISCONNECT_IN_1_MINUTE, FORCENICKCHANGE_IN_1_MINUTE);
        mapstring(DISCONNECT_IN_20_SECONDS, FORCENICKCHANGE_IN_20_SECONDS);
    }

    if (linked)
        introduce_nickserv(NULL);

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (linked)
        send_cmd(s_NickServ, "QUIT :");

    if (old_REGISTER_SYNTAX >= 0) {
        mapstring(NICK_REGISTER_SYNTAX, old_REGISTER_SYNTAX);
        old_REGISTER_SYNTAX = -1;
    }
    if (old_HELP_REGISTER_EMAIL >= 0) {
        mapstring(NICK_HELP_REGISTER_EMAIL, old_HELP_REGISTER_EMAIL);
        old_HELP_REGISTER_EMAIL = -1;
    }
    if (old_HELP_UNSET >= 0) {
        mapstring(NICK_HELP_UNSET, old_HELP_UNSET);
        old_HELP_UNSET = -1;
    }
    if (old_DISCONNECT_IN_1_MINUTE >= 0) {
        mapstring(DISCONNECT_IN_1_MINUTE, old_DISCONNECT_IN_1_MINUTE);
        old_DISCONNECT_IN_1_MINUTE = -1;
    }
    if (old_DISCONNECT_IN_20_SECONDS >= 0) {
        mapstring(DISCONNECT_IN_20_SECONDS, old_DISCONNECT_IN_20_SECONDS);
        old_DISCONNECT_IN_20_SECONDS = -1;
    }

    exit_util();
    exit_set();
    exit_collide();

    unregister_dbtable(&nick_dbtable);
    unregister_dbtable(&nickgroup_dbtable);
    clean_dbtables();

    remove_callback(THIS_MODULE, "REGISTER/LINK check", do_reglink_check);
    remove_callback(NULL, "user delete", do_user_delete);
    remove_callback(NULL, "user nickchange (after)",
                    do_user_nickchange_after);
    remove_callback(NULL, "user nickchange (before)",
                    do_user_nickchange_before);
    remove_callback(NULL, "user create", do_user_create);
    remove_callback(NULL, "m_whois", nickserv_whois);
    remove_callback(NULL, "m_privmsg", nickserv);
    remove_callback(NULL, "introduce_user", introduce_nickserv);
    remove_callback(NULL, "reconfigure", do_reconfigure);
    remove_callback(NULL, "command line", do_command_line);

    unregister_callback(cb_identified);
    unregister_callback(cb_id_check);
    unregister_callback(cb_registered);
    unregister_callback(cb_reglink_check);
    unregister_callback(cb_help_cmds);
    unregister_callback(cb_help);
    unregister_callback(cb_command);
    unregister_callback(cb_check_expire);

    /* These are static, so the pointers don't need to be cleared */
    if (cmd_GETPASS)
        cmd_GETPASS->name = "GETPASS";
    if (cmd_DROPEMAIL_CONFIRM)
        cmd_DROPEMAIL_CONFIRM->name = "DROPEMAIL-CONFIRM";
    if (cmd_DROPEMAIL)
        cmd_DROPEMAIL->name = "DROPEMAIL";
    if (cmd_REGISTER)
        cmd_REGISTER->name = "REGISTER";
    unregister_commands(THIS_MODULE, cmds);
    del_commandlist(THIS_MODULE);

    if (module_operserv) {
        remove_callback(module_operserv, "STATS ALL", do_stats_all);
        p_ServicesRoot = NULL;
        unuse_module(module_operserv);
        module_operserv = NULL;
    }

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
