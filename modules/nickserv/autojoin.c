/* NickServ auto-join module.
 * Written by Yusuf Iskenderoglu <uhc0@stud.uni-karlsruhe.de>
 * Idea taken from PTlink Services.
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
#include "nickserv.h"
#include "modules/operserv/operserv.h"
#include "modules/chanserv/chanserv.h"

/*************************************************************************/

static Module *module_nickserv;
static Module *module_chanserv;
static typeof(check_access_cmd) *check_access_cmd_p;

static int cb_send_svsjoin = -1;

static int NSAutojoinMax;

/*************************************************************************/

static void do_ajoin(User *u);

static Command cmds[] = {
    { "AJOIN",       do_ajoin, NULL,  NICK_HELP_AJOIN,
                -1, NICK_OPER_HELP_AJOIN },
    { NULL }
};

/*************************************************************************/
/**************************** Database stuff *****************************/
/*************************************************************************/

/*
 * Implementation note:
 *
 * Ideally, we would implement the autojoin database as e.g. a hash table
 * completely separate from the nickgroup table, with each record
 * consisting of a nickgroup ID and array of channels; in fact, if we had
 * a working SELECT-like function, we could just have one channel per
 * record and iterate through all matches for a particular ID.  Aside from
 * having to hook into the nickgroup delete function to ensure thaat
 * autojoin records are deleted along with nickgroup data, this would make
 * the autojoin database completely independent.
 *
 * However, saving autojoin data using the database/version4 module would
 * no longer work correctly, because the database code would have no easy
 * way to know whether the table even existed, and would also require the
 * table's get() function to be exported from this module (or else have to
 * use first()/next() for _every_ nickgroup to find the appropriate
 * autojoin data, if any).
 *
 * For this reason, the ajoin[] array has been left in the NickGroupInfo
 * structure in this version.  A proper database implementation is left as
 * an exercise for the reader.
 */

/*************************************************************************/

/* Temporary structure for loading/saving autojoin records */
typedef struct {
    uint32 nickgroup;
    char *channel;
} DBRecord;
static DBRecord dbrec_static;

/* Iterators for first/next routines */
static NickGroupInfo *db_ngi_iterator;
static int db_array_iterator;

/*************************************************************************/

/* Table access routines */

static void *new_autojoin(void)
{
    return memset(&dbrec_static, 0, sizeof(dbrec_static));
}

static void free_autojoin(void *record)
{
    free(((DBRecord *)record)->channel);
}

static void insert_autojoin(void *record)
{
    DBRecord *dbrec = record;
    NickGroupInfo *ngi = get_nickgroupinfo(dbrec->nickgroup);
    if (!ngi) {
        module_log("Discarding autojoin record for missing nickgroup %u: %s",
                   dbrec->nickgroup, dbrec->channel);
        free_autojoin(record);
    } else {
        ARRAY_EXTEND(ngi->ajoin);
        ngi->ajoin[ngi->ajoin_count-1] = dbrec->channel;
    }
}

static void *next_autojoin(void)
{
    while (db_ngi_iterator
        && db_array_iterator >= db_ngi_iterator->ajoin_count
    ) {
        db_ngi_iterator = next_nickgroupinfo();
        db_array_iterator = 0;
    }
    if (db_ngi_iterator) {
        dbrec_static.nickgroup = db_ngi_iterator->id;
        dbrec_static.channel = db_ngi_iterator->ajoin[db_array_iterator++];
        return &dbrec_static;
    } else {
        return NULL;
    }
}

static void *first_autojoin(void)
{
    db_ngi_iterator = first_nickgroupinfo();
    db_array_iterator = 0;
    return next_autojoin();
}

/*************************************************************************/

/* Database table definition */

