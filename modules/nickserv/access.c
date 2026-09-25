/* Nickname access list module.
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
#include "modules/operserv/operserv.h"

#include "modules/nickserv/nickserv.h"
#include "modules/nickserv/ns-local.h"

/*************************************************************************/

static Module *module_nickserv;

static int32 NSAccessMax;
static int   NSFirstAccessEnable;
static int   NSFirstAccessWild;

/*************************************************************************/

static void do_access(User *u);

static Command cmds[] = {
    {"ACCESS",    do_access,     NULL,  NICK_HELP_ACCESS,
                -1, NICK_OPER_HELP_ACCESS},
    { NULL }
};

/*************************************************************************/

/* The access list of a nickname group is part of the group's record, which
 * nickserv/main keeps in the database (see include/store.h): nothing to
 * load or save here. */

/*************************************************************************/
/**************************** Local routines *****************************/
/*************************************************************************/

/* Handle the ACCESS command. */

static void do_access(User *u)
{
    char *cmd = strtok(NULL, " ");
    char *mask = strtok(NULL, " ");
    NickInfo *ni;
    NickGroupInfo *ngi;
    int i;

    if (cmd && stricmp(cmd, "LIST") == 0 && mask && is_services_admin(u)) {
        ni = get_nickinfo(mask);
        ngi = NULL;
        if (!ni) {
            notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, mask);
        } else if (ni->status & NS_VERBOTEN) {
            notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, mask);
        } else if (!(ngi = get_ngi(ni))) {
            notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
        } else if (ngi->access_count == 0) {
            notice_lang(nickserv_service.nick, u, NICK_ACCESS_LIST_X_EMPTY, mask);
        } else {
            notice_lang(nickserv_service.nick, u, NICK_ACCESS_LIST_X, mask);
            ARRAY_FOREACH (i, ngi->access)
                notice(nickserv_service.nick, u->nick, "    %s", ngi->access[i]);
        }
        put_nickinfo(ni);
        put_nickgroupinfo(ngi);

    } else if (!cmd || ((stricmp(cmd,"LIST")==0) ? mask!=NULL : mask==NULL)) {
        syntax_error(nickserv_service.nick, u, "ACCESS", NICK_ACCESS_SYNTAX);

    } else if (mask && !strchr(mask, '@')) {
        notice_lang(nickserv_service.nick, u, BAD_USERHOST_MASK);
        notice_lang(nickserv_service.nick, u, MORE_INFO, nickserv_service.nick, "ACCESS");

    } else if (ngi = u->ngi, !(ni = u->ni)) {
        notice_lang(nickserv_service.nick, u, NICK_NOT_REGISTERED);

    } else if (!user_identified(u)) {
        notice_lang(nickserv_service.nick, u, NICK_IDENTIFY_REQUIRED, nickserv_service.nick);

    } else if (stricmp(cmd, "ADD") == 0) {
        if (readonly) {
            notice_lang(nickserv_service.nick, u, NICK_ACCESS_DISABLED);
            return;
        }
        if (ngi->access_count >= NSAccessMax) {
            notice_lang(nickserv_service.nick, u, NICK_ACCESS_REACHED_LIMIT, NSAccessMax);
            return;
        }
        ARRAY_FOREACH (i, ngi->access) {
            if (stricmp(ngi->access[i], mask) == 0) {
                notice_lang(nickserv_service.nick, u, NICK_ACCESS_ALREADY_PRESENT, mask);
                return;
            }
        }
        if (strchr(mask, '!'))
            notice_lang(nickserv_service.nick, u, NICK_ACCESS_NO_NICKS);
        ARRAY_EXTEND(ngi->access);
        ngi->access[ngi->access_count-1] = sstrdup(mask);
        notice_lang(nickserv_service.nick, u, NICK_ACCESS_ADDED, mask);

    } else if (stricmp(cmd, "DEL") == 0) {
        if (readonly) {
            notice_lang(nickserv_service.nick, u, NICK_ACCESS_DISABLED);
            return;
        }
        /* First try for an exact match; then, a case-insensitive one. */
        ARRAY_SEARCH_PLAIN(ngi->access, mask, strcmp, i);
        if (i == ngi->access_count)
            ARRAY_SEARCH_PLAIN(ngi->access, mask, stricmp, i);
        if (i == ngi->access_count) {
            notice_lang(nickserv_service.nick, u, NICK_ACCESS_NOT_FOUND, mask);
            return;
        }
        notice_lang(nickserv_service.nick, u, NICK_ACCESS_DELETED, ngi->access[i]);
        free(ngi->access[i]);
        ARRAY_REMOVE(ngi->access, i);

    } else if (stricmp(cmd, "LIST") == 0) {
        if (ngi->access_count == 0) {
            notice_lang(nickserv_service.nick, u, NICK_ACCESS_LIST_EMPTY);
        } else {
            notice_lang(nickserv_service.nick, u, NICK_ACCESS_LIST);
            ARRAY_FOREACH (i, ngi->access)
                notice(nickserv_service.nick, u->nick, "    %s", ngi->access[i]);
        }

    } else {
        syntax_error(nickserv_service.nick, u, "ACCESS", NICK_ACCESS_SYNTAX);

    }
}

