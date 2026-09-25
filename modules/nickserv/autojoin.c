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
#include "modules/nickserv/nickserv.h"
#include "modules/operserv/operserv.h"
#include "modules/chanserv/chanserv.h"

/*************************************************************************/

static Module *module_nickserv;
static Module *module_chanserv;
static typeof(check_access_cmd) *check_access_cmd_p;

static Event* send_svsjoin_event;

static int NSAutojoinMax;

/*************************************************************************/

static void do_ajoin(User *u);

static Command cmds[] = {
    { "AJOIN",       do_ajoin, NULL,  NICK_HELP_AJOIN,
                -1, NICK_OPER_HELP_AJOIN },
    { NULL }
};

/*************************************************************************/

/* The autojoin list of a nickname group is part of the group's record, which
 * nickserv/main keeps in the database (see include/store.h): nothing to
 * load or save here. */

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
            notice_lang(nickserv_service.nick, u, NICK_AJOIN_AUTO_REMOVE, ngi->ajoin[i]);
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
                send_cmd(nickserv_service.nick, "INVITE %s %s", u->nick, ngi->ajoin[i]);
            }
            event_emit(send_svsjoin_event, u->nick, ngi->ajoin[i]);
        }
    }
    return 0;
}

/*************************************************************************/

/* Handle autojoin help. */

static int do_help(User *u, const char *param)
{
    if (stricmp(param, "AJOIN") == 0) {
        struct Service *chanserv = NULL;
        notice_help(nickserv_service.nick, u, NICK_HELP_AJOIN);
        if (module_chanserv)
            chanserv = module_symbol(module_chanserv, "chanserv_service");
        if (chanserv) {
            notice_help(nickserv_service.nick, u, NICK_HELP_AJOIN_END_CHANSERV,
                        chanserv->nick);
        } else {
            notice_help(nickserv_service.nick, u, NICK_HELP_AJOIN_END);
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
            notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, chan);
        } else if (ni->status & NS_VERBOTEN) {
            notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, chan);
        } else if (!(ngi = get_ngi(ni))) {
            notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
        } else if (!ngi->ajoin_count) {
            notice_lang(nickserv_service.nick, u, NICK_AJOIN_LIST_X_EMPTY, chan);
        } else {
            notice_lang(nickserv_service.nick, u, NICK_AJOIN_LIST_X, chan);
            ARRAY_FOREACH (i, ngi->ajoin)
                notice(nickserv_service.nick, u->nick, "    %s", ngi->ajoin[i]);
        }
        put_nickinfo(ni);
        put_nickgroupinfo(ngi);

    } else if (!cmd || ((stricmp(cmd,"LIST")==0) && chan)) {
        syntax_error(nickserv_service.nick, u, "AJOIN", NICK_AJOIN_SYNTAX);

    } else if (!valid_ngi(u)) {
        notice_lang(nickserv_service.nick, u, NICK_NOT_REGISTERED);

    } else if (!user_identified(u)) {
        notice_lang(nickserv_service.nick, u, NICK_IDENTIFY_REQUIRED, nickserv_service.nick);

    } else if (stricmp(cmd, "ADD") == 0) {
        if (readonly) {
            notice_lang(nickserv_service.nick, u, NICK_AJOIN_DISABLED);
            return;
        }
        if (!chan || *chan != '#') {
            syntax_error(nickserv_service.nick, u, "AJOIN", NICK_AJOIN_ADD_SYNTAX);
            return;
        }
        if (!valid_chan(chan)) {
            notice_lang(nickserv_service.nick, u, CHAN_INVALID, chan);
            return;
        }
        if (ngi->ajoin_count + 1 > NSAutojoinMax) {
            notice_lang(nickserv_service.nick, u, NICK_AJOIN_LIST_FULL, NSAutojoinMax);
            return;
        }
        ARRAY_FOREACH (i, ngi->ajoin) {
            if (stricmp(ngi->ajoin[i], chan) == 0) {
                notice_lang(nickserv_service.nick, u,
                        NICK_AJOIN_ALREADY_PRESENT, ngi->ajoin[i]);
                return;
            }
        }
        ARRAY_EXTEND(ngi->ajoin);
        ngi->ajoin[ngi->ajoin_count-1] = sstrdup(chan);
        notice_lang(nickserv_service.nick, u, NICK_AJOIN_ADDED, chan);

   } else if (stricmp(cmd, "DEL") == 0) {
        if (readonly) {
            notice_lang(nickserv_service.nick, u, NICK_AJOIN_DISABLED);
            return;
        }
        if (!chan || *chan != '#') {
            syntax_error(nickserv_service.nick, u, "AJOIN", NICK_AJOIN_DEL_SYNTAX);
            return;
        }
        ARRAY_SEARCH_PLAIN(ngi->ajoin, chan, strcmp, i);
        if (i == ngi->ajoin_count)
            ARRAY_SEARCH_PLAIN(ngi->ajoin, chan, irc_stricmp, i);
        if (i == ngi->ajoin_count) {
            notice_lang(nickserv_service.nick, u, NICK_AJOIN_NOT_FOUND, chan);
            return;
        }
        free(ngi->ajoin[i]);
        ARRAY_REMOVE(ngi->ajoin, i);
        notice_lang(nickserv_service.nick, u, NICK_AJOIN_DELETED, chan);

    } else if (stricmp(cmd, "LIST") == 0) {
        if (!ngi->ajoin_count) {
            notice_lang(nickserv_service.nick, u, NICK_AJOIN_LIST_EMPTY);
        } else {
            notice_lang(nickserv_service.nick, u, NICK_AJOIN_LIST);
            ARRAY_FOREACH (i, ngi->ajoin)
                notice(nickserv_service.nick, u->nick, "    %s", ngi->ajoin[i]);
        }

    } else {
        syntax_error(nickserv_service.nick, u, "AJOIN", NICK_AJOIN_SYNTAX);
    }
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static ConfigDirective autojoin_config[] = {
    { "NSAutojoinMax",   { { CD_POSINT, CF_DIRREQ, &NSAutojoinMax } } },
    { NULL }
};

