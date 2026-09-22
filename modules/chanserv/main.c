/* Main ChanServ module.
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
#include "modules/nickserv/nickserv.h"
#include "modules/operserv/operserv.h"

#include "chanserv.h"
#include "cs-local.h"

/*************************************************************************/
/************************** Declaration section **************************/
/*************************************************************************/

static Module *module_operserv;
static Module *module_nickserv;

static int cb_clear     = -1;
static int cb_command   = -1;
static int cb_help      = -1;
static int cb_help_cmds = -1;
static int cb_invite    = -1;
static int cb_unban     = -1;

       char *s_ChanServ;
static char *desc_ChanServ;
EXPORT_VAR(char *,s_ChanServ)

static int    CSEnableRegister;
       int    CSRegisteredOnly;
       int32  CSMaxReg;
       int32  CSDefFlags;
static int32  CSDefModeLockOn = CMODE_n | CMODE_t;
static int32  CSDefModeLockOff = 0;
       time_t CSExpire;
       int    CSShowPassword;
       int32  CSAccessMax;
       int32  CSAutokickMax;
       time_t CSInhabit;
       time_t CSRestrictDelay;
       int    CSListOpersOnly;
       time_t CSSuspendExpire;
       time_t CSSuspendGrace;
       int    CSForbidShortChannel;
       int    CSSkipModeRCheck;
EXPORT_VAR(int32,CSMaxReg)

/*************************************************************************/

/* Channel option list. */

