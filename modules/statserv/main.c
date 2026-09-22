/* Statistics generation (StatServ) main module.
 * Based on code by Andrew Kempe (TheShadow).
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
#include "modules/operserv/operserv.h"

#include "statserv.h"

/*************************************************************************/

static Module *module_operserv;
static Module *module_nickserv;

static int cb_command   = -1;
static int cb_help      = -1;
static int cb_help_cmds = -1;


       char *s_StatServ;
static char *desc_StatServ;
static int   SSOpersOnly;

static int16 servercnt = 0;     /* Number of online servers */

/*************************************************************************/

static void do_help(User *u);
static void do_servers(User *u);
static void do_users(User *u);

/*************************************************************************/

static Command cmds[] = {
    { "HELP",        do_help,     NULL,  -1,                   -1,-1 },
    { "SERVERS",     do_servers,  NULL,  -1, STAT_HELP_SERVERS,
                STAT_OPER_HELP_SERVERS },
    { "USERS",       do_users,    NULL,  STAT_HELP_USERS,      -1,-1 },
    { NULL }
};

/*************************************************************************/
/**************************** Database stuff *****************************/
/*************************************************************************/

#define HASHFUNC(key) DEFAULT_HASHFUNC(key)
#define EXPIRE_CHECK(node) 0
#define add_serverstats   static _add_serverstats
#define del_serverstats   static _del_serverstats
#include "hash.h"
DEFINE_HASH(serverstats, ServerStats, name);
#undef add_serverstats
#undef del_serverstats

static void *alloc_serverstats(void)
{
    return scalloc(sizeof(ServerStats), 1);
}

ServerStats *add_serverstats(ServerStats *ss)
{
    _add_serverstats(ss);
    return ss;
}

void del_serverstats(ServerStats *ss)
{
    _del_serverstats(ss);
    free_serverstats(ss);
}

ServerStats *put_serverstats(ServerStats *ss)
{
    return ss;
}

EXPORT_FUNC(add_serverstats)
EXPORT_FUNC(del_serverstats)
EXPORT_FUNC(get_serverstats)
EXPORT_FUNC(put_serverstats)
EXPORT_FUNC(first_serverstats)
EXPORT_FUNC(next_serverstats)

/*************************************************************************/

/* Free all memory used by database tables. */

static void clean_dbtables(void)
{
    ServerStats *ss;
    for (ss = first_serverstats(); ss; ss = next_serverstats())
        free_serverstats(ss);
}

/*************************************************************************/

/* Database load helper: if the server was online when the databas was last
 * saved, set t_quit to a time just before Services started up, so servers
 * are never seen as online until we actually receive the SERVER message.
 */
static void db_put_t_quit(void *record, const void *value)
{
    ServerStats *ss = (ServerStats *)record;
    if (SS_IS_ONLINE(ss)) {
        ss->t_quit = time(NULL) - 1;
        if (SS_IS_ONLINE(ss)) {  /* Just in case */
            ss->t_quit = ss->t_join + 1;
        }
    }
}

static DBField stat_servers_dbfields[] = {
    { "name",         DBTYPE_STRING, offsetof(ServerStats,name) },
    { "t_join",       DBTYPE_TIME,   offsetof(ServerStats,t_join) },
    { "t_quit",       DBTYPE_TIME,   offsetof(ServerStats,t_quit),
          .put = db_put_t_quit},
    { "quit_message", DBTYPE_STRING, offsetof(ServerStats,quit_message) },
    { NULL }
};
static DBTable stat_servers_dbtable = {
    .name    = "stat-servers",
    .newrec  = alloc_serverstats,
    .freerec = (void *)free_serverstats,
    .insert  = (void *)add_serverstats,
    .first   = (void *)first_serverstats,
    .next    = (void *)next_serverstats,
    .fields  = stat_servers_dbfields,
};

/*************************************************************************/
/****************************** Statistics *******************************/
/*************************************************************************/

