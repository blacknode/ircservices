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

#include "modules/statserv/statserv.h"

/*************************************************************************/

static Event* command_event;
static Event* help_event;
static Event* help_cmds_event;

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
#undef HASH_MODIFY_STATIC
#define HASH_MODIFY_STATIC static
#define add_serverstats   _add_serverstats
#define del_serverstats   _del_serverstats
#include "hash.h"
DEFINE_HASH(serverstats, ServerStats, name);
#undef add_serverstats
#undef del_serverstats
#undef HASH_MODIFY_STATIC
#define HASH_MODIFY_STATIC HASH_STATIC

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

/* Main StatServ routine: a PRIVMSG to StatServ. */

static void statserv_message(struct Service *service, User *u, char *buf)
{
    const char *cmd;

    if (SSOpersOnly && !is_oper(u)) {
        notice_lang(service->nick, u, ACCESS_DENIED);
        return;
    }

    cmd = strtok(buf, " ");
    if (cmd && event_emit(command_event, u, cmd) <= 0)
        run_cmd(service->nick, u, THIS_MODULE, cmd);
}

/*************************************************************************/
/************************* Server info display ***************************/
/*************************************************************************/

/* Return a help message. */

static void do_help(User *u)
{
    char *cmd = strtok_remaining();

    if (!cmd) {
        notice_help(statserv_service.nick, u, STAT_HELP);
    } else if (stricmp(cmd, "COMMANDS") == 0) {
        notice_help(statserv_service.nick, u, STAT_HELP_COMMANDS);
        event_emit(help_cmds_event, u, 0);
    } else if (event_emit(help_event, u, cmd) > 0) {
        return;
    } else {
        help_cmd(statserv_service.nick, u, THIS_MODULE, cmd);
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

        notice_lang(statserv_service.nick, u, STAT_SERVERS_STATS_TOTAL, nservers);
        notice_lang(statserv_service.nick, u, STAT_SERVERS_STATS_ON_OFFLINE,
                    onlinecount, (onlinecount*100)/nservers,
                    nservers-onlinecount,
                    ((nservers-onlinecount)*100)/nservers);
        if (ss_lastquit) {
            strftime_lang(lastquit_buf, sizeof(lastquit_buf), u->ngi,
                          STRFTIME_DATE_TIME_FORMAT, ss_lastquit->t_quit);
            notice_lang(statserv_service.nick, u, STAT_SERVERS_LASTQUIT_WAS,
                        ss_lastquit->name, lastquit_buf);
        }


    } else if (stricmp(cmd, "LIST") == 0) {
        int matchcount = 0;

        notice_lang(statserv_service.nick, u, STAT_SERVERS_LIST_HEADER);
        for (ss = first_serverstats(); ss; ss = next_serverstats()) {
            if (mask && !match_wild_nocase(mask, ss->name))
                continue;
            matchcount++;
            if (!SS_IS_ONLINE(ss))
                continue;
            count++;
            notice_lang(statserv_service.nick, u, STAT_SERVERS_LIST_FORMAT,
                   ss->name, ss->usercnt,
                   !usercnt ? 0 : (ss->usercnt*100)/usercnt,
                   ss->opercnt,
                   !opcnt ? 0 : (ss->opercnt*100)/opcnt);
        }
        notice_lang(statserv_service.nick, u, STAT_SERVERS_LIST_RESULTS,
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

            notice_lang(statserv_service.nick, u,
                        is_online ? STAT_SERVERS_VIEW_HEADER_ONLINE
                                  : STAT_SERVERS_VIEW_HEADER_OFFLINE,
                        ss->name);
            notice_lang(statserv_service.nick, u, STAT_SERVERS_VIEW_LASTJOIN, join_buf);
            if (ss->t_quit > 0)
                notice_lang(statserv_service.nick, u, STAT_SERVERS_VIEW_LASTQUIT,
                            quit_buf);
            if (ss->quit_message)
                notice_lang(statserv_service.nick, u, STAT_SERVERS_VIEW_QUITMSG,
                            ss->quit_message);
            if (is_online)
                notice_lang(statserv_service.nick, u, STAT_SERVERS_VIEW_USERS_OPERS,
                            ss->usercnt,
                            !usercnt ? 0 : (ss->usercnt*100)/usercnt,
                            ss->opercnt,
                            !opcnt ? 0 : (ss->opercnt*100)/opcnt);
        }
        notice_lang(statserv_service.nick, u, STAT_SERVERS_VIEW_RESULTS, count, nservers);

    } else if (!is_services_admin(u)) {
        if (is_oper(u))
            notice_lang(statserv_service.nick, u, PERMISSION_DENIED);
        else
            syntax_error(statserv_service.nick, u, "SERVERS", STAT_SERVERS_SYNTAX);

    /* Only Services admins have access from here on! */

    } else if (stricmp(cmd, "DELETE") == 0) {
        if (!mask) {
            syntax_error(statserv_service.nick, u, "SERVERS", STAT_SERVERS_DELETE_SYNTAX);
        } else if (!(ss = get_serverstats(mask))) {
            notice_lang(statserv_service.nick, u, SERV_X_NOT_FOUND, mask);
        } else if (SS_IS_ONLINE(ss)) {
            notice_lang(statserv_service.nick, u, STAT_SERVERS_REMOVE_SERV_FIRST, mask);
        } else {
            del_serverstats(ss);
            ss = NULL;
            notice_lang(statserv_service.nick, u, STAT_SERVERS_DELETE_DONE, mask);
        }

    } else if (stricmp(cmd, "COPY") == 0) {
        const char *newname = strtok(NULL, " ");
        ServerStats *newss;
        if (!mask || !newname) {
            syntax_error(statserv_service.nick, u, "SERVERS", STAT_SERVERS_COPY_SYNTAX);
        } else if (!(ss = get_serverstats(mask))) {
            notice_lang(statserv_service.nick, u, SERV_X_NOT_FOUND, mask);
        } else if ((newss = get_serverstats(newname)) != NULL) {
            put_serverstats(newss);
            notice_lang(statserv_service.nick, u, STAT_SERVERS_SERVER_EXISTS, newname);
        } else {
            newss = new_serverstats(newname);
            newss->t_join = ss->t_join;
            newss->t_quit = ss->t_quit;
            if (ss->quit_message) {
                newss->quit_message = sstrdup(ss->quit_message);
            }
            add_serverstats(newss);
            put_serverstats(newss);
            notice_lang(statserv_service.nick, u, STAT_SERVERS_COPY_DONE, mask, newname);
        }

    } else if (stricmp(cmd, "RENAME") == 0) {
        const char *newname = strtok(NULL, " ");
        ServerStats *newss;
        if (!mask || !newname) {
            syntax_error(statserv_service.nick, u, "SERVERS", STAT_SERVERS_RENAME_SYNTAX);
        } else if (!(ss = get_serverstats(mask))) {
            notice_lang(statserv_service.nick, u, SERV_X_NOT_FOUND, mask);
        } else if ((newss = get_serverstats(newname)) != NULL) {
            put_serverstats(newss);
            notice_lang(statserv_service.nick, u, STAT_SERVERS_SERVER_EXISTS, newname);
        } else if (SS_IS_ONLINE(ss)) {
            notice_lang(statserv_service.nick, u, STAT_SERVERS_REMOVE_SERV_FIRST, mask);
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
            notice_lang(statserv_service.nick, u, STAT_SERVERS_RENAME_DONE,
                        mask, newname);
        }

    } else {
        syntax_error(statserv_service.nick, u, "SERVERS", STAT_SERVERS_SYNTAX);
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
        notice_lang(statserv_service.nick, u, STAT_USERS_TOTUSERS, usercnt);
        notice_lang(statserv_service.nick, u, STAT_USERS_TOTOPERS, opcnt);
        avgusers = (usercnt + servercnt/2) / servercnt;
        avgopers = (opcnt*10 + servercnt/2) / servercnt;
        notice_lang(statserv_service.nick, u, STAT_USERS_SERVUSERS, avgusers);
        notice_lang(statserv_service.nick, u, STAT_USERS_SERVOPERS,
               avgopers/10, avgopers%10);
    } else {
        syntax_error(statserv_service.nick, u, "USERS", STAT_USERS_SYNTAX);
    }
}

