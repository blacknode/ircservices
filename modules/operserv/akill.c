/* Autokill list module.
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
#include "language.h"
#include "timeout.h"

#include "modules/operserv/operserv.h"
#define NEED_MAKE_REASON
#include "modules/operserv/maskdata.h"
#include "modules/operserv/akill.h"

/*************************************************************************/


static Event* send_akill_event;
static Event* send_exclude_event;
static Event* cancel_akill_event;
static Event* cancel_exclude_event;

static char * AutokillReason;
static int    ImmediatelySendAutokill;
static time_t AutokillExpiry;
static time_t AkillChanExpiry;
static time_t OperMaxExpiry;
static int    WallOSAkill;
static int    WallAutokillExpire;

       int    EnableExclude; /* not static because main.c/do_help() needs it */
static time_t ExcludeExpiry;
static char * ExcludeReason;

static void do_akill(User *u);
static void do_akillchan(User *u);
static void do_exclude(User *u);

static Command cmds[] = {
    {"AKILL",     do_akill,     is_services_oper, OPER_HELP_AKILL,     -1,-1},
    {"AKILLCHAN", do_akillchan, is_services_oper, OPER_HELP_AKILLCHAN, -1,-1},
    {"EXCLUDE",   do_exclude,   is_services_oper, OPER_HELP_EXCLUDE,   -1,-1},
    { NULL }
};

/*************************************************************************/
/************************** Internal functions ***************************/
/*************************************************************************/

/* Send an autokill to the uplink server. */

static void send_akill(const MaskData *akill)
{
    char *username, *host;
    static int warned_exclude = 0;

    /* Don't send autokills if EnableExclude but no ircd support */
    if (EnableExclude && !(protocol_features & PF_AKILL_EXCL)) {
        if (!warned_exclude) {
            wallops(operserv_service.nick, "Warning: Autokill exclusions are enabled,"
                    " but this IRC server does not support autokill"
                    " exclusions; autokills will not be sent to servers.");
            module_log("EnableExclude on server type without exclusions--"
                       "autokill sending disabled");
            warned_exclude = 1;
        }
        return;
    } else {
        warned_exclude = 0;
    }

    username = sstrdup(akill->mask);
    host = strchr(username, '@');
    if (!host) {
        /* Glurp... this oughtn't happen, but if it does, let's not
         * play with null pointers.  Yell and bail out. */
        wallops(NULL, "Missing @ in autokill: %s", akill->mask);
        module_log("BUG: (send_akill) Missing @ in mask: %s", akill->mask);
        free(username);
        return;
    }
    *host++ = 0;
    event_emit(send_akill_event, username, host, akill->expires, akill->who,
                    make_reason(AutokillReason, akill));
    free(username);
}

/*************************************************************************/

/* Remove an autokill from the uplink server. */

static void cancel_akill(char *mask)
{
    char *s = strchr(mask, '@');
    if (s) {
        *s++ = 0;
        event_emit(cancel_akill_event, mask, s);
    } else {
        module_log("BUG: (cancel_akill) Missing @ in mask: %s", mask);
    }
}

/*************************************************************************/

/* Send an autokill exclusion to the uplink server. */

static void send_exclude(const MaskData *exclude)
{
    char *username, *host;

    username = sstrdup(exclude->mask);
    host = strchr(username, '@');
    if (!host) {
        wallops(NULL, "Missing @ in autokill exclusion: %s", exclude->mask);
        module_log("BUG: (send_exclude) Missing @ in mask: %s", exclude->mask);
        return;
    }
    *host++ = 0;
    event_emit(send_exclude_event, username, host, exclude->expires,
                    exclude->who, make_reason(ExcludeReason, exclude));
    free(username);
}

/*************************************************************************/

/* Remove an autokill exclusion from the uplink server. */

static void cancel_exclude(char *mask)
{
    char *s = strchr(mask, '@');
    if (s) {
        *s++ = 0;
        event_emit(cancel_exclude_event, mask, s);
    } else {
        module_log("BUG: (cancel_exclude) Missing @ in mask: %s", mask);
    }
}

/*************************************************************************/
/************************** External functions ***************************/
/*************************************************************************/

/* Does the user match any autokills?  Return 1 (and kill the user) if so,
 * else 0.
 */

