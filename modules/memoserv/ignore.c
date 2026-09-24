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

static Module *module_memoserv;

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
            notice_lang(s_MemoServ, u, NICK_X_NOT_REGISTERED, mask);
        } else if (ni->status & NS_VERBOTEN) {
            notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, mask);
        } else if (!(ngi = get_ngi(ni))) {
            notice_lang(s_MemoServ, u, INTERNAL_ERROR);
        } else if (ngi->ignore_count == 0) {
            notice_lang(s_MemoServ, u, MEMO_IGNORE_LIST_X_EMPTY, mask);
        } else {
            notice_lang(s_MemoServ, u, MEMO_IGNORE_LIST_X, mask);
            ARRAY_FOREACH (i, ngi->ignore)
                notice(s_MemoServ, u->nick, "    %s", ngi->ignore[i]);
        }
        put_nickinfo(ni);
        put_nickgroupinfo(ngi);

    } else if (!cmd || ((stricmp(cmd,"LIST")==0) && mask)) {
        syntax_error(s_MemoServ, u, "IGNORE", MEMO_IGNORE_SYNTAX);

    } else if (!(ngi = u->ngi) || ngi == NICKGROUPINFO_INVALID) {
        notice_lang(s_MemoServ, u, NICK_NOT_REGISTERED);

    } else if (!user_identified(u)) {
        notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);

    } else if (stricmp(cmd, "ADD") == 0) {
        if (!mask) {
            syntax_error(s_MemoServ, u, "IGNORE", MEMO_IGNORE_ADD_SYNTAX);
            return;
        }
        if (ngi->ignore_count >= MSIgnoreMax) {
            notice_lang(s_MemoServ, u, MEMO_IGNORE_LIST_FULL);
            return;
        }
        ARRAY_FOREACH (i, ngi->ignore) {
            if (stricmp(ngi->ignore[i], mask) == 0) {
                notice_lang(s_MemoServ, u,
                        MEMO_IGNORE_ALREADY_PRESENT, ngi->ignore[i]);
                return;
            }
        }
        ARRAY_EXTEND(ngi->ignore);
        ngi->ignore[ngi->ignore_count-1] = sstrdup(mask);
        notice_lang(s_MemoServ, u, MEMO_IGNORE_ADDED, mask);

   } else if (stricmp(cmd, "DEL") == 0) {
        if (!mask) {
            syntax_error(s_MemoServ, u, "IGNORE", MEMO_IGNORE_DEL_SYNTAX);
            return;
        }
        ARRAY_SEARCH_PLAIN(ngi->ignore, mask, strcmp, i);
        if (i == ngi->ignore_count)
            ARRAY_SEARCH_PLAIN(ngi->ignore, mask, stricmp, i);
        if (i == ngi->ignore_count) {
            notice_lang(s_MemoServ, u, MEMO_IGNORE_NOT_FOUND, mask);
            return;
        }
        notice_lang(s_MemoServ, u, MEMO_IGNORE_DELETED, mask);
        free(ngi->ignore[i]);
        ARRAY_REMOVE(ngi->ignore, i);

    } else if (stricmp(cmd, "LIST") == 0) {
        if (ngi->ignore_count == 0) {
            notice_lang(s_MemoServ, u, MEMO_IGNORE_LIST_EMPTY);
        } else {
            notice_lang(s_MemoServ, u, MEMO_IGNORE_LIST);
            ARRAY_FOREACH (i, ngi->ignore)
                notice(s_MemoServ, u->nick, "    %s", ngi->ignore[i]);
        }

    } else {
        syntax_error(s_MemoServ, u, "IGNORE", MEMO_IGNORE_SYNTAX);
    }
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "MSIgnoreMax",      { { CD_POSINT, CF_DIRREQ, &MSIgnoreMax } } },
    { NULL }
};


/*************************************************************************/

int init_module(void)
{
    module_memoserv = find_module("memoserv/main");
    if (!module_memoserv) {
        module_log("Main MemoServ module not loaded");
        return 0;
    }
    use_module(module_memoserv);

    if (!register_commands(module_memoserv, cmds)) {
        module_log("Unable to register commands");
        exit_module(0);
        return 0;
    }

    if (!add_callback_pri(module_memoserv, "receive memo",
                          check_if_ignored, MS_RECEIVE_PRI_CHECK)
    ) {
        module_log("Unable to add callback");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{

    if (module_memoserv) {
        remove_callback(module_memoserv, "receive memo", check_if_ignored);
        unregister_commands(module_memoserv, cmds);
        unuse_module(module_memoserv);
        module_memoserv = NULL;
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