/*************************************************************************/
/*************************** Callback routines ***************************/
/*************************************************************************/

/* Nick-registration handler (initializes access list). */

static int do_registered(User *u, NickInfo *ni, NickGroupInfo *ngi,
                         int *replied)
{
    if (NSFirstAccessEnable) {
        ngi->access_count = 1;
        ngi->access = smalloc(sizeof(char *));
        if (NSFirstAccessWild) {
            ngi->access[0] = create_mask(u, 0);
        } else {
            ngi->access[0] = smalloc(strlen(u->username)+strlen(u->host)+2);
            sprintf(ngi->access[0], "%s@%s", u->username, u->host);
        }
    }
    return 0;
}

/*************************************************************************/

/* Check whether a user is on the access list of the nick they're using.
 * Return 1 if on the access list, 0 if not.
 */

static int check_on_access(User *u)
{
    int i;
    char buf[BUFSIZE];

    if (!u->ni || !u->ngi) {
        module_log("check_on_access() BUG: ni or ngi is NULL!");
        return 0;
    }
    if (u->ngi->access_count == 0)
        return 0;
    i = strlen(u->username);
    snprintf(buf, sizeof(buf), "%s@%s", u->username, u->host);
    ARRAY_FOREACH (i, u->ngi->access) {
        if (match_wild_nocase(u->ngi->access[i], buf))
            return 1;
    }
    return 0;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static ConfigDirective access_config[] = {
    { "NSAccessMax",      { { CD_POSINT, CF_DIRREQ, &NSAccessMax } } },
    { "NSFirstAccessEnable",{{CD_SET, 0, &NSFirstAccessEnable } } },
    { "NSFirstAccessWild",{ { CD_SET, 0, &NSFirstAccessWild } } },
    { NULL }
};

/*************************************************************************/

static int access_init(Module *module)
{
    if (NSAccessMax > MAX_NICK_ACCESS) {
        module_log("NSAccessMax upper-bounded at MAX_NICK_ACCESS (%d)",
                   MAX_NICK_ACCESS);
        NSAccessMax = MAX_NICK_ACCESS;
    }

    module_nickserv = module_find("nickserv/main");

    if (!register_commands(module_nickserv, cmds)) {
        module_log("Unable to register commands");
        return 0;
    }

    if (!event_attach(module, NICKSERV_EVENT_CHECK_RECOGNIZED, check_on_access)
     || !event_attach(module, NICKSERV_EVENT_REGISTERED, do_registered)
    ) {
        module_log("Unable to attach event handlers");
        return 0;
    }

    return 1;
}

/*************************************************************************/

static int access_fini(Module *module, int shutdown)
{
    if (module_nickserv) {
        unregister_commands(module_nickserv, cmds);
        module_nickserv = NULL;
    }

    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "NickServ ACCESS: address lists that recognize a user",
    .requires = MODULE_REQUIRES("nickserv/main"),
    .config = access_config,
    .init = access_init,
    .fini = access_fini,
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