static int do_user_check(int ac, char **av)
{
    const char *nick = av[0], *username = av[3], *host = av[4];
    char buf[BUFSIZE];
    MaskData *akill;

    if (noakill)
        return 0;

    snprintf(buf, sizeof(buf), "%s@%s", username, host);
    if ((akill = get_matching_maskdata(MD_AKILL, buf)) != NULL) {
        if (EnableExclude
         && put_maskdata(get_matching_maskdata(MD_EXCLUDE, buf))
        ) {
            put_maskdata(akill);
            return 0;
        }
        /* Don't use kill_user(); that's for people who have already
         * signed on.  This is called before the User structure is
         * created. */
        send_cmd(operserv_service.nick, "KILL %s :%s (%s)", nick, operserv_service.nick,
                 make_reason(AutokillReason, akill));
        send_akill(akill);
        time(&akill->lastused);
        put_maskdata(akill);
        return 1;
    }
    return 0;
}

/*************************************************************************/
/************************** AKILL list editing ***************************/
/*************************************************************************/

/* Note that all string parameters are assumed to be non-NULL; expiry must
 * be set to the time when the autokill should expire (0 for none).  Mask
 * is converted to lowercase on return.
 */

void create_akill(char *mask, const char *reason, const char *who,
                  time_t expiry)
{
    MaskData *akill;

    strlower(mask);
    if (maskdata_count(MD_AKILL) >= MAX_MASKDATA) {
        module_log("Attempt to add autokill to full list!");
        return;
    }
    akill = scalloc(1, sizeof(*akill));
    akill->mask = sstrdup(mask);
    akill->reason = sstrdup(reason);
    akill->time = time(NULL);
    akill->expires = expiry;
    strbcpy(akill->who, who);
    akill = add_maskdata(MD_AKILL, akill);
    if (ImmediatelySendAutokill)
        send_akill(akill);
}

/*************************************************************************/
/*************************************************************************/

/* Handle an OperServ AKILL command. */

/*************************************************************************/

static int check_add_akill(const User *u, uint8 type, char *mask,
                           time_t *expiry_ptr);
static void do_add_akill(const User *u, uint8 type, MaskData *md);
static void do_del_akill(const User *u, uint8 type, MaskData *md);

static MaskDataCmdInfo akill_cmd_info = {
    .name               = "AKILL",
    .md_type            = MD_AKILL,
    .def_expiry_ptr     = &AutokillExpiry,

    .msg_add_too_many   = OPER_TOO_MANY_AKILLS,
    .msg_add_exists     = OPER_AKILL_EXISTS,
    .msg_added          = OPER_AKILL_ADDED,
    .msg_del_not_found  = OPER_AKILL_NOT_FOUND,
    .msg_deleted        = OPER_AKILL_REMOVED,
    .msg_cleared        = OPER_AKILL_CLEARED,
    .msg_list_header    = OPER_AKILL_LIST_HEADER,
    .msg_list_empty     = OPER_AKILL_LIST_EMPTY,
    .msg_list_no_match  = OPER_AKILL_LIST_NO_MATCH,
    .msg_check_no_match = OPER_AKILL_CHECK_NO_MATCH,
    .msg_check_header   = OPER_AKILL_CHECK_HEADER,
    .msg_check_count    = OPER_AKILL_CHECK_TRAILER,
    .msg_count          = OPER_AKILL_COUNT,

    .mangle_mask        = (void (*))strlower,
    .check_add_mask     = check_add_akill,
    .do_add_mask        = do_add_akill,
    .do_del_mask        = do_del_akill,
    .do_unknown_cmd     = NULL,
};

/*************************************************************************/

static void do_akill(User *u)
{
    if (is_services_admin(u) || !OperMaxExpiry)
        akill_cmd_info.def_expiry_ptr = &AutokillExpiry;
    else
        akill_cmd_info.def_expiry_ptr = &OperMaxExpiry;
    do_maskdata_cmd(&akill_cmd_info, u);
}

/*************************************************************************/