/* Introduce the StatServ pseudoclient. */

static int introduce_statserv(const char *nick)
{
    if (!nick || irc_stricmp(nick, s_StatServ) == 0) {
        send_pseudo_nick(s_StatServ, desc_StatServ, PSEUDO_INVIS);
        if (nick)
            return 1;
    }
    return 0;
}

/*************************************************************************/

/* Main StatServ routine. */

static int statserv(const char *source, const char *target, char *buf)
{
    const char *cmd;
    const char *s;
    User *u;

    if (irc_stricmp(target, s_StatServ) != 0)
        return 0;

    u = get_user(source);
    if (!u) {
        module_log("user record for %s not found", source);
        notice(s_StatServ, source, getstring(NULL,INTERNAL_ERROR));
        return 1;
    }

    if (SSOpersOnly && !is_oper(u)) {
        notice_lang(s_StatServ, u, ACCESS_DENIED);
        return 1;
    }

    cmd = strtok(buf, " ");
    if (!cmd) {
        return 1;
    } else if (stricmp(cmd, "\1PING") == 0) {
        if (!(s = strtok_remaining()))
            s = "\1";
        notice(s_StatServ, source, "\1PING %s", s);
    } else {
        if (call_callback_2(cb_command, u, cmd) <= 0)
            run_cmd(s_StatServ, u, THIS_MODULE, cmd);
    }
    return 1;
}

/*************************************************************************/

/* Return a /WHOIS response for StatServ. */

static int statserv_whois(const char *source, char *who, char *extra)
{
    if (irc_stricmp(who, s_StatServ) != 0)
        return 0;
    send_cmd(ServerName, "311 %s %s %s %s * :%s", source, who,
             ServiceUser, ServiceHost, desc_StatServ);
    send_cmd(ServerName, "312 %s %s %s :%s", source, who,
             ServerName, ServerDesc);
    send_cmd(ServerName, "313 %s %s :is a network service", source, who);
    send_cmd(ServerName, "318 %s %s End of /WHOIS response.", source, who);
    return 1;
}

/*************************************************************************/

/* Callback for NickServ REGISTER/LINK check; we disallow
 * registration/linking of the StatServ pseudoclient nickname.
 */

static int do_reglink_check(const User *u, const char *nick,
                            const char *pass, const char *email)
{
    return irc_stricmp(nick, s_StatServ) == 0;
}

/*************************************************************************/
/************************* Server info display ***************************/
/*************************************************************************/

/* Return a help message. */

static void do_help(User *u)
{
    char *cmd = strtok_remaining();

    if (!cmd) {
        notice_help(s_StatServ, u, STAT_HELP);
    } else if (stricmp(cmd, "COMMANDS") == 0) {
        notice_help(s_StatServ, u, STAT_HELP_COMMANDS);
        call_callback_2(cb_help_cmds, u, 0);
    } else if (call_callback_2(cb_help, u, cmd) > 0) {
        return;
    } else {
        help_cmd(s_StatServ, u, THIS_MODULE, cmd);
    }
}

/*************************************************************************/

