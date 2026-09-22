/* Main OperServ module.
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
#include "databases.h"
#include "language.h"
#include "commands.h"
#include "timeout.h"
#include "encrypt.h"
#include "modules/nickserv/nickserv.h"
#ifdef DEBUG_COMMANDS
# include "modules/chanserv/chanserv.h"  /* for send_user_info */
#endif

#include "operserv.h"
#include "maskdata.h"
#include "akill.h"
#include "news.h"

/* Maximum depth for SERVERMAP command */
#define MAXMAPLEVEL     64

/*************************************************************************/

static Module *module_akill;
static Module *module_nickserv;

       char *s_OperServ;
static char *desc_OperServ;
       char *s_GlobalNoticer;
static char *desc_GlobalNoticer;
EXPORT_VAR(char *,s_OperServ)
EXPORT_VAR(char *,s_GlobalNoticer)

       char * ServicesRoot;
static int    WallOper;
static int    WallBadOS;
static int    WallOSChannel;
static int    WallSU;
static int    KillClonesAutokill;
static time_t KillClonesAutokillExpire;
static int    AllowRaw;
EXPORT_VAR(char *,ServicesRoot)

static int cb_command   = -1;
static int cb_help      = -1;
static int cb_help_cmds = -1;
static int cb_set       = -1;
static int cb_stats     = -1;
static int cb_stats_all = -1;

/* Non-volatile data (saved to OperServ database) */
static struct {
    /* Maximum user count and time (copied to global variables) */
    uint32 maxusercnt;
    time_t maxusertime;
    /* Services super-user password */
    Password supass;
    /* Is the password currently unset? */
    int8 no_supass;
} operserv_data;

/* Flag indicating whether a REHASH command is currently being processed;
 * used to prevent the module from being unloaded as the result of a rehash */
static int in_rehash = 0;

/*************************************************************************/

static void do_help(User *u);
static void do_global(User *u);
static void do_stats(User *u);
static void do_servermap(User *u);
static void do_admin(User *u);
static void do_oper(User *u);
static void do_getkey(User *u);
static void do_os_mode(User *u);
static void do_clearmodes(User *u);
static void do_clearchan(User *u);
static void do_os_kick(User *u);
static void do_su(User *u);
static void do_set(User *u);
static void do_jupe(User *u);
static void do_raw(User *u);
static void do_update(User *u);
static void do_os_quit(User *u);
static void do_shutdown(User *u);
static void do_restart(User *u);
static void do_rehash(User *u);
static void do_killclones(User *u);

#ifdef DEBUG_COMMANDS
static void send_server_list(User *u);
static void send_channel_list(User *u);
static void send_channel_users(User *u);
static void send_user_list(User *u);
static void send_user_info(User *u);
static void do_matchwild(User *u);
static void do_setcmode(User *u);
static void do_monitor_ignore(User *u);
static void do_getstring(User *u);
static void do_setstring(User *u);
static void do_mapstring(User *u);
static void do_addstring(User *u);
#endif

/*************************************************************************/

static Command cmds[] = {
    {"HELP",      do_help,      NULL,             -1,                  -1,-1},
    {"GLOBAL",    do_global,    NULL,             OPER_HELP_GLOBAL,    -1,-1},
    {"STATS",     do_stats,     NULL,             OPER_HELP_STATS,     -1,-1},
    {"UPTIME",    do_stats,     NULL,             OPER_HELP_STATS,     -1,-1},
    {"SERVERMAP", do_servermap, NULL,             OPER_HELP_SERVERMAP, -1,-1},
    /* Anyone can use the LIST option to the ADMIN and OPER commands; those
     * routines check privileges to ensure that only authorized users
     * modify the list. */
    {"ADMIN",     do_admin,     NULL,             OPER_HELP_ADMIN,     -1,-1},
    {"OPER",      do_oper,      NULL,             OPER_HELP_OPER,      -1,-1},

    /* Commands for Services opers: */
    {"GETKEY",    do_getkey,    is_services_oper, OPER_HELP_GETKEY,    -1,-1},
    {"MODE",      do_os_mode,   is_services_oper, OPER_HELP_MODE,      -1,-1},
    {"KICK",      do_os_kick,   is_services_oper, OPER_HELP_KICK,      -1,-1},
    {"CLEARMODES",do_clearmodes,is_services_oper, OPER_HELP_CLEARMODES,-1,-1},
    {"CLEARCHAN", do_clearchan, is_services_oper, OPER_HELP_CLEARCHAN, -1,-1},
    {"KILLCLONES",do_killclones,is_services_oper, OPER_HELP_KILLCLONES,-1,-1},

    /* Commands for Services admins: */
    {"SU",        do_su,        NULL,             OPER_HELP_SU,        -1,-1},
    {"SET",       do_set,       is_services_admin,OPER_HELP_SET,       -1,-1},
    {"SET READONLY",NULL,       NULL,           OPER_HELP_SET_READONLY,-1,-1},
    {"SET DEBUG", NULL,         NULL,             OPER_HELP_SET_DEBUG, -1,-1},
    {"SET SUPASS",NULL,         NULL,             OPER_HELP_SET_SUPASS,-1,-1},
    {"JUPE",      do_jupe,      is_services_admin,OPER_HELP_JUPE,      -1,-1},
    {"UPDATE",    do_update,    is_services_admin,OPER_HELP_UPDATE,    -1,-1},
    {"QUIT",      do_os_quit,   is_services_admin,OPER_HELP_QUIT,      -1,-1},
    {"SHUTDOWN",  do_shutdown,  is_services_admin,OPER_HELP_SHUTDOWN,  -1,-1},
    {"RESTART",   do_restart,   is_services_admin,OPER_HELP_RESTART,   -1,-1},
    {"REHASH",    do_rehash,    is_services_admin,OPER_HELP_REHASH,    -1,-1,
                IRCSERVICES_CONF },

    /* Commands for Services super-user: */
    {"RAW",       do_raw,       is_services_root, OPER_HELP_RAW,       -1,-1},

#ifdef DEBUG_COMMANDS
    {"LISTSERVERS", send_server_list,   is_services_root, -1,-1,-1},
    {"LISTCHANS",   send_channel_list,  is_services_root, -1,-1,-1},
    {"LISTCHAN",    send_channel_users, is_services_root, -1,-1,-1},
    {"LISTUSERS",   send_user_list,     is_services_root, -1,-1,-1},
    {"LISTUSER",    send_user_info,     is_services_root, -1,-1,-1},
    {"LISTTIMERS",  send_timeout_list,  is_services_root, -1,-1,-1},
    {"MATCHWILD",   do_matchwild,       is_services_root, -1,-1,-1},
    {"SETCMODE",    do_setcmode,        is_services_root, -1,-1,-1},
    {"MONITOR-IGNORE",do_monitor_ignore,is_services_root, -1,-1,-1},
    {"GETSTRING",   do_getstring,       is_services_root, -1,-1,-1},
    {"SETSTRING",   do_setstring,       is_services_root, -1,-1,-1},
    {"MAPSTRING",   do_mapstring,       is_services_root, -1,-1,-1},
    {"ADDSTRING",   do_addstring,       is_services_root, -1,-1,-1},
#endif

    /* Fencepost: */
    { NULL }
};

/*************************************************************************/
/**************************** Database stuff *****************************/
/*************************************************************************/

/* Record access routines; we only store one record to the database */

static void *new_operserv_data(void)           { return &operserv_data; }
static void free_operserv_data(void *record)   { /* nothing */          }
static void insert_operserv_data(void *record) { /* nothing */          }
static void *first_operserv_data(void)         { return &operserv_data; }
static void *next_operserv_data(void)          { return NULL;           }

/*************************************************************************/

/* Routines to allow external access to OperServ data (for xml-import and
 * xml-export) */

EXPORT_FUNC(get_operserv_data)
int get_operserv_data(int what, void *ret)
{
    switch (what) {
      case OSDATA_MAXUSERCNT:
        *((int32 *)ret) = operserv_data.maxusercnt;
        return 1;
      case OSDATA_MAXUSERTIME:
        *((time_t *)ret) = operserv_data.maxusertime;
        return 1;
      case OSDATA_SUPASS:
        if (operserv_data.no_supass)
            *((Password **)ret) = NULL;
        else
            *((Password **)ret) = &operserv_data.supass;
        return 1;
    }
    return 0;
}


EXPORT_FUNC(put_operserv_data)
int put_operserv_data(int what, void *ptr)
{
    switch (what) {
      case OSDATA_MAXUSERCNT:
        operserv_data.maxusercnt = *((int32 *)ptr);
        return 1;
      case OSDATA_MAXUSERTIME:
        operserv_data.maxusertime = *((time_t *)ptr);
        return 1;
      case OSDATA_SUPASS:
        if (ptr) {
            operserv_data.no_supass = 0;
            copy_password(&operserv_data.supass, ptr);
        } else {
            operserv_data.no_supass = 1;
            clear_password(&operserv_data.supass);
        }
        return 1;
    }
    return 0;
}

/*************************************************************************/

/* Free all memory used by database tables. */

static void clean_dbtables(void)
{
    clear_password(&operserv_data.supass);
}

/*************************************************************************/

