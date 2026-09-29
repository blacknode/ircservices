/* Pseudo-clients: the nicks Services show on the network (NickServ,
 * ChanServ, ...).  Include "modules.h", which includes this file.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * A module that provides a pseudo-client declares a struct Service and
 * lists it in its ModuleInfo.services:
 *
 *     struct Service helpserv_service = {
 *         .directive  = "HelpServName",
 *         .on_message = helpserv_message,
 *     };
 *
 * The configuration of the pseudo-client comes from the module block,
 * through the directive the Service names (required):
 *
 *     module "misc/helpserv" {
 *         HelpServName = "HelpServ", "Help Server";   # nick, description
 *     };
 *
 * The core does the rest, for every pseudo-client alike: it introduces it
 * when Services link (and the module is loaded, if they are linked
 * already), brings it back after a KILL, changes its nick when a REHASH
 * changes the directive, answers WHOIS for it, hands it the PRIVMSGs sent
 * to it (answering CTCP PING itself), and makes it quit when the module is
 * unloaded.  Joining it to the serverinfo channel and opping it there is
 * ChanServ's (see CHANSERV_EVENT_SERVICES_JOINED).
 */

#ifndef SERVICE_H
#define SERVICE_H

/*************************************************************************/

/* Flags for Service.flags. */
#define SERVICE_OPER      0x0001 /* Needs IRC operator privileges (+o) */
#define SERVICE_INVISIBLE 0x0002 /* Invisible (+i) */
#define SERVICE_CHANSERV  0x0004 /* Service is ChanServ */
#define SERVICE_NICKSERV  0x0008 /* Service is NickServ */
#define SERVICE_OPERSERV  0x0010 /* Service is OperServ */
/*************************************************************************/

struct Service {
    /* Declared by the module: */

    /* The directive of the module block that sets `nick' and
     * `description' ("<Directive> = <nick>, <description>;"). */
    const char* directive;

    /* Bitwise combination of SERVICE_* flags, or 0. */
    unsigned int flags;

    /* A PRIVMSG to the pseudo-client, from `user'; `text' may be modified
     * (strtok() and friends).  NULL for a pseudo-client that ignores what
     * it is sent. */
    void (*on_message)(struct Service* service, User* user, char* text);

    /* Set by the core from the configuration, and valid while the module
     * is loaded: */

    char* nick;        /* The pseudo-client's nick */
    char* description; /* Its real name, shown by WHOIS */

    /* Private to the core: */
    Module* owner;
    char* previous_nick;
    char* previous_description;
    char numeric[6];
    struct Service* next;
};

/*************************************************************************/

/** Find service by flag */
extern struct Service* service_find_by_flag(unsigned int flag);
/* The pseudo-client using `nick' (case-insensitively), or NULL. */
extern struct Service* service_find(const char* nick);

/* Iterate over every pseudo-client: service_first(), then service_next()
 * until NULL. */
extern struct Service* service_first(void);
extern struct Service* service_next(const struct Service* service);

/*************************************************************************/

/* The core. */

/* Introduce the pseudo-client using `nick', or every pseudo-client if
 * `nick' is NULL (on linking).  Returns nonzero if `nick' is a
 * pseudo-client (always nonzero for NULL). */
extern int service_introduce(const char* nick);

/* Hand a PRIVMSG to the pseudo-client it is for / answer a WHOIS for one.
 * Return nonzero if `target' / `nick' is a pseudo-client. */
extern int service_deliver_message(const char* source, const char* target,
                                   char* text);
extern int service_answer_whois(const char* source, const char* nick);

/* The loader: attach a module's pseudo-clients (after reading its
 * configuration) and bring them onto the network (after `init'); detach
 * them when it is unloaded, taking them off the network first if they
 * were introduced (`quit' nonzero). */
extern int service_attach_module(Module* module,
                                 struct Service* const* services);
extern void service_introduce_module(Module* module);
extern void service_detach_module(Module* module, int quit);

/* The loader, around a REHASH: remember the nicks and descriptions, then
 * rename the pseudo-clients whose nick changed. */
extern void service_reconfigure_begin(void);
extern void service_reconfigure_end(void);

/*************************************************************************/

#endif /* SERVICE_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