#define CHANOPT(x) \
    { #x, CF_##x, CHAN_INFO_OPT_##x, \
      CHAN_SET_##x##_ON, CHAN_SET_##x##_OFF, CHAN_SET_##x##_SYNTAX }
ChanOpt chanopts[] = {
    CHANOPT(KEEPTOPIC),
    CHANOPT(TOPICLOCK),
    CHANOPT(PRIVATE),
    CHANOPT(SECUREOPS),
    CHANOPT(LEAVEOPS),
    CHANOPT(RESTRICTED),
    CHANOPT(SECURE),
    CHANOPT(OPNOTICE),
    CHANOPT(ENFORCE),
    { "MEMO-RESTRICTED", CF_MEMO_RESTRICTED, CHAN_INFO_OPT_MEMO_RESTRICTED,
          CHAN_SET_MEMO_RESTRICTED_ON, CHAN_SET_MEMO_RESTRICTED_OFF,
          CHAN_SET_MEMO_RESTRICTED_SYNTAX },
    { "NOEXPIRE", CF_NOEXPIRE, -1, CHAN_SET_NOEXPIRE_ON,
          CHAN_SET_NOEXPIRE_OFF, CHAN_SET_NOEXPIRE_SYNTAX },
    { NULL }
};
#undef CHANOPT

/*************************************************************************/

/* Local functions. */

static void do_help(User *u);
static void do_register(User *u);
static void do_identify(User *u);
static void do_drop(User *u);
static void do_dropchan(User *u);
static void do_info(User *u);
static void do_list(User *u);
static void do_akick(User *u);
static void do_op(User *u);
static void do_deop(User *u);
static void do_voice(User *u);
static void do_devoice(User *u);
static void do_halfop(User *u);
static void do_dehalfop(User *u);
static void do_protect(User *u);
static void do_deprotect(User *u);
static void do_invite(User *u);
static void do_unban(User *u);
static void do_cskick(User *u);
static void do_cstopic(User *u);
static void do_clear(User *u);
static void do_getpass(User *u);
static void do_forbid(User *u);
static void do_suspend(User *u);
static void do_unsuspend(User *u);
static void do_status(User *u);

/*************************************************************************/

/* Command list. */

static Command cmds[] = {
    { "HELP",     do_help,     NULL,  -1,                       -1,-1 },
    { "REGISTER", do_register, NULL,  CHAN_HELP_REGISTER,       -1,-1 },
    { "IDENTIFY", do_identify, NULL,  CHAN_HELP_IDENTIFY,       -1,-1 },
    { "DROP",     do_drop,     NULL,  CHAN_HELP_DROP,           -1,-1 },
    { "DROPCHAN", do_dropchan, is_services_admin, -1,
                -1, CHAN_OPER_HELP_DROPCHAN },
    { "SET",      do_set,      NULL,  CHAN_HELP_SET, -1, CHAN_OPER_HELP_SET },
    { "SET FOUNDER",    NULL,  NULL,  CHAN_HELP_SET_FOUNDER,    -1,-1 },
    { "SET SUCCESSOR",  NULL,  NULL,  CHAN_HELP_SET_SUCCESSOR,  -1,-1 },
    { "SET PASSWORD",   NULL,  NULL,  CHAN_HELP_SET_PASSWORD,   -1,-1 },
    { "SET DESC",       NULL,  NULL,  CHAN_HELP_SET_DESC,       -1,-1 },
    { "SET URL",        NULL,  NULL,  CHAN_HELP_SET_URL,        -1,-1 },
    { "SET EMAIL",      NULL,  NULL,  CHAN_HELP_SET_EMAIL,      -1,-1 },
    { "SET ENTRYMSG",   NULL,  NULL,  CHAN_HELP_SET_ENTRYMSG,   -1,-1 },
    { "SET KEEPTOPIC",  NULL,  NULL,  CHAN_HELP_SET_KEEPTOPIC,  -1,-1 },
    { "SET TOPICLOCK",  NULL,  NULL,  CHAN_HELP_SET_TOPICLOCK,  -1,-1 },
    { "SET MLOCK",      NULL,  NULL,  CHAN_HELP_SET_MLOCK,      -1,-1 },
    { "SET HIDE",       NULL,  NULL,  CHAN_HELP_SET_HIDE,       -1,-1 },
    { "SET PRIVATE",    NULL,  NULL,  CHAN_HELP_SET_PRIVATE,    -1,-1 },
    { "SET RESTRICTED", NULL,  NULL,  CHAN_HELP_SET_RESTRICTED, -1,-1 },
    { "SET SECURE",     NULL,  NULL,  CHAN_HELP_SET_SECURE,     -1,-1 },
    { "SET SECUREOPS",  NULL,  NULL,  CHAN_HELP_SET_SECUREOPS,  -1,-1 },
    { "SET LEAVEOPS",   NULL,  NULL,  CHAN_HELP_SET_LEAVEOPS,   -1,-1 },
    { "SET OPNOTICE",   NULL,  NULL,  CHAN_HELP_SET_OPNOTICE,   -1,-1 },
    { "SET ENFORCE",    NULL,  NULL,  CHAN_HELP_SET_ENFORCE,    -1,-1 },
    { "SET MEMO-RESTRICTED",NULL,NULL,CHAN_HELP_SET_MEMO_RESTRICTED,-1,-1 },
    { "SET NOEXPIRE",   NULL,  NULL,  -1, -1, CHAN_OPER_HELP_SET_NOEXPIRE },
    { "UNSET",    do_unset,    NULL,  CHAN_HELP_UNSET,
                -1, CHAN_OPER_HELP_UNSET },
    { "INFO",     do_info,     NULL,  CHAN_HELP_INFO,
                -1, CHAN_OPER_HELP_INFO },
    { "LIST",     do_list,     NULL,  -1,
                CHAN_HELP_LIST, CHAN_OPER_HELP_LIST },
    { "AKICK",    do_akick,    NULL,  CHAN_HELP_AKICK,          -1,-1,
                (void *)ACCLEV_SOP },
    { "OP",       do_op,       NULL,  CHAN_HELP_OP,             -1,-1,
                (void *)ACCLEV_AOP },
    { "DEOP",     do_deop,     NULL,  CHAN_HELP_DEOP,           -1,-1,
                (void *)ACCLEV_AOP },
    { "VOICE",    do_voice,    NULL,  CHAN_HELP_VOICE,          -1,-1,
                (void *)ACCLEV_VOP },
    { "DEVOICE",  do_devoice,  NULL,  CHAN_HELP_DEVOICE,        -1,-1,
                (void *)ACCLEV_VOP },
    { "INVITE",   do_invite,   NULL,  CHAN_HELP_INVITE,         -1,-1,
                (void *)ACCLEV_AOP },
    { "UNBAN",    do_unban,    NULL,  CHAN_HELP_UNBAN,          -1,-1,
                (void *)ACCLEV_AOP },
    { "KICK",     do_cskick,   NULL,  CHAN_HELP_KICK,           -1,-1,
                (void *)ACCLEV_AOP },
    { "TOPIC",    do_cstopic,  NULL,  CHAN_HELP_TOPIC,          -1,-1,
                (void *)ACCLEV_AOP },
    { "CLEAR",    do_clear,    NULL,  CHAN_HELP_CLEAR,          -1,-1,
                (void *)ACCLEV_SOP },
    { "STATUS",   do_status,   NULL,  CHAN_HELP_STATUS,         -1,-1,
                (void *)ACCLEV_SOP },
    { "GETPASS",  do_getpass,  is_services_admin,  -1,
                -1, CHAN_OPER_HELP_GETPASS },
    { "FORBID",   do_forbid,   is_services_admin,  -1,
                -1, CHAN_OPER_HELP_FORBID },
    { "SUSPEND",  do_suspend,  is_services_admin,  -1,
                -1, CHAN_OPER_HELP_SUSPEND },
    { "UNSUSPEND",do_unsuspend,is_services_admin,  -1,
                -1, CHAN_OPER_HELP_UNSUSPEND },
    { NULL }
};

static Command cmds_halfop[] = {
    { "HALFOP",   do_halfop,   NULL,  CHAN_HELP_HALFOP,         -1,-1,
                (void *)ACCLEV_AOP },
    { "DEHALFOP", do_dehalfop, NULL,  CHAN_HELP_DEHALFOP,       -1,-1,
                (void *)ACCLEV_AOP },
    { NULL }
};

static Command cmds_chanprot[] = {
    { "PROTECT",  do_protect,  NULL,  CHAN_HELP_PROTECT,        -1,-1,
                (void *)ACCLEV_AOP },
    { "DEPROTECT",do_deprotect,NULL,  CHAN_HELP_DEPROTECT,      -1,-1,
                (void *)ACCLEV_AOP },
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

/* Check whether the given channel (or its suspension) should be expired,
 * and do the expiration if so.  Return 1 if the channel was deleted, else 0.
 */

static int check_expire_channel(ChannelInfo *ci)
{
    time_t now = time(NULL);

    /* Don't bother updating last used time unless it's been a while */
    if (ci->c && CSExpire && now >= ci->last_used + CSExpire/2) {
        struct c_userlist *cu;
        LIST_FOREACH (cu, ci->c->users) {
            if (check_access(cu->user, ci, CA_AUTOOP)) {
                module_log_debug(2, "updating last used time for %s",
                                 ci->name);
                ci->last_used = time(NULL);
                break;
            }
        }
    }

    if (CSExpire
     && now >= ci->last_used + CSExpire
     && !(ci->flags & (CF_VERBOTEN | CF_SUSPENDED | CF_NOEXPIRE))
    ) {
        module_log("Expiring channel %s", ci->name);
        delchan(ci);
        return 1;
    }

    if ((ci->flags & CF_SUSPENDED)
     && ci->suspend_expires
     && now >= ci->suspend_expires
    ) {
        module_log("Expiring suspension for %s", ci->name);
        unsuspend_channel(ci, 1);
    }
    return 0;
}

/*************************************************************************/

#define HASH_STATIC static
#define HASHFUNC(key) DEFAULT_HASHFUNC(key+1)
#define EXPIRE_CHECK(node) check_expire_channel(node)
#include "hash.h"
DEFINE_HASH(channelinfo_, ChannelInfo, name);

EXPORT_FUNC(add_channelinfo)
ChannelInfo *add_channelinfo(ChannelInfo *ci)
{
    add_channelinfo_(ci);
    ci->usecount = 1;
    return ci;
}

EXPORT_FUNC(del_channelinfo)
void del_channelinfo(ChannelInfo *ci)
{
    del_channelinfo_(ci);
    free_channelinfo(ci);
}

EXPORT_FUNC(get_channelinfo)
ChannelInfo *get_channelinfo(const char *chan)
{
    ChannelInfo *ci;
    if ((ci = get_channelinfo_(chan)) != NULL)
        ci->usecount++;
    return ci;
}

EXPORT_FUNC(put_channelinfo)
ChannelInfo *put_channelinfo(ChannelInfo *ci)
{
    if (ci) {
        if (ci->usecount > 0)
            ci->usecount--;
        else
            module_log_debug(1, "BUG: put_channelinfo(%s) with usecount==0",
                             ci->name);
    }
    return ci;
}

EXPORT_FUNC(first_channelinfo)
ChannelInfo *first_channelinfo(void)
{
    return first_channelinfo_();
}

EXPORT_FUNC(next_channelinfo)
ChannelInfo *next_channelinfo(void)
{
    return next_channelinfo_();
}

/*************************************************************************/

/* Free all memory used by database tables. */

static void clean_dbtables(void)
{
    ChannelInfo *ci;
    int save_noexpire = noexpire;

    noexpire = 1;
    for (ci = first_channelinfo(); ci; ci = next_channelinfo())
        free_channelinfo(ci);
    noexpire = save_noexpire;
}

/*************************************************************************/

/* Helper functions for loading and saving database tables. */

static ChanAccess dbuse_chan_access;
static AutoKick dbuse_chan_akick;
static ChannelInfo *dbuse_ci_iterator;
static int dbuse_ca_iterator, dbuse_ak_iterator;

/************************************/

static void insert_channel(void *record)
{
    ChannelInfo *ci = add_channelinfo(record);
    count_chan(ci);
    put_channelinfo(ci);
}

/************************************/

static ChanAccess *new_chan_access(void) {
    return memset(&dbuse_chan_access, 0, sizeof(dbuse_chan_access));
}

static void free_chan_access(ChanAccess *ca) {
    /* nothing to do */
}

static void insert_chan_access(ChanAccess *ca) {
    if (ca->channel) {
        ChanAccess *ca2;
        ARRAY_EXTEND(ca->channel->access);
        ca2 = &ca->channel->access[ca->channel->access_count-1];
        memcpy(ca2, ca, sizeof(*ca2));
    } else {
        free_chan_access(ca);
    }
}

static ChanAccess *next_chan_access(void) {
    while (dbuse_ci_iterator
        && dbuse_ca_iterator >= dbuse_ci_iterator->access_count
    ) {
        dbuse_ci_iterator = next_channelinfo();
        dbuse_ca_iterator = 0;
    }
    if (dbuse_ci_iterator)
        return &dbuse_ci_iterator->access[dbuse_ca_iterator++];
    else
        return NULL;
}

static ChanAccess *first_chan_access(void) {
    dbuse_ci_iterator = first_channelinfo();
    dbuse_ca_iterator = 0;
    return next_chan_access();
}

static void chan_access_get_channel(const ChanAccess *record, char **value_ret)
{
    *value_ret = record->channel->name;
}

static void chan_access_put_channel(ChanAccess *record, const char **value) {
    ChannelInfo *ci = get_channelinfo(*value);
    if (ci) {
        record->channel = ci;
    } else {
        module_log("Skipping access record for missing channel %s", *value);
        record->channel = NULL;
    }
    free((char *)*value);
}

/************************************/

static AutoKick *new_chan_akick(void) {
    return memset(&dbuse_chan_akick, 0, sizeof(dbuse_chan_akick));
}

static void free_chan_akick(AutoKick *ak) {
    free(ak->mask);
    free(ak->reason);
}

static void insert_chan_akick(AutoKick *ak) {
    if (ak->channel) {
        AutoKick *ak2;
        ARRAY_EXTEND(ak->channel->akick);
        ak2 = &ak->channel->akick[ak->channel->akick_count-1];
        memcpy(ak2, ak, sizeof(*ak2));
    } else {
        free_chan_akick(ak);
    }
}

static AutoKick *next_chan_akick(void) {
    while (dbuse_ci_iterator
        && dbuse_ak_iterator >= dbuse_ci_iterator->akick_count
    ) {
        dbuse_ci_iterator = next_channelinfo();
        dbuse_ak_iterator = 0;
    }
    if (dbuse_ci_iterator)
        return &dbuse_ci_iterator->akick[dbuse_ak_iterator++];
    else
        return NULL;
}

static AutoKick *first_chan_akick(void) {
    dbuse_ci_iterator = first_channelinfo();
    dbuse_ak_iterator = 0;
    return next_chan_akick();
}

static void chan_akick_get_channel(const AutoKick *record, char **value_ret) {
    *value_ret = record->channel->name;
}

static void chan_akick_put_channel(AutoKick *record, const char **value) {
    ChannelInfo *ci = get_channelinfo(*value);
    if (ci) {
        record->channel = ci;
    } else {
        module_log("Skipping autokick record for missing channel %s", *value);
        record->channel = NULL;
    }
    free((char *)*value);
}

/*************************************************************************/

/* Channel database table info */

#define FIELD(name,type,...) \
    { #name, type, offsetof(ChannelInfo,name) , ##__VA_ARGS__ }
#define FIELD_LEVELS(name) \
    { "levels." #name, DBTYPE_INT16, \
      offsetof(ChannelInfo,levels) + (sizeof(int16) * CA_##name) }
#define FIELD_MLOCK(name,type,...) \
    { "mlock." #name, type, \
      offsetof(ChannelInfo,mlock) + offsetof(ModeLock,name) , ##__VA_ARGS__ }

static DBField chan_dbfields[] = {
    FIELD(name,              DBTYPE_BUFFER, CHANMAX),
    FIELD(founder,           DBTYPE_UINT32),
    FIELD(successor,         DBTYPE_UINT32),
    FIELD(founderpass,       DBTYPE_PASSWORD),
    FIELD(desc,              DBTYPE_STRING),
    FIELD(url,               DBTYPE_STRING),
    FIELD(email,             DBTYPE_STRING),
    FIELD(entry_message,     DBTYPE_STRING),
    FIELD(time_registered,   DBTYPE_TIME),
    FIELD(last_used,         DBTYPE_TIME),
    FIELD(last_topic,        DBTYPE_STRING),
    FIELD(last_topic_setter, DBTYPE_BUFFER, NICKMAX),
    FIELD(last_topic_time,   DBTYPE_TIME),
    FIELD(flags,             DBTYPE_INT32),
    FIELD(suspend_who,       DBTYPE_BUFFER, NICKMAX),
    FIELD(suspend_reason,    DBTYPE_STRING),
    FIELD(suspend_time,      DBTYPE_TIME),
    FIELD(suspend_expires,   DBTYPE_TIME),
    FIELD_LEVELS(INVITE),
    FIELD_LEVELS(AKICK),
    FIELD_LEVELS(SET),
    FIELD_LEVELS(UNBAN),
    FIELD_LEVELS(AUTOOP),
    FIELD_LEVELS(AUTODEOP),
    FIELD_LEVELS(AUTOVOICE),
    FIELD_LEVELS(OPDEOP),
    FIELD_LEVELS(ACCESS_LIST),
    FIELD_LEVELS(CLEAR),
    FIELD_LEVELS(NOJOIN),
    FIELD_LEVELS(ACCESS_CHANGE),
    FIELD_LEVELS(MEMO),
    FIELD_LEVELS(VOICE),
    FIELD_LEVELS(AUTOHALFOP),
    FIELD_LEVELS(HALFOP),
    FIELD_LEVELS(AUTOPROTECT),
    FIELD_LEVELS(PROTECT),
    FIELD_LEVELS(KICK),
    FIELD_LEVELS(STATUS),
    FIELD_LEVELS(TOPIC),
    FIELD_MLOCK(on,          DBTYPE_INT32),
    FIELD_MLOCK(off,         DBTYPE_INT32),
    FIELD_MLOCK(limit,       DBTYPE_INT32),
    FIELD_MLOCK(key,         DBTYPE_STRING),
    FIELD_MLOCK(link,        DBTYPE_STRING),
    FIELD_MLOCK(flood,       DBTYPE_STRING),
    FIELD_MLOCK(joindelay,   DBTYPE_INT32),
    FIELD_MLOCK(joinrate1,   DBTYPE_INT32),
    FIELD_MLOCK(joinrate2,   DBTYPE_INT32),

    { "suspendinfo.who",       DBTYPE_BUFFER,
      offsetof(ChannelInfo,suspend_who), NICKMAX, .load_only = 1 },
    { "suspendinfo.reason",    DBTYPE_STRING,
      offsetof(ChannelInfo,suspend_reason), .load_only = 1 },
    { "suspendinfo.suspended", DBTYPE_TIME,
      offsetof(ChannelInfo,suspend_time), .load_only = 1 },
    { "suspendinfo.expires",   DBTYPE_TIME,
      offsetof(ChannelInfo,suspend_expires), .load_only = 1 },

    { NULL }
};
static DBTable chan_dbtable = {
    .name    = "chan",
    .newrec  = (void *)new_channelinfo,
    .freerec = (void *)free_channelinfo,
    .insert  = insert_channel,
    .first   = (void *)first_channelinfo,
    .next    = (void *)next_channelinfo,
    .fields  = chan_dbfields,
};

static DBField chan_access_dbfields[] = {
    { "channel",   DBTYPE_STRING,
      .get = (void *)chan_access_get_channel,
      .put = (void *)chan_access_put_channel },
    { "nickgroup", DBTYPE_UINT32, offsetof(ChanAccess,nickgroup) },
    { "level",     DBTYPE_INT16,  offsetof(ChanAccess,level) },
    { NULL }
};
static DBTable chan_access_dbtable = {
    .name    = "chan-access",
    .newrec  = (void *)new_chan_access,
    .freerec = (void *)free_chan_access,
    .insert  = (void *)insert_chan_access,
    .first   = (void *)first_chan_access,
    .next    = (void *)next_chan_access,
    .fields  = chan_access_dbfields,
};

static DBField chan_akick_dbfields[] = {
    { "channel",   DBTYPE_STRING,
      .get = (void *)chan_akick_get_channel,
      .put = (void *)chan_akick_put_channel },
    { "mask",     DBTYPE_STRING, offsetof(AutoKick,mask) },
    { "reason",   DBTYPE_STRING, offsetof(AutoKick,reason) },
    { "who",      DBTYPE_BUFFER, offsetof(AutoKick,who), NICKMAX },
    { "set",      DBTYPE_TIME,   offsetof(AutoKick,set) },
    { "lastused", DBTYPE_TIME,   offsetof(AutoKick,lastused) },
    { NULL }
};
static DBTable chan_akick_dbtable = {
    .name    = "chan-akick",
    .newrec  = (void *)new_chan_akick,
    .freerec = (void *)free_chan_akick,
    .insert  = (void *)insert_chan_akick,
    .first   = (void *)first_chan_akick,
    .next    = (void *)next_chan_akick,
    .fields  = chan_akick_dbfields,
};

/*************************************************************************/
/************************ Main ChanServ routines *************************/
/*************************************************************************/

/* Introduce the ChanServ pseudoclient. */

static int introduce_chanserv(const char *nick)
{
    if (!nick || irc_stricmp(nick, s_ChanServ) == 0) {
        send_pseudo_nick(s_ChanServ, desc_ChanServ, PSEUDO_OPER);
        return nick ? 1 : 0;
    }
    return 0;
}

/*************************************************************************/

/* Main ChanServ routine. */

static int chanserv(const char *source, const char *target, char *buf)
{
    char *cmd;
    User *u = get_user(source);

    if (irc_stricmp(target, s_ChanServ) != 0)
        return 0;

    if (!u) {
        module_log("user record for %s not found", source);
        notice(s_ChanServ, source, getstring(NULL, INTERNAL_ERROR));
        return 1;
    }

    cmd = strtok(buf, " ");

    if (!cmd) {
        return 1;
    } else if (stricmp(cmd, "\1PING") == 0) {
        const char *s;
        if (!(s = strtok_remaining()))
            s = "\1";
        notice(s_ChanServ, source, "\1PING %s", s);
    } else {
        int i;
        ARRAY_FOREACH (i, aliases) {
            if (stricmp(cmd, aliases[i].alias) == 0) {
                cmd = aliases[i].command;
                break;
            }
        }
        if (call_callback_2(cb_command, u, cmd) <= 0)
            run_cmd(s_ChanServ, u, THIS_MODULE, cmd);
    }
    return 1;
}

/*************************************************************************/

/* Return a /WHOIS response for ChanServ. */

static int chanserv_whois(const char *source, char *who, char *extra)
{
    if (irc_stricmp(who, s_ChanServ) != 0)
        return 0;
    send_cmd(ServerName, "311 %s %s %s %s * :%s", source, who,
             ServiceUser, ServiceHost, desc_ChanServ);
    send_cmd(ServerName, "312 %s %s %s :%s", source, who,
             ServerName, ServerDesc);
    send_cmd(ServerName, "318 %s %s End of /WHOIS response.", source, who);
    return 1;
}

/*************************************************************************/

/* Callback for newly-created channels. */

static int do_channel_create(Channel *c, User *u, int32 modes)
{
    /* Store ChannelInfo pointer in channel record */
    c->ci = get_channelinfo(c->name);
    if (c->ci) {
        /* Store return pointer in ChannelInfo record */
        c->ci->c = c;
    }

    /* Restore locked modes and saved topic */
    /* Note: these should be outside the c->ci test to ensure any spurious
     *       +r modes are cleared */
    check_modes(c);
    restore_topic(c);

    return 0;
}

/*************************************************************************/

/* Callback for users trying to join channels. */

static int do_channel_join_check(const char *channel, User *user)
{
    return check_kick(user, channel, 1);
}

/*************************************************************************/

/* Callback for users joining channels. */

static int do_channel_join(Channel *c, struct c_userlist *u)
{
    User *user = u->user;
    ChannelInfo *ci = c->ci;

    check_chan_user_modes(NULL, u, c, -1);
    if (ci && ci->entry_message)
        notice(s_ChanServ, user->nick, "(%s) %s", ci->name, ci->entry_message);
    return 0;
}

/*************************************************************************/

/* Callback for users leaving channels.  Update the channel's last used
 * time if the user was an auto-op user.
 */

static int do_channel_part(Channel *c, User *u, const char *reason)
{
    if (c->ci && check_access(u, c->ci, CA_AUTOOP))
        c->ci->last_used = time(NULL);
    return 0;
}

/*************************************************************************/

/* Callback for channels being deleted. */

static int do_channel_delete(Channel *c)
{
    if (c->ci) {
        put_channelinfo(c->ci);
        c->ci->c = NULL;
    }
    return 0;
}

/*************************************************************************/

/* Callback for channel mode changes. */

static int do_channel_mode_change(const char *source_unused, Channel *c)
{
    check_modes(c);
    return 0;
}

/*************************************************************************/

/* Callback for channel user mode changes. */

static int do_channel_umode_change(const char *source, Channel *c,
                                   struct c_userlist *u, int32 oldmodes)
{
    if (!(u->mode & CUMODE_o))
        u->flags &= ~CUFLAG_DEOPPED;
    check_chan_user_modes(source, u, c, oldmodes);
    return 0;
}

/*************************************************************************/

/* Callback for channel topic changes. */

static int do_channel_topic(Channel *c, const char *topic, const char *setter,
                            time_t topic_time)
{
    ChannelInfo *ci = c->ci;

    if (check_topiclock(c, topic_time))
        return 1;
    record_topic(ci, topic, setter, topic_time);
    return 0;
}

/*************************************************************************/

/* Callback for NickServ REGISTER/LINK check; we disallow
 * registration/linking of the ChanServ pseudoclient nickname.
 */

static int do_reglink_check(const User *u, const char *nick,
                            const char *pass, const char *email)
{
    return irc_stricmp(nick, s_ChanServ) == 0;
}

/*************************************************************************/

/* Callback for users who have identified to their nicks: give them modes
 * as if they had just joined the channel.
 */

static int do_nick_identified(User *u, int old_authstat)
{
    struct u_chanlist *uc;      /* Node in list of channels the user is in */
    struct c_userlist *cu;      /* Node in list of users in a channel */

    LIST_FOREACH (uc, u->chans) {
        LIST_SEARCH_SCALAR(uc->chan->users, user, u, cu);
        if (!cu) {
            module_log("do_nick_identified(): BUG: user record not found in"
                       " channel %s for user %s", uc->chan->name, u->nick);
            continue;
        }
        /* Use an empty source to force a mode recheck */
        check_chan_user_modes("", cu, uc->chan, -1);
    }
    return 0;
}

/*************************************************************************/

/* Remove a (deleted or expired) nickname group from all channel lists. */

static int do_nickgroup_delete(const NickGroupInfo *ngi, const char *oldnick)
{
    int i;
    int id = ngi->id;
    ChannelInfo *ci;

    for (ci = first_channelinfo(); ci; ci = next_channelinfo()) {
        if (ci->founder == id) {
            int was_suspended = (ci->flags & CF_SUSPENDED);
            char name_save[CHANMAX];
            strbcpy(name_save, ci->name);
            if (ci->successor) {
                NickGroupInfo *ngi2 = get_ngi_id(ci->successor);
                if (!ngi2) {
                    module_log("Unable to access successor group %u for"
                               " deleted channel %s, deleting channel",
                               ci->successor, ci->name);
                    goto delete;
                } else if (check_channel_limit(ngi2, NULL) < 0) {
                    module_log("Transferring foundership of %s from deleted"
                               " nick %s to successor %s", ci->name,
                               oldnick, ngi_mainnick(ngi2));
                    put_nickgroupinfo(ngi2);
                    uncount_chan(ci);
                    ci->founder = ci->successor;
                    ci->successor = 0;
                    count_chan(ci);
                } else {
                    module_log("Successor (%s) of %s owns too many channels,"
                               " deleting channel", ngi_mainnick(ngi2),
                               ci->name);
                    put_nickgroupinfo(ngi2);
                    goto delete;
                }
            } else {
                module_log("Deleting channel %s owned by deleted nick %s",
                           ci->name, oldnick);
              delete:
                delchan(ci);
                if (was_suspended) {
                    /* Channel was suspended, so make it forbidden */
                    Channel *c;
                    module_log("Channel %s was suspended, forbidding it",
                               name_save);
                    ci = makechan(name_save);
                    ci->flags |= CF_VERBOTEN;
                    if ((c = get_channel(ci->name)) != NULL) {
                        c->ci = ci;
                        ci->c = c;
                    } else {
                        put_channelinfo(ci);
                    }
                }
                continue;
            }
        }
        if (ci->successor == id)
            ci->successor = 0;
        ARRAY_FOREACH (i, ci->access) {
            if (ci->access[i].nickgroup == id)
                ci->access[i].nickgroup = 0;
        }
    }
    return 0;
}

/*************************************************************************/

static int do_stats_all(User *user, const char *s_OperServ)
{
    int32 count, mem;
    int i;
    ChannelInfo *ci;

    count = mem = 0;
    for (ci = first_channelinfo(); ci; ci = next_channelinfo()) {
        count++;
        mem += sizeof(*ci);
        if (ci->desc)
            mem += strlen(ci->desc)+1;
        if (ci->url)
            mem += strlen(ci->url)+1;
        if (ci->email)
            mem += strlen(ci->email)+1;
        if (ci->last_topic)
            mem += strlen(ci->last_topic)+1;
        if (ci->suspend_reason)
            mem += strlen(ci->suspend_reason)+1;
        if (ci->levels)
            mem += sizeof(*ci->levels) * CA_SIZE;
        mem += ci->access_count * sizeof(*ci->access);
        mem += ci->akick_count * sizeof(*ci->akick);
        ARRAY_FOREACH (i, ci->akick) {
            if (ci->akick[i].mask)
                mem += strlen(ci->akick[i].mask)+1;
            if (ci->akick[i].reason)
                mem += strlen(ci->akick[i].reason)+1;
        }
        if (ci->mlock.key)
            mem += strlen(ci->mlock.key)+1;
        if (ci->mlock.link)
            mem += strlen(ci->mlock.link)+1;
        if (ci->mlock.flood)
            mem += strlen(ci->mlock.flood)+1;
        if (ci->entry_message)
            mem += strlen(ci->entry_message)+1;
    }
    notice_lang(s_OperServ, user, OPER_STATS_ALL_CHANSERV_MEM,
                count, (mem+512) / 1024);

    return 0;
}

/*************************************************************************/
/*********************** ChanServ command routines ***********************/
/*************************************************************************/

/* Short routine for do_help() to return the proper access level string for
 * a given level based on which access modules are loaded.  Assumes numeric
 * levels if no access module is loaded.
 */
static const char *getstring_cmdacc(NickGroupInfo *ngi, int16 level)
{
    int str_levxop, str_lev, str_xop;

    switch (level) {
      case ACCLEV_SOP:
        str_levxop = CHAN_HELP_REQSOP_LEVXOP;
        str_lev    = CHAN_HELP_REQSOP_LEV;
        str_xop    = CHAN_HELP_REQSOP_XOP;
        break;
      case ACCLEV_AOP:
        str_levxop = CHAN_HELP_REQAOP_LEVXOP;
        str_lev    = CHAN_HELP_REQAOP_LEV;
        str_xop    = CHAN_HELP_REQAOP_XOP;
        break;
      case ACCLEV_HOP:
        str_levxop = CHAN_HELP_REQHOP_LEVXOP;
        str_lev    = CHAN_HELP_REQHOP_LEV;
        str_xop    = CHAN_HELP_REQHOP_XOP;
        break;
      case ACCLEV_VOP:
        str_levxop = CHAN_HELP_REQVOP_LEVXOP;
        str_lev    = CHAN_HELP_REQVOP_LEV;
        str_xop    = CHAN_HELP_REQVOP_XOP;
        break;
      default:
        module_log("BUG: weird level (%d) in getstring_cmdacc()", level);
        return "???";
    }
    if (find_module("chanserv/access-xop")) {
        if (find_module("chanserv/access-levels"))
            return getstring(ngi, str_levxop);
        else
            return getstring(ngi, str_xop);
    } else {
        return getstring(ngi, str_lev);
    } 
}


static void do_help(User *u)
{
    char *cmd = strtok_remaining();
    Command *cmdrec;

    if (!cmd) {
        notice_help(s_ChanServ, u, CHAN_HELP);
        if (CSExpire)
            notice_help(s_ChanServ, u, CHAN_HELP_EXPIRES,
                        maketime(u->ngi,CSExpire,0));
    } else if (call_callback_2(cb_help, u, cmd) > 0) {
        return;
    } else if (stricmp(cmd, "COMMANDS") == 0) {
        notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS);
        if (find_module("chanserv/access-levels"))
            notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS_LEVELS);
        if (find_module("chanserv/access-xop")) {
            notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS_XOP);
            if (protocol_features & PF_HALFOP)
                notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS_HOP);
            notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS_XOP_2);
        }
        notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS_OPVOICE);
        if (protocol_features & PF_HALFOP)
            notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS_HALFOP);
        if (protocol_features & PF_CHANPROT)
            notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS_PROTECT);
        notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS_INVITE);
        if (!CSListOpersOnly)
            notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS_LIST);
        notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS_AKICK);
        call_callback_2(cb_help_cmds, u, 0);
        if (is_oper(u)) {
            notice_help(s_ChanServ, u, CHAN_OPER_HELP_COMMANDS);
            if (EnableGetpass)
                notice_help(s_ChanServ, u, CHAN_OPER_HELP_COMMANDS_GETPASS);
            notice_help(s_ChanServ, u, CHAN_OPER_HELP_COMMANDS_FORBID);
            if (CSListOpersOnly)
                notice_help(s_ChanServ, u, CHAN_HELP_COMMANDS_LIST);
            call_callback_2(cb_help_cmds, u, 1);
            notice_help(s_ChanServ, u, CHAN_OPER_HELP_COMMANDS_END);
        }
    } else if (!CSEnableRegister && is_oper(u) && stricmp(cmd,"REGISTER")==0) {
        notice_help(s_ChanServ, u, CHAN_HELP_REGISTER, s_NickServ);
        notice_help(s_ChanServ, u, CHAN_HELP_REGISTER_ADMINONLY);
    } else if (stricmp(cmd, "LIST") == 0) {
        if (is_oper(u))
            notice_help(s_ChanServ, u, CHAN_OPER_HELP_LIST);
        else
            notice_help(s_ChanServ, u, CHAN_HELP_LIST);
        if (CSListOpersOnly)
            notice_help(s_ChanServ, u, CHAN_HELP_LIST_OPERSONLY);
    } else if (stricmp(cmd, "KICK") == 0) {
        cmdrec = lookup_cmd(THIS_MODULE, cmd);
        notice_help(s_ChanServ, u, CHAN_HELP_KICK,
             getstring_cmdacc(u->ngi, cmdrec ? (int)(long)cmdrec->help_param1 : -1));
        if (protocol_features & PF_CHANPROT)
            notice_help(s_ChanServ, u, CHAN_HELP_KICK_PROTECTED);
    } else if (stricmp(cmd, "CLEAR") == 0) {
        notice_help(s_ChanServ, u, CHAN_HELP_CLEAR);
        if (protocol_features & PF_BANEXCEPT)
            notice_help(s_ChanServ, u, CHAN_HELP_CLEAR_EXCEPTIONS);
        if (protocol_features & PF_INVITEMASK)
            notice_help(s_ChanServ, u, CHAN_HELP_CLEAR_INVITES);
        notice_help(s_ChanServ, u, CHAN_HELP_CLEAR_MID);
        if (protocol_features & PF_HALFOP)
            notice_help(s_ChanServ, u, CHAN_HELP_CLEAR_HALFOPS);
        cmdrec = lookup_cmd(THIS_MODULE, cmd);
        notice_help(s_ChanServ, u, CHAN_HELP_CLEAR_END,
             getstring_cmdacc(u->ngi, cmdrec ? (int)(long)cmdrec->help_param1 : -1));
    } else if ((stricmp(cmd, "AKICK") == 0
                || stricmp(cmd, "OP") == 0
                || stricmp(cmd, "DEOP") == 0
                || stricmp(cmd, "VOICE") == 0
                || stricmp(cmd, "DEVOICE") == 0
                || stricmp(cmd, "HALFOP") == 0
                || stricmp(cmd, "DEHALFOP") == 0
                || stricmp(cmd, "PROTECT") == 0
                || stricmp(cmd, "DEPROTECT") == 0
                || stricmp(cmd, "INVITE") == 0
                || stricmp(cmd, "UNBAN") == 0
                || stricmp(cmd, "TOPIC") == 0
                || stricmp(cmd, "CLEAR") == 0
                || stricmp(cmd, "STATUS") == 0)
            && (cmdrec = lookup_cmd(THIS_MODULE, cmd)) != NULL
    ) {
        notice_help(s_ChanServ, u, cmdrec->helpmsg_all,
                    getstring_cmdacc(u->ngi, (int)(long)cmdrec->help_param1));
    } else {
        help_cmd(s_ChanServ, u, THIS_MODULE, cmd);
    }
}