/* OperServ database table info */
static DBField oper_dbfields[] = {
    { "maxusercnt",  DBTYPE_INT32,
          offsetof(typeof(operserv_data),maxusercnt) },
    { "maxusertime", DBTYPE_TIME,
          offsetof(typeof(operserv_data),maxusertime) },
    { "supass",      DBTYPE_PASSWORD,
          offsetof(typeof(operserv_data),supass) },
    { "no_supass",   DBTYPE_INT8,
          offsetof(typeof(operserv_data),no_supass) },
    { NULL }
};
static DBTable oper_dbtable = {
    .name    = "oper",
    .newrec  = new_operserv_data,
    .freerec = free_operserv_data,
    .insert  = insert_operserv_data,
    .first   = first_operserv_data,
    .next    = next_operserv_data,
    .fields  = oper_dbfields,
};

/*************************************************************************/
/*************************** NickServ imports ****************************/
/*************************************************************************/

/* This is somewhat inefficient, but since these aren't called often they
 * should do for the time being.
 */

#define get_nickinfo local_get_nickinfo
#define put_nickinfo local_put_nickinfo
#define _get_ngi local__get_ngi
#define put_nickgroupinfo local_put_nickgroupinfo
#define first_nickgroupinfo local_first_nickgroupinfo
#define next_nickgroupinfo local_next_nickgroupinfo

static NickInfo *local_get_nickinfo(const char *nick)
{
    typeof(get_nickinfo) *p_get_nickinfo;

    if (!module_nickserv)
        return NULL;
    p_get_nickinfo = get_module_symbol(module_nickserv, "get_nickinfo");
    if (!p_get_nickinfo)
        return NULL;
    return p_get_nickinfo(nick);
}

static NickInfo *local_put_nickinfo(NickInfo *ni)
{
    typeof(put_nickinfo) *p_put_nickinfo;

    if (!module_nickserv)
        return NULL;
    p_put_nickinfo = get_module_symbol(module_nickserv, "put_nickinfo");
    if (!p_put_nickinfo)
        return NULL;
    return p_put_nickinfo(ni);
}

static NickGroupInfo *local__get_ngi(const NickInfo *ni, const char *file,
                                     int line)
{
    typeof(_get_ngi) *p__get_ngi;

    if (!module_nickserv)
        return NULL;
    if (!check_module_symbol(module_nickserv, "_get_ngi",
                             (void **)&p__get_ngi, NULL)) {
        module_log("Unable to find symbol `_get_ngi' in module"
                   " `nickserv/main' (called from %s:%d)", file, line);
        return NULL;
    }
    return p__get_ngi(ni, file, line);
}

static NickGroupInfo *local_put_nickgroupinfo(NickGroupInfo *ngi)
{
    typeof(put_nickgroupinfo) *p_put_nickgroupinfo;

    if (!module_nickserv)
        return NULL;
    p_put_nickgroupinfo =
        get_module_symbol(module_nickserv, "put_nickgroupinfo");
    if (!p_put_nickgroupinfo)
        return NULL;
    return p_put_nickgroupinfo(ngi);
}

static NickGroupInfo *local_first_nickgroupinfo(void)
{
    typeof(first_nickgroupinfo) *p_first_nickgroupinfo;

    if (!module_nickserv)
        return NULL;
    p_first_nickgroupinfo =
        get_module_symbol(module_nickserv, "first_nickgroupinfo");
    if (!p_first_nickgroupinfo)
        return NULL;
    return p_first_nickgroupinfo();
}

static NickGroupInfo *local_next_nickgroupinfo(void)
{
    typeof(next_nickgroupinfo) *p_next_nickgroupinfo;

    if (!module_nickserv)
        return NULL;
    p_next_nickgroupinfo =
        get_module_symbol(module_nickserv, "next_nickgroupinfo");
    if (!p_next_nickgroupinfo)
        return NULL;
    return p_next_nickgroupinfo();
}

/*************************************************************************/
/***************************** Main routines *****************************/
/*************************************************************************/

/* Introduce the OperServ pseudoclient and related clients. */

static int introduce_operserv(const char *nick)
{
    if (!nick || irc_stricmp(nick, s_OperServ) == 0) {
        send_pseudo_nick(s_OperServ, desc_OperServ, PSEUDO_OPER|PSEUDO_INVIS);
        if (nick)
            return 1;
    }
    if (!nick || irc_stricmp(nick, s_GlobalNoticer) == 0) {
        send_pseudo_nick(s_GlobalNoticer, desc_GlobalNoticer,
                         PSEUDO_OPER|PSEUDO_INVIS);
        if (nick)
            return 1;
    }
    return 0;
}

/*************************************************************************/

/* Main OperServ routine. */

static int operserv(const char *source, const char *target, char *buf)
{
    char *cmd;
    const char *s;
    User *u = get_user(source);

    if (irc_stricmp(target, s_OperServ) != 0)
        return 0;

    if (!u) {
        module_log("user record for %s not found", source);
        notice(s_OperServ, source, "Access denied.");
        if (WallBadOS)
            wallops(s_OperServ, "Denied access to %s from %s (user record"
                                " missing)", s_OperServ, source);
        return 1;
    }

    if (!is_oper(u)) {
        notice_lang(s_OperServ, u, ACCESS_DENIED);
        if (WallBadOS)
            wallops(s_OperServ, "Denied access to %s from %s (non-oper)",
                    s_OperServ, source);
        module_log("Non-oper %s!%s@%s sent: %s",
                   u->nick, u->username, u->host, buf);
        return 1;
    }

    /* Don't log stuff that might be passwords */
    if (strnicmp(buf, "SU ", 3) == 0) {
        module_log("%s: SU xxxxxx", source);
    } else if (strnicmp(buf, "SET ", 4) == 0
               && (s = stristr(buf, "SUPASS")) != NULL
               && strspn(buf+4, " ") == s-(buf+4)) {
        /* All that was needed to make sure someone doesn't fool us with
         * "SET READONLY ON SUPASS".  Which wouldn't work anyway, but we
         * ought to log it properly... */
        module_log("%s: SET SUPASS xxxxxx", source);
    } else {
        module_log("%s: %s", source, buf);
    }

    cmd = strtok(buf, " ");
    if (!cmd) {
        return 1;
    } else if (stricmp(cmd, "\1PING") == 0) {
        if (!(s = strtok_remaining()))
            s = "\1";
        notice(s_OperServ, source, "\1PING %s", s);
    } else {
        if (call_callback_2(cb_command, u, cmd) <= 0)
            run_cmd(s_OperServ, u, THIS_MODULE, cmd);
    }

    return 1;
}

/*************************************************************************/

/* Return a /WHOIS response for OperServ or the global noticer. */

static int operserv_whois(const char *source, char *who, char *extra)
{
    if (irc_stricmp(who, s_OperServ) == 0) {
        send_cmd(ServerName, "311 %s %s %s %s * :%s", source, who,
                 ServiceUser, ServiceHost, desc_OperServ);
    } else if (irc_stricmp(who, s_GlobalNoticer) == 0) {
        send_cmd(ServerName, "311 %s %s %s %s * :%s", source, who,
                 ServiceUser, ServiceHost, desc_GlobalNoticer);
    } else {
        return 0;
    }
    send_cmd(ServerName, "312 %s %s %s :%s", source, who,
             ServerName, ServerDesc);
    send_cmd(ServerName, "313 %s %s :is a network service", source, who);
    send_cmd(ServerName, "318 %s %s End of /WHOIS response.", source, who);
    return 1;
}

/*************************************************************************/
/**************************** Privilege checks ***************************/
/*************************************************************************/

/* Does the given user have Services super-user privileges? */

EXPORT_FUNC(is_services_root)
int is_services_root(const User *u)
{
    NickInfo *ni;
    int rootid;
    static int warned_ni = 0, warned_id = 0;

    if (u->flags & UF_SERVROOT)
        return 1;
    if (!(ni = get_nickinfo(ServicesRoot))) {
        if (!warned_ni) {
            wallops(s_OperServ, "Warning: Services super-user nickname %s"
                    " is not registered", ServicesRoot);
            warned_ni = 1;
        }
        module_log("warning: ServicesRoot nickname not registered");
        return 0;
    }
    warned_ni = 0;
    rootid = ni->nickgroup;
    put_nickinfo(ni);
    if (!rootid) {
        if (!warned_id) {
            wallops(s_OperServ, "Warning: Services super-user nickname %s"
                    " is forbidden or not properly registered", ServicesRoot);
            warned_id = 1;
        }
        module_log("warning: ServicesRoot nickname forbidden or registered"
                   " data corrupt");
        return 0;
    }
    if (!is_oper(u) || !u->ni || u->ni->nickgroup != rootid)
        return 0;
    if (user_identified(u))
        return 1;
    return 0;
}

/*************************************************************************/

/* Does the given user have Services admin privileges? */

EXPORT_FUNC(is_services_admin)
int is_services_admin(const User *u)
{
    if (!is_oper(u) || !user_identified(u))
        return 0;
    if (is_services_root(u))
        return 1;
    return u->ngi && u->ngi != NICKGROUPINFO_INVALID
                  && u->ngi->os_priv >= NP_SERVADMIN;
}

/*************************************************************************/

/* Does the given user have Services oper privileges? */