/*************************************************************************/

static int do_load_module(Module *mod, const char *name)
{
    if (strcmp(name,"chanserv/main") == 0) {
        module_chanserv = mod;
        if (!(check_access_cmd_p = module_symbol(mod,"check_access_cmd"))){
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

static int autojoin_init(Module *module)
{
    Module *mod;

    if (!(protocol_features & PF_SVSJOIN)) {
        module_log("SVSJOIN not supported by this IRC server (%s)",
                   protocol_name);
        return 0;
    }

    module_nickserv = module_find("nickserv/main");

    if (!register_commands(module_nickserv, cmds)) {
        module_log("Unable to register commands");
        return 0;
    }

    send_svsjoin_event = event_declare(module, AUTOJOIN_EVENT_SEND_SVSJOIN);
    if (!send_svsjoin_event) {
        module_log("Unable to declare events");
        return 0;
    }

    if (!event_attach(module, EVENT_MODULE_LOADED, do_load_module)
     || !event_attach(module, EVENT_MODULE_UNLOADED, do_unload_module)
     || !event_attach(module, NICKSERV_EVENT_IDENTIFIED, do_identified)
     || !event_attach(module, NICKSERV_EVENT_HELP, do_help)
    ) {
        module_log("Unable to attach event handlers");
        return 0;
    }

    mod = module_find("chanserv/main");
    if (mod)
        do_load_module(mod, "chanserv/main");

    return 1;
}

/*************************************************************************/

static int autojoin_fini(Module *module, int shutdown)
{
    if (module_chanserv)
        do_unload_module(module_chanserv);

    if (module_nickserv) {
        unregister_commands(module_nickserv, cmds);
        module_nickserv = NULL;
    }

    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "NickServ AJOIN: channels joined on identifying",
    .requires = MODULE_REQUIRES("nickserv/main"),
    .config = autojoin_config,
    .init = autojoin_init,
    .fini = autojoin_fini,
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