/*************************************************************************/

static void do_register(User *u)
{
    char *chan = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    char *desc = strtok_remaining();
    NickInfo *ni = u->ni;
    NickGroupInfo *ngi = u->ngi;
    Channel *c;
    ChannelInfo *ci;
    struct u_chaninfolist *uc;
    int max;

    if (readonly) {
        notice_lang(s_ChanServ, u, CHAN_REGISTER_DISABLED);
        return;
    }

    if (!chan || !pass || !desc) {
        syntax_error(s_ChanServ, u, "REGISTER", CHAN_REGISTER_SYNTAX);
    } else if (!is_chanop(u, chan)) {
        notice_lang(s_ChanServ, u, CHAN_MUST_BE_CHANOP);
    } else if (strcmp(chan, "#") == 0) {
        notice_lang(s_ChanServ, u, CHAN_REGISTER_SHORT_CHANNEL);
    } else if (*chan == '&') {
        notice_lang(s_ChanServ, u, CHAN_REGISTER_NOT_LOCAL);
    } else if (*chan != '#') {
        notice_lang(s_ChanServ, u, CHAN_REGISTER_INVALID_NAME);
    } else if (!ni) {
        notice_lang(s_ChanServ, u, CHAN_MUST_REGISTER_NICK, s_NickServ);
    } else if (!user_identified(u)) {
        notice_lang(s_ChanServ, u, CHAN_MUST_IDENTIFY_NICK,
                s_NickServ, s_NickServ);

    } else if ((ci = get_channelinfo(chan)) != NULL) {
        if (ci->flags & CF_VERBOTEN) {
            module_log("Attempt to register forbidden channel %s by %s!%s@%s",
                       ci->name, u->nick, u->username, u->host);
            notice_lang(s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan);
        } else if (ci->flags & CF_SUSPENDED) {
            module_log("Attempt to register suspended channel %s by %s!%s@%s",
                       ci->name, u->nick, u->username, u->host);
            notice_lang(s_ChanServ, u, CHAN_ALREADY_REGISTERED, chan);
        } else {
            notice_lang(s_ChanServ, u, CHAN_ALREADY_REGISTERED, chan);
        }
        put_channelinfo(ci);

    } else if (!(NoAdminPasswordCheck && is_services_admin(u))
               && (stricmp(pass, chan) == 0
                || stricmp(pass, chan+1) == 0
                || stricmp(pass, u->nick) == 0
                || (StrictPasswords && strlen(pass) < 5))
    ) {
        notice_lang(s_ChanServ, u, MORE_OBSCURE_PASSWORD);

    } else if (!is_services_admin(u) && check_channel_limit(ngi, &max) >= 0) {
        notice_lang(s_ChanServ, u, ngi->channels_count > max
                                   ? CHAN_EXCEEDED_CHANNEL_LIMIT
                                   : CHAN_REACHED_CHANNEL_LIMIT, max);

    } else if (!(c = get_channel(chan))) {
        /* Should not fail because we checked is_chanop() above, but just
         * in case... */
        module_log("Channel %s not found for REGISTER", chan);
        notice_lang(s_ChanServ, u, CHAN_REGISTRATION_FAILED);

    } else {
        Password passbuf;

        init_password(&passbuf);
        if (encrypt_password(pass, strlen(pass), &passbuf) != 0) {
            clear_password(&passbuf);
            memset(pass, 0, strlen(pass));
            module_log("Failed to encrypt password for %s (register)", chan);
            notice_lang(s_ChanServ, u, CHAN_REGISTRATION_FAILED);
            return;
        }
        ci = makechan(chan);
        if (!ci) {
            clear_password(&passbuf);
            module_log("makechan() failed for REGISTER %s", chan);
            notice_lang(s_ChanServ, u, CHAN_REGISTRATION_FAILED);
            return;
        }
        c->ci = ci;
        ci->c = c;
        ci->flags = CSDefFlags;
        ci->mlock.on = CSDefModeLockOn;
        ci->mlock.off = CSDefModeLockOff;
        ci->last_used = ci->time_registered;
        ci->founder = u->ngi->id;
        copy_password(&ci->founderpass, &passbuf);
        clear_password(&passbuf);
        ci->desc = sstrdup(desc);
        if (c->topic) {
            ci->last_topic = sstrdup(c->topic);
            strbcpy(ci->last_topic_setter, c->topic_setter);
            ci->last_topic_time = c->topic_time;
        }
        count_chan(ci);
        module_log("Channel %s registered by %s!%s@%s",
                   chan, u->nick, u->username, u->host);
        notice_lang(s_ChanServ, u, CHAN_REGISTERED, chan, u->nick);
        if (CSShowPassword)
            notice_lang(s_ChanServ, u, CHAN_PASSWORD_IS, pass);
        memset(pass, 0, strlen(pass));
        uc = smalloc(sizeof(*uc));
        LIST_INSERT(uc, u->id_chans);
        strbcpy(uc->chan, ci->name);
        /* Implement new mode lock */
        check_modes(ci->c);

    }
}