#define FIELD(name,type,...) \
    { #name, type, offsetof(DBRecord,name) , ##__VA_ARGS__ }

static DBField autojoin_dbfields[] = {
    FIELD(nickgroup, DBTYPE_UINT32),
    FIELD(channel,   DBTYPE_STRING),
    { NULL }
};

static DBTable autojoin_dbtable = {
    .name    = "nick-autojoin",
    .newrec  = new_autojoin,
    .freerec = free_autojoin,
    .insert  = insert_autojoin,
    .first   = first_autojoin,
    .next    = next_autojoin,
    .fields  = autojoin_dbfields,
};

#undef FIELD

/*************************************************************************/
/******************************* Callbacks *******************************/
/*************************************************************************/

/* Send autojoin commands for an identified user. */

static int do_identified(User *u, int unused)
{
    NickGroupInfo *ngi = u->ngi;
    int i;

    ARRAY_FOREACH (i, ngi->ajoin) {
        struct u_chanlist *uc;
        if (!valid_chan(ngi->ajoin[i])) {
            notice_lang(s_NickServ, u, NICK_AJOIN_AUTO_REMOVE, ngi->ajoin[i]);
            free(ngi->ajoin[i]);
            ARRAY_REMOVE(ngi->ajoin, i);
            i--;
            continue;
        }
        LIST_SEARCH(u->chans, chan->name, ngi->ajoin[i], irc_stricmp, uc);
        if (!uc) {
            Channel *c = get_channel(ngi->ajoin[i]);
            if (c && (c->mode & CMODE_i)
             && c->ci && check_access_cmd_p
             && (*check_access_cmd_p)(u, c->ci, "INVITE", NULL) > 0
            ) {
                send_cmd(s_NickServ, "INVITE %s %s", u->nick, ngi->ajoin[i]);
            }
            call_callback_2(cb_send_svsjoin, u->nick, ngi->ajoin[i]);
        }
    }
    return 0;
}

/*************************************************************************/

/* Handle autojoin help. */

static int do_help(User *u, const char *param)
{
    if (stricmp(param, "AJOIN") == 0) {
        Module *mod;
        notice_help(s_NickServ, u, NICK_HELP_AJOIN);
        if ((mod = find_module("chanserv/main")) != NULL) {
            const char *my_s_ChanServ;
            const char **ptr = get_module_symbol(mod, "s_ChanServ");
            if (ptr) {
                my_s_ChanServ = *ptr;
            } else {
                static int warned = 0;
                if (!warned) {
                    module_log("HELP AJOIN: cannot retrieve symbol"
                               " `s_ChanServ' from module `chanserv/main'");
                    warned = 1;
                }
                my_s_ChanServ = "ChanServ";
            }
            notice_help(s_NickServ, u, NICK_HELP_AJOIN_END_CHANSERV,
                        my_s_ChanServ);
        } else {
            notice_help(s_NickServ, u, NICK_HELP_AJOIN_END);
        }
        return 1;
    }
    return 0;
}

/*************************************************************************/
/*************************** Command functions ***************************/
/*************************************************************************/

void do_ajoin(User *u)
{
    char *cmd = strtok(NULL, " ");
    char *chan = strtok(NULL, " ");
    NickGroupInfo *ngi = u->ngi;
    int i;

    if (cmd && chan && stricmp(cmd,"LIST") == 0 && is_services_admin(u)) {
        NickInfo *ni = get_nickinfo(chan);
        ngi = NULL;
        if (!ni) {
            notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, chan);
        } else if (ni->status & NS_VERBOTEN) {
            notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, chan);
        } else if (!(ngi = get_ngi(ni))) {
            notice_lang(s_NickServ, u, INTERNAL_ERROR);
        } else if (!ngi->ajoin_count) {
            notice_lang(s_NickServ, u, NICK_AJOIN_LIST_X_EMPTY, chan);
        } else {
            notice_lang(s_NickServ, u, NICK_AJOIN_LIST_X, chan);
            ARRAY_FOREACH (i, ngi->ajoin)
                notice(s_NickServ, u->nick, "    %s", ngi->ajoin[i]);
        }
        put_nickinfo(ni);
        put_nickgroupinfo(ngi);

    } else if (!cmd || ((stricmp(cmd,"LIST")==0) && chan)) {
        syntax_error(s_NickServ, u, "AJOIN", NICK_AJOIN_SYNTAX);

    } else if (!valid_ngi(u)) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);

    } else if (!user_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);

    } else if (stricmp(cmd, "ADD") == 0) {
        if (readonly) {
            notice_lang(s_NickServ, u, NICK_AJOIN_DISABLED);
            return;
        }
        if (!chan || *chan != '#') {
            syntax_error(s_NickServ, u, "AJOIN", NICK_AJOIN_ADD_SYNTAX);
            return;
        }
        if (!valid_chan(chan)) {
            notice_lang(s_NickServ, u, CHAN_INVALID, chan);
            return;
        }
        if (ngi->ajoin_count + 1 > NSAutojoinMax) {
            notice_lang(s_NickServ, u, NICK_AJOIN_LIST_FULL, NSAutojoinMax);
            return;
        }
        ARRAY_FOREACH (i, ngi->ajoin) {
            if (stricmp(ngi->ajoin[i], chan) == 0) {
                notice_lang(s_NickServ, u,
                        NICK_AJOIN_ALREADY_PRESENT, ngi->ajoin[i]);
                return;
            }
        }
        ARRAY_EXTEND(ngi->ajoin);
        ngi->ajoin[ngi->ajoin_count-1] = sstrdup(chan);
        notice_lang(s_NickServ, u, NICK_AJOIN_ADDED, chan);

   } else if (stricmp(cmd, "DEL") == 0) {
        if (readonly) {
            notice_lang(s_NickServ, u, NICK_AJOIN_DISABLED);
            return;
        }
        if (!chan || *chan != '#') {
            syntax_error(s_NickServ, u, "AJOIN", NICK_AJOIN_DEL_SYNTAX);
            return;
        }
        ARRAY_SEARCH_PLAIN(ngi->ajoin, chan, strcmp, i);
        if (i == ngi->ajoin_count)
            ARRAY_SEARCH_PLAIN(ngi->ajoin, chan, irc_stricmp, i);
        if (i == ngi->ajoin_count) {
            notice_lang(s_NickServ, u, NICK_AJOIN_NOT_FOUND, chan);
            return;
        }
        free(ngi->ajoin[i]);
        ARRAY_REMOVE(ngi->ajoin, i);
        notice_lang(s_NickServ, u, NICK_AJOIN_DELETED, chan);

    } else if (stricmp(cmd, "LIST") == 0) {
        if (!ngi->ajoin_count) {
            notice_lang(s_NickServ, u, NICK_AJOIN_LIST_EMPTY);
        } else {
            notice_lang(s_NickServ, u, NICK_AJOIN_LIST);
            ARRAY_FOREACH (i, ngi->ajoin)
                notice(s_NickServ, u->nick, "    %s", ngi->ajoin[i]);
        }

    } else {
        syntax_error(s_NickServ, u, "AJOIN", NICK_AJOIN_SYNTAX);
    }
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "NSAutojoinMax",   { { CD_POSINT, CF_DIRREQ, &NSAutojoinMax } } },
    { NULL }
};