static int check_add_akill(const User *u, uint8 type, char *mask,
                           time_t *expiry_ptr)
{
    char *s, *t;
    time_t len;

    if (strchr(mask, '!')) {
        notice_lang(operserv_service.nick, u, OPER_AKILL_NO_NICK);
        notice_lang(operserv_service.nick, u, BAD_USERHOST_MASK);
        return 0;
    }
    s = strchr(mask, '@');
    if (!s || s == mask || s[1] == 0) {
        notice_lang(operserv_service.nick, u, BAD_USERHOST_MASK);
        return 0;
    }

    /* Make sure mask is not too general. */
    *s++ = 0;
    if (strchr(mask,'*') != NULL && mask[strspn(mask,"*?")] == 0
     && ((t = strchr(mask,'?')) == NULL || strchr(t+1,'?') == NULL)
    ) {
        /* Username part matches anything; check host part */
        if (strchr(s,'*') != NULL && s[strspn(s,"*?.")] == 0
         && ((t = strchr(s,'.')) == NULL || strchr(t+1,'.') == NULL)
        ) {
            /* Hostname mask matches anything or nearly anything, so
             * disallow mask. */
            notice_lang(operserv_service.nick, u, OPER_AKILL_MASK_TOO_GENERAL);
            return 0;
        }
    }
    s[-1] = '@';        /* Replace "@" that we killed above */

    /* Check expiration limit for non-servadmins. */
    len = *expiry_ptr - time(NULL);
    if (OperMaxExpiry && !is_services_admin(u)
     && (!*expiry_ptr || len > OperMaxExpiry)
    ) {
        notice_lang(operserv_service.nick, u, OPER_AKILL_EXPIRY_LIMITED,
                    maketime(u->ngi, OperMaxExpiry, MT_DUALUNIT));
        return 0;
    }

    return 1;
}

/*************************************************************************/

static void do_add_akill(const User *u, uint8 type, MaskData *md)
{
    if (WallOSAkill) {
        char buf[BUFSIZE];
        expires_in_lang(buf, sizeof(buf), NULL, md->expires);
        wallops(operserv_service.nick, "%s added an autokill for \2%s\2 (%s)",
                u->nick, md->mask, buf);
    }
    if (ImmediatelySendAutokill)
        send_akill(md);
}

/*************************************************************************/

static void do_del_akill(const User *u, uint8 type, MaskData *md)
{
    cancel_akill(md->mask);
}

/*************************************************************************/
/*************************************************************************/

/* Handle an OperServ EXCLUDE command. */

/*************************************************************************/

static int check_add_exclude(const User *u, uint8 type, char *mask,
                             time_t *expiry_ptr);
static void do_add_exclude(const User *u, uint8 type, MaskData *md);
static void do_del_exclude(const User *u, uint8 type, MaskData *md);

static MaskDataCmdInfo exclude_cmd_info = {
    .name               = "EXCLUDE",
    .md_type            = MD_EXCLUDE,
    .def_expiry_ptr     = &ExcludeExpiry,

    .msg_add_too_many   = OPER_TOO_MANY_EXCLUDES,
    .msg_add_exists     = OPER_EXCLUDE_EXISTS,
    .msg_added          = OPER_EXCLUDE_ADDED,
    .msg_del_not_found  = OPER_EXCLUDE_NOT_FOUND,
    .msg_deleted        = OPER_EXCLUDE_REMOVED,
    .msg_cleared        = OPER_EXCLUDE_CLEARED,
    .msg_list_header    = OPER_EXCLUDE_LIST_HEADER,
    .msg_list_empty     = OPER_EXCLUDE_LIST_EMPTY,
    .msg_list_no_match  = OPER_EXCLUDE_LIST_NO_MATCH,
    .msg_check_no_match = OPER_EXCLUDE_CHECK_NO_MATCH,
    .msg_check_header   = OPER_EXCLUDE_CHECK_HEADER,
    .msg_check_count    = OPER_EXCLUDE_CHECK_TRAILER,
    .msg_count          = OPER_EXCLUDE_COUNT,

    .mangle_mask        = (void (*))strlower,
    .check_add_mask     = check_add_exclude,
    .do_add_mask        = do_add_exclude,
    .do_del_mask        = do_del_exclude,
    .do_unknown_cmd     = NULL,
};

/*************************************************************************/

static void do_exclude(User *u)
{
    if (is_services_admin(u) || !OperMaxExpiry)
        exclude_cmd_info.def_expiry_ptr = &AutokillExpiry;
    else
        exclude_cmd_info.def_expiry_ptr = &OperMaxExpiry;
    do_maskdata_cmd(&exclude_cmd_info, u);
}

/*************************************************************************/

static int check_add_exclude(const User *u, uint8 type, char *mask,
                             time_t *expiry_ptr)
{
    char *s;

    s = strchr(mask, '@');
    if (!s || s == mask || s[1] == 0) {
        notice_lang(operserv_service.nick, u, BAD_USERHOST_MASK);
        return 0;
    }
    return 1;
}

/*************************************************************************/