/*************************************************************************/

static void do_identify(User *u)
{
    char *chan = strtok(NULL, " ");
    char *pass = strtok_remaining();
    ChannelInfo *ci = NULL;
    struct u_chaninfolist *uc;

    if (!chan || !pass) {
        syntax_error(s_ChanServ, u, "IDENTIFY", CHAN_IDENTIFY_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (ci->flags & CF_SUSPENDED) {
        notice_lang(s_ChanServ, u, CHAN_X_SUSPENDED, chan);
    } else {
        int res = check_password(pass, &ci->founderpass);
        if (res == 1) {
            ci->bad_passwords = 0;
            ci->last_used = time(NULL);
            if (!is_identified(u, ci)) {
                uc = smalloc(sizeof(*uc));
                LIST_INSERT(uc, u->id_chans);
                strbcpy(uc->chan, ci->name);
                module_log("%s!%s@%s identified for %s",
                           u->nick, u->username, u->host, ci->name);
            }
            notice_lang(s_ChanServ, u, CHAN_IDENTIFY_SUCCEEDED, chan);
        } else if (res < 0) {
            module_log("check_password failed for %s", ci->name);
            notice_lang(s_ChanServ, u, CHAN_IDENTIFY_FAILED);
        } else {
            module_log("Failed IDENTIFY for %s by %s!%s@%s",
                       ci->name, u->nick, u->username, u->host);
            chan_bad_password(u, ci);
        }
    }
    put_channelinfo(ci);
}

/*************************************************************************/

static void do_drop(User *u)
{
    const char *chan = strtok(NULL, " ");
    const char *pass = strtok_remaining();
    ChannelInfo *ci;
    int res;

    if (readonly) {
        notice_lang(s_ChanServ, u, CHAN_DROP_DISABLED);
        return;
    }

    if (!chan || !pass) {
        syntax_error(s_ChanServ, u, "DROP", CHAN_DROP_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
        put_channelinfo(ci);
    } else if (ci->flags & CF_SUSPENDED) {
        notice_lang(s_ChanServ, u, CHAN_X_SUSPENDED, chan);
        put_channelinfo(ci);
    } else if ((res = check_password(pass, &ci->founderpass)) != 1) {
        if (res < 0) {
            module_log("check_password failed for %s", ci->name);
            notice_lang(s_ChanServ, u, INTERNAL_ERROR);
        } else {
            module_log("Failed DROP for %s by %s!%s@%s",
                       ci->name, u->nick, u->username, u->host);
            chan_bad_password(u, ci);
        }
        put_channelinfo(ci);
    } else {
        const char *founder;
        char tmpbuf[64];

        if (ci->founder) {
            NickGroupInfo *ngi = get_ngi_id(ci->founder);
            if (ngi) {
                founder = ngi_mainnick(ngi);
            } else {
                snprintf(tmpbuf, sizeof(tmpbuf), "<unknown: ID %u>",
                         ci->founder);
                founder = tmpbuf;
            }
        } else {
            founder = "<none>";
        }
        module_log("Channel %s (founder %s) dropped by %s!%s@%s",
                   ci->name, founder, u->nick, u->username, u->host);
        delchan(ci);
        notice_lang(s_ChanServ, u, CHAN_DROPPED, chan);
    }
}

/*************************************************************************/

static void do_dropchan(User *u)
{
    const char *chan = strtok(NULL, " ");
    ChannelInfo *ci;

    if (!chan) {
        syntax_error(s_ChanServ, u, "DROPCHAN", CHAN_DROPCHAN_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else {
        const char *founder;
        char tmpbuf[64];

        if (readonly)
            notice_lang(s_ChanServ, u, READ_ONLY_MODE);
        if (ci->founder) {
            NickGroupInfo *ngi = get_ngi_id(ci->founder);
            if (ngi) {
                founder = ngi_mainnick(ngi);
            } else {
                snprintf(tmpbuf, sizeof(tmpbuf), "<unknown: ID %u>",
                         ci->founder);
                founder = tmpbuf;
            }
        } else {
            founder = "<none>";
        }
        module_log("Channel %s (founder %s) dropped by %s!%s@%s",
                   ci->name, founder, u->nick, u->username, u->host);
        delchan(ci);
        notice_lang(s_ChanServ, u, CHAN_DROPPED, chan);
    }
}

/*************************************************************************/

/* SADMINS, and users who have identified for a channel, can now cause its
 * entry message and successor to be displayed by supplying the ALL
 * parameter.
 * Syntax: INFO channel [ALL]
 * -TheShadow (29 Mar 1999)
 */

/* Check the status of show_all and make a note of having done so.  See
 * comments at nickserv/main.c/do_info() for details. */
#define CHECK_SHOW_ALL (used_all++, show_all)

static void do_info(User *u)
{
    char *chan = strtok(NULL, " ");
    char *param = strtok(NULL, " ");
    ChannelInfo *ci = NULL;
    NickGroupInfo *ngi = NULL, *ngi2 = NULL;
    char buf[BUFSIZE], *end, *s;
    int is_servadmin = is_services_admin(u);
    int can_show_all = 0, show_all = 0, used_all = 0;

    if (!chan) {
        syntax_error(s_ChanServ, u, "INFO", CHAN_INFO_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!ci->founder) {
        /* Paranoia... this shouldn't be able to happen */
        module_log("INFO: non-forbidden channel %s has no founder, deleting",
                   ci->name);
        delchan(ci);
        ci = NULL;
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (!(ngi = get_ngi_id(ci->founder))
               || (ci->successor && !(ngi2 = get_ngi_id(ci->successor)))
    ) {
        notice_lang(s_ChanServ, u, INTERNAL_ERROR);
    } else {

        /* Update last used time if the channel is currently in use. */
        if (ci->c) {
            struct c_userlist *cu;
            LIST_FOREACH (cu, ci->c->users) {
                if (check_access(cu->user, ci, CA_AUTOOP)) {
                    module_log_debug(2, "updating last used time for %s"
                                     " (INFO)", ci->name);
                    ci->last_used = time(NULL);
                    break;
                }
            }
        }

        /* Only show all the channel's settings to sadmins and founders. */
        can_show_all = (is_founder(u, ci) || is_servadmin);

        if ((param && stricmp(param, "ALL") == 0) && can_show_all)
            show_all = 1;

        notice_lang(s_ChanServ, u, CHAN_INFO_HEADER, chan);
        notice_lang(s_ChanServ, u, CHAN_INFO_FOUNDER, ngi_mainnick(ngi));
        if (ngi2 != NULL && CHECK_SHOW_ALL) {
            notice_lang(s_ChanServ, u, CHAN_INFO_SUCCESSOR,
                        ngi_mainnick(ngi2));
        }
        notice_lang(s_ChanServ, u, CHAN_INFO_DESCRIPTION, ci->desc);
        strftime_lang(buf, sizeof(buf), u->ngi, STRFTIME_DATE_TIME_FORMAT,
                      ci->time_registered);
        notice_lang(s_ChanServ, u, CHAN_INFO_TIME_REGGED, buf);
        strftime_lang(buf, sizeof(buf), u->ngi, STRFTIME_DATE_TIME_FORMAT,
                      ci->last_used);
        notice_lang(s_ChanServ, u, CHAN_INFO_LAST_USED, buf);

        /* Do not show last_topic if channel is mlock'ed +s or +p, or if the
         * channel's current modes include +s or +p. -TheShadow */
        /* But show it if we're showing all info. --AC */
        if (ci->last_topic) {
            int mlock_sp = (ci->mlock.on & (CMODE_s | CMODE_p));
            int mode_sp = (ci->c && (ci->c->mode & (CMODE_s | CMODE_p)));
            int hide = (ci->flags & CF_HIDE_TOPIC);
            if ((!mlock_sp && !mode_sp && !hide) || CHECK_SHOW_ALL) {
                notice_lang(s_ChanServ, u, CHAN_INFO_LAST_TOPIC,
                            ci->last_topic);
                notice_lang(s_ChanServ, u, CHAN_INFO_TOPIC_SET_BY,
                            ci->last_topic_setter);
            }
        }

        if (ci->entry_message && CHECK_SHOW_ALL)
            notice_lang(s_ChanServ, u, CHAN_INFO_ENTRYMSG, ci->entry_message);
        if (ci->url)
            notice_lang(s_ChanServ, u, CHAN_INFO_URL, ci->url);
        if (ci->email && (!(ci->flags & CF_HIDE_EMAIL) || CHECK_SHOW_ALL))
            notice_lang(s_ChanServ, u, CHAN_INFO_EMAIL, ci->email);
        s = chanopts_to_string(ci, u->ngi);
        notice_lang(s_ChanServ, u, CHAN_INFO_OPTIONS,
                    *s ? s : getstring(u->ngi, CHAN_INFO_OPT_NONE));
        end = buf;
        *end = 0;
        if (ci->mlock.on || ci->mlock.key || ci->mlock.limit)
            end += snprintf(end, sizeof(buf)-(end-buf), "+%s",
                            mode_flags_to_string(ci->mlock.on, MODE_CHANNEL));
        if (ci->mlock.off)
            end += snprintf(end, sizeof(buf)-(end-buf), "-%s",
                            mode_flags_to_string(ci->mlock.off, MODE_CHANNEL));
        if (*buf && (!(ci->flags & CF_HIDE_MLOCK) || CHECK_SHOW_ALL))
            notice_lang(s_ChanServ, u, CHAN_INFO_MODE_LOCK, buf);

        if ((ci->flags & CF_NOEXPIRE) && CHECK_SHOW_ALL)
            notice_lang(s_ChanServ, u, CHAN_INFO_NO_EXPIRE);

        if (ci->flags & CF_SUSPENDED) {
            notice_lang(s_ChanServ, u, CHAN_X_SUSPENDED, chan);
            if (CHECK_SHOW_ALL) {
                char timebuf[BUFSIZE], expirebuf[BUFSIZE];
                strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                              STRFTIME_DATE_TIME_FORMAT, ci->suspend_time);
                expires_in_lang(expirebuf, sizeof(expirebuf), u->ngi,
                                ci->suspend_expires);
                notice_lang(s_ChanServ, u, CHAN_INFO_SUSPEND_DETAILS,
                            ci->suspend_who, timebuf, expirebuf);
                notice_lang(s_ChanServ, u, CHAN_INFO_SUSPEND_REASON,
                            ci->suspend_reason);
            }
        }

        if (can_show_all && !show_all && used_all)
            notice_lang(s_ChanServ, u, CHAN_INFO_SHOW_ALL, s_ChanServ,
                        ci->name);

    }

    put_channelinfo(ci);
    put_nickgroupinfo(ngi);
    put_nickgroupinfo(ngi2);
}

/*************************************************************************/

/* SADMINS can search for channels based on their CF_VERBOTEN and
 * CF_NOEXPIRE flags and suspension status. This works in the same way as
 * NickServ's LIST command.
 * Syntax for sadmins: LIST pattern [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
 * Also fixed CF_PRIVATE channels being shown to non-sadmins.
 * -TheShadow
 */

static void do_list(User *u)
{
    char *pattern = strtok(NULL, " ");
    char *keyword;
    ChannelInfo *ci;
    int nchans;
    char buf[BUFSIZE];
    int is_servadmin = is_services_admin(u);
    int32 matchflags = 0;  /* CF_ flags a chan must match one of to qualify */
    int skip = 0;          /* number of records to skip before displaying */


    if (CSListOpersOnly && (!u || !is_oper(u))) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
        return;
    }

    if (pattern && *pattern == '+') {
        skip = (int)atolsafe(pattern+1, 0, INT_MAX);
        if (skip < 0) {
            syntax_error(s_ChanServ, u, "LIST",
                         is_oper(u)? CHAN_LIST_OPER_SYNTAX: CHAN_LIST_SYNTAX);
            return;
        }
        pattern = strtok(NULL, " ");
    }

    if (!pattern) {
        syntax_error(s_ChanServ, u, "LIST",
                     is_oper(u) ? CHAN_LIST_OPER_SYNTAX : CHAN_LIST_SYNTAX);
    } else {
        nchans = 0;

        while (is_servadmin && (keyword = strtok(NULL, " "))) {
            if (stricmp(keyword, "FORBIDDEN") == 0) {
                matchflags |= CF_VERBOTEN;
            } else if (stricmp(keyword, "NOEXPIRE") == 0) {
                matchflags |= CF_NOEXPIRE;
            } else if (stricmp(keyword, "SUSPENDED") == 0) {
                matchflags |= CF_SUSPENDED;
            } else {
                syntax_error(s_ChanServ, u, "LIST",
                     is_oper(u) ? CHAN_LIST_OPER_SYNTAX : CHAN_LIST_SYNTAX);
            }
        }

        for (ci = first_channelinfo(); ci; ci = next_channelinfo()) {
            if (!is_servadmin && (ci->flags & (CF_PRIVATE | CF_VERBOTEN)))
                continue;
            if (matchflags && !(ci->flags & matchflags))
                continue;

            snprintf(buf, sizeof(buf), "%-20s  %s", ci->name,
                     ci->desc ? ci->desc : "");
            if (irc_stricmp(pattern, ci->name) == 0
             || match_wild_nocase(pattern, buf)
            ) {
                nchans++;
                if (nchans > skip && nchans <= skip+ListMax) {
                    char noexpire_char = ' ', suspended_char = ' ';
                    if (is_servadmin) {
                        if (ci->flags & CF_NOEXPIRE)
                            noexpire_char = '!';
                        if (ci->flags & CF_SUSPENDED)
                            suspended_char = '*';
                    }

                    /* This can only be true for SADMINS - normal users
                     * will never get this far with a VERBOTEN channel.
                     * -TheShadow */
                    if (ci->flags & CF_VERBOTEN) {
                        snprintf(buf, sizeof(buf), "%-20s  [Forbidden]",
                                 ci->name);
                    }

                    if (nchans == 1)  /* display header before first result */
                        notice_lang(s_ChanServ, u, CHAN_LIST_HEADER, pattern);
                    notice(s_ChanServ, u->nick, "  %c%c%s",
                           suspended_char, noexpire_char, buf);
                }
            }
        }
        if (nchans) {
            int count = nchans - skip;
            if (count < 0)
                count = 0;
            else if (count > ListMax)
                count = ListMax;
            notice_lang(s_ChanServ, u, LIST_RESULTS, count, nchans);
        } else {
            notice_lang(s_ChanServ, u, CHAN_LIST_NO_MATCH);
        }
    }

}

/*************************************************************************/

static int list_akick(User *u, int index, ChannelInfo *ci, int *sent_header,
                      int is_view);

static void do_akick(User *u)
{
    char *chan   = strtok(NULL, " ");
    char *cmd    = strtok(NULL, " ");
    char *mask   = strtok(NULL, " ");
    char *reason = strtok_remaining();
    ChannelInfo *ci = NULL;
    int i;
    int is_list = (cmd && (stricmp(cmd,"LIST") == 0
                           || stricmp(cmd,"VIEW") == 0
                           || stricmp(cmd,"COUNT") == 0));

    if (!chan
     || !cmd
     || (!mask && (stricmp(cmd, "ADD") == 0 || stricmp(cmd, "DEL") == 0))
    ) {
        syntax_error(s_ChanServ, u, "AKICK", CHAN_AKICK_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!check_access_cmd(u, ci, "AKICK", is_list ? "LIST" : cmd)
               && !is_services_admin(u)) {
        if (ci->founder && valid_ngi(u) && ci->founder == u->ngi->id)
            notice_lang(s_ChanServ, u, CHAN_IDENTIFY_REQUIRED, s_ChanServ,
                        chan);
        else
            notice_lang(s_ChanServ, u, ACCESS_DENIED);

    } else if (stricmp(cmd, "ADD") == 0) {

        char *mask2, *user, *host;
        const char *nick;

        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
            put_channelinfo(ci);
            return;
        }

        /* Make sure we have a valid nick!user@host mask (fill in missing
         * parts with "*").  Error out on @ in nick (also catches a@b!c),
         * missing host, or empty nick/user/host. */
        mask2 = sstrdup(mask);
        nick = mask2;
        user = strchr(mask2, '!');
        if (user) {
            *user++ = 0;
        } else {
            nick = "*";
            user = mask2;
        }
        host = strchr(user, '@');
        if (host)
            *host++ = 0;
        if (!*nick || !*user || !host || !*host || strchr(nick, '@')) {
            notice_lang(s_ChanServ, u, BAD_NICKUSERHOST_MASK);
            free(mask2);
            put_channelinfo(ci);
            return;
        }
        mask = smalloc(strlen(nick)+strlen(user)+strlen(host)+3);
        sprintf(mask, "%s!%s@%s", nick, user, host);
        free(mask2);

        ARRAY_SEARCH(ci->akick, mask, mask, stricmp, i);
        if (i < ci->akick_count) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_ALREADY_EXISTS,
                        ci->akick[i].mask, chan);
            free(mask);
        } else if (ci->akick_count >= CSAutokickMax) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_REACHED_LIMIT,
                        CSAutokickMax);
            free(mask);
        } else {
            ARRAY_EXTEND(ci->akick);
            ci->akick[i].channel = ci;  /* i points at new entry, from above */
            ci->akick[i].mask = mask;
            ci->akick[i].reason = reason ? sstrdup(reason) : NULL;
            time(&ci->akick[i].set);
            ci->akick[i].lastused = 0;
            memset(ci->akick[i].who, 0, NICKMAX);  // Avoid leaking random data
            strbcpy(ci->akick[i].who, u->nick);
            notice_lang(s_ChanServ, u, CHAN_AKICK_ADDED, mask, chan);
        }

    } else if (stricmp(cmd, "DEL") == 0) {

        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
            put_channelinfo(ci);
            return;
        }

        ARRAY_SEARCH(ci->akick, mask, mask, stricmp, i);
        if (i < ci->akick_count) {
            free(ci->akick[i].mask);
            free(ci->akick[i].reason);
            ARRAY_REMOVE(ci->akick, i);
            notice_lang(s_ChanServ, u, CHAN_AKICK_DELETED, mask, chan);
        } else {
            notice_lang(s_ChanServ, u, CHAN_AKICK_NOT_FOUND, mask, chan);
        }

    } else if (stricmp(cmd, "LIST") == 0 || stricmp(cmd, "VIEW") == 0) {
        int is_view = stricmp(cmd,"VIEW")==0;
        int count = 0, sent_header = 0, skip = 0;

        if (mask && *mask == '+') {
            skip = (int)atolsafe(mask+1, 0, INT_MAX);
            if (skip < 0) {
                syntax_error(s_ChanServ, u, "AKICK",
                             is_view ? CHAN_AKICK_VIEW_SYNTAX
                                     : CHAN_AKICK_LIST_SYNTAX);
                put_channelinfo(ci);
                return;
            }
            mask = reason ? strtok(reason, " ") : NULL;
        }
        if (ci->akick_count == 0) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, chan);
            put_channelinfo(ci);
            return;
        }
        ARRAY_FOREACH (i, ci->akick) {
            if (mask) {
                if (!match_wild_nocase(mask, ci->akick[i].mask))
                    continue;
            }
            count++;
            if (count > skip && count <= skip+ListMax)
                list_akick(u, i, ci, &sent_header, is_view);
        }
        if (count) {
            int shown = count - skip;
            if (shown < 0)
                shown = 0;
            else if (shown > ListMax)
                shown = ListMax;
            notice_lang(s_ChanServ, u, LIST_RESULTS, shown, count);
        } else {
            notice_lang(s_ChanServ, u, CHAN_AKICK_NO_MATCH, chan);
        }

    } else if (stricmp(cmd, "ENFORCE") == 0) {
        Channel *c = get_channel(ci->name);
        struct c_userlist *cu, *next;
        int count = 0;

        if (!c) {
            notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, ci->name);
            put_channelinfo(ci);
            return;
        }
        LIST_FOREACH_SAFE (cu, c->users, next) {
            if (check_kick(cu->user, c->name, 0))
                count++;
        }

        notice_lang(s_ChanServ, u, CHAN_AKICK_ENFORCE_DONE, chan, count);

    } else if (stricmp(cmd, "COUNT") == 0) {
        notice_lang(s_ChanServ, u, CHAN_AKICK_COUNT, ci->name,
                    ci->akick_count);

    } else {
        syntax_error(s_ChanServ, u, "AKICK", CHAN_AKICK_SYNTAX);
    }

    put_channelinfo(ci);
}

