/* Declarations for command data.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

/* Note that modules.h MUST be included before this file (for the Module
 * type). */

/*************************************************************************/

/* Structure for information about a *Serv command. */

typedef struct command_ Command;
struct command_ {
    const char *name;
    void (*routine)(User *u);
    int (*has_priv)(const User *u); /* Returns 1 if user may use cmd, else 0 */
    int helpmsg_all;            /* Displayed to all users; -1 = no message */
    int helpmsg_reg;            /* Displayed to regular users only */
    int helpmsg_oper;           /* Displayed to IRC operators only */
    const char *help_param1;
    const char *help_param2;
    const char *help_param3;
    const char *help_param4;
    Command *next;              /* Next command with this name */
};

/*************************************************************************/

/* Commands must be registered with Services to be usable; an array of
 * Command structures (terminated with name == NULL) can be registered and
 * unregistered with the following routines.  All routines return 1 on
 * success, 0 on failure.  ("Failure" occurs only when parameters are
 * invalid.)
 */

/* Set up a new command list using the given module pointer as an ID value.
 * Fails if a command list associated with `id' already exists. */
extern int new_commandlist(Module *id);

/* Register a command array under the given ID.  Fails if there is no
 * command list associated with `id', `array' is NULL, `array' has already
 * been added to the list, or there are multiple command entries in `array'
 * with the same name (case-insensitive).  If an entry in `array' has the
 * same name as a previously-registered entry, the entry in `array' will
 * take precendence, and a pointer to the previous entry will be stored in
 * the `next' field of the entry. */
extern int register_commands(Module *id, Command *array);

/* Unregister a command array from the given ID.  Fails if there is no
 * command list associated with `id' or `array' was not in the list in the
 * first place. */
extern int unregister_commands(Module *id, Command *array);

/* Delete the command list associated with the given ID.  Fails if there is
 * no command list associated with `id' or the command list is not empty. */
extern int del_commandlist(Module *id);

/*************************************************************************/

/* Routines for looking up and doing other things with commands. */

/* Returns the Command structure associated with the given command for the
 * given command list (`id'), or NULL if no such command exists. */
extern Command *lookup_cmd(Module *id, const char *name);

/* Runs the routine associated with the given command, sending a help
 * message if there is no such command or the user does not have privileges
 * to use the command.  Equivalent to
 *      lookup_cmd(id,name)->routine(u)
 * with privilege and error checking. */
extern void run_cmd(const char *service, User *u, Module *id, const char *cmd);

/* Sends the help message associated with the given command, or a generic
 * "command not found" message if there is no such command.  Multiple
 * spaces in `cmd' will be compressed to a single space (thus modifying the
 * string). */
extern void help_cmd(const char *service, User *u, Module *id, char *cmd);

/*************************************************************************/

#endif  /* COMMANDS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
