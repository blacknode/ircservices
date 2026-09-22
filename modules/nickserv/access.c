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

#include "nickserv.h"
#include "ns-local.h"

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
/**************************** Database stuff *****************************/
/*************************************************************************/

/* See autojoin.c for why we don't create our own table. */

/* Temporary structure for loading/saving access records */
typedef struct {
    uint32 nickgroup;
    char *mask;
} DBRecord;
static DBRecord dbrec_static;

/* Iterators for first/next routines */
static NickGroupInfo *db_ngi_iterator;
static int db_array_iterator;

/*************************************************************************/

/* Table access routines */

static void *new_access(void)
{
    return memset(&dbrec_static, 0, sizeof(dbrec_static));
}

static void free_access(void *record)
{
    free(((DBRecord *)record)->mask);
}

static void insert_access(void *record)
{
    DBRecord *dbrec = record;
    NickGroupInfo *ngi = get_nickgroupinfo(dbrec->nickgroup);
    if (!ngi) {
        module_log("Discarding access record for missing nickgroup %u: %s",
                   dbrec->nickgroup, dbrec->mask);
        free_access(record);
    } else {
        ARRAY_EXTEND(ngi->access);
        ngi->access[ngi->access_count-1] = dbrec->mask;
    }
}

static void *next_access(void)
{
    while (db_ngi_iterator
        && db_array_iterator >= db_ngi_iterator->access_count
    ) {
        db_ngi_iterator = next_nickgroupinfo();
        db_array_iterator = 0;
    }
    if (db_ngi_iterator) {
        dbrec_static.nickgroup = db_ngi_iterator->id;
        dbrec_static.mask = db_ngi_iterator->access[db_array_iterator++];
        return &dbrec_static;
    } else {
        return NULL;
    }
}

static void *first_access(void)
{
    db_ngi_iterator = first_nickgroupinfo();
    db_array_iterator = 0;
    return next_access();
}

/*************************************************************************/

/* Database table definition */

#define FIELD(name,type,...) \
    { #name, type, offsetof(DBRecord,name) , ##__VA_ARGS__ }

static DBField access_dbfields[] = {
    FIELD(nickgroup, DBTYPE_UINT32),
    FIELD(mask,      DBTYPE_STRING),
    { NULL }
};

static DBTable access_dbtable = {
    .name    = "nick-access",
    .newrec  = new_access,
    .freerec = free_access,
    .insert  = insert_access,
    .first   = first_access,
    .next    = next_access,
    .fields  = access_dbfields,
};

#undef FIELD

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
            notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, mask);
        } else if (ni->status & NS_VERBOTEN) {
            notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, mask);
        } else if (!(ngi = get_ngi(ni))) {
            notice_lang(s_NickServ, u, INTERNAL_ERROR);
        } else if (ngi->access_count == 0) {
            notice_lang(s_NickServ, u, NICK_ACCESS_LIST_X_EMPTY, mask);
        } else {
            notice_lang(s_NickServ, u, NICK_ACCESS_LIST_X, mask);
            ARRAY_FOREACH (i, ngi->access)
                notice(s_NickServ, u->nick, "    %s", ngi->access[i]);
        }
        put_nickinfo(ni);
        put_nickgroupinfo(ngi);

    } else if (!cmd || ((stricmp(cmd,"LIST")==0) ? mask!=NULL : mask==NULL)) {
        syntax_error(s_NickServ, u, "ACCESS", NICK_ACCESS_SYNTAX);

    } else if (mask && !strchr(mask, '@')) {
        notice_lang(s_NickServ, u, BAD_USERHOST_MASK);
        notice_lang(s_NickServ, u, MORE_INFO, s_NickServ, "ACCESS");

    } else if (ngi = u->ngi, !(ni = u->ni)) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);

    } else if (!user_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);

    } else if (stricmp(cmd, "ADD") == 0) {
        if (readonly) {
            notice_lang(s_NickServ, u, NICK_ACCESS_DISABLED);
            return;
        }
        if (ngi->access_count >= NSAccessMax) {
            notice_lang(s_NickServ, u, NICK_ACCESS_REACHED_LIMIT, NSAccessMax);
            return;
        }
        ARRAY_FOREACH (i, ngi->access) {
            if (stricmp(ngi->access[i], mask) == 0) {
                notice_lang(s_NickServ, u, NICK_ACCESS_ALREADY_PRESENT, mask);
                return;
            }
        }
        if (strchr(mask, '!'))
            notice_lang(s_NickServ, u, NICK_ACCESS_NO_NICKS);
        ARRAY_EXTEND(ngi->access);
        ngi->access[ngi->access_count-1] = sstrdup(mask);
        notice_lang(s_NickServ, u, NICK_ACCESS_ADDED, mask);

    } else if (stricmp(cmd, "DEL") == 0) {
        if (readonly) {
            notice_lang(s_NickServ, u, NICK_ACCESS_DISABLED);
            return;
        }
        /* First try for an exact match; then, a case-insensitive one. */
        ARRAY_SEARCH_PLAIN(ngi->access, mask, strcmp, i);
        if (i == ngi->access_count)
            ARRAY_SEARCH_PLAIN(ngi->access, mask, stricmp, i);
        if (i == ngi->access_count) {
            notice_lang(s_NickServ, u, NICK_ACCESS_NOT_FOUND, mask);
            return;
        }
        notice_lang(s_NickServ, u, NICK_ACCESS_DELETED, ngi->access[i]);
        free(ngi->access[i]);
        ARRAY_REMOVE(ngi->access, i);

    } else if (stricmp(cmd, "LIST") == 0) {
        if (ngi->access_count == 0) {
            notice_lang(s_NickServ, u, NICK_ACCESS_LIST_EMPTY);
        } else {
            notice_lang(s_NickServ, u, NICK_ACCESS_LIST);
            ARRAY_FOREACH (i, ngi->access)
                notice(s_NickServ, u->nick, "    %s", ngi->access[i]);
        }

    } else {
        syntax_error(s_NickServ, u, "ACCESS", NICK_ACCESS_SYNTAX);

    }
}

/*************************************************************************/
/*************************** Callback routines ***************************/
/*************************************************************************/

/* Nick-registration callback (initializes access list). */

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

ConfigDirective module_config[] = {
    { "NSAccessMax",      { { CD_POSINT, CF_DIRREQ, &NSAccessMax } } },
    { "NSFirstAccessEnable",{{CD_SET, 0, &NSFirstAccessEnable } } },
    { "NSFirstAccessWild",{ { CD_SET, 0, &NSFirstAccessWild } } },
    { NULL }
};

/*************************************************************************/

int init_module()
{
    if (NSAccessMax > MAX_NICK_ACCESS) {
        module_log("NSAccessMax upper-bounded at MAX_NICK_ACCESS (%d)",
                   MAX_NICK_ACCESS);
        NSAccessMax = MAX_NICK_ACCESS;
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

    if (!add_callback(module_nickserv, "check recognized", check_on_access)
     || !add_callback(module_nickserv, "registered", do_registered)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    if (!register_dbtable(&access_dbtable)) {
        module_log("Unable to register database table");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    unregister_dbtable(&access_dbtable);

    if (module_nickserv) {
        remove_callback(module_nickserv, "registered", do_registered);
        remove_callback(module_nickserv, "check recognized", check_on_access);
        unregister_commands(module_nickserv, cmds);
        unuse_module(module_nickserv);
        module_nickserv = NULL;
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