EXPORT_FUNC(is_services_oper)
int is_services_oper(const User *u)
{
    if (!is_oper(u) || !user_identified(u))
        return 0;
    if (is_services_root(u))
        return 1;
    return u->ngi && u->ngi != NICKGROUPINFO_INVALID
                  && u->ngi->os_priv >= NP_SERVOPER;
}

/*************************************************************************/

/* Is the given nick a Services admin/root nick? */

/* NOTE: Do not use this to check if a user who is online is a services
 * admin or root. This function only checks if a user has the ABILITY to be
 * a services admin. Rather use is_services_admin(User *u). -TheShadow */

EXPORT_FUNC(nick_is_services_admin)
int nick_is_services_admin(const NickInfo *ni)
{
    NickGroupInfo *ngi;
    NickInfo *rootni;
    uint32 rootid = 0;
    int isroot, isadmin;

    if (!ni || !ni->nickgroup || !(ngi = get_ngi(ni)))
        return 0;
    if ((rootni = get_nickinfo(ServicesRoot)) != NULL) {
        rootid = rootni->nickgroup;
        put_nickinfo(rootni);
    }
    isroot = (rootid != 0 && ni->nickgroup == rootid);
    isadmin = (ngi->os_priv >= NP_SERVADMIN);
    put_nickgroupinfo(ngi);
    return isroot || isadmin;
}

/*************************************************************************/
/********************** Admin/oper list modification *********************/
/*************************************************************************/

#define LIST_ADMIN      0
#define LIST_OPER       1

#define MSG_EXISTS      0
#define MSG_EXISTS_NEXT 1
#define MSG_ADDED       2
#define MSG_TOOMANY     3
#define MSG_REMOVED     4
#define MSG_NOTFOUND    5

static int privlist_msgs[2][6] = {
    { OPER_ADMIN_EXISTS,
      0,
      OPER_ADMIN_ADDED,
      OPER_ADMIN_TOO_MANY,
      OPER_ADMIN_REMOVED,
      OPER_ADMIN_NOT_FOUND,
    },
    { OPER_OPER_EXISTS,
      OPER_ADMIN_EXISTS,
      OPER_OPER_ADDED,
      OPER_OPER_TOO_MANY,
      OPER_OPER_REMOVED,
      OPER_OPER_NOT_FOUND,
    },
};

/*************************************************************************/

/* Add a nick to the Services admin/oper list.  u is the command sender. */

static void privlist_add(User *u, int listid, const char *nick)
{
    int16 level = (listid==LIST_ADMIN ? NP_SERVADMIN : NP_SERVOPER);
    int16 nextlevel = (listid==LIST_ADMIN ? 0 : NP_SERVADMIN);
    int *msgs = privlist_msgs[listid];
    NickInfo *ni;
    NickGroupInfo *ngi;

    if (!(ni = get_nickinfo(nick))) {
        notice_lang(s_OperServ, u, NICK_X_NOT_REGISTERED, nick);
        return;
    }
    ngi = get_ngi(ni);
    put_nickinfo(ni);
    if (!ngi) {
        notice_lang(s_OperServ, u, INTERNAL_ERROR);
        return;
    }
    if (nextlevel && ngi->os_priv >= nextlevel) {
        notice_lang(s_OperServ, u, msgs[MSG_EXISTS_NEXT], nick);
        put_nickgroupinfo(ngi);
        return;
    } else if (ngi->os_priv >= level) {
        notice_lang(s_OperServ, u, msgs[MSG_EXISTS], nick);
        put_nickgroupinfo(ngi);
        return;
    }
    ngi->os_priv = level;
    put_nickgroupinfo(ngi);
    notice_lang(s_OperServ, u, msgs[MSG_ADDED], nick);
    if (readonly)
        notice_lang(s_OperServ, u, READ_ONLY_MODE);
}

/*************************************************************************/

/* Remove a nick from the Services admin/oper list. */

static void privlist_rem(User *u, int listid, const char *nick)
{
    int16 level = (listid==LIST_ADMIN ? NP_SERVADMIN : NP_SERVOPER);
    int16 nextlevel = (listid==LIST_ADMIN ? 0 : NP_SERVADMIN);
    int *msgs = privlist_msgs[listid];
    NickInfo *ni;
    NickGroupInfo *ngi;

    if (!(ni = get_nickinfo(nick))) {
        notice_lang(s_OperServ, u, NICK_X_NOT_REGISTERED, nick);
        return;
    }
    ngi = get_ngi(ni);
    put_nickinfo(ni);
    if (!ngi) {
        notice_lang(s_OperServ, u, INTERNAL_ERROR);
        return;
    }
    if (ngi->os_priv < level || (nextlevel && ngi->os_priv >= nextlevel)) {
        notice_lang(s_OperServ, u, msgs[MSG_NOTFOUND], nick);
        put_nickgroupinfo(ngi);
        return;
    }
    ngi->os_priv = 0;
    put_nickgroupinfo(ngi);
    notice_lang(s_OperServ, u, msgs[MSG_REMOVED], nick);
    if (readonly)
        notice_lang(s_OperServ, u, READ_ONLY_MODE);
}

/*************************************************************************/
/*********************** OperServ command functions **********************/
/*************************************************************************/

/* HELP command. */

static void do_help(User *u)
{
    char *cmd = strtok_remaining();
    Module *mod;

    if (!cmd) {
        notice_help(s_OperServ, u, OPER_HELP);
    } else if (call_callback_2(cb_help, u, cmd) > 0) {
        return;
    } else if (stricmp(cmd, "COMMANDS") == 0) {
        notice_help(s_OperServ, u, OPER_HELP_COMMANDS);
        call_callback_2(cb_help_cmds, u, 0);
        notice_help(s_OperServ, u, OPER_HELP_COMMANDS_SERVOPER);
        if ((mod = find_module("operserv/akill"))) {
            int *p_EnableExclude;
            notice_help(s_OperServ, u, OPER_HELP_COMMANDS_AKILL);
            p_EnableExclude = get_module_symbol(mod, "EnableExclude");
            if (p_EnableExclude && *p_EnableExclude)
                notice_help(s_OperServ, u, OPER_HELP_COMMANDS_EXCLUDE);
        }
        if (find_module("operserv/sline"))
            notice_help(s_OperServ, u, OPER_HELP_COMMANDS_SLINE);
        if (find_module("operserv/sessions"))
            notice_help(s_OperServ, u, OPER_HELP_COMMANDS_SESSION);
        if (find_module("operserv/news"))
            notice_help(s_OperServ, u, OPER_HELP_COMMANDS_NEWS);
        call_callback_2(cb_help_cmds, u, 1);
        notice_help(s_OperServ, u, OPER_HELP_COMMANDS_SERVADMIN);
        call_callback_2(cb_help_cmds, u, 2);
        notice_help(s_OperServ, u, OPER_HELP_COMMANDS_SERVROOT);
        if (AllowRaw)
            notice_help(s_OperServ, u, OPER_HELP_COMMANDS_RAW);
        call_callback_2(cb_help_cmds, u, 3);
    } else {
        help_cmd(s_OperServ, u, THIS_MODULE, cmd);
    }
}

/*************************************************************************/

/* Global notice sending via GlobalNoticer. */

static void do_global(User *u)
{
    char *msg = strtok_remaining();

    if (!msg) {
        syntax_error(s_OperServ, u, "GLOBAL", OPER_GLOBAL_SYNTAX);
        return;
    }
    notice_all(s_GlobalNoticer, "%s", msg);
}

/*************************************************************************/

/* STATS command. */

static void do_stats(User *u)
{
    time_t uptime = time(NULL) - start_time;
    char *extra = strtok_remaining();
    int days = uptime/86400, hours = (uptime/3600)%24,
        mins = (uptime/60)%60;
    char timebuf[BUFSIZE];

    if (extra && stricmp(extra, "UPTIME") == 0)
        extra = NULL;

    if (extra && stricmp(extra, "ALL") != 0) {
        if (stricmp(extra, "RESET") == 0) {
            operserv_data.maxusercnt = usercnt;
            operserv_data.maxusertime = time(NULL);
            notice_lang(s_OperServ, u, OPER_STATS_RESET_USER_COUNT);
        } else if (call_callback_2(cb_stats, u, extra) > 0) {
            /* nothing */
        } else if (stricmp(extra, "NETWORK") == 0) {
            uint64 read, written;
            uint32 socksize, totalsize;
            int ratio1, ratio2;
            sock_rwstat(servsock, &read, &written);
            sock_bufstat(servsock, &socksize, &totalsize, &ratio1, &ratio2);
            socksize /= 1024;
            totalsize /= 1024;
            notice_lang(s_OperServ, u, OPER_STATS_KBYTES_READ,
                        (unsigned int)(read/1024));
            notice_lang(s_OperServ, u, OPER_STATS_KBYTES_WRITTEN,
                        (unsigned int)(written/1024));
            if (ratio1)
                notice_lang(s_OperServ, u, OPER_STATS_NETBUF_SOCK_PERCENT,
                            socksize, ratio1);
            else
                notice_lang(s_OperServ, u, OPER_STATS_NETBUF_SOCK, socksize);
            if (ratio2)
                notice_lang(s_OperServ, u, OPER_STATS_NETBUF_TOTAL_PERCENT,
                            totalsize, ratio2);
            else
                notice_lang(s_OperServ, u, OPER_STATS_NETBUF_TOTAL, totalsize);
        } else {
            notice_lang(s_OperServ, u, OPER_STATS_UNKNOWN_OPTION,
                        strupper(extra));
        }
        return;
    }

    notice_lang(s_OperServ, u, OPER_STATS_CURRENT_USERS, usercnt, opcnt);
    strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                  STRFTIME_DATE_TIME_FORMAT, operserv_data.maxusertime);
    notice_lang(s_OperServ, u, OPER_STATS_MAX_USERS, operserv_data.maxusercnt,
                timebuf);
    if (days >= 1) {
        const char *str = getstring(u->ngi, days!=1 ? STR_DAYS : STR_DAY);
        notice_lang(s_OperServ, u, OPER_STATS_UPTIME_DHM,
                days, str, hours, mins);
    } else {
        notice_lang(s_OperServ, u, OPER_STATS_UPTIME_HM_MS,
                    maketime(u->ngi, uptime, MT_DUALUNIT|MT_SECONDS));
    }

    if (extra && stricmp(extra, "ALL") == 0 && is_services_admin(u)) {
        long count, mem;

        get_user_stats(&count, &mem);
        notice_lang(s_OperServ, u, OPER_STATS_ALL_USER_MEM,
                    (int)count, (int)((mem+512) / 1024));
        get_channel_stats(&count, &mem);
        notice_lang(s_OperServ, u, OPER_STATS_ALL_CHANNEL_MEM,
                    (int)count, (int)((mem+512) / 1024));
        get_server_stats(&count, &mem);
        notice_lang(s_OperServ, u, OPER_STATS_ALL_SERVER_MEM,
                    (int)count, (int)((mem+512) / 1024));
        call_callback_2(cb_stats_all, u, s_OperServ);

    }
}

