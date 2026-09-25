/* MemoServ IGNORE module.
 * Written by Yusuf Iskenderoglu <uhc0@stud.uni-karlsruhe.de>
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
#include "modules/memoserv/memoserv.h"

/*************************************************************************/


static int MSIgnoreMax;

/*************************************************************************/

static void do_ignore(User *u);

static Command cmds[] = {
    { "IGNORE",       do_ignore, NULL,  MEMO_HELP_IGNORE,     -1,-1},
    { NULL }
};

/*************************************************************************/

/* The memo ignore list of a nickname group is part of the group's record, which
 * nickserv/main keeps in the database (see include/store.h): nothing to
 * load or save here. */

/*************************************************************************/
/*************************** Callback routines ***************************/
/*************************************************************************/

/* Check whether the sender is allowed to send to the recipient (i.e. is
 * not being ignored by the recipient).
 */

static int check_if_ignored(User *sender, const char *target,
                            NickGroupInfo *ngi, const char *channel,
                            const char *text)
{
    int i;

    if (!ngi)
        return 0;
    ARRAY_FOREACH (i, ngi->ignore) {
        if (match_wild_nocase(ngi->ignore[i], sender->nick)
         || match_usermask(ngi->ignore[i], sender)
        ) {
            return MEMO_X_GETS_NO_MEMOS;
        }
        if (sender->ngi) {
            /* See if the ignore entry is a nick in the same group as the
             * sender, and ignore if so */
            NickInfo *ignore_ni;
            if ((ignore_ni = get_nickinfo(ngi->ignore[i])) != NULL) {
                NickGroupInfo *ignore_ngi =
                    get_nickgroupinfo(ignore_ni->nickgroup);
                uint32 ignore_id = ignore_ngi ? ignore_ngi->id : 0;
                if (ignore_ngi)
                    put_nickgroupinfo(ignore_ngi);
                put_nickinfo(ignore_ni);
                if (ignore_id == sender->ngi->id) {
                    return MEMO_X_GETS_NO_MEMOS;
                }
            }
        }
    }
    return 0;
}


/*************************************************************************/
/*************************** Command functions ***************************/
/*************************************************************************/

/* Handle the MemoServ IGNORE command. */

void do_ignore(User *u)
{
    char *cmd = strtok(NULL, " ");
    char *mask = strtok(NULL, " ");
    NickGroupInfo *ngi = NULL;
    NickInfo *ni;
    int i;

    if (cmd && mask && stricmp(cmd,"LIST") == 0 && is_services_admin(u)) {
        if (!(ni = get_nickinfo(mask))) {
            notice_lang(memoserv_service.nick, u, NICK_X_NOT_REGISTERED, mask);
        } else if (ni->status & NS_VERBOTEN) {
            notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, mask);
        } else if (!(ngi = get_ngi(ni))) {
            notice_lang(memoserv_service.nick, u, INTERNAL_ERROR);
        } else if (ngi->ignore_count == 0) {
            notice_lang(memoserv_service.nick, u, MEMO_IGNORE_LIST_X_EMPTY, mask);
        } else {
            notice_lang(memoserv_service.nick, u, MEMO_IGNORE_LIST_X, mask);
            ARRAY_FOREACH (i, ngi->ignore)
                notice(memoserv_service.nick, u->nick, "    %s", ngi->ignore[i]);
        }
        put_nickinfo(ni);
        put_nickgroupinfo(ngi);

    } else if (!cmd || ((stricmp(cmd,"LIST")==0) && mask)) {
        syntax_error(memoserv_service.nick, u, "IGNORE", MEMO_IGNORE_SYNTAX);

    } else if (!(ngi = u->ngi) || ngi == NICKGROUPINFO_INVALID) {
        notice_lang(memoserv_service.nick, u, NICK_NOT_REGISTERED);

    } else if (!user_identified(u)) {
        notice_lang(memoserv_service.nick, u, NICK_IDENTIFY_REQUIRED, nickserv_service.nick);

    } else if (stricmp(cmd, "ADD") == 0) {
        if (!mask) {
            syntax_error(memoserv_service.nick, u, "IGNORE", MEMO_IGNORE_ADD_SYNTAX);
            return;
        }
        if (ngi->ignore_count >= MSIgnoreMax) {
            notice_lang(memoserv_service.nick, u, MEMO_IGNORE_LIST_FULL);
            return;
        }
        ARRAY_FOREACH (i, ngi->ignore) {
            if (stricmp(ngi->ignore[i], mask) == 0) {
                notice_lang(memoserv_service.nick, u,
                        MEMO_IGNORE_ALREADY_PRESENT, ngi->ignore[i]);
                return;
            }
        }
        ARRAY_EXTEND(ngi->ignore);
        ngi->ignore[ngi->ignore_count-1] = sstrdup(mask);
        notice_lang(memoserv_service.nick, u, MEMO_IGNORE_ADDED, mask);

   } else if (stricmp(cmd, "DEL") == 0) {
        if (!mask) {
            syntax_error(memoserv_service.nick, u, "IGNORE", MEMO_IGNORE_DEL_SYNTAX);
            return;
        }
        ARRAY_SEARCH_PLAIN(ngi->ignore, mask, strcmp, i);
        if (i == ngi->ignore_count)
            ARRAY_SEARCH_PLAIN(ngi->ignore, mask, stricmp, i);
        if (i == ngi->ignore_count) {
            notice_lang(memoserv_service.nick, u, MEMO_IGNORE_NOT_FOUND, mask);
            return;
        }
        notice_lang(memoserv_service.nick, u, MEMO_IGNORE_DELETED, mask);
        free(ngi->ignore[i]);
        ARRAY_REMOVE(ngi->ignore, i);

    } else if (stricmp(cmd, "LIST") == 0) {
        if (ngi->ignore_count == 0) {
            notice_lang(memoserv_service.nick, u, MEMO_IGNORE_LIST_EMPTY);
        } else {
            notice_lang(memoserv_service.nick, u, MEMO_IGNORE_LIST);
            ARRAY_FOREACH (i, ngi->ignore)
                notice(memoserv_service.nick, u->nick, "    %s", ngi->ignore[i]);
        }

    } else {
        syntax_error(memoserv_service.nick, u, "IGNORE", MEMO_IGNORE_SYNTAX);
    }
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static ConfigDirective memo_ignore_config[] = {
    { "MSIgnoreMax",      { { CD_POSINT, CF_DIRREQ, &MSIgnoreMax } } },
    { NULL }
};

/*************************************************************************/

/* The IGNORE command goes into MemoServ's command list. */

static int memo_ignore_init(Module *module)
{
    if (!register_commands(module_find("memoserv/main"), cmds)) {
        module_log("Unable to register commands");
        return 0;
    }
    if (!event_attach_priority(module, MEMOSERV_EVENT_RECEIVE_MEMO,
                               check_if_ignored, MS_RECEIVE_PRI_CHECK)) {
        module_log("Unable to attach to " MEMOSERV_EVENT_RECEIVE_MEMO);
        return 0;
    }
    return 1;
}

/*************************************************************************/

static int memo_ignore_fini(Module *module, int shutdown)
{
    unregister_commands(module_find("memoserv/main"), cmds);
    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "MemoServ IGNORE: refuse memos from chosen senders",
    .requires = MODULE_REQUIRES("memoserv/main"),
    .config = memo_ignore_config,
    .init = memo_ignore_init,
    .fini = memo_ignore_fini,
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