/*************************************************************************/
/******************** ServerStats new/free (global) **********************/
/*************************************************************************/

/* Create a new ServerStats structure for the given server name and return
 * it.  Always successful.
 */

ServerStats *new_serverstats(const char *servername)
{
    ServerStats *ss = alloc_serverstats();
    if (ss)
        ss->name = sstrdup(servername);
    return ss;
}

/*************************************************************************/

/* Free a ServerStats structure and associated data. */

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

/* OperServ STATS ALL handler. */

static int do_stats_all(User *user, const char *operserv_nick)
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
    notice_lang(operserv_nick, user, OPER_STATS_ALL_STATSERV_MEM,
                count, (mem+512) / 1024);

    return 0;
}

/*************************************************************************/
/**************************** Module functions ***************************/
/*************************************************************************/

/* StatServName = <nick>, <description>; in the module block. */
struct Service statserv_service = {
    .directive = "StatServName",
    .flags = SERVICE_INVISIBLE,
    .on_message = statserv_message,
};

static ConfigDirective statserv_config[] = {
    { "SSOpersOnly",      { { CD_SET, 0, &SSOpersOnly } } },
    { NULL }
};

/*************************************************************************/

/* EVENT_SERVER_EOB_ACK: the uplink's burst is over, so join the
 * serverinfo channel (see service.h). */
static int do_eob_ack(void)
{
    service_join_channel(&statserv_service);
    return 0;
}

static int statserv_init(Module *module)
{
    if (!new_commandlist(module) || !register_commands(module, cmds)) {
        module_log("Unable to register commands");
        return 0;
    }

    command_event = event_declare(module, STATSERV_EVENT_COMMAND);
    help_event = event_declare(module, STATSERV_EVENT_HELP);
    help_cmds_event = event_declare(module, STATSERV_EVENT_HELP_COMMANDS);
    if (!command_event || !help_event || !help_cmds_event) {
        module_log("Unable to declare events");
        return 0;
    }

    if (!event_attach(module, EVENT_SERVER_EOB_ACK, do_eob_ack)
     || !event_attach(module, EVENT_SERVER_CREATE, stats_do_server)
     || !event_attach(module, EVENT_SERVER_DELETE, stats_do_squit)
     || !event_attach(module, EVENT_USER_CREATE, stats_do_newuser)
     || !event_attach(module, EVENT_USER_DELETE, stats_do_quit)
     || !event_attach(module, EVENT_USER_MODE, stats_do_umode)
     || !event_attach(module, OPERSERV_EVENT_STATS_ALL, do_stats_all)
    ) {
        module_log("Unable to attach event handlers");
        return 0;
    }

    if (!register_dbtable(&stat_servers_dbtable)) {
        module_log("Unable to register database table");
        return 0;
    }

    return 1;
}

/*************************************************************************/

static int statserv_fini(Module *module, int shutdown)
{
    unregister_dbtable(&stat_servers_dbtable);
    clean_dbtables();
    unregister_commands(module, cmds);
    del_commandlist(module);
    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "StatServ: statistics about the network's servers",
    .config = statserv_config,
    .services = MODULE_SERVICES(&statserv_service),
    .init = statserv_init,
    .fini = statserv_fini,
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