/*************************************************************************/

/* Server map display. */

static void map_server(User *u, Server *s, int level);  /* defined below */

static void do_servermap(User *u)
{
    Server *root = get_server("");
    if (!root) {
        module_log("BUG: root server not found for SERVERMAP");
        notice_lang(s_OperServ, u, INTERNAL_ERROR);
        return;
    }
    map_server(u, root, 0);
}


/* Recursively print the server map.  This routine prints the given server
 * at level `level' of indentation (0 = left margin), then prints its
 * children in child->sibling->sibling... order.
 */

static void map_server(User *u, Server *s, int level)
{
    static const char indentstr[] = "  +-- ";
    static const char barstr[]    = "  |   ";
    static const char nobarstr[]  = "      ";
    /* Which levels need a continuation bar?  Note that this must be static
     * so that all values are available acr*/
    static int need_bar[MAXMAPLEVEL];

    char buf[BUFSIZE], *ptr;
    int i;

    ptr = buf;
    *ptr = 0;
    for (i = 0; i < level-1; i++) {
        ptr += snprintf(ptr, sizeof(buf)-(ptr-buf), "%s",
                        need_bar[i] ? barstr : nobarstr);
    }
    if (level)
        ptr += snprintf(ptr, sizeof(buf)-(ptr-buf), "%s", indentstr);
    notice(s_OperServ, u->nick, "%s%s%s", buf,
           s ? (*s->name ? s->name : ServerName) : "...",
           (s && s->fake) ? "(*)" : "");
    if (s && s->child) {
        if (level+1 >= MAXMAPLEVEL) {
            /* Nesting too deep--use a NULL server parameter to display a
             * "..." indicating more servers */
            map_server(u, NULL, level+1);
            return;
        }
        for (s = s->child; s; s = s->sibling) {
            need_bar[level] = (s->sibling != NULL);
            map_server(u, s, level+1);
        }
    }
}

/*************************************************************************/

/* Retrieve the key for a channel. */

static void do_getkey(User *u)
{
    char *chan = strtok(NULL, " ");
    Channel *c;

    if (!chan || strtok_remaining()) {
        syntax_error(s_OperServ, u, "GETKEY", OPER_GETKEY_SYNTAX);
        return;
    }
    if (!(c = get_channel(chan))) {
        notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
    } else {
        if (WallOSChannel)
            wallops(s_OperServ, "%s used GETKEY on %s", u->nick, chan);
        if (c->key)
            notice_lang(s_OperServ, u, OPER_GETKEY_KEY_IS, c->name, c->key);
        else
            notice_lang(s_OperServ, u, OPER_GETKEY_NO_KEY, c->name);
    }
}

/*************************************************************************/

/* Channel mode changing (MODE command). */

static void do_os_mode(User *u)
{
    int argc;
    char **argv;
    char *s = strtok_remaining();
    char *chan, *modes;
    Channel *c;

    if (!s) {
        syntax_error(s_OperServ, u, "MODE", OPER_MODE_SYNTAX);
        return;
    }
    chan = s;
    s += strcspn(s, " ");
    if (!*s) {
        syntax_error(s_OperServ, u, "MODE", OPER_MODE_SYNTAX);
        return;
    }
    *s = 0;
    modes = (s+1) + strspn(s+1, " ");
    if (!*modes) {
        syntax_error(s_OperServ, u, "MODE", OPER_MODE_SYNTAX);
        return;
    }
    if (!(c = get_channel(chan))) {
        notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_OperServ, u, OPER_BOUNCY_MODES);
        return;
    } else {
        send_cmd(s_OperServ, "MODE %s %s", chan, modes);
        if (WallOSChannel)
            wallops(s_OperServ, "%s used MODE %s on %s", u->nick, modes, chan);
        *s = ' ';
        argc = split_buf(chan, &argv, 1);
        do_cmode(s_OperServ, argc, argv);
    }
}

/*************************************************************************/

/* Clear all modes from a channel. */

static void do_clearmodes(User *u)
{
    char *s;
    char *chan = strtok(NULL, " ");
    Channel *c;
    int all = 0;

    if (!chan) {
        syntax_error(s_OperServ, u, "CLEARMODES", OPER_CLEARMODES_SYNTAX);
    } else if (!(c = get_channel(chan))) {
        notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_OperServ, u, OPER_BOUNCY_MODES);
        return;
    } else {
        s = strtok(NULL, " ");
        if (s) {
            if (stricmp(s, "ALL") == 0) {
                all = 1;
            } else {
                syntax_error(s_OperServ,u,"CLEARMODES",OPER_CLEARMODES_SYNTAX);
                return;
            }
        }
        if (WallOSChannel)
            wallops(s_OperServ, "%s used CLEARMODES%s on %s",
                        u->nick, all ? " ALL" : "", chan);
        if (all) {
            clear_channel(c, CLEAR_UMODES, (void *)MODE_ALL);
            clear_channel(c, CLEAR_CMODES, NULL);
            notice_lang(s_OperServ, u, OPER_CLEARMODES_ALL_DONE, chan);
        } else {
            clear_channel(c, CLEAR_CMODES, NULL);
            notice_lang(s_OperServ, u, OPER_CLEARMODES_DONE, chan);
        }
    }
}

/*************************************************************************/

/* Remove all users from a channel. */

static void do_clearchan(User *u)
{
    char *chan = strtok(NULL, " ");
    Channel *c;
    char buf[BUFSIZE];

    if (!chan) {
        syntax_error(s_OperServ, u, "CLEARCHAN", OPER_CLEARCHAN_SYNTAX);
    } else if (!(c = get_channel(chan))) {
        notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_OperServ, u, OPER_BOUNCY_MODES);
        return;
    } else {
        if (WallOSChannel)
            wallops(s_OperServ, "%s used CLEARCHAN on %s", u->nick, chan);
        snprintf(buf, sizeof(buf), "CLEARCHAN by %s", u->nick);
        clear_channel(c, CLEAR_USERS, buf);
        notice_lang(s_OperServ, u, OPER_CLEARCHAN_DONE, chan);
    }
}

/*************************************************************************/

/* Kick a user from a channel (KICK command). */

static void do_os_kick(User *u)
{
    char *argv[3];
    char *chan, *nick, *s;
    Channel *c;

    chan = strtok(NULL, " ");
    nick = strtok(NULL, " ");
    s = strtok_remaining();
    if (!chan || !nick || !s) {
        syntax_error(s_OperServ, u, "KICK", OPER_KICK_SYNTAX);
        return;
    }
    if (!(c = get_channel(chan))) {
        notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_OperServ, u, OPER_BOUNCY_MODES);
        return;
    }
    send_cmd(s_OperServ, "KICK %s %s :%s (%s)", chan, nick, u->nick, s);
    if (WallOSChannel)
        wallops(s_OperServ, "%s used KICK on %s/%s", u->nick, nick, chan);
    argv[0] = chan;
    argv[1] = nick;
    argv[2] = s;
    do_kick(s_OperServ, 3, argv);
}

/*************************************************************************/

/* Services admin list viewing/modification. */

