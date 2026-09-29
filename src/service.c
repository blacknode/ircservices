/* Pseudo-clients: the nicks the modules provide (service.h).
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "config.h"
#include "extern.h"
#include "language.h"
#include "memory.h"
#include "modules.h"
#include "p10.h"
#include "users.h"

/*************************************************************************/

/* Every attached pseudo-client, in the order their modules were loaded. */
static struct Service* service_list;

/*************************************************************************/

struct Service* service_find_by_flag(unsigned int flag)
{
    struct Service* service;
    for (service = service_list; service; service = service->next)
        if (service->flags & flag)
            return service;
    return NULL;
}

struct Service* service_find(const char* nick)
{
    struct Service* service;

    if (!nick)
        return NULL;
    for (service = service_list; service; service = service->next) {
        if (service->nick && irc_stricmp(service->nick, nick) == 0)
            return service;
    }
    return NULL;
}

struct Service* service_first(void)
{
    return service_list;
}

struct Service* service_next(const struct Service* service)
{
    return service ? service->next : NULL;
}

/* Services' own event, declared by init.c. */
Event* pseudo_client_joined_event;

/*************************************************************************/
/*************************************************************************/

void service_join_channel(struct Service* service)
{
    if (!service || !service->nick || !ServicesChannel || !*ServicesChannel)
        return;
    send_cmd(service->nick, "JOIN %s", ServicesChannel);
    event_emit(pseudo_client_joined_event, ServicesChannel, service);
}

/*************************************************************************/

/* Put one pseudo-client on the network.  Until the uplink's burst is over
 * its module joins it to the serverinfo channel (see service.h); after
 * that, it is joined here. */

static void introduce_one(struct Service* service)
{
    int flags = 0;
    User* u;

    if (service->flags & SERVICE_OPER)
        flags |= PSEUDO_OPER;
    if (service->flags & SERVICE_INVISIBLE)
        flags |= PSEUDO_INVIS;
    send_pseudo_nick(service->nick, service->description, flags);
    /* A pseudo-client brought back after a KILL still has its User. */
    u = get_user(service->nick);
    if (!u || !(u->flags & UF_PSEUDO_CLIENT))
        u = new_user(service->nick, 1);
    if (u) {
        time_t now = time(NULL);
        u->flags |= UF_PSEUDO_CLIENT;
        u->signon = now;
        u->my_signon = now;
        u->host = ServiceHost;
        u->username = ServiceUser;
        u->realname = service->description;
        u->servicestamp = (uint32)u->signon;
        strbcpy(u->numeric, service->numeric);
    }
    if (p10_uplink_synced())
        service_join_channel(service);
}

/*************************************************************************/

int service_introduce(const char* nick)
{
    struct Service* service;

    if (nick) {
        service = service_find(nick);
        if (!service)
            return 0;
        /* A pseudo-client killed as soon as it comes back would bring us
         * straight back here; give up rather than loop forever. */
        {
#define RECENT_COUNT 20
            static time_t recent[RECENT_COUNT];
            if (recent[0] >= time(NULL) - 3)
                fatal("service_introduce(): loop detected (%s)", nick);
            memmove(recent, recent + 1, sizeof(recent) - sizeof(*recent));
            recent[RECENT_COUNT - 1] = time(NULL);
#undef RECENT_COUNT
        }
        introduce_one(service);
        return 1;
    }
    for (service = service_list; service; service = service->next)
        introduce_one(service);
    return 1;
}

/*************************************************************************/

int service_deliver_message(const char* source, const char* target, char* text)
{
    struct Service* service = service_find(target);
    User* user;

    if (!service)
        return 0;
    if (!service->on_message)
        return 1; /* A pseudo-client that listens to nothing */

    user = get_user(source);
    if (!user) {
        log("%s: user record for %s not found", service->nick, source);
        notice(service->nick, source, "%s", getstring(NULL, INTERNAL_ERROR));
        return 1;
    }

    /* CTCP PING: answered the same way for every pseudo-client. */
    if (strnicmp(text, "\1PING", 5) == 0 && (!text[5] || text[5] == ' ')) {
        const char* rest = text + 5;
        rest += strspn(rest, " ");
        notice(service->nick, source, "\1PING %s", *rest ? rest : "\1");
        return 1;
    }

    service->on_message(service, user, text);
    return 1;
}

/*************************************************************************/

int service_answer_whois(const char* source, const char* nick)
{
    struct Service* service = service_find(nick);

    if (!service)
        return 0;
    send_cmd(ServerName, "311 %s %s %s %s * :%s", source, service->nick,
             ServiceUser, ServiceHost, service->description);
    send_cmd(ServerName, "312 %s %s %s :%s", source, service->nick, ServerName,
             ServerDesc);
    send_cmd(ServerName, "313 %s %s :is a network service", source,
             service->nick);
    send_cmd(ServerName, "318 %s %s End of /WHOIS response.", source,
             service->nick);
    return 1;
}

/*************************************************************************/
/*************************************************************************/

int service_attach_module(Module* module, struct Service* const* services)
{
    struct Service **tail, *other;
    int i;

    for (i = 0; services && services[i]; i++) {
        struct Service* service = services[i];
        if (!service->nick || !*service->nick) {
            log("%s: pseudo-client with no nick (directive %s)",
                module_name(module), service->directive);
            return 0;
        }
        other = service_find(service->nick);
        if (other && other != service) {
            log("%s: nick %s is already used by a pseudo-client of %s",
                module_name(module), service->nick, module_name(other->owner));
            return 0;
        }
    }
    for (tail = &service_list; *tail; tail = &(*tail)->next)
        ;
    for (i = 0; services && services[i]; i++) {
        services[i]->owner = module;
        services[i]->next = NULL;
        *tail = services[i];
        tail = &services[i]->next;
    }
    return 1;
}

/*************************************************************************/

void service_introduce_module(Module* module)
{
    struct Service* service;

    if (!linked)
        return;
    for (service = service_list; service; service = service->next) {
        if (service->owner == module)
            introduce_one(service);
    }
}

/*************************************************************************/

void service_detach_module(Module* module, int quit)
{
    struct Service** link = &service_list;

    while (*link) {
        struct Service* service = *link;
        if (service->owner == module) {
            if (quit && linked && service->nick)
                send_cmd(service->nick, "QUIT :");
            *link = service->next;
            service->next = NULL;
            service->owner = NULL;
            free(service->previous_nick);
            free(service->previous_description);
            service->previous_nick = service->previous_description = NULL;
        }
        else {
            link = &service->next;
        }
    }
}

/*************************************************************************/

void service_reconfigure_begin(void)
{
    struct Service* service;

    for (service = service_list; service; service = service->next) {
        free(service->previous_nick);
        free(service->previous_description);
        service->previous_nick = sstrdup(service->nick);
        service->previous_description =
            sstrdup(service->description ? service->description : "");
    }
}

void service_reconfigure_end(void)
{
    struct Service* service;

    for (service = service_list; service; service = service->next) {
        if (!service->previous_nick)
            continue;
        if (strcmp(service->previous_nick, service->nick) != 0)
            send_nickchange(service->previous_nick, service->nick);
        if (strcmp(service->previous_description,
                   service->description ? service->description : "") != 0)
            send_namechange(service->nick, service->description);
        free(service->previous_nick);
        free(service->previous_description);
        service->previous_nick = service->previous_description = NULL;
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