static void do_servers(User *u)
{
    ServerStats *ss = NULL;
    const char *cmd = strtok(NULL, " ");
    char *mask = strtok(NULL, " ");
    int count = 0, nservers = 0;

    if (!cmd)
        cmd = "";

    if (stricmp(cmd, "STATS") == 0) {
        ServerStats *ss_lastquit = NULL;
        int onlinecount = 0;
        char lastquit_buf[BUFSIZE];

        for (ss = first_serverstats(); ss; ss = next_serverstats()) {
            nservers++;
            if (ss->t_quit > 0
                && (!ss_lastquit
                    || ss->t_quit > ss_lastquit->t_quit))
                ss_lastquit = ss;
            if (SS_IS_ONLINE(ss))
                onlinecount++;
        }

        notice_lang(s_StatServ, u, STAT_SERVERS_STATS_TOTAL, nservers);
        notice_lang(s_StatServ, u, STAT_SERVERS_STATS_ON_OFFLINE,
                    onlinecount, (onlinecount*100)/nservers,
                    nservers-onlinecount,
                    ((nservers-onlinecount)*100)/nservers);
        if (ss_lastquit) {
            strftime_lang(lastquit_buf, sizeof(lastquit_buf), u->ngi,
                          STRFTIME_DATE_TIME_FORMAT, ss_lastquit->t_quit);
            notice_lang(s_StatServ, u, STAT_SERVERS_LASTQUIT_WAS,
                        ss_lastquit->name, lastquit_buf);
        }


    } else if (stricmp(cmd, "LIST") == 0) {
        int matchcount = 0;

        notice_lang(s_StatServ, u, STAT_SERVERS_LIST_HEADER);
        for (ss = first_serverstats(); ss; ss = next_serverstats()) {
            if (mask && !match_wild_nocase(mask, ss->name))
                continue;
            matchcount++;
            if (!SS_IS_ONLINE(ss))
                continue;
            count++;
            notice_lang(s_StatServ, u, STAT_SERVERS_LIST_FORMAT,
                   ss->name, ss->usercnt,
                   !usercnt ? 0 : (ss->usercnt*100)/usercnt,
                   ss->opercnt,
                   !opcnt ? 0 : (ss->opercnt*100)/opcnt);
        }
        notice_lang(s_StatServ, u, STAT_SERVERS_LIST_RESULTS,
                        count, matchcount);

    } else if (stricmp(cmd, "VIEW") == 0) {
        char *param = strtok(NULL, " ");
        char join_buf[BUFSIZE];
        char quit_buf[BUFSIZE];
        int is_online;
        int limitto = 0;        /* 0 == none; 1 == online; 2 == offline */

        if (param) {
            if (stricmp(param, "ONLINE") == 0) {
                limitto = 1;
            } else if (stricmp(param, "OFFLINE") == 0) {
                limitto = 2;
            }
        }

        for (ss = first_serverstats(); ss; ss = next_serverstats()) {
            nservers++;
            if (mask && !match_wild_nocase(mask, ss->name))
                continue;
            is_online = SS_IS_ONLINE(ss);
            if (limitto && !((is_online && limitto == 1) ||
                             (!is_online && limitto == 2)))
                continue;

            count++;
            strftime_lang(join_buf, sizeof(join_buf), u->ngi,
                          STRFTIME_DATE_TIME_FORMAT, ss->t_join);
            if (ss->t_quit != 0) {
                strftime_lang(quit_buf, sizeof(quit_buf), u->ngi,
                              STRFTIME_DATE_TIME_FORMAT, ss->t_quit);
            }

            notice_lang(s_StatServ, u,
                        is_online ? STAT_SERVERS_VIEW_HEADER_ONLINE
                                  : STAT_SERVERS_VIEW_HEADER_OFFLINE,
                        ss->name);
            notice_lang(s_StatServ, u, STAT_SERVERS_VIEW_LASTJOIN, join_buf);
            if (ss->t_quit > 0)
                notice_lang(s_StatServ, u, STAT_SERVERS_VIEW_LASTQUIT,
                            quit_buf);
            if (ss->quit_message)
                notice_lang(s_StatServ, u, STAT_SERVERS_VIEW_QUITMSG,
                            ss->quit_message);
            if (is_online)
                notice_lang(s_StatServ, u, STAT_SERVERS_VIEW_USERS_OPERS,
                            ss->usercnt,
                            !usercnt ? 0 : (ss->usercnt*100)/usercnt,
                            ss->opercnt,
                            !opcnt ? 0 : (ss->opercnt*100)/opcnt);
        }
        notice_lang(s_StatServ, u, STAT_SERVERS_VIEW_RESULTS, count, nservers);

    } else if (!is_services_admin(u)) {
        if (is_oper(u))
            notice_lang(s_StatServ, u, PERMISSION_DENIED);
        else
            syntax_error(s_StatServ, u, "SERVERS", STAT_SERVERS_SYNTAX);

    /* Only Services admins have access from here on! */

    } else if (stricmp(cmd, "DELETE") == 0) {
        if (!mask) {
            syntax_error(s_StatServ, u, "SERVERS", STAT_SERVERS_DELETE_SYNTAX);
        } else if (!(ss = get_serverstats(mask))) {
            notice_lang(s_StatServ, u, SERV_X_NOT_FOUND, mask);
        } else if (SS_IS_ONLINE(ss)) {
            notice_lang(s_StatServ, u, STAT_SERVERS_REMOVE_SERV_FIRST, mask);
        } else {
            del_serverstats(ss);
            ss = NULL;
            notice_lang(s_StatServ, u, STAT_SERVERS_DELETE_DONE, mask);
        }

    } else if (stricmp(cmd, "COPY") == 0) {
        const char *newname = strtok(NULL, " ");
        ServerStats *newss;
        if (!mask || !newname) {
            syntax_error(s_StatServ, u, "SERVERS", STAT_SERVERS_COPY_SYNTAX);
        } else if (!(ss = get_serverstats(mask))) {
            notice_lang(s_StatServ, u, SERV_X_NOT_FOUND, mask);
        } else if ((newss = get_serverstats(newname)) != NULL) {
            put_serverstats(newss);
            notice_lang(s_StatServ, u, STAT_SERVERS_SERVER_EXISTS, newname);
        } else {
            newss = new_serverstats(newname);
            newss->t_join = ss->t_join;
            newss->t_quit = ss->t_quit;
            if (ss->quit_message) {
                newss->quit_message = sstrdup(ss->quit_message);
            }
            add_serverstats(newss);
            put_serverstats(newss);
            notice_lang(s_StatServ, u, STAT_SERVERS_COPY_DONE, mask, newname);
        }

    } else if (stricmp(cmd, "RENAME") == 0) {
        const char *newname = strtok(NULL, " ");
        ServerStats *newss;
        if (!mask || !newname) {
            syntax_error(s_StatServ, u, "SERVERS", STAT_SERVERS_RENAME_SYNTAX);
        } else if (!(ss = get_serverstats(mask))) {
            notice_lang(s_StatServ, u, SERV_X_NOT_FOUND, mask);
        } else if ((newss = get_serverstats(newname)) != NULL) {
            put_serverstats(newss);
            notice_lang(s_StatServ, u, STAT_SERVERS_SERVER_EXISTS, newname);
        } else if (SS_IS_ONLINE(ss)) {
            notice_lang(s_StatServ, u, STAT_SERVERS_REMOVE_SERV_FIRST, mask);
        } else {
            newss = new_serverstats(newname);
            newss->t_join = ss->t_join;
            newss->t_quit = ss->t_quit;
            if (ss->quit_message) {
                newss->quit_message = sstrdup(ss->quit_message);
            }
            del_serverstats(ss);
            ss = NULL;
            add_serverstats(newss);
            put_serverstats(newss);
            notice_lang(s_StatServ, u, STAT_SERVERS_RENAME_DONE,
                        mask, newname);
        }

    } else {
        syntax_error(s_StatServ, u, "SERVERS", STAT_SERVERS_SYNTAX);
    }

    put_serverstats(ss);
}