static void do_admin(User *u)
{
    const char *cmd, *nick;

    if (!module_nickserv) {
        notice_lang(s_OperServ, u, OPER_ADMIN_NO_NICKSERV);
        return;
    }
    cmd = strtok(NULL, " ");
    if (!cmd)
        cmd = "";

    if (stricmp(cmd, "ADD") == 0) {
        if (!is_services_root(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return;
        }
        nick = strtok(NULL, " ");
        if (nick)
            privlist_add(u, LIST_ADMIN, nick);
        else
            syntax_error(s_OperServ, u, "ADMIN", OPER_ADMIN_ADD_SYNTAX);

    } else if (stricmp(cmd, "DEL") == 0) {
        if (!is_services_root(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return;
        }
        nick = strtok(NULL, " ");
        if (nick)
            privlist_rem(u, LIST_ADMIN, nick);
        else
            syntax_error(s_OperServ, u, "ADMIN", OPER_ADMIN_DEL_SYNTAX);

    } else if (stricmp(cmd, "LIST") == 0) {
        NickGroupInfo *ngi;
        notice_lang(s_OperServ, u, OPER_ADMIN_LIST_HEADER);
        for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {
            if (ngi->os_priv >= NP_SERVADMIN)
                notice(s_OperServ, u->nick, "%s", ngi_mainnick(ngi));
        }

    } else {
        syntax_error(s_OperServ, u, "ADMIN", OPER_ADMIN_SYNTAX);
    }
}

/*************************************************************************/

/* Services oper list viewing/modification. */

static void do_oper(User *u)
{
    const char *cmd, *nick;

    if (!module_nickserv) {
        notice_lang(s_OperServ, u, OPER_OPER_NO_NICKSERV);
        return;
    }
    cmd = strtok(NULL, " ");
    if (!cmd)
        cmd = "";

    if (stricmp(cmd, "ADD") == 0) {
        if (!is_services_admin(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return;
        }
        nick = strtok(NULL, " ");
        if (nick)
            privlist_add(u, LIST_OPER, nick);
        else
            syntax_error(s_OperServ, u, "OPER", OPER_OPER_ADD_SYNTAX);

    } else if (stricmp(cmd, "DEL") == 0) {
        if (!is_services_admin(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return;
        }
        nick = strtok(NULL, " ");
        if (nick)
            privlist_rem(u, LIST_OPER, nick);
        else
            syntax_error(s_OperServ, u, "OPER", OPER_OPER_DEL_SYNTAX);

    } else if (stricmp(cmd, "LIST") == 0) {
        NickGroupInfo *ngi;
        notice_lang(s_OperServ, u, OPER_OPER_LIST_HEADER);
        for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {
            if (ngi->os_priv >= NP_SERVOPER && ngi->os_priv < NP_SERVADMIN)
                notice(s_OperServ, u->nick, "%s", ngi_mainnick(ngi));
        }

    } else {
        syntax_error(s_OperServ, u, "OPER", OPER_OPER_SYNTAX);
    }
}

/*************************************************************************/

/* Obtain Services root privileges.  We check permissions here instead of
 * letting run_cmd() do it for us so we can send out warnings when a
 * non-admin tries to use the command, and allow anyone to use it when
 * NickServ isn't loaded.
 */

static void do_su(User *u)
{
    char *password = strtok(NULL, " ");
    int res;

    if (module_nickserv && !is_services_admin(u)) {
        wallops(s_OperServ, "\2NOTICE:\2 %s!%s@%s attempted to use SU "
                "command (not Services admin)",
                u->nick, u->username, u->host);
        notice_lang(s_OperServ, u, PERMISSION_DENIED);
        return;
    }

    if (!password || strtok_remaining()) {
        syntax_error(s_OperServ, u, "SU", OPER_SU_SYNTAX);
    } else if (operserv_data.no_supass) {
        notice_lang(s_OperServ, u, OPER_SU_NO_PASSWORD);
    } else if ((res = check_password(password, &operserv_data.supass)) < 0) {
        notice_lang(s_OperServ, u, OPER_SU_FAILED);
    } else if (res == 0) {
        module_log("Failed SU by %s!%s@%s", u->nick, u->username, u->host);
        wallops(s_OperServ, "\2NOTICE:\2 Failed SU by %s!%s@%s",
                u->nick, u->username, u->host);
        bad_password(s_OperServ, u, "Services root");
    } else {
        u->flags |= UF_SERVROOT;
        if (WallSU)
            wallops(s_OperServ,
                    "%s!%s@%s obtained Services super-user privileges",
                    u->nick, u->username, u->host);
        notice_lang(s_OperServ, u, OPER_SU_SUCCEEDED);
    }
}

/*************************************************************************/

/* Set various Services runtime options. */

static void do_set(User *u)
{
    char *option = strtok(NULL, " ");
    char *setting = strtok(NULL, " ");

    if (!option || (!setting && stricmp(option, "SUPASS") != 0)
     || strtok_remaining()
    ) {
        syntax_error(s_OperServ, u, "SET", OPER_SET_SYNTAX);
        return;
    }

    if (call_callback_3(cb_set, u, option, setting) > 0)
        return;

    if (stricmp(option, "IGNORE") == 0) {
        if (stricmp(setting, "on") == 0) {
            allow_ignore = 1;
            notice_lang(s_OperServ, u, OPER_SET_IGNORE_ON);
        } else if (stricmp(setting, "off") == 0) {
            allow_ignore = 0;
            notice_lang(s_OperServ, u, OPER_SET_IGNORE_OFF);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_IGNORE_ERROR);
        }

    } else if (stricmp(option, "READONLY") == 0) {
        if (stricmp(setting, "on") == 0) {
            readonly = 1;
            log("Read-only mode activated");
            close_log();
            notice_lang(s_OperServ, u, OPER_SET_READONLY_ON);
        } else if (stricmp(setting, "off") == 0) {
            readonly = 0;
            open_log();
            log("Read-only mode deactivated");
            notice_lang(s_OperServ, u, OPER_SET_READONLY_OFF);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_READONLY_ERROR);
        }

    } else if (stricmp(option, "DEBUG") == 0) {
        if (stricmp(setting, "on") == 0) {
            debug = 1;
            log("Debug mode activated");
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_ON);
        } else if (stricmp(setting, "off") == 0 ||
                                (*setting == '0' && atoi(setting) == 0)) {
            log("Debug mode deactivated");
            debug = 0;
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_OFF);
        } else if (isdigit(*setting) && atoi(setting) > 0) {
            debug = atoi(setting);
            log("Debug mode activated (level %d)", debug);
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_LEVEL, debug);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_ERROR);
        }

    } else if (stricmp(option, "SUPASS") == 0) {
        Password newpass;
        int res;

        if (!is_services_root(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return;
        }
        init_password(&newpass);
        if (!setting) {
            clear_password(&operserv_data.supass);
            operserv_data.no_supass = 1;
            put_operserv_data(OSDATA_SUPASS, NULL);
            notice_lang(s_OperServ, u, OPER_SET_SUPASS_NONE);
            return;
        }
        res = encrypt_password(setting, strlen(setting), &newpass);
        memset(setting, 0, strlen(setting));
        if (res != 0) {
            notice_lang(s_OperServ, u, OPER_SET_SUPASS_FAILED);
        } else {
            operserv_data.no_supass = 0;
            copy_password(&operserv_data.supass, &newpass);
            clear_password(&newpass);
            notice_lang(s_OperServ, u, OPER_SET_SUPASS_OK);
        }

    } else {
        notice_lang(s_OperServ, u, OPER_SET_UNKNOWN_OPTION, option);
    }
}

/*************************************************************************/

static void do_jupe(User *u)
{
    char *jserver = strtok(NULL, " ");
    char *reason = strtok_remaining();
    char buf[BUFSIZE];
    Server *server;

    if (!jserver) {
        syntax_error(s_OperServ, u, "JUPE", OPER_JUPE_SYNTAX);
    } else if (!strchr(jserver, '.')) {
        notice_lang(s_OperServ, u, OPER_JUPE_INVALID_NAME);
    } else if ((server = get_server(jserver)) != NULL && server->fake) {
        notice_lang(s_OperServ, u, OPER_JUPE_ALREADY_JUPED, jserver);
    } else {
        wallops(s_OperServ, "\2Juping\2 %s by request of \2%s\2.",
                jserver, u->nick);
        if (reason)
            snprintf(buf, sizeof(buf), "%s (%s)", reason, u->nick);
        else
            snprintf(buf, sizeof(buf), "Jupitered by %s", u->nick);
        if (server) {
            char *argv[2];
            argv[0] = jserver;
            argv[1] = buf;
            send_cmd(ServerName, "SQUIT %s :%s", jserver, buf);
            do_squit(ServerName, 2, argv);
        }
        send_server_remote(jserver, buf);
        do_server("", -1, &jserver);
    }
}

/*************************************************************************/

static void do_raw(User *u)
{
    char *text = strtok_remaining();

    if (!text)
        syntax_error(s_OperServ, u, "RAW", OPER_RAW_SYNTAX);
    else
        send_cmd(NULL, "%s", text);
}

/*************************************************************************/

/* Callback and data used to send "update complete" message */
static int do_update_complete(int successful);
static User *update_sender = NULL;