/************************************/

static int list_akick(User *u, int index, ChannelInfo *ci, int *sent_header,
                      int is_view)
{
    AutoKick *akick = &ci->akick[index];
    char buf[BUFSIZE];

    if (!akick->mask)
        return 0;
    if (!*sent_header) {
        notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_HEADER, ci->name);
        *sent_header = 1;
    }
    if (akick->reason)
        snprintf(buf, sizeof(buf), " (%s)", akick->reason);
    else
        *buf = 0;
    if (is_view) {
        char setbuf[BUFSIZE], usedbuf[BUFSIZE];
        strftime_lang(setbuf, sizeof(setbuf), u->ngi,
                      STRFTIME_DATE_TIME_FORMAT, akick->set);
        if (akick->lastused) {
            strftime_lang(usedbuf, sizeof(usedbuf), u->ngi,
                          STRFTIME_DATE_TIME_FORMAT, akick->lastused);
            notice_lang(s_ChanServ, u, CHAN_AKICK_VIEW_FORMAT,
                        akick->mask, akick->who[0] ? akick->who : "<unknown>",
                        setbuf, usedbuf, buf);
        } else {
            notice_lang(s_ChanServ, u, CHAN_AKICK_VIEW_UNUSED_FORMAT,
                        akick->mask, akick->who[0] ? akick->who : "<unknown>",
                        setbuf, buf);
        }
    } else {
        notice(s_ChanServ, u->nick, "    %s%s", akick->mask, buf);
    }
    return 1;
}