static void do_add_exclude(const User *u, uint8 type, MaskData *md)
{
    if (WallOSAkill) {
        char buf[BUFSIZE];
        expires_in_lang(buf, sizeof(buf), NULL, md->expires);
        wallops(operserv_service.nick, "%s added an EXCLUDE for \2%s\2 (%s)",
                u->nick, md->mask, buf);
    }
    send_exclude(md);
}

/*************************************************************************/

static void do_del_exclude(const User *u, uint8 type, MaskData *md)
{
    cancel_exclude(md->mask);
}

/*************************************************************************/
/*************************************************************************/

/* Handle an OperServ AKILLCHAN command. */

/*************************************************************************/

static void do_akillchan(User *u)
{
    char *channel, *expiry_str, *reason, *s;
    int kill;  /* kill users in the channel? */
    int32 expiry;
    Channel *c;
    struct c_userlist *cu, *cu2;
    int count;
    int old_immed;  /* saved value of ImmediatelySendAutokill */

    kill = 0;
    expiry_str = NULL;

    s = strtok(NULL, " ");
    if (s && stricmp(s,"KILL") == 0) {
        kill = 1;
        s = strtok(NULL, " ");
    }
    if (s && *s == '+') {
        expiry_str = s+1;
        s = strtok(NULL, " ");
    }
    if (!s || *s != '#') {
        syntax_error(operserv_service.nick, u, "AKILLCHAN", OPER_AKILLCHAN_SYNTAX);
        return;
    }
    channel = s;
    reason = strtok_remaining();
    if (!reason) {
        syntax_error(operserv_service.nick, u, "AKILLCHAN", OPER_AKILLCHAN_SYNTAX);
        return;
    }

    if (!(c = get_channel(channel))) {
        notice_lang(operserv_service.nick, u, CHAN_X_NOT_IN_USE, channel);
        return;
    }
    if (expiry_str) {
        expiry = dotime(expiry_str);
    } else {
        if (!is_services_admin(u) && OperMaxExpiry
         && (!AkillChanExpiry || OperMaxExpiry < AkillChanExpiry)
        ) {
            expiry = OperMaxExpiry;
        } else {
            expiry = AkillChanExpiry;
        }
    }
    if (expiry)
        expiry += time(NULL);

    if (WallOSAkill)
        wallops(operserv_service.nick, "%s used AKILLCHAN for \2%s\2", u->nick, c->name);

    count = 0;
    old_immed = ImmediatelySendAutokill;
    ImmediatelySendAutokill = 1;
    LIST_FOREACH_SAFE (cu, c->users, cu2) {
        char buf[BUFSIZE];
        if (is_oper(cu->user))
            continue;
        /* Killing the user before adding the autokill opens a small hole
         * in that the user may be able to reconnect before the new
         * autokill reaches the server, but under normal conditions the
         * actual chance of this is vanishingly small.  On the other hand,
         * the chance of people complaining about "user not found" errors
         * from ircds that kill users on autokill if we do the autokill
         * first is significantly greater... */
        /* But as long as you kill the user first, make sure you save the
         * hostname _before_ it gets freed, idiot. */
        snprintf(buf, sizeof(buf), "*@%s", cu->user->host);
        if (kill)
            kill_user(operserv_service.nick, cu->user->nick, reason);
        if (!put_maskdata(get_maskdata(MD_AKILL, buf)))
            create_akill(buf, reason, u->nick, expiry);
        count++;
    }
    ImmediatelySendAutokill = old_immed;

    if (count == 1) {
        notice_lang(operserv_service.nick, u,
                    kill ? OPER_AKILLCHAN_KILLED_ONE
                         : OPER_AKILLCHAN_AKILLED_ONE);
    } else {
        notice_lang(operserv_service.nick, u,
                    kill ? OPER_AKILLCHAN_KILLED : OPER_AKILLCHAN_AKILLED,
                    count);
    }
}

/*************************************************************************/
/******************************* Callbacks *******************************/
/*************************************************************************/

/* Callback on connection to uplink server. */

static int do_connect(void)
{
    if (ImmediatelySendAutokill) {
        MaskData *akill;
        for (akill = first_maskdata(MD_AKILL); akill;
             akill = next_maskdata(MD_AKILL)
        ) {
            send_akill(akill);
        }
    }
    return 0;
}

/*************************************************************************/

/* Handler for autokill expiration. */