static void do_update(User *u)
{
    char *param = strtok_remaining();

    if (param && *param) {
        if (stricmp(param, "FORCE") != 0) {
            syntax_error(s_OperServ, u, "UPDATE", OPER_UPDATE_SYNTAX);
            return;
        } else if (!is_services_admin(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return;
        }
        switch (is_data_locked()) {
          case 1:
            if (!unlock_data()) {
                module_log_perror("UPDATE FORCE lock removal failed");
                notice_lang(s_OperServ, u, OPER_UPDATE_FORCE_FAILED);
                return;
            }
            break;
          case -1:
            module_log_perror("UPDATE FORCE lock check failed");
            break;
        }
    }
    notice_lang(s_OperServ, u, OPER_UPDATING);
    save_data = 1;
    update_sender = u;  /* to send to when the update completes--it's safe
                         * to save this pointer since no more data will be
                         * processed before we use it again */
    add_callback(NULL, "save data complete", do_update_complete);
}

static int do_update_complete(int successful)
{
    if (update_sender) {
        if (successful)
            notice_lang(s_OperServ, update_sender, OPER_UPDATE_COMPLETE);
        else
            notice_lang(s_OperServ, update_sender, OPER_UPDATE_FAILED);
        update_sender = NULL;
    } else {
        log("BUG: no sender in do_update_complete()");
    }
    remove_callback(NULL, "save data complete", do_update_complete);
    return 0;
}

/*************************************************************************/

static void do_os_quit(User *u)
{
    snprintf(quitmsg, sizeof(quitmsg),
             "QUIT command received from %s", u->nick);
    quitting = 1;
}

/*************************************************************************/

static void do_shutdown(User *u)
{
    snprintf(quitmsg, sizeof(quitmsg),
            "SHUTDOWN command received from %s", u->nick);
    save_data = 1;
    delayed_quit = 1;
}

/*************************************************************************/

static void do_restart(User *u)
{
    snprintf(quitmsg, sizeof(quitmsg),
             "RESTART command received from %s", u->nick);
    save_data = 1;
    delayed_quit = 1;
    restart = 1;
}

/*************************************************************************/

static void do_rehash(User *u)
{
    /* Don't let ourselves be unloaded */
    in_rehash = 1;

    notice_lang(s_OperServ, u, OPER_REHASHING);
    wallops(NULL, "Rehashing configuration files (REHASH from %s)", u->nick);
    if (reconfigure())
        notice_lang(s_OperServ, u, OPER_REHASHED);
    else
        notice_lang(s_OperServ, u, OPER_REHASH_ERROR);

    /* Restore normal module behavior */
    in_rehash = 0;
}

/*************************************************************************/

/* Kill all users matching a certain host.  The host is obtained from the
 * supplied nick.  The raw hostmask is not supplied with the command in an
 * effort to prevent abuse and mistakes from being made--which might cause
 * *.com to be killed. It also makes it very quick and simple to use--which
 * is usually what you want when someone starts loading numerous clones.  In
 * addition to killing the clones, we add a temporary autokill to prevent
 * them from immediately reconnecting.
 * Syntax: KILLCLONES nick
 * -TheShadow (29 Mar 1999)
 *
 * Added KillClonesAutokill support  --AC
 */

static void do_killclones(User *u)
{
    char *clonenick = strtok(NULL, " ");
    User *cloneuser;
    typeof(create_akill) *p_create_akill =
        module_akill ? get_module_symbol(module_akill,"create_akill") : NULL;

    if (!clonenick) {
        notice_lang(s_OperServ, u, OPER_KILLCLONES_SYNTAX);

    } else if (!(cloneuser = get_user(clonenick))) {
        notice_lang(s_OperServ, u, OPER_KILLCLONES_UNKNOWN_NICK, clonenick);

    } else {
        char clonemask[BUFSIZE];
        User *user;
        int count = 0;

        snprintf(clonemask, sizeof(clonemask), "*!*@%s", cloneuser->host);
        for (user = first_user(); user; user = next_user()) {
            if (match_usermask(clonemask, user) != 0) {
                char killreason[32];
                count++;
                snprintf(killreason, sizeof(killreason),
                         "Cloning [%d]", count);
                kill_user(NULL, user->nick, killreason);
            }
        }
        module_log("KILLCLONES: %d clone(s) matching %s killed.",
                   count, clonemask);

        if (KillClonesAutokill && p_create_akill) {
            /* Add autokill if it doesn't exist.  Use get_matching_maskdata()
             * so that we find both exact matches and more general masks. */
            if (!put_maskdata(get_matching_maskdata(MD_AKILL, clonemask+2))) {
                /* No mask fits these clones, so add a new one. */
                const char akillreason[] = "Temporary KILLCLONES akill.";
                p_create_akill(clonemask+2, akillreason, u->nick,
                               time(NULL) + KillClonesAutokillExpire);
                wallops(s_OperServ,
                        getstring(NULL,OPER_KILLCLONES_KILLED_AKILL),
                        u->nick, clonemask, count, clonemask+2);
            } else {
                /* There's already a matching mask, so don't add a new one. */
                wallops(s_OperServ, getstring(NULL,OPER_KILLCLONES_KILLED),
                        u->nick, clonemask, count);
            }
        } else {
            /* Autokill option not set or module not available. */
            wallops(s_OperServ, getstring(NULL,OPER_KILLCLONES_KILLED),
                    u->nick, clonemask, count);
        }

    }
}

/*************************************************************************/

#ifdef DEBUG_COMMANDS

/* All commands from here down are enabled only when DEBUG_COMMANDS is
 * also enabled. */

/*************************************************************************/

/* Send the current list of servers to the given user. */

void send_server_list(User *user)
{
    Server *server;

    for (server = first_server(); server; server = next_server()) {
        notice(s_OperServ, user->nick, "%s fake:%d hub:%s child:%s sibling:%s",
               server->name, server->fake,
               server->hub ? server->hub->name : "-",
               server->child ? server->child->name : "-",
               server->sibling ? server->sibling->name : "-");

    }
}

/*************************************************************************/

/* Send the current list of channels to the named user. */

static void send_channel_list(User *user)
{
    Channel *c;
    char buf[BUFSIZE], *end;
    struct c_userlist *u;
    const char *source = user->nick;

    for (c = first_channel(); c; c = next_channel()) {
        notice(s_OperServ, source, "%s %ld +%s %d %s %s %s %d %d %d",
               c->name, (long)c->creation_time,
               mode_flags_to_string(c->mode, MODE_CHANNEL),
               c->limit, c->key ? c->key : "-", c->link ? c->link : "-",
               c->flood ? c->flood : "-", c->joindelay, c->joinrate1,
               c->joinrate2);
        notice(s_OperServ, source, "%s TOPIC %s %ld :%s", c->name,
               c->topic_setter, (long)c->topic_time, c->topic ? c->topic : "");
        end = buf;
        end += snprintf(end, sizeof(buf)-(end-buf), "%s", c->name);
        LIST_FOREACH (u, c->users) {
            end += snprintf(end, sizeof(buf)-(end-buf), " +%s/%s",
                            mode_flags_to_string(u->mode,MODE_CHANUSER),
                            u->user->nick);
        }
        notice(s_OperServ, source, buf);
    }
}

/*************************************************************************/

/* Send list of users on a single channel, taken from strtok(). */

static void send_channel_users(User *user)
{
    char *chan = strtok(NULL, " ");
    Channel *c = chan ? get_channel(chan) : NULL;
    struct c_userlist *u;
    const char *source = user->nick;

    if (!c) {
        notice(s_OperServ, source, "Channel %s not found!",
                chan ? chan : "(null)");
        return;
    }
    notice(s_OperServ, source, "Channel %s users:", chan);
    LIST_FOREACH (u, c->users)
        notice(s_OperServ, source, "%x/%s", u->mode, u->user->nick);
}

/*************************************************************************/

/* Send the current list of users to the given user. */

static void send_user_list(User *user)
{
    User *u;
    const char *source = user->nick;

    for (u = first_user(); u; u = next_user()) {
        char buf[5];
        if (u->ni)
            snprintf(buf, sizeof(buf), "%04X", u->ni->status & 0xFFFF);
        else
            strcpy(buf, "-");
        notice(s_OperServ, source, "%s!%s@%s %s %s +%s %ld %u %s %s %.6f :%s",
               u->nick, u->username, u->host, u->fakehost ? u->fakehost : "-",
               u->ipaddr ? u->ipaddr : "-",
               mode_flags_to_string(u->mode, MODE_USER), (long)u->signon,
               u->servicestamp, u->server->name, buf, u->ignore, u->realname);
    }
}

/*************************************************************************/

/* Send information about a single user to the given user.  Target nick is
 * taken from strtok(). */

static void send_user_info(User *user)
{
    char *nick = strtok(NULL, " ");
    User *u = nick ? get_user(nick) : NULL;
    char buf[BUFSIZE], *s;
    struct u_chanlist *c;
    struct u_chaninfolist *ci;
    const char *source = user->nick;

    if (!u) {
        notice(s_OperServ, source, "User %s not found!",
                nick ? nick : "(null)");
        return;
    }
    if (u->ni)
        snprintf(buf, sizeof(buf), "%04X", u->ni->status & 0xFFFF);
    else
        strcpy(buf, "-");
    notice(s_OperServ, source, "%s!%s@%s %s %s +%s %ld %u %s %s %.6f :%s",
           u->nick, u->username, u->host, u->fakehost ? u->fakehost : "-",
           u->ipaddr ? u->ipaddr : "-",
           mode_flags_to_string(u->mode, MODE_USER), (long)u->signon,
           u->servicestamp, u->server->name, buf, u->ignore, u->realname);
    buf[0] = 0;
    s = buf;
    LIST_FOREACH (c, u->chans)
        s += snprintf(s, sizeof(buf)-(s-buf), " %s", c->chan->name);
    notice(s_OperServ, source, "%s%s", u->nick, buf);
    buf[0] = 0;
    s = buf;
    LIST_FOREACH (ci, u->id_chans)
        s += snprintf(s, sizeof(buf)-(s-buf), " %s", ci->chan);
    notice(s_OperServ, source, "%s%s", u->nick, buf);
}

/*************************************************************************/

/* Perform a wildcard match and report the result (either 0 or 1) to the
 * given user. */

static void do_matchwild(User *u)
{
    char *pat = strtok(NULL, " ");
    char *str = strtok(NULL, " ");
    if (pat && str)
        notice(s_OperServ, u->nick, "%d", match_wild(pat, str));
    else
        notice(s_OperServ, u->nick, "Syntax error.");
}

/*************************************************************************/

/* Call set_cmode() with ServerName and parameters taken from strtok() (up
 * to SETCMODE_NPARAMS). */

#define SETCMODE_NPARAMS        10

static void do_setcmode(User *u)
{
    char *channame, *params[SETCMODE_NPARAMS];
    Channel *channel;
    int i;

    channame = strtok(NULL, " ");
    for (i = 0; i < SETCMODE_NPARAMS; i++)
        params[i] = strtok(NULL, " ");
    if (!channame) {
        notice(s_OperServ, u->nick, "SETCMODE: not enough parameters");
        return;
    }
    if (strcmp(channame, "-") == 0) {
        channel = NULL;
    } else {
        channel = get_channel(channame);
        if (!channel) {
            notice(s_OperServ, u->nick, "SETCMODE: channel not found (%s)",
                   channame);
            return;
        }
    }
    set_cmode(channel ? ServerName : NULL, channel,
              params[0], params[1], params[2], params[3], params[4],
              params[5], params[6], params[7], params[8], params[9]);
    notice(s_OperServ, u->nick, "SETCMODE: done");
}

/*************************************************************************/

/* Monitor the ignore value of a particular user, writing it to the log 10
 * times a second.  With no parameters, cancels any current logging.  The
 * timeout routine will call ignore_update() to ensure u->ignore is up to
 * date.  Note that using this command will force the timeout check
 * interval and socket read timeout to 0.01 seconds (not reversed even when
 * logging is cancelled).  This command will cause a sizeof(Timeout) memory
 * leak on exit if not cancelled first. */

static void log_monitor_ignore(Timeout *to)
{
    User *u = (User *)to->data;
    if (u) {
        ignore_update(u, 0);
        module_log_debug(1, "ignore_level(%s) = %.10f", u->nick, u->ignore);
    }
}

static void do_monitor_ignore(User *u)
{
    static Timeout *log_timeout = NULL;
    char *nick = strtok(NULL, " ");

    if (nick) {
        User *u = get_user(nick);
        if (!u) {
            notice(s_OperServ, u->nick,
                   "MONITOR-IGNORE: user %s not found!", nick);
            return;
        }
        if (log_timeout)
            del_timeout(log_timeout);
        log_timeout = add_timeout_ms(100, log_monitor_ignore, 1);
        if (!log_timeout) {
            notice(s_OperServ, u->nick,
                   "MONITOR-IGNORE: unable to add timeout!");
            return;
        }
        log_timeout->data = u;
        TimeoutCheck = 10;  /* 0.01s */
        sock_set_rto(ReadTimeout = 10);
        notice(s_OperServ, u->nick, "MONITOR-IGNORE: monitoring %s", u->nick);
    } else {  /* !nick */
        if (log_timeout) {
            del_timeout(log_timeout);
            log_timeout = NULL;
            notice(s_OperServ, u->nick,
                   "MONITOR-IGNORE: cancelled monitoring");
        } else {
            notice(s_OperServ, u->nick,
                   "MONITOR-IGNORE: wasn't monitoring anything");
        }
    }
}

/*************************************************************************/

/* Return the value of the given language and string, using
 * getstring_lang().
 */

static void do_getstring(User *u)
{
    char *s;
    long lang, index;

    s = strtok(NULL, " ");
    if (!s) {
        notice(s_OperServ, u->nick,
               "[ERROR] Syntax: \2GETSTRING \037language\037 \037index\037\2");
        return;
    }
    lang = strtol(s, &s, 0);
    if ((lang < 0 || *s) && ((lang = lookup_language(s)) < 0)) {
        notice(s_OperServ, u->nick, "[ERROR] Invalid language number/name");
        return;
    }

    s = strtok(NULL, " ");
    if (!s) {
        notice(s_OperServ, u->nick,
               "[ERROR] Syntax: \2GETSTRING \037language\037 \037index\037\2");
        return;
    }
    index = strtol(s, &s, 0);
    if ((index < 0 || *s) && ((index = lookup_string(s)) < 0)) {
        notice(s_OperServ, u->nick, "[ERROR] Invalid string number/name");
        return;
    }

    notice(s_OperServ, u->nick, "%s", getstring_lang(lang,index));
}

/*************************************************************************/

/* Set the text of a string using setstring(). */

static void do_setstring(User *u)
{
    char *s;
    long lang, index;

    s = strtok(NULL, " ");
    if (!s) {
        notice(s_OperServ, u->nick,
               "[ERROR] Syntax: \2SETSTRING \037language\037 \037index\037"
               " [\037text\037]\2");
        return;
    }
    lang = strtol(s, &s, 0);
    if ((lang < 0 || *s) && ((lang = lookup_language(s)) < 0)) {
        notice(s_OperServ, u->nick, "[ERROR] Invalid language number/name");
        return;
    }

    s = strtok(NULL, " ");
    if (!s) {
        notice(s_OperServ, u->nick,
               "[ERROR] Syntax: \2SETSTRING \037language\037 \037index\037"
               " [\037text\037]\2");
        return;
    }
    index = strtol(s, &s, 0);
    if ((index < 0 || *s) && ((index = lookup_string(s)) < 0)) {
        notice(s_OperServ, u->nick, "[ERROR] Invalid string number/name");
        return;
    }

    s = strtok_remaining();
    if (setstring(lang, index, s ? s : "")) {
        notice(s_OperServ, u->nick, "setstring(%ld,%ld) succeeded",
               lang, index);
    } else {
        notice(s_OperServ, u->nick, "[ERROR] setstring(%ld,%ld) failed",
               lang, index);
    }
}

/*************************************************************************/

/* Map one string to another using mapstring(). */

static void do_mapstring(User *u)
{
    char *s;
    long old, new;
    int ret;

    s = strtok(NULL, " ");
    if (!s) {
        notice(s_OperServ, u->nick,
               "[ERROR] Syntax: \2MAPSTRING \037old\037 \037new\037");
        return;
    }
    old = strtol(s, &s, 0);
    if ((old < 0 || *s) && ((old = lookup_string(s)) < 0)) {
        notice(s_OperServ, u->nick, "[ERROR] Invalid old string number/name");
        return;
    }

    s = strtok(NULL, " ");
    if (!s) {
        notice(s_OperServ, u->nick,
               "[ERROR] Syntax: \2MAPSTRING \037old\037 \037new\037");
        return;
    }
    new = strtol(s, &s, 0);
    if ((new < 0 || *s) && ((new = lookup_string(s)) < 0)) {
        notice(s_OperServ, u->nick, "[ERROR] Invalid new string number/name");
        return;
    }

    s = strtok_remaining();
    if ((ret = mapstring(old, new)) >= 0) {
        notice(s_OperServ, u->nick, "mapstring(%ld,%ld) succeeded => %d",
               old, new, ret);
    } else {
        notice(s_OperServ, u->nick, "[ERROR] mapstring(%ld,%ld) failed",
               old, new);
    }
}

/*************************************************************************/

/* Add a new string to the language system using addstring(). */

static void do_addstring(User *u)
{
    char *s;
    int index;

    s = strtok(NULL, " ");
    if (!s) {
        notice(s_OperServ, u->nick,
               "[ERROR] Syntax: \2ADDSTRING \037name\037\2");
        return;
    }
    if ((index = addstring(s)) >= 0) {
        notice(s_OperServ, u->nick, "addstring(%s) succeeded => %d", s, index);
    } else {
        notice(s_OperServ, u->nick, "[ERROR] addstring(%s) failed", s);
    }
}

/*************************************************************************/

#endif  /* DEBUG_COMMANDS */

/*************************************************************************/
/******************************* Callbacks *******************************/
/*************************************************************************/

/* New user callback: update the maximum user count. */

static int do_user_create(const User *user, int ac, char **av)
{
    if (usercnt > operserv_data.maxusercnt) {
        operserv_data.maxusercnt = usercnt;
        operserv_data.maxusertime = time(NULL);
        if (LogMaxUsers)
            log("user: New maximum user count: %d", operserv_data.maxusercnt);
    }
    return 0;
}

/*************************************************************************/

/* Watch for umode +o and send wallops.  This callback is only activated if
 * WallOper is specified in modules.conf.
 */

static int wall_oper_callback(User *u, int modechar, int add)
{
    if (modechar == 'o' && add)
        wallops(s_OperServ, "\2%s\2 is now an IRC operator.", u->nick);
    return 0;
}

/*************************************************************************/

/* Callback for NickServ REGISTER/LINK check; we disallow
 * registration/linking of the OperServ pseudoclient nickname.
 */

static int do_reglink_check(const User *u, const char *nick,
                            const char *pass, const char *email)
{
    return irc_stricmp(nick, s_OperServ) == 0
        || irc_stricmp(nick, s_GlobalNoticer) == 0;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "AllowRaw",         { { CD_SET, 0, &AllowRaw } } },
    { "GlobalName",       { { CD_STRING, CF_DIRREQ, &s_GlobalNoticer },
                            { CD_STRING, 0, &desc_GlobalNoticer } } },
    { "KillClonesAutokill",{{ CD_SET, 0, &KillClonesAutokill },
                            { CD_TIME, 0, &KillClonesAutokillExpire } } },
    { "OperServName",     { { CD_STRING, CF_DIRREQ, &s_OperServ },
                            { CD_STRING, 0, &desc_OperServ } } },
    { "ServicesRoot",     { { CD_STRING, CF_DIRREQ, &ServicesRoot } } },
    { "WallBadOS",        { { CD_SET, 0, &WallBadOS } } },
    { "WallOper",         { { CD_SET, 0, &WallOper } } },
    { "WallOSChannel",    { { CD_SET, 0, &WallOSChannel } } },
    { "WallSU",           { { CD_SET, 0, &WallSU } } },
    { NULL }
};