/*************************************************************************/

static void do_users(User *u)
{
    const char *cmd = strtok(NULL, " ");
    int avgusers, avgopers;

    if (!cmd)
        cmd = "";

    if (stricmp(cmd, "STATS") == 0) {
        notice_lang(s_StatServ, u, STAT_USERS_TOTUSERS, usercnt);
        notice_lang(s_StatServ, u, STAT_USERS_TOTOPERS, opcnt);
        avgusers = (usercnt + servercnt/2) / servercnt;
        avgopers = (opcnt*10 + servercnt/2) / servercnt;
        notice_lang(s_StatServ, u, STAT_USERS_SERVUSERS, avgusers);
        notice_lang(s_StatServ, u, STAT_USERS_SERVOPERS,
               avgopers/10, avgopers%10);
    } else {
        syntax_error(s_StatServ, u, "USERS", STAT_USERS_SYNTAX);
    }
}

/*************************************************************************/
/******************** ServerStats new/free (global) **********************/
/*************************************************************************/

/* Create a new ServerStats structure for the given server name and return
 * it.  Always successful.
 */

EXPORT_FUNC(new_serverstats)
ServerStats *new_serverstats(const char *servername)
{
    ServerStats *ss = alloc_serverstats();
    if (ss)
        ss->name = sstrdup(servername);
    return ss;
}

