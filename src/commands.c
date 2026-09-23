/* Routines for looking up commands in a *Serv command list.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "commands.h"
#include "language.h"

/*************************************************************************/

typedef struct commandlist_ CommandList;
typedef struct commandarray_ CommandArray;

struct commandlist_ {
    CommandList *next, *prev;
    Module *id;
    CommandArray *first_array;
};

struct commandarray_ {
    CommandArray *next, *prev;
    Command *commands;
};

/*************************************************************************/

/* List of all commands registered. */
static CommandList *cmdlist;

/*************************************************************************/
/*************************************************************************/

/* Set up a new command list using the given module pointer as an ID value.
 * Fails if a command list associated with `id' already exists.
 */

int new_commandlist(Module *id)
{
    CommandList *cl;

    LIST_SEARCH_SCALAR(cmdlist, id, id, cl);
    if (cl)
        return 0;
    cl = smalloc(sizeof(*cl));
    cl->id = id;
    cl->first_array = NULL;
    LIST_INSERT(cl, cmdlist);
    return 1;
}

/*************************************************************************/

/* Register a command array under the given ID.  Fails if there is no
 * command list associated with `id', `array' is NULL, `array' has already
 * been added to the list, or there are multiple command entries in `array'
 * with the same name (case-insensitive).  If an entry in `array' has the
 * same name as a previously-registered entry, the entry in `array' will
 * take precendence, and a pointer to the previous entry will be stored in
 * the `next' field of the entry.
 */

int register_commands(Module *id, Command *array)
{
    CommandList *cl;
    CommandArray *ca;
    Command *c, *c2;

    /* Basic sanity checking */
    if (!array)
        return 0;
    LIST_SEARCH_SCALAR(cmdlist, id, id, cl);
    if (!cl)
        return 0;
    LIST_SEARCH_SCALAR(cl->first_array, commands, array, ca);
    if (ca)
        return 0;
    /* Check for duplicate commands and set `next' pointers */
    for (c = array; c->name; c++) {
        for (c2 = c+1; c2->name; c2++) {
            if (stricmp(c->name, c2->name) == 0)
                return 0;
        }
        c->next = lookup_cmd(id, c->name);
    }
    /* All's well, insert it in the list and return success */
    ca = smalloc(sizeof(*ca));
    ca->commands = array;
    LIST_INSERT(ca, cl->first_array);
    return 1;
}

/*************************************************************************/

/* Unregister a command array from the given ID.  Fails if there is no
 * command list associated with `id' or `array' was not in the list in the
 * first place.
 */

int unregister_commands(Module *id, Command *array)
{
    CommandList *cl;
    CommandArray *ca;

    LIST_SEARCH_SCALAR(cmdlist, id, id, cl);
    if (!cl)
        return 0;
    LIST_SEARCH_SCALAR(cl->first_array, commands, array, ca);
    if (!ca)
        return 0;
    LIST_REMOVE(ca, cl->first_array);
    free(ca);
    return 1;
}

/*************************************************************************/

/* Delete the command list associated with the given ID.  Fails if there is
 * no command list associated with `id' or the command list is not empty.
 */

int del_commandlist(Module *id)
{
    CommandList *cl;

    LIST_SEARCH_SCALAR(cmdlist, id, id, cl);
    if (!cl || cl->first_array)
        return 0;
    LIST_REMOVE(cl, cmdlist);
    free(cl);
    return 1;
}

/*************************************************************************/
/*************************************************************************/

/* Returns the Command structure associated with the given command for the
 * given command list (`id'), or NULL if no such command exists.
 */

Command *lookup_cmd(Module *id, const char *name)
{
    CommandList *cl;
    CommandArray *ca;
    Command *c;

    LIST_SEARCH_SCALAR(cmdlist, id, id, cl);
    if (!cl)
        return NULL;
    LIST_FOREACH (ca, cl->first_array) {
        for (c = ca->commands; c->name; c++) {
            if (stricmp(c->name, name) == 0)
                return c;
        }
    }
    return NULL;
}

/*************************************************************************/

/* Run the routine for the given command, if it exists and the user has
 * privilege to do so; if not, print an appropriate error message.
 */

void run_cmd(const char *service, User *u, Module *id, const char *cmd)
{
    Command *c = lookup_cmd(id, cmd);

    if (c && c->routine) {
        if ((c->has_priv == NULL) || c->has_priv(u))
            c->routine(u);
        else
            notice_lang(service, u, ACCESS_DENIED);
    } else {
        notice_lang(service, u, UNKNOWN_COMMAND_HELP, cmd, service);
    }
}

/*************************************************************************/

/* Print a help message for the given command.  Multiple spaces between
 * words are compressed to a single space before looking up the command
 * (this will cause `cmd' to be modified).
 */

void help_cmd(const char *service, User *u, Module *id, char *cmd)
{
    Command *c;
    char *s;

    s = cmd-1;
    while ((s = strpbrk(s+1, " \t")) != NULL) {
        char *t = s + strspn(s, " \t");
        if (t > s+1)
            memmove(s+1, t, strlen(t)+1);
    }

    c = lookup_cmd(id, cmd);
    if (c) {
        const char *p1 = c->help_param1,
                   *p2 = c->help_param2,
                   *p3 = c->help_param3,
                   *p4 = c->help_param4;

        if (c->helpmsg_all >= 0)
            notice_help(service, u, c->helpmsg_all, p1, p2, p3, p4);

        if (is_oper(u)) {
            if (c->helpmsg_oper >= 0)
                notice_help(service, u, c->helpmsg_oper, p1, p2, p3, p4);
            else if (c->helpmsg_all < 0)
                notice_lang(service, u, NO_HELP_AVAILABLE, cmd);
        } else {
            if (c->helpmsg_reg >= 0)
                notice_help(service, u, c->helpmsg_reg, p1, p2, p3, p4);
            else if (c->helpmsg_all < 0)
                notice_lang(service, u, NO_HELP_AVAILABLE, cmd);
        }

    } else {

        notice_lang(service, u, NO_HELP_AVAILABLE, cmd);

    }
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