/* For enabling/disabling RAW command (AllowRaw) */
static Command *cmd_RAW = NULL;

/* Previous value of clear_channel() sender */
static char old_clearchan_sender[NICKMAX];
static int old_clearchan_sender_set = 0;

/*************************************************************************/

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "operserv/akill") == 0) {
        module_akill = mod;

    } else if (strcmp(modname, "nickserv/main") == 0) {
        char **p_s_NickServ;
        Command *cmd;

        module_nickserv = mod;
        p_s_NickServ = get_module_symbol(mod, "s_NickServ");
        if (p_s_NickServ) {
            cmd = lookup_cmd(THIS_MODULE, "ADMIN");
            if (cmd)
                cmd->help_param1 = *p_s_NickServ;
            cmd = lookup_cmd(THIS_MODULE, "OPER");
            if (cmd)
                cmd->help_param1 = *p_s_NickServ;
        }
        if (!add_callback(mod, "REGISTER/LINK check", do_reglink_check))
            module_log("Unable to register NickServ REGISTER/LINK check"
                       " callback");
    }

    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_akill) {
        module_akill = NULL;
    } else if (mod == module_nickserv) {
        Command *cmd;
        cmd = lookup_cmd(THIS_MODULE, "ADMIN");
        if (cmd)
            cmd->help_param1 = "NickServ";
        cmd = lookup_cmd(THIS_MODULE, "OPER");
        if (cmd)
            cmd->help_param1 = "NickServ";
        module_nickserv = NULL;
    }
    return 0;
}