static int expire_omitted_count = 0;
static time_t expire_last_time = 0;
static Timeout *expire_to = NULL;
static void expire_ratelimit_timeout(Timeout *to);

static int do_expire_maskdata(uint32 type, MaskData *md)
{
    if (type == MD_AKILL) {
        if (WallAutokillExpire) {
            if (time(NULL) == expire_last_time) {
                if (expire_to)
                    del_timeout(expire_to);
                expire_to = add_timeout_ms(1500, expire_ratelimit_timeout, 0);
                expire_omitted_count++;
            } else {
                wallops(operserv_service.nick, "Autokill on %s has expired", md->mask);
            }
            expire_last_time = time(NULL);
        }
        cancel_akill(md->mask);
        return 1;
    }
    return 0;
}

static void expire_ratelimit_timeout(Timeout *to)
{
    wallops(operserv_service.nick, "%d more autokill%s ha%s expired",
            expire_omitted_count, expire_omitted_count==1 ? "" : "s",
            expire_omitted_count==1 ? "s" : "ve");
    expire_omitted_count = 0;
    expire_last_time = 0;
    expire_to = NULL;
}

/*************************************************************************/

/* Callback to display help text for HELP AKILL. */

static int do_help(User *u, const char *param)
{
    if (stricmp(param, "AKILL") == 0) {
        notice_help(operserv_service.nick, u, OPER_HELP_AKILL);
        if (OperMaxExpiry)
            notice_help(operserv_service.nick, u, OPER_HELP_AKILL_OPERMAXEXPIRY,
                        maketime(u->ngi, OperMaxExpiry, MT_DUALUNIT));
        notice_help(operserv_service.nick, u, OPER_HELP_AKILL_END);
        return 1;
    } else if (stricmp(param, "AKILLCHAN") == 0) {
        notice_help(operserv_service.nick, u, OPER_HELP_AKILLCHAN,
                    maketime(u->ngi, AkillChanExpiry, 0));
        return 1;
    }
    return 0;
}

/*************************************************************************/

static int do_stats_all(User *user, const char *operserv_nick)
{
    int32 count, mem;
    MaskData *md;

    count = mem = 0;
    for (md = first_maskdata(MD_AKILL); md; md = next_maskdata(MD_AKILL)) {
        count++;
        mem += sizeof(*md);
        if (md->mask)
            mem += strlen(md->mask)+1;
        if (md->reason)
            mem += strlen(md->reason)+1;
    }
    for (md = first_maskdata(MD_EXCLUDE); md;
         md = next_maskdata(MD_EXCLUDE)
    ) {
        count++;
        mem += sizeof(*md);
        if (md->mask)
            mem += strlen(md->mask)+1;
        if (md->reason)
            mem += strlen(md->reason)+1;
    }
    notice_lang(operserv_nick, user, OPER_STATS_ALL_AKILL_MEM,
                count, (mem+512) / 1024);

    return 0;
}

/*************************************************************************/
/**************************** Database stuff *****************************/
/*************************************************************************/

static void insert_akill(void *md)   { add_maskdata(MD_AKILL, md); }
static void *first_akill(void)       { return first_maskdata(MD_AKILL); }
static void *next_akill(void)        { return next_maskdata(MD_AKILL); }

static void insert_exclude(void *md) { add_maskdata(MD_EXCLUDE, md); }
static void *first_exclude(void)     { return first_maskdata(MD_EXCLUDE); }
static void *next_exclude(void)      { return next_maskdata(MD_EXCLUDE); }