/*************************************************************************/

static int do_load_module(Module *mod, const char *name)
{
    if (strcmp(name,"chanserv/main") == 0) {
        module_chanserv = mod;
        if (!(check_access_cmd_p = get_module_symbol(mod,"check_access_cmd"))){
            module_log("Symbol `check_access_cmd' not found, auto-inviting"
                       " disabled");
        }
    }
    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_chanserv) {
        check_access_cmd_p = NULL;
        module_chanserv = NULL;
    }
    return 0;
}

/*************************************************************************/

int init_module()
{
    Module *mod;


    if (!(protocol_features & PF_SVSJOIN)) {
        if (protocol_features & PF_UNSET) {
            module_log("No protocol module loaded--you must load a"
                       " protocol module before loading this module");
        } else {
            module_log("SVSJOIN not supported by this IRC server (%s)",
                       protocol_name);
        }
        return 0;
    }

    module_nickserv = find_module("nickserv/main");
    if (!module_nickserv) {
        module_log("Main NickServ module not loaded");
        return 0;
    }
    use_module(module_nickserv);

    if (!register_commands(module_nickserv, cmds)) {
        module_log("Unable to register commands");
        exit_module(0);
        return 0;
    }

    cb_send_svsjoin = register_callback("send_svsjoin");
    if (cb_send_svsjoin < 0) {
        module_log("Unable to register callback");
        exit_module(0);
        return 0;
    }

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(module_nickserv, "identified", do_identified)
     || !add_callback(module_nickserv, "HELP", do_help)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    if (!register_dbtable(&autojoin_dbtable)) {
        module_log("Unable to register database table");
        exit_module(0);
        return 0;
    }

    mod = find_module("chanserv/main");
    if (mod)
        do_load_module(mod, "chanserv/main");

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (module_chanserv)
        do_unload_module(module_chanserv);

    unregister_dbtable(&autojoin_dbtable);

    if (module_nickserv) {
        remove_callback(module_nickserv, "HELP", do_help);
        remove_callback(module_nickserv, "identified", do_identified);
        unregister_commands(module_nickserv, cmds);
        unuse_module(module_nickserv);
        module_nickserv = NULL;
    }

    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

    unregister_callback(cb_send_svsjoin);

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