/*************************************************************************/

static int do_reconfigure(int after_configure)
{
    static char old_s_OperServ[NICKMAX];
    static char *old_desc_OperServ = NULL;

    if (!after_configure) {
        /* Before reconfiguration: save old values. */
        /* Note that old_desc_OperServ might be non-NULL if a previous
         * reconfigure failed. */
        free(old_desc_OperServ);
        strbcpy(old_s_OperServ, s_OperServ);
        old_desc_OperServ = strdup(desc_OperServ);
    } else {
        Command *cmd;
        /* After reconfiguration: handle value changes. */
        if (strcmp(old_s_OperServ, s_OperServ) != 0) {
            if (strcmp(set_clear_channel_sender(PTR_INVALID),old_s_OperServ)==0)
                set_clear_channel_sender(s_OperServ);
            send_nickchange(old_s_OperServ, s_OperServ);
        }
        if (!old_desc_OperServ || strcmp(old_desc_OperServ,desc_OperServ) != 0)
            send_namechange(s_OperServ, desc_OperServ);
        /* Free and clear old values */
        free(old_desc_OperServ);
        old_desc_OperServ = NULL;
        /* Activate/deactivate commands */
        if (cmd_RAW) {
            if (AllowRaw)
                cmd_RAW->name = "RAW";
            else
                cmd_RAW->name = "";
        }
        /* Update command help parameters */
        if (module_nickserv) {
            char **p_s_NickServ;
            p_s_NickServ = get_module_symbol(module_nickserv, "s_NickServ");
            if (p_s_NickServ) {
                cmd = lookup_cmd(THIS_MODULE, "ADMIN");
                if (cmd)
                    cmd->help_param1 = *p_s_NickServ;
                cmd = lookup_cmd(THIS_MODULE, "OPER");
                if (cmd)
                    cmd->help_param1 = *p_s_NickServ;
            }
        }
        cmd = lookup_cmd(THIS_MODULE, "GLOBAL");
        if (cmd)
            cmd->help_param1 = s_GlobalNoticer;
    }  /* if (!after_configure) */
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    Command *cmd;


    if (!new_commandlist(THIS_MODULE)
     || !register_commands(THIS_MODULE, cmds)
    ) {
        module_log("Unable to register commands");
        exit_module(0);
        return 0;
    }

    cb_command   = register_callback("command");
    cb_help      = register_callback("HELP");
    cb_help_cmds = register_callback("HELP COMMANDS");
    cb_set       = register_callback("SET");
    cb_stats     = register_callback("STATS");
    cb_stats_all = register_callback("STATS ALL");
    if (cb_command < 0 || cb_help < 0 || cb_help_cmds < 0 || cb_set < 0
     || cb_stats < 0 || cb_stats_all < 0
    ) {
        module_log("Unable to register callbacks");
        exit_module(0);
        return 0;
    }

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "reconfigure", do_reconfigure)
     || !add_callback(NULL, "user create", do_user_create)
     || !add_callback(NULL, "introduce_user", introduce_operserv)
     || !add_callback(NULL, "m_privmsg", operserv)
     || !add_callback(NULL, "m_whois", operserv_whois)
     || (WallOper && !add_callback(NULL, "user MODE", wall_oper_callback))
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    if (!init_maskdata()) {
        exit_module(0);
        return 0;
    }

    init_password(&operserv_data.supass);
    if (!register_dbtable(&oper_dbtable)) {
        module_log("Unable to register database table");
        exit_module(0);
        return 0;
    }

    if (encrypt_all && !operserv_data.no_supass) {
        if ((EncryptionType && operserv_data.supass.cipher
             && strcmp(operserv_data.supass.cipher, EncryptionType) == 0)
         || (!EncryptionType && !operserv_data.supass.cipher)
        ) {
            module_log("-encrypt-all: Superuser password already encrypted");
        } else {
            char plainbuf[PASSMAX];
            int res;

            res = decrypt_password(&operserv_data.supass, plainbuf,
                                   sizeof(plainbuf));
            if (res != 0) {
                module_log("-encrypt-all: Unable to decrypt superuser"
                           " password");
            } else {
                res = encrypt_password(plainbuf, strlen(plainbuf),
                                       &operserv_data.supass);
                memset(plainbuf, 0, sizeof(plainbuf));
                if (res != 0) {
                    module_log("-encrypt-all: Unable to re-encrypt"
                               " superuser password");
                } else {
                    module_log("Re-encrypted superuser password");
                }
            }
        }
    }

    cmd_RAW = lookup_cmd(THIS_MODULE, "RAW");
    if (cmd_RAW && !AllowRaw)
        cmd_RAW->name = "";
    cmd = lookup_cmd(THIS_MODULE, "GLOBAL");
    if (cmd)
        cmd->help_param1 = s_GlobalNoticer;

    if (linked)
        introduce_operserv(NULL);

    strbcpy(old_clearchan_sender, set_clear_channel_sender(s_OperServ));
    old_clearchan_sender_set = 1;

    in_rehash = 0;

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown)
{
    if (!shutdown && in_rehash) {
        module_log("Refusing to unload because a REHASH command is in"
                   " progress");
        return 0;
    }

    if (old_clearchan_sender_set) {
        set_clear_channel_sender(old_clearchan_sender);
        old_clearchan_sender_set = 0;
    }

    if (linked) {
        send_cmd(s_OperServ, "QUIT :");
        send_cmd(s_GlobalNoticer, "QUIT :");
    }

    if (cmd_RAW)
        cmd_RAW->name = "RAW";

    exit_maskdata();

    unregister_dbtable(&oper_dbtable);
    clean_dbtables();

    clear_password(&operserv_data.supass);
    operserv_data.no_supass = 1;

    if (module_nickserv)
        do_unload_module(module_nickserv);
    if (module_akill)
        do_unload_module(module_akill);

    remove_callback(NULL, "user MODE", wall_oper_callback);
    remove_callback(NULL, "m_whois", operserv_whois);
    remove_callback(NULL, "m_privmsg", operserv);
    remove_callback(NULL, "introduce_user", introduce_operserv);
    remove_callback(NULL, "user create", do_user_create);
    remove_callback(NULL, "reconfigure", do_reconfigure);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

    unregister_callback(cb_stats_all);
    unregister_callback(cb_stats);
    unregister_callback(cb_set);
    unregister_callback(cb_help_cmds);
    unregister_callback(cb_help);
    unregister_callback(cb_command);

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