static DBField akill_exclude_dbfields[] = {
    { "mask",     DBTYPE_STRING, offsetof(MaskData,mask) },
    { "reason",   DBTYPE_STRING, offsetof(MaskData,reason) },
    { "who",      DBTYPE_BUFFER, offsetof(MaskData,who), NICKMAX },
    { "time",     DBTYPE_TIME,   offsetof(MaskData,time) },
    { "expires",  DBTYPE_TIME,   offsetof(MaskData,expires) },
    { "lastused", DBTYPE_TIME,   offsetof(MaskData,lastused) },
    { NULL }
};
static DBTable akill_dbtable = {
    .name    = "akill",
    .newrec  = new_maskdata,
    .freerec = free_maskdata,
    .insert  = insert_akill,
    .first   = first_akill,
    .next    = next_akill,
    .fields  = akill_exclude_dbfields,
};
static DBTable exclude_dbtable = {
    .name    = "exclude",
    .newrec  = new_maskdata,
    .freerec = free_maskdata,
    .insert  = insert_exclude,
    .first   = first_exclude,
    .next    = next_exclude,
    .fields  = akill_exclude_dbfields,
};

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static ConfigDirective akill_config[] = {
    { "AkillChanExpiry",  { { CD_TIME, 0, &AkillChanExpiry } } },
    { "AutokillExpiry",   { { CD_TIME, 0, &AutokillExpiry } } },
    { "AutokillReason",   { { CD_STRING, CF_DIRREQ, &AutokillReason } } },
    { "EnableExclude",    { { CD_SET, 0, &EnableExclude } } },
    { "ExcludeExpiry",    { { CD_TIME, 0, &ExcludeExpiry } } },
    { "ExcludeReason",    { { CD_STRING, 0, &ExcludeReason } } },
    { "ImmediatelySendAutokill",{{CD_SET, 0, &ImmediatelySendAutokill } } },
    { "OperMaxExpiry",    { { CD_TIME, 0, &OperMaxExpiry } } },
    { "WallAutokillExpire",{{ CD_SET, 0, &WallAutokillExpire } } },
    { "WallOSAkill",      { { CD_SET, 0, &WallOSAkill } } },
    { NULL }
};

/* Pointer to EXCLUDE command record (for EnableExclude) */
static Command *cmd_EXCLUDE;

/*************************************************************************/

static void akill_rehash(Module *module)
{
    if (EnableExclude && !ExcludeReason) {
        module_log("EXCLUDE enabled but ExcludeReason not set; disabling"
                   " EXCLUDE");
        EnableExclude = 0;
    }
    if (EnableExclude)
        cmd_EXCLUDE->name = "EXCLUDE";
    else
        cmd_EXCLUDE->name = "";
}

/*************************************************************************/

/* AKILL, EXCLUDE and AKILLCHAN go into OperServ's command list. */

static int akill_init(Module *module)
{
    Module *operserv = module_find("operserv/main");

    if (EnableExclude && !ExcludeReason) {
        module_log("EXCLUDE enabled but ExcludeReason not set");
        return 0;
    }

    if (!register_commands(operserv, cmds)) {
        module_log("Unable to register commands");
        return 0;
    }
    cmd_EXCLUDE = lookup_cmd(operserv, "EXCLUDE");
    if (!cmd_EXCLUDE) {
        module_log("BUG: unable to find EXCLUDE command entry");
        return 0;
    }
    if (!EnableExclude)
        cmd_EXCLUDE->name = "";

    send_akill_event = event_declare(module, AKILL_EVENT_SEND_AKILL);
    send_exclude_event = event_declare(module, AKILL_EVENT_SEND_EXCLUDE);
    cancel_akill_event = event_declare(module, AKILL_EVENT_CANCEL_AKILL);
    cancel_exclude_event = event_declare(module, AKILL_EVENT_CANCEL_EXCLUDE);
    if (!send_akill_event || !send_exclude_event || !cancel_akill_event
     || !cancel_exclude_event
    ) {
        module_log("Unable to declare events");
        return 0;
    }

    if (!event_attach(module, EVENT_UPLINK_LINKED, do_connect)
     || !event_attach(module, EVENT_USER_CHECK, do_user_check)
     || !event_attach(module, OPERSERV_EVENT_EXPIRE_MASKDATA,
                      do_expire_maskdata)
     || !event_attach(module, OPERSERV_EVENT_HELP, do_help)
     || !event_attach(module, OPERSERV_EVENT_STATS_ALL, do_stats_all)
    ) {
        module_log("Unable to attach event handlers");
        return 0;
    }

    if (!register_dbtable(&akill_dbtable)
     || !register_dbtable(&exclude_dbtable)
    ) {
        module_log("Unable to register database tables");
        return 0;
    }

    return 1;
}

/*************************************************************************/

static int akill_fini(Module *module, int shutdown)
{
    unregister_dbtable(&exclude_dbtable);
    unregister_dbtable(&akill_dbtable);
    unregister_commands(module_find("operserv/main"), cmds);
    if (cmd_EXCLUDE) {
        cmd_EXCLUDE->name = "EXCLUDE";
        cmd_EXCLUDE = NULL;
    }
    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "OperServ AKILL: network-wide bans (G-lines)",
    .requires = MODULE_REQUIRES("operserv/main"),
    .config = akill_config,
    .init = akill_init,
    .fini = akill_fini,
    .rehash = akill_rehash,
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