/*************************************************************************/

/* Internal routine to handle all op/voice-type requests. */

static struct {
    const char *cmd;
    int add;
    char mode;
    int target_acc;     /* Target access (CA_*) at which we refuse command */
    int success_msg, already_msg, failure_msg;
} opvoice_data[] = {
    { "VOICE",     1, 'v', -1,
          CHAN_VOICE_SUCCEEDED, CHAN_VOICE_ALREADY, CHAN_VOICE_FAILED },
    { "HALFOP",    1, 'h', CA_AUTODEOP,
          CHAN_HALFOP_SUCCEEDED, CHAN_HALFOP_ALREADY, CHAN_HALFOP_FAILED },
    { "OP",        1, 'o', CA_AUTODEOP,
          CHAN_OP_SUCCEEDED, CHAN_OP_ALREADY, CHAN_OP_FAILED },
    { "PROTECT",   1, 'a', CA_AUTODEOP,
          CHAN_PROTECT_SUCCEEDED, CHAN_PROTECT_ALREADY, CHAN_PROTECT_FAILED },

    { "DEVOICE",   0, 'v', CA_AUTOVOICE,
          CHAN_DEVOICE_SUCCEEDED, CHAN_DEVOICE_ALREADY, CHAN_DEVOICE_FAILED },
    { "DEHALFOP",  0, 'h', CA_AUTOHALFOP,
          CHAN_DEHALFOP_SUCCEEDED, CHAN_DEHALFOP_ALREADY,
          CHAN_DEHALFOP_FAILED },
    { "DEOP",      0, 'o', CA_AUTOOP,
          CHAN_DEOP_SUCCEEDED, CHAN_DEOP_ALREADY, CHAN_DEOP_FAILED },
    { "DEPROTECT", 0, 'a', CA_AUTOPROTECT,
          CHAN_DEPROTECT_SUCCEEDED, CHAN_DEPROTECT_ALREADY,
          CHAN_DEPROTECT_FAILED },
};

static void do_opvoice(User *u, const char *cmd)
{
    const char *cmd2 = (strnicmp(cmd,"DE",2) == 0 ? cmd+2 : cmd);
    char *chan = strtok(NULL, " ");
    char *target = strtok(NULL, " ");
    Channel *c;
    ChannelInfo *ci;
    User *target_user;
    int i;
    int add, target_acc, success_msg, failure_msg, already_msg;
    int target_nextacc;  /* Target level upper bound for DEVOICE, DEHALFOP */
    int32 mode;

    ARRAY2_SEARCH(opvoice_data, lenof(opvoice_data), cmd, cmd, strcmp, i);
    if (i >= lenof(opvoice_data)) {
        module_log("do_opvoice: BUG: command `%s' not found in table", cmd);
        notice_lang(s_ChanServ, u, INTERNAL_ERROR);
        return;
    }
    add            = opvoice_data[i].add;
    mode           = mode_char_to_flag(opvoice_data[i].mode, MODE_CHANUSER);
    target_acc     = opvoice_data[i].target_acc;
    success_msg    = opvoice_data[i].success_msg;
    already_msg    = opvoice_data[i].already_msg;
    failure_msg    = opvoice_data[i].failure_msg;
    if (strcmp(cmd, "DEVOICE") == 0)
        target_nextacc = (protocol_features & PF_HALFOP)
                       ? CA_AUTOHALFOP : CA_AUTOOP;
    else if (strcmp(cmd, "DEHALFOP") == 0)
        target_nextacc = CA_AUTOOP;
    else
        target_nextacc = -1;

    do {
        if (target) {
            target_user = get_user(target);
        } else {
            target = u->nick;
            target_user = u;
        }
        if (!chan) {
            syntax_error(s_ChanServ, u, cmd, CHAN_OPVOICE_SYNTAX);
        } else if (!(c = get_channel(chan))) {
            notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
        } else if (c->bouncy_modes) {
            notice_lang(s_ChanServ, u, CHAN_BOUNCY_MODES, cmd);
        } else if (!(ci = c->ci)) {
            notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
        } else if (ci->flags & CF_VERBOTEN) {
            notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
        } else if (!u || !check_access_cmd(u, ci, cmd2, NULL)) {
            notice_lang(s_ChanServ, u, PERMISSION_DENIED);
        } else if (!target_user) {
            notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, target);
        } else {
            struct c_userlist *cu;
            LIST_SEARCH_SCALAR(c->users, user, target_user, cu);
            if (!cu) {
                notice_lang(s_ChanServ, u, NICK_X_NOT_ON_CHAN_X, target, chan);
                return;
            } else if (
                /* Allow changing own mode */
                       !(target_user == u)
                /* Allow deops if !ENFORCE */
                    && !(!add && !(ci->flags & CF_ENFORCE))
                /* Disallow if user is at/above disallow level... */
                    && target_acc >= 0 
                    && check_access(target_user, ci, target_acc)
                /* ... and below level-above-disallow-level (if any) */
                    && (target_nextacc < 0
                        || !check_access(target_user, ci, target_nextacc))
            ) {
                notice_lang(s_ChanServ, u, failure_msg, target, chan);
            } else {
                char modebuf[3];
                int32 umode, thismode;

                /* Check whether they already have / don't have the mode */
                umode = cu->mode & mode;
                if (add)
                    umode ^= mode;  /* note which ones they DON'T have */
                if (!umode) {
                    /* Target user already has (or doesn't have, if !add)
                     * mode(s), so don't do anything */
                    notice_lang(s_ChanServ, u, already_msg, target, chan);
                    return;
                }

                /* Set appropriate mode(s) */
                modebuf[0] = add ? '+' : '-';
                modebuf[2] = 0;
                thismode = 1;
                while (umode) {
                    while (!(umode & thismode))
                        thismode <<= 1;
                    if (!thismode) {  /* impossible */
                        module_log("BUG: thismode==0 in opvoice!");
                        break;
                    }
                    modebuf[1] = mode_flag_to_char(thismode, MODE_CHANUSER);
                    set_cmode(s_ChanServ, c, modebuf, target);
                    umode &= ~thismode;
                }
                set_cmode(NULL, c);  /* Flush mode change(s) out */
                if (ci->flags & CF_OPNOTICE) {
                    notice(s_ChanServ, chan, "%s command used for %s by %s",
                           cmd, target, u->nick);
                }
                notice_lang(s_ChanServ, u, success_msg, target, chan);
                /* If it was an OP command, update the last-used time */
                if (strcmp(cmd, "OP") == 0)
                    ci->last_used = time(NULL);
            }  /* if allowed */
        }
    } while ((target = strtok(NULL, " ")) != NULL);
}

static void do_op(User *u)
{
    do_opvoice(u, "OP");
}

static void do_deop(User *u)
{
    do_opvoice(u, "DEOP");
}

static void do_voice(User *u)
{
    do_opvoice(u, "VOICE");
}

static void do_devoice(User *u)
{
    do_opvoice(u, "DEVOICE");
}

static void do_halfop(User *u)
{
    do_opvoice(u, "HALFOP");
}

static void do_dehalfop(User *u)
{
    do_opvoice(u, "DEHALFOP");
}

static void do_protect(User *u)
{
    do_opvoice(u, "PROTECT");
}

static void do_deprotect(User *u)
{
    do_opvoice(u, "DEPROTECT");
}

/*************************************************************************/