/*************************************************************************/

/* Free a ServerStats structure and associated data. */

EXPORT_FUNC(free_serverstats)
void free_serverstats(ServerStats *ss)
{
    free(ss->name);
    free(ss->quit_message);
    free(ss);
}

/*************************************************************************/
/************************** Callback routines ****************************/
/*************************************************************************/

/* Handle a server joining. */

static int stats_do_server(Server *server)
{
    ServerStats *ss;

    servercnt++;

    ss = get_serverstats(server->name);
    if (ss) {
        /* Server has rejoined us */
        ss->usercnt = 0;
        ss->opercnt = 0;
        ss->t_join = time(NULL);
    } else {
        /* Totally new server */
        ss = new_serverstats(server->name);  /* cleared to zero */
        ss->t_join = time(NULL);
        add_serverstats(ss);
    }

    server->stats = ss;
    return 0;
}

/*************************************************************************/

/* Handle a server quitting. */

static int stats_do_squit(Server *server, const char *quit_message)
{
    ServerStats *ss = server->stats;

    servercnt--;
    ss->t_quit = time(NULL);
    free(ss->quit_message);
    ss->quit_message = *quit_message ? sstrdup(quit_message) : NULL;
    put_serverstats(ss);
    return 0;
}

/*************************************************************************/

/* Handle a user joining. */

static int stats_do_newuser(User *user)
{
    if (user->server)
        user->server->stats->usercnt++;
    return 0;
}

/*************************************************************************/

/* Handle a user quitting. */

static int stats_do_quit(User *user)
{
    if (user->server) {
        ServerStats *ss = user->server->stats;
        if (!ss) {
            module_log("BUG! no serverstats for %s in do_quit(%s)",
                       user->server->name, user->nick);
            return 0;
        }
        ss->usercnt--;
        if (is_oper(user))
            ss->opercnt--;
    }
    return 0;
}

/*************************************************************************/

/* Handle a user mode change. */

static int stats_do_umode(User *user, int modechar, int add)
{
    if (user->server) {
        if (modechar == 'o') {
            ServerStats *ss = user->server->stats;
            if (!ss) {
                module_log("BUG! no serverstats for %s in do_quit(%s)",
                           user->server->name, user->nick);
                return 0;
            }
            if (add)
                ss->opercnt++;
            else
                ss->opercnt--;
        }
    }
    return 0;
}

/*************************************************************************/

/* OperServ STATS ALL callback. */

static int do_stats_all(User *user, const char *s_OperServ)
{
    int32 count, mem;
    ServerStats *ss;

    count = mem = 0;
    for (ss = first_serverstats(); ss; ss = next_serverstats()) {
        count++;
        mem += sizeof(*ss) + strlen(ss->name)+1;
        if (ss->quit_message)
            mem += strlen(ss->quit_message)+1;
    }
    notice_lang(s_OperServ, user, OPER_STATS_ALL_STATSERV_MEM,
                count, (mem+512) / 1024);

    return 0;
}

