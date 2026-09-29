/* EchoServ: an example pseudo-client, to copy when writing a new one.
 * Everything it does is explained in docs/readme.mod_api, section 9.
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

/* Settings of the module block. */
static int32 echo_max_length;
static int echo_greet;

/* Our event: someone was echoed.  (In a module with a public header,
 * the constant and its documentation go there.) */
#define ECHOSERV_EVENT_ECHOED "echoserv.echoed"
static Event *echoed_event;

/*************************************************************************/

/* Commands. */

static void do_help(User *u);
static void do_echo(User *u);

static Command echoserv_commands[] = {
    { "HELP", do_help, NULL, -1, -1, -1 },
    { "ECHO", do_echo, NULL, -1, -1, -1 },
    { NULL }
};

/* Declared here, defined at the end with the module description. */
static struct Service echoserv_service;

static void do_help(User *u)
{
    notice(echoserv_service.nick, u->nick,
           "Commands: ECHO <text> (at most %d characters)",
           (int)echo_max_length);
}

static void do_echo(User *u)
{
    char *text = strtok_remaining();

    if (!text) {
        notice(echoserv_service.nick, u->nick, "Syntax: ECHO <text>");
        return;
    }
    if (strlen(text) > (size_t)echo_max_length)
        text[echo_max_length] = 0;
    notice(echoserv_service.nick, u->nick, "%s", text);
    event_emit(echoed_event, u, text);
}

/*************************************************************************/

/* A PRIVMSG to EchoServ: the first word is the command. */

static void echoserv_message(struct Service *service, User *u, char *text)
{
    char *command = strtok(text, " ");

    if (command)
        run_cmd(service->nick, u, THIS_MODULE, command);
}

/*************************************************************************/

/* Core event: a user connected. */

static int on_user_create(User *user, int ac, char **av, int reconnect)
{
    if (echo_greet && linked)
        notice(echoserv_service.nick, user->nick, "Hello, %s!",
               user->nick);
    return EVENT_CONTINUE;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static ConfigDirective echoserv_config[] = {
    { "EchoMaxLength", { { CD_POSINT, 0, &echo_max_length } } },
    { "EchoGreet",     { { CD_SET, 0, &echo_greet } } },
    { NULL }
};

static void echoserv_rehash(Module *module)
{
    if (!echo_max_length)
        echo_max_length = 100;
}

/* EVENT_SERVER_EOB_ACK: the uplink's burst is over, so join the
 * serverinfo channel (see service.h). */
static int do_eob_ack(void)
{
    service_join_channel(&echoserv_service);
    return 0;
}

static int echoserv_init(Module *module)
{
    echoserv_rehash(module);

    if (!new_commandlist(module)
     || !register_commands(module, echoserv_commands)) {
        module_log("Unable to register commands");
        return 0;
    }
    echoed_event = event_declare(module, ECHOSERV_EVENT_ECHOED);
    if (!echoed_event) {
        module_log("Unable to declare " ECHOSERV_EVENT_ECHOED);
        return 0;
    }
    if (!event_attach(module, EVENT_USER_CREATE, on_user_create)
     || !event_attach(module, EVENT_SERVER_EOB_ACK, do_eob_ack)) {
        module_log("Unable to attach event handlers");
        return 0;
    }
    return 1;
}

static int echoserv_fini(Module *module, int shutdown)
{
    unregister_commands(module, echoserv_commands);
    del_commandlist(module);
    return 1;
}

/* EchoServName = <nick>, <description>; in the module block. */
static struct Service echoserv_service = {
    .directive = "EchoServName",
    .on_message = echoserv_message,
};

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "EchoServ: repeats what it is told (example)",
    .config = echoserv_config,
    .services = MODULE_SERVICES(&echoserv_service),
    .init = echoserv_init,
    .fini = echoserv_fini,
    .rehash = echoserv_rehash,
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