static void do_invite(User *u)
{
    char *chan = strtok(NULL, " ");
    Channel *c;
    ChannelInfo *ci;

    if (!chan) {
        syntax_error(s_ChanServ, u, "INVITE", CHAN_INVITE_SYNTAX);
    } else if (!(c = get_channel(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_ChanServ, u, CHAN_BOUNCY_MODES, "INVITE");
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!u || !check_access_cmd(u, ci, "INVITE", NULL)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else if (call_callback_3(cb_invite, u, c, ci) <= 0) {
        send_cmd(s_ChanServ, "INVITE %s %s", u->nick, chan);
        notice_lang(s_ChanServ, u, CHAN_INVITE_OK, u->nick, chan);
    }
}

/*************************************************************************/

static void do_unban(User *u)
{
    char *chan = strtok(NULL, " ");
    Channel *c;
    ChannelInfo *ci;

    if (!chan) {
        syntax_error(s_ChanServ, u, "UNBAN", CHAN_UNBAN_SYNTAX);
    } else if (!(c = get_channel(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_ChanServ, u, CHAN_BOUNCY_MODES, "UNBAN");
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!u || !check_access_cmd(u, ci, "UNBAN", NULL)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else if (call_callback_3(cb_unban, u, c, ci) <= 0) {
        clear_channel(c, CLEAR_BANS, u);
        notice_lang(s_ChanServ, u, CHAN_UNBANNED, chan);
    }
}

/*************************************************************************/

/* do_kick() is used by users.c, so we use a different function name */

static void do_cskick(User *u)
{
    char *chan = strtok(NULL, " ");
    char *target = strtok(NULL, " ");
    char *reason = strtok_remaining();
    Channel *c;
    ChannelInfo *ci;
    User *target_user;

    if (!chan || !target) {
        syntax_error(s_ChanServ, u, "KICK", CHAN_KICK_SYNTAX);
    } else if (!(c = get_channel(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_ChanServ, u, CHAN_BOUNCY_MODES, "KICK");
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!u || !check_access_cmd(u, ci, "KICK", NULL)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else if (!(target_user = get_user(target))) {
        notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, target);
    } else {
        struct c_userlist *cu;
        char reasonbuf[BUFSIZE];
        char *kick_av[3];

        /* Retrieve c_userlist entry and see (1) if they're even on the
         * channel and (2) if they're protected (if the ircd supports that) */
        LIST_SEARCH_SCALAR(c->users, user, target_user, cu);
        if (!cu) {
            notice_lang(s_ChanServ, u, NICK_X_NOT_ON_CHAN_X, target, chan);
            return;
        }
        if (protocol_features & PF_CHANPROT) {
            if (cu->mode & mode_char_to_flag('a', MODE_CHANUSER)) {
                notice_lang(s_ChanServ, u, CHAN_KICK_PROTECTED, target, chan);
                return;
            }
        }
        /* Also prevent Services opers and above from being kicked */
        if (is_services_oper(target_user)) {
            notice_lang(s_ChanServ, u, CHAN_KICK_PROTECTED, target, chan);
            return;
        }

        /* Construct reason string: "KICK by Nick" / "KICK by Nick (reason)" */
        if (reason && !*reason)
            reason = NULL;
        snprintf(reasonbuf, sizeof(reasonbuf), "KICK by %s%s%s%s", u->nick,
                 reason ? " (" : "", reason ? reason : "", reason ? ")" : "");

        /* Actually kick user */
        send_cmd(s_ChanServ, "KICK %s %s :%s", chan, target, reasonbuf);
        kick_av[0] = chan;
        kick_av[1] = target;
        kick_av[2] = reasonbuf;
        do_kick(s_ChanServ, 3, kick_av);
        notice_lang(s_ChanServ, u, CHAN_KICKED, target, chan);
    }
}

/*************************************************************************/

static void do_cstopic(User *u)
{
    char *chan = strtok(NULL, " ");
    char *topic = strtok_remaining();
    Channel *c;
    ChannelInfo *ci;

    if (!chan || !topic) {
        syntax_error(s_ChanServ, u, "TOPIC", CHAN_TOPIC_SYNTAX);
    } else if (!(c = get_channel(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_ChanServ, u, CHAN_BOUNCY_MODES, "TOPIC");
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!u || !check_access_cmd(u, ci, "TOPIC", NULL)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else {
        time_t now = time(NULL);
        set_topic(s_ChanServ, c, topic, u->nick, now);
        record_topic(ci, topic, u->nick, now);
    }
}

/*************************************************************************/

static void do_clear(User *u)
{
    char *chan = strtok(NULL, " ");
    char *what = strtok(NULL, " ");
    Channel *c;
    ChannelInfo *ci;

    if (!chan || !what) {
        syntax_error(s_ChanServ, u, "CLEAR", CHAN_CLEAR_SYNTAX);
    } else if (!(c = get_channel(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_ChanServ, u, CHAN_BOUNCY_MODES, "CLEAR");
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!u || !check_access_cmd(u, ci, "CLEAR", what)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else if (call_callback_3(cb_clear, u, c, what) > 0) {
        return;
    } else if (stricmp(what, "BANS") == 0) {
        clear_channel(c, CLEAR_BANS, NULL);
        notice_lang(s_ChanServ, u, CHAN_CLEARED_BANS, chan);
    } else if (stricmp(what, "MODES") == 0) {
        clear_channel(c, CLEAR_MODES, NULL);
        notice_lang(s_ChanServ, u, CHAN_CLEARED_MODES, chan);
    } else if (stricmp(what, "OPS") == 0) {
        clear_channel(c, CLEAR_UMODES, (void *)CUMODE_o);
        notice_lang(s_ChanServ, u, CHAN_CLEARED_OPS, chan);
    } else if (stricmp(what, "VOICES") == 0) {
        clear_channel(c, CLEAR_UMODES, (void *)CUMODE_v);
        notice_lang(s_ChanServ, u, CHAN_CLEARED_VOICES, chan);
    } else if (stricmp(what, "USERS") == 0) {
        char buf[BUFSIZE];
        snprintf(buf, sizeof(buf), "CLEAR USERS command from %s", u->nick);
        clear_channel(c, CLEAR_USERS, buf);
        notice_lang(s_ChanServ, u, CHAN_CLEARED_USERS, chan);
    } else {
        syntax_error(s_ChanServ, u, "CLEAR", CHAN_CLEAR_SYNTAX);
    }
}

/*************************************************************************/

static void do_status(User *u)
{
    ChannelInfo *ci;
    User *u2;
    char *nick, *chan;

    chan = strtok(NULL, " ");
    nick = strtok(NULL, " ");
    if (!chan || !nick || strtok(NULL, " ")) {
        notice(s_ChanServ, u->nick, "STATUS ? ? ERROR Syntax error");
        return;
    }
    if (!(ci = get_channelinfo(chan))) {
        char *temp = chan;
        chan = nick;
        nick = temp;
        ci = get_channelinfo(chan);
    }
    if (!ci) {
        notice(s_ChanServ, u->nick, "STATUS %s %s ERROR Channel not"
               " registered", chan, nick);
    } else if (ci->flags & CF_VERBOTEN) {
        notice(s_ChanServ, u->nick, "STATUS %s %s ERROR Channel forbidden",
               chan, nick);
    } else if (!is_services_admin(u)
               && !check_access_cmd(u, ci, "STATUS", NULL)) {
        notice(s_ChanServ, u->nick, "STATUS %s %s ERROR Permission denied",
               chan, nick);
    } else if ((u2 = get_user(nick)) != NULL) {
        int acc = get_access(u2, ci);
        int have_acclev = (find_module("chanserv/access-levels") != NULL);
        int have_accxop = (find_module("chanserv/access-xop") != NULL);
        char accbuf[BUFSIZE];

        if (have_accxop) {
            const char *xop;
            if (acc == ACCLEV_FOUNDER)
                xop = "Founder";
            else if (acc >= ACCLEV_SOP)
                xop = "SOP";
            else if (acc >= ACCLEV_AOP)
                xop = "AOP";
            else if (acc >= ACCLEV_HOP && (protocol_features & PF_HALFOP))
                xop = "HOP";
            else if (acc >= ACCLEV_VOP)
                xop = "VOP";
            else
                xop = "---";
            if (have_acclev)
                snprintf(accbuf, sizeof(accbuf), "%d (%s)", acc, xop);
            else
                snprintf(accbuf, sizeof(accbuf), "%s", xop);
        } else {  /* access-levels only, or none */
            snprintf(accbuf, sizeof(accbuf), "%d", acc);
        }
        notice(s_ChanServ, u->nick, "STATUS %s %s %s", chan, nick, accbuf);
    } else { /* !u2 */
        notice(s_ChanServ, u->nick, "STATUS %s %s ERROR Nick not online",
               chan, nick);
    }
    put_channelinfo(ci);
}

/*************************************************************************/

/* Assumes that permission checking has already been done. */

static void do_getpass(User *u)
{
    char *chan = strtok(NULL, " ");
    char pass[PASSMAX];
    ChannelInfo *ci = NULL;
    int i;

    if (!chan) {
        syntax_error(s_ChanServ, u, "GETPASS", CHAN_GETPASS_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if ((i = decrypt_password(&ci->founderpass, pass, PASSMAX)) == -2) {
        notice_lang(s_ChanServ, u, CHAN_GETPASS_UNAVAILABLE, ci->name);
    } else if (i != 0) {
        module_log("decrypt_password() failed for GETPASS on %s", ci->name);
        notice_lang(s_ChanServ, u, INTERNAL_ERROR);
    } else {
        module_log("%s!%s@%s used GETPASS on %s",
                   u->nick, u->username, u->host, ci->name);
        if (WallAdminPrivs) {
            wallops(s_ChanServ, "\2%s\2 used GETPASS on \2%s\2",
                    u->nick, ci->name);
        }
        notice_lang(s_ChanServ, u, CHAN_GETPASS_PASSWORD_IS, ci->name, pass);
    }
    put_channelinfo(ci);
}

/*************************************************************************/

static void do_forbid(User *u)
{
    ChannelInfo *ci;
    char *chan = strtok(NULL, " ");

    /* Assumes that permission checking has already been done. */
    if (!chan || *chan != '#') {
        syntax_error(s_ChanServ, u, "FORBID", CHAN_FORBID_SYNTAX);
        return;
    } else if (strcmp(chan, "#") == 0) {
        notice_lang(s_ChanServ, u, CHAN_FORBID_SHORT_CHANNEL);
        return;
    }
    if (readonly)
        notice_lang(s_ChanServ, u, READ_ONLY_MODE);
    if ((ci = get_channelinfo(chan)) != NULL)
        delchan(ci);
    ci = makechan(chan);
    if (ci) {
        Channel *c;
        module_log("%s!%s@%s set FORBID for channel %s",
                   u->nick, u->username, u->host, ci->name);
        ci->flags |= CF_VERBOTEN;
        ci->time_registered = time(NULL);
        notice_lang(s_ChanServ, u, CHAN_FORBID_SUCCEEDED, chan);
        if (WallAdminPrivs) {
            wallops(s_ChanServ, "\2%s\2 used FORBID on \2%s\2",
                    u->nick, ci->name);
        }
        c = get_channel(chan);
        if (c) {
            c->ci = ci;
            ci->c = c;
            clear_channel(c, CLEAR_USERS,
                          "Use of this channel has been forbidden");
        } else {
            put_channelinfo(ci);
        }
    } else {
        module_log("Valid FORBID for %s by %s!%s@%s failed",
                   ci->name, u->nick, u->username, u->host);
        notice_lang(s_ChanServ, u, CHAN_FORBID_FAILED, chan);
    }
}

/*************************************************************************/

static void do_suspend(User *u)
{
    ChannelInfo *ci = NULL;
    char *expiry, *chan, *reason;
    time_t expires;

    chan = strtok(NULL, " ");
    if (chan && *chan == '+') {
        expiry = chan+1;
        chan = strtok(NULL, " ");
    } else {
        expiry = NULL;
    }
    reason = strtok_remaining();

    if (!chan || !reason) {
        syntax_error(s_ChanServ, u, "SUSPEND", CHAN_SUSPEND_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (ci->flags & CF_SUSPENDED) {
        notice_lang(s_ChanServ, u, CHAN_SUSPEND_ALREADY_SUSPENDED, chan);
    } else {
        Channel *c;
        if (expiry)
            expires = dotime(expiry);
        else
            expires = CSSuspendExpire;
        if (expires < 0) {
            notice_lang(s_ChanServ, u, BAD_EXPIRY_TIME);
            return;
        } else if (expires > 0) {
            expires += time(NULL);      /* Set an absolute time */
        }
        module_log("%s!%s@%s suspended %s",
                   u->nick, u->username, u->host, ci->name);
        suspend_channel(ci, reason, u->nick, expires);
        notice_lang(s_ChanServ, u, CHAN_SUSPEND_SUCCEEDED, chan);
        c = get_channel(chan);
        if (c)
            clear_channel(c, CLEAR_USERS,
                          "Use of this channel has been forbidden");
        if (readonly)
            notice_lang(s_ChanServ, u, READ_ONLY_MODE);
        if (WallAdminPrivs) {
            wallops(s_ChanServ, "\2%s\2 used SUSPEND on \2%s\2",
                    u->nick, ci->name);
        }
    }
    put_channelinfo(ci);
}

/*************************************************************************/

static void do_unsuspend(User *u)
{
    ChannelInfo *ci = NULL;
    char *chan = strtok(NULL, " ");

    if (!chan) {
        syntax_error(s_ChanServ, u, "UNSUSPEND", CHAN_UNSUSPEND_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!(ci->flags & CF_SUSPENDED)) {
        notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_NOT_SUSPENDED, chan);
    } else {
        if (readonly)
            notice_lang(s_ChanServ, u, READ_ONLY_MODE);
        module_log("%s!%s@%s unsuspended %s",
                   u->nick, u->username, u->host, ci->name);
        unsuspend_channel(ci, 1);
        notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_SUCCEEDED, chan);
        if (WallAdminPrivs) {
            wallops(s_ChanServ, "\2%s\2 used UNSUSPEND on \2%s\2",
                    u->nick, ci->name);
        }
    }
    put_channelinfo(ci);
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int CSDefKeepTopic;
static int CSDefSecureOps;
static int CSDefPrivate;
static int CSDefTopicLock;
static int CSDefLeaveOps;
static int CSDefSecure;
static int CSDefOpNotice;
static int CSDefEnforce;
static int CSDefMemoRestricted;
static int CSDefHideEmail;
static int CSDefHideTopic;
static int CSDefHideMlock;

static int do_CSAlias(const char *filename, int linenum, char *param);
static int do_CSDefModeLock(const char *filename, int linenum, char *param);

ConfigDirective module_config[] = {
    { "ChanServName",     { { CD_STRING, CF_DIRREQ, &s_ChanServ },
                            { CD_STRING, 0, &desc_ChanServ } } },
    { "CSAccessMax",      { { CD_POSINT, CF_DIRREQ, &CSAccessMax } } },
    { "CSAlias",          { { CD_FUNC, 0, do_CSAlias } } },
    { "CSAutokickMax",    { { CD_POSINT, CF_DIRREQ, &CSAutokickMax } } },
    { "CSDefEnforce",     { { CD_SET, 0, &CSDefEnforce } } },
    { "CSDefHideEmail",   { { CD_SET, 0, &CSDefHideEmail } } },
    { "CSDefHideMlock",   { { CD_SET, 0, &CSDefHideMlock } } },
    { "CSDefHideTopic",   { { CD_SET, 0, &CSDefHideTopic } } },
    { "CSDefKeepTopic",   { { CD_SET, 0, &CSDefKeepTopic } } },
    { "CSDefLeaveOps",    { { CD_SET, 0, &CSDefLeaveOps } } },
    { "CSDefMemoRestricted",{{CD_SET, 0, &CSDefMemoRestricted } } },
    { "CSDefModeLock",    { { CD_FUNC, 0, do_CSDefModeLock } } },
    { "CSDefOpNotice",    { { CD_SET, 0, &CSDefOpNotice } } },
    { "CSDefPrivate",     { { CD_SET, 0, &CSDefPrivate } } },
    { "CSDefSecure",      { { CD_SET, 0, &CSDefSecure } } },
    { "CSDefSecureOps",   { { CD_SET, 0, &CSDefSecureOps } } },
    { "CSDefTopicLock",   { { CD_SET, 0, &CSDefTopicLock } } },
    { "CSEnableRegister", { { CD_SET, 0, &CSEnableRegister } } },
    { "CSExpire",         { { CD_TIME, 0, &CSExpire } } },
    { "CSForbidShortChannel",{{CD_SET, 0, &CSForbidShortChannel } } },
    { "CSInhabit",        { { CD_TIME, CF_DIRREQ, &CSInhabit } } },
    { "CSListOpersOnly",  { { CD_SET, 0, &CSListOpersOnly } } },
    { "CSMaxReg",         { { CD_POSINT, 0, &CSMaxReg } } },
    { "CSRegisteredOnly", { { CD_SET, 0, &CSRegisteredOnly } } },
    { "CSRestrictDelay",  { { CD_TIME, 0, &CSRestrictDelay } } },
    { "CSShowPassword",   { { CD_SET, 0, &CSShowPassword } } },
    { "CSSkipModeRCheck", { { CD_SET, 0, &CSSkipModeRCheck } } },
    { "CSSuspendExpire",  { { CD_TIME, 0 , &CSSuspendExpire },
                            { CD_TIME, 0 , &CSSuspendGrace } } },
    { NULL }
};

/* Pointers to command records (for !CSEnableCommand) */
static Command *cmd_REGISTER;
static Command *cmd_GETPASS;

/* Previous value of clear_channel() sender */
static char old_clearchan_sender[NICKMAX];
static int old_clearchan_sender_set = 0;

/*************************************************************************/

/* CSAlias handling */

static int do_CSAlias(const char *filename, int linenum, char *param)
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

static int do_CSDefModeLock(const char *filename, int linenum, char *param)
{
    static int32 new_on = 0, new_off = 0;

    if (!filename) {
        switch (linenum) {
          case CDFUNC_INIT:
            /* Prepare for reading config file */
            new_on = new_off = 0;
            break;
          case CDFUNC_SET:
            /* Copy data to config variables */
            CSDefModeLockOn = new_on;
            CSDefModeLockOff = new_off;
            break;
          case CDFUNC_DECONFIG:
            /* Reset to initial values */
            CSDefModeLockOn = CMODE_n | CMODE_t;
            CSDefModeLockOff = 0;
            break;
        } /* switch (linenum) */
    } else {  /* filename != NULL, process parameter */
        int add = -1;
        while (*param) {
            if (*param == '+') {
                add = 1;
            } else if (*param == '-') {
                add = 0;
            } else if (add < 0) {
                config_error(filename, linenum, "Mode characters must be"
                             " preceded by + or -");
                return 0;
            } else {
                int32 flag = mode_char_to_flag(*param, MODE_CHANNEL);
                int nparams = mode_char_to_params(*param, MODE_CHANNEL);
                if (!flag) {
                    config_error(filename, linenum, "Invalid mode"
                                 " character `%c'", *param);
                    return 0;
                } else if (nparams) {
                    config_error(filename, linenum, "Modes with parameters"
                                 " cannot be used with CSDefModeLock");
                    return 0;
                } else {
                    if (add)
                        new_on |= flag;
                    else
                        new_off |= flag;
                }
            }
            param++;
        }
    }
    return 1;
}

/*************************************************************************/

static void handle_config(void)
{
    CSDefFlags = 0;
    if (CSDefKeepTopic)
        CSDefFlags |= CF_KEEPTOPIC;
    if (CSDefSecureOps)
        CSDefFlags |= CF_SECUREOPS;
    if (CSDefPrivate)
        CSDefFlags |= CF_PRIVATE;
    if (CSDefTopicLock)
        CSDefFlags |= CF_TOPICLOCK;
    if (CSDefLeaveOps)
        CSDefFlags |= CF_LEAVEOPS;
    if (CSDefSecure)
        CSDefFlags |= CF_SECURE;
    if (CSDefOpNotice)
        CSDefFlags |= CF_OPNOTICE;
    if (CSDefEnforce)
        CSDefFlags |= CF_ENFORCE;
    if (CSDefMemoRestricted)
        CSDefFlags |= CF_MEMO_RESTRICTED;
    if (CSDefHideEmail)
        CSDefFlags |= CF_HIDE_EMAIL;
    if (CSDefHideTopic)
        CSDefFlags |= CF_HIDE_TOPIC;
    if (CSDefHideMlock)
        CSDefFlags |= CF_HIDE_MLOCK;

    if (CSMaxReg > MAX_CHANNELCOUNT) {
        module_log("CSMaxReg upper-bounded at MAX_CHANNELCOUNT (%d)",
                   MAX_CHANNELCOUNT);
        CSMaxReg = MAX_CHANNELCOUNT;
    }
}

/*************************************************************************/

static int do_reconfigure(int after_configure)
{
    static char old_s_ChanServ[NICKMAX];
    static char *old_desc_ChanServ = NULL;
    static int old_CSEnableRegister;

    if (!after_configure) {
        /* Before reconfiguration: save old values. */
        strbcpy(old_s_ChanServ, s_ChanServ);
        old_desc_ChanServ    = strdup(desc_ChanServ);
        old_CSEnableRegister = CSEnableRegister;
    } else {
        Command *cmd;
        /* After reconfiguration: handle value changes. */
        handle_config();
        if (strcmp(old_s_ChanServ, s_ChanServ) != 0) {
            if (strcmp(set_clear_channel_sender(PTR_INVALID),old_s_ChanServ)==0)
                set_clear_channel_sender(s_ChanServ);
            send_nickchange(old_s_ChanServ, s_ChanServ);
        }
        if (!old_desc_ChanServ || strcmp(old_desc_ChanServ,desc_ChanServ) != 0)
            send_namechange(s_ChanServ, desc_ChanServ);
        free(old_desc_ChanServ);
        if (CSEnableRegister && !old_CSEnableRegister) {
            cmd_REGISTER->helpmsg_all = cmd_REGISTER->helpmsg_oper;
            cmd_REGISTER->helpmsg_oper = -1;
            cmd_REGISTER->has_priv = NULL;
        } else if (!CSEnableRegister && old_CSEnableRegister) {
            cmd_REGISTER->has_priv = is_services_admin;
            cmd_REGISTER->helpmsg_oper = cmd_REGISTER->helpmsg_all;
            cmd_REGISTER->helpmsg_all = -1;
        }
        if (EnableGetpass)
            cmd_GETPASS->name = "GETPASS";
        else
            cmd_GETPASS->name = "";
        /* Update command help parameters */
        cmd_REGISTER->help_param1 = s_NickServ;
        if ((cmd = lookup_cmd(THIS_MODULE, "SET SECURE")) != NULL) {
            cmd->help_param1 = s_NickServ;
            cmd->help_param2 = s_NickServ;
        }
    }  /* if (!after_configure) */
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    Command *cmd;


    handle_config();

    module_operserv = find_module("operserv/main");
    if (!module_operserv) {
        module_log("OperServ main module not loaded");
        exit_module(0);
        return 0;
    }
    use_module(module_operserv);

    module_nickserv = find_module("nickserv/main");
    if (!module_nickserv) {
        module_log("NickServ main module not loaded");
        exit_module(0);
        return 0;
    }
    use_module(module_nickserv);

    if (!new_commandlist(THIS_MODULE) || !register_commands(THIS_MODULE, cmds)
        || ((protocol_features & PF_HALFOP)
            && !register_commands(THIS_MODULE, cmds_halfop))
        || ((protocol_features & PF_CHANPROT)
            && !register_commands(THIS_MODULE, cmds_chanprot))
    ) {
        module_log("Unable to register commands");
        exit_module(0);
        return 0;
    }

    cb_clear     = register_callback("CLEAR");
    cb_command   = register_callback("command");
    cb_help      = register_callback("HELP");
    cb_help_cmds = register_callback("HELP COMMANDS");
    cb_invite    = register_callback("INVITE");
    cb_unban     = register_callback("UNBAN");
    if (cb_command < 0 || cb_clear < 0 || cb_help < 0 || cb_help_cmds < 0
     || cb_invite < 0 || cb_unban < 0
    ) {
        module_log("Unable to register callbacks");
        exit_module(0);
        return 0;
    }

    cmd_REGISTER = lookup_cmd(THIS_MODULE, "REGISTER");
    if (!cmd_REGISTER) {
        module_log("BUG: unable to find REGISTER command entry");
        exit_module(0);
        return 0;
    }
    cmd_REGISTER->help_param1 = s_NickServ;
    if (!CSEnableRegister) {
        cmd_REGISTER->has_priv = is_services_admin;
        cmd_REGISTER->helpmsg_oper = cmd_REGISTER->helpmsg_all;
        cmd_REGISTER->helpmsg_all = -1;
    }
    cmd_GETPASS = lookup_cmd(THIS_MODULE, "GETPASS");
    if (!cmd_GETPASS) {
        module_log("BUG: unable to find GETPASS command entry");
        exit_module(0);
        return 0;
    }
    if (!EnableGetpass)
        cmd_GETPASS->name = "";
    cmd = lookup_cmd(THIS_MODULE, "SET SECURE");
    if (cmd) {
        cmd->help_param1 = s_NickServ;
        cmd->help_param2 = s_NickServ;
    }
    cmd = lookup_cmd(THIS_MODULE, "SET SUCCESSOR");
    if (cmd)
        cmd->help_param1 = (char *)(long)CSMaxReg;
    cmd = lookup_cmd(THIS_MODULE, "SUSPEND");
    if (cmd)
        cmd->help_param1 = s_OperServ;

    if (!add_callback(NULL, "reconfigure", do_reconfigure)
     || !add_callback(NULL, "introduce_user", introduce_chanserv)
     || !add_callback(NULL, "m_privmsg", chanserv)
     || !add_callback(NULL, "m_whois", chanserv_whois)
     || !add_callback(NULL, "channel create", do_channel_create)
     || !add_callback(NULL, "channel JOIN check", do_channel_join_check)
     || !add_callback(NULL, "channel JOIN", do_channel_join)
     || !add_callback(NULL, "channel PART", do_channel_part)
     || !add_callback(NULL, "channel delete", do_channel_delete)
     || !add_callback(NULL, "channel mode change", do_channel_mode_change)
     || !add_callback(NULL, "channel umode change", do_channel_umode_change)
     || !add_callback(NULL, "channel TOPIC", do_channel_topic)
     || !add_callback(module_operserv, "STATS ALL", do_stats_all)
     || !add_callback(module_nickserv, "REGISTER/LINK check", do_reglink_check)
     || !add_callback(module_nickserv, "identified", do_nick_identified)
     || !add_callback(module_nickserv, "nickgroup delete", do_nickgroup_delete)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    if (!register_dbtable(&chan_dbtable)
     || !register_dbtable(&chan_access_dbtable)
     || !register_dbtable(&chan_akick_dbtable)
    ) {
        module_log("Unable to register database tables");
        exit_module(0);
        return 0;
    }

    if (!init_access() || !init_check() || !init_set()) {
        exit_module(0);
        return 0;
    }

    if (encrypt_all) {
        ChannelInfo *ci;
        int done = 0, already = 0, failed = 0;
        module_log("Re-encrypting passwords...");
        for (ci = first_channelinfo(); ci; ci = next_channelinfo()) {
            if ((EncryptionType && ci->founderpass.cipher
                 && strcmp(ci->founderpass.cipher, EncryptionType) == 0)
             || (!EncryptionType && !ci->founderpass.cipher)
            ) {
                already++;
            } else {
                char plainbuf[PASSMAX];
                Password newpass;
                int res;

                init_password(&newpass);
                res = decrypt_password(&ci->founderpass, plainbuf,
                                       sizeof(plainbuf));
                if (res != 0) {
                    failed++;
                } else {
                    res = encrypt_password(plainbuf,strlen(plainbuf),&newpass);
                    memset(plainbuf, 0, sizeof(plainbuf));
                    if (res != 0) {
                        failed++;
                    } else {
                        copy_password(&ci->founderpass, &newpass);
                        clear_password(&newpass);
                        done++;
                    }
                }
            }
        }
        module_log("%d passwords re-encrypted, %d already encrypted, %d"
                   " failed", done, already, failed);
    } /* if (encrypt_all) */

    if (linked)
        introduce_chanserv(NULL);

    strbcpy(old_clearchan_sender, set_clear_channel_sender(s_ChanServ));
    old_clearchan_sender_set = 1;

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (old_clearchan_sender_set) {
        set_clear_channel_sender(old_clearchan_sender);
        old_clearchan_sender_set = 0;
    }

    if (linked)
        send_cmd(s_ChanServ, "QUIT :");

    exit_set();
    exit_check();
    exit_access();

    unregister_dbtable(&chan_akick_dbtable);
    unregister_dbtable(&chan_access_dbtable);
    unregister_dbtable(&chan_dbtable);
    clean_dbtables();

    remove_callback(NULL, "channel TOPIC", do_channel_topic);
    remove_callback(NULL, "channel umode change", do_channel_umode_change);
    remove_callback(NULL, "channel mode change", do_channel_mode_change);
    remove_callback(NULL, "channel delete", do_channel_delete);
    remove_callback(NULL, "channel PART", do_channel_part);
    remove_callback(NULL, "channel JOIN", do_channel_join);
    remove_callback(NULL, "channel JOIN check", do_channel_join_check);
    remove_callback(NULL, "channel create", do_channel_create);
    remove_callback(NULL, "m_whois", chanserv_whois);
    remove_callback(NULL, "m_privmsg", chanserv);
    remove_callback(NULL, "introduce_user", introduce_chanserv);
    remove_callback(NULL, "reconfigure", do_reconfigure);

    cmd_GETPASS->name = "GETPASS";
    if (!CSEnableRegister) {
        cmd_REGISTER->helpmsg_all = cmd_REGISTER->helpmsg_oper;
        cmd_REGISTER->helpmsg_oper = -1;
        cmd_REGISTER->has_priv = NULL;
    }

    unregister_callback(cb_unban);
    unregister_callback(cb_invite);
    unregister_callback(cb_help_cmds);
    unregister_callback(cb_help);
    unregister_callback(cb_command);
    unregister_callback(cb_clear);

    if (protocol_features & PF_CHANPROT)
        unregister_commands(THIS_MODULE, cmds_chanprot);
    if (protocol_features & PF_HALFOP)
        unregister_commands(THIS_MODULE, cmds_halfop);
    unregister_commands(THIS_MODULE, cmds);
    del_commandlist(THIS_MODULE);

    if (module_nickserv) {
        remove_callback(module_nickserv, "nickgroup delete",
                        do_nickgroup_delete);
        remove_callback(module_nickserv, "identified", do_nick_identified);
        remove_callback(module_nickserv, "REGISTER/LINK check",
                        do_reglink_check);
        unuse_module(module_nickserv);
        module_nickserv = NULL;
    }

    if (module_operserv) {
        remove_callback(module_operserv, "STATS ALL", do_stats_all);
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