/*************************************************************************/
/**************************** Module functions ***************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "SSOpersOnly",      { { CD_SET, 0, &SSOpersOnly } } },
    { "StatServName",     { { CD_STRING, CF_DIRREQ, &s_StatServ },
                            { CD_STRING, 0, &desc_StatServ } } },
    { NULL }
};

/*************************************************************************/

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "operserv/main") == 0) {
        module_operserv = mod;
        if (!add_callback(mod, "STATS ALL", do_stats_all))
            module_log("Unable to register OperServ STATS ALL callback");
    }
    if (strcmp(modname, "nickserv/main") == 0) {
        module_nickserv = mod;
        if (!add_callback(mod, "REGISTER/LINK check", do_reglink_check))
            module_log("Unable to register NickServ REGISTER/LINK check"
                       " callback");
    }
    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_operserv) {
        remove_callback(mod, "STATS ALL", do_stats_all);
        module_operserv = NULL;
    }
    if (mod == module_nickserv) {
        remove_callback(mod, "REGISTER/LINK check", do_reglink_check);
        module_nickserv = NULL;
    }
    return 0;
}

/*************************************************************************/

static int do_reconfigure(int after_configure)
{
    static char old_s_StatServ[NICKMAX];
    static char *old_desc_StatServ = NULL;

    if (!after_configure) {
        /* Before reconfiguration: save old values. */
        strbcpy(old_s_StatServ, s_StatServ);
        old_desc_StatServ = strdup(desc_StatServ);
    } else {
        /* After reconfiguration: handle value changes. */
        if (strcmp(old_s_StatServ, s_StatServ) != 0)
            send_nickchange(old_s_StatServ, s_StatServ);
        if (!old_desc_StatServ || strcmp(old_desc_StatServ,desc_StatServ) != 0)
            send_namechange(s_StatServ, desc_StatServ);
        free(old_desc_StatServ);
        old_desc_StatServ = NULL;
    }  /* if (!after_configure) */
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    Module *tmpmod;


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
    if (cb_command < 0 || cb_help < 0 || cb_help_cmds < 0) {
        module_log("Unable to register callbacks");
        exit_module(0);
        return 0;
    }

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "reconfigure", do_reconfigure)
     || !add_callback(NULL, "introduce_user", introduce_statserv)
     || !add_callback(NULL, "m_privmsg", statserv)
     || !add_callback(NULL, "m_whois", statserv_whois)
     || !add_callback(NULL, "server create", stats_do_server)
     || !add_callback(NULL, "server delete", stats_do_squit)
     || !add_callback(NULL, "user create", stats_do_newuser)
     || !add_callback(NULL, "user delete", stats_do_quit)
     || !add_callback(NULL, "user MODE", stats_do_umode)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    tmpmod = find_module("nickserv/main");
    if (tmpmod)
        do_load_module(tmpmod, "nickserv/main");

    if (!register_dbtable(&stat_servers_dbtable)) {
        module_log("Unable to register database table");
        exit_module(0);
        return 0;
    }

    if (linked)
        introduce_statserv(NULL);

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (linked)
        send_cmd(s_StatServ, "QUIT :");

    unregister_dbtable(&stat_servers_dbtable);
    clean_dbtables();

    if (module_nickserv)
        do_unload_module(module_nickserv);
    if (module_operserv)
        do_unload_module(module_operserv);

    remove_callback(NULL, "user MODE", stats_do_umode);
    remove_callback(NULL, "user delete", stats_do_quit);
    remove_callback(NULL, "user create", stats_do_newuser);
    remove_callback(NULL, "server delete", stats_do_squit);
    remove_callback(NULL, "server create", stats_do_server);
    remove_callback(NULL, "m_whois", statserv_whois);
    remove_callback(NULL, "m_privmsg", statserv);
    remove_callback(NULL, "introduce_user", introduce_statserv);
    remove_callback(NULL, "reconfigure", do_reconfigure);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

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
