/* Events: named points that the core and the modules announce, and that
 * any module can attach handlers to.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * An event has a name, "<owner>.<what>" by convention ("user.create" for
 * the core, "nickserv.identified" for NickServ), and one owner: the core
 * or the module that declares it with event_declare().  Only the owner
 * emits it, with event_emit().  Any module attaches handlers to it by
 * name, with event_attach().
 *
 * A handler may be attached before its event is declared -- for instance
 * by a module loaded before the one that owns the event, or one whose
 * owner is not loaded at all.  It is kept, and starts being called when
 * the owner declares the event.  When the owner is unloaded its events
 * are retracted, and the handlers attached to them wait for it to come
 * back.  So a module never has to watch other modules load and unload
 * just to attach its handlers.
 *
 * Handlers run in order of priority (higher first; equal priorities in
 * the order they were attached), each with the arguments given to
 * event_emit().  A handler returns EVENT_CONTINUE (zero) to let the next
 * one run, or a nonzero value -- usually EVENT_STOP -- to end the chain;
 * event_emit() returns that value, zero if every handler continued, and
 * -1 if the event is not declared.  What the arguments are and what
 * stopping means is part of each event's documentation: the core's are in
 * the list below, a module's in its public header.
 *
 * Everything a module attached is detached, and every event it declared
 * retracted, when it is unloaded.
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <stdint.h>

/*************************************************************************/

/* A declared event.  Opaque; the owner keeps the pointer event_declare()
 * returned and passes it to event_emit(). */
struct Event_;
typedef struct Event_ Event;

/* A handler.  Declare it with the parameters of the event it handles,
 * e.g. for "user.delete":
 *     static int on_user_delete(User* user, const char* reason,
 *                               int is_kill);
 * Every argument is passed as a value the size of a pointer, so pointers,
 * `int', `long' and `time_t' parameters all work. */
typedef int (*EventHandler)();

/* Handler return values. */
#define EVENT_CONTINUE 0 /* Carry on with the next handler */
#define EVENT_STOP     1 /* Handled: no further handler runs */

/* Handler priorities: higher runs first. */
#define EVENT_PRIORITY_FIRST   10000
#define EVENT_PRIORITY_DEFAULT 0
#define EVENT_PRIORITY_LAST    -10000

/* Most arguments an event may carry. */
#define EVENT_MAX_ARGS 6

/*************************************************************************/

/* For the owner of an event. */

/* Declare the event `name', owned by `owner' (THIS_MODULE; NULL for the
 * core).  Returns the event, or NULL if the name is already declared. */
extern Event* event_declare(Module* owner, const char* name);

/* Retract an event: it is no longer emitted, and its handlers wait for it
 * to be declared again.  Done by the loader for every event of a module
 * that is unloaded; a module only needs this for an event it stops
 * providing while it stays loaded. */
extern void event_retract(Event* event);

/* Call the handlers of `event' with the given arguments (at most
 * EVENT_MAX_ARGS):
 *     event_emit(user_delete_event, user, reason, is_kill);
 * Returns the value of the handler that stopped the chain, zero if none
 * did, or -1 if `event' is NULL or retracted. */
#define event_emit(...)                                                       \
    event_emit_values(EVENT_COUNT(__VA_ARGS__),                               \
                      EVENT_AS_VALUES(EVENT_COUNT(__VA_ARGS__), __VA_ARGS__))
extern int event_emit_values(int count, Event* event, ...);

/*************************************************************************/

/* For modules handling an event. */

/* Attach `handler' to the event `name' on behalf of `module'
 * (THIS_MODULE), with EVENT_PRIORITY_DEFAULT or the given priority.  The
 * event need not be declared yet.  Returns nonzero on success; zero if the
 * priority is out of range or the module already attached this handler
 * to this event. */
extern int event_attach(Module* module, const char* name,
                        EventHandler handler);
extern int event_attach_priority(Module* module, const char* name,
                                 EventHandler handler, int priority);

/* Detach a handler `module' attached.  Returns nonzero if it was
 * attached.  Unloading the module detaches all of its handlers. */
extern int event_detach(Module* module, const char* name,
                        EventHandler handler);

/*************************************************************************/

/* The core's events.  Parameters are listed as they reach the handler;
 * "stop" says what a handler returning nonzero does. */

/* The command line, for options the core does not know.
 *     (const char* option, const char* value)   value NULL if no "="
 * Return 0 if the option is not yours, 1 if it was handled, 2 for an
 * error (Services exit with an error), 3 to exit successfully. */
#define EVENT_COMMAND_LINE "core.command_line"

/* Services have linked to the network and introduced their
 * pseudo-clients.  () */
#define EVENT_UPLINK_LINKED "core.uplink_linked"

/* A database save finished.  (int succeeded) */
#define EVENT_SAVE_COMPLETE "core.save_complete"

/* A module was loaded and started / is about to be unloaded.
 *     "module.loaded":   (Module* module, const char* name)
 *     "module.unloaded": (Module* module)
 * Only needed for what events do not cover, e.g. looking up the symbols
 * of an optional module. */
#define EVENT_MODULE_LOADED   "module.loaded"
#define EVENT_MODULE_UNLOADED "module.unloaded"

/* A line from the uplink, before it is dispatched.
 *     (const char* source, const char* command, int ac, char** av)
 * Stop: the line is not dispatched. */
#define EVENT_MESSAGE_RECEIVE "message.receive"

/* A PRIVMSG to Services, before it goes to its pseudo-client.
 *     (const char* source, const char* target, char* text)
 * Stop: the message is not delivered. */
#define EVENT_MESSAGE_PRIVMSG "message.privmsg"

/* A WHOIS for a nick of Services, before the pseudo-clients answer.
 *     (const char* source, const char* nick, const char* extra)
 * Stop: the WHOIS was answered. */
#define EVENT_MESSAGE_WHOIS "message.whois"

/* Users.
 *     "user.check":   (int ac, char** av)   a NICK introducing a user;
 *                     stop: the user was killed, do not create it
 *     "user.create":  (User* user, int ac, char** av, int reconnect)
 *     "user.servicestamp_change": (User* user)
 *     "user.nick_change_before":  (User* user, const char* new_nick)
 *     "user.nick_change_after":   (User* user, const char* old_nick)
 *     "user.delete":  (User* user, const char* reason, int is_kill)
 *     "user.mode":    (User* user, int mode, int add, char** av)
 *                     stop: the mode was handled */
#define EVENT_USER_CHECK              "user.check"
#define EVENT_USER_CREATE             "user.create"
#define EVENT_USER_SERVICESTAMP       "user.servicestamp_change"
#define EVENT_USER_NICK_CHANGE_BEFORE "user.nick_change_before"
#define EVENT_USER_NICK_CHANGE_AFTER  "user.nick_change_after"
#define EVENT_USER_DELETE             "user.delete"
#define EVENT_USER_MODE               "user.mode"

/* Channels.
 *     "channel.join_check": (const char* channel, User* user)
 *                           stop: the user was kicked out
 *     "channel.create":     (Channel* channel, User* user, int32 modes)
 *     "channel.join":       (Channel* channel, struct c_userlist* member)
 *     "channel.part":       (Channel* channel, User* user,
 *                            const char* reason, const char* source)
 *     "channel.kick":       (Channel* channel, User* user,
 *                            const char* reason, const char* source)
 *     "channel.delete":     (Channel* channel)
 *     "channel.mode":       (const char* source, Channel* channel,
 *                            int mode, int add, char** av)
 *                           stop: the mode was handled
 *     "channel.mode_change":      (const char* source, Channel* channel)
 *     "channel.user_mode_change": (const char* source, Channel* channel,
 *                                  struct c_userlist* member,
 *                                  int32 old_modes)
 *     "channel.topic":      (Channel* channel, const char* topic,
 *                            const char* setter, time_t time)
 *                           stop: the topic change is refused
 *     "channel.clear":      (const char* source, Channel* channel,
 *                            int what, const void* param)
 *                           stop: the clearing was done
 *     "channel.set_topic":  (const char* source, Channel* channel,
 *                            const char* topic, const char* setter,
 *                            time_t time)   Services set a topic; after
 *                           the fact with topic NULL, stop: the topic
 *                           was sent */
#define EVENT_CHANNEL_JOIN_CHECK       "channel.join_check"
#define EVENT_CHANNEL_CREATE           "channel.create"
#define EVENT_CHANNEL_JOIN             "channel.join"
#define EVENT_CHANNEL_PART             "channel.part"
#define EVENT_CHANNEL_KICK             "channel.kick"
#define EVENT_CHANNEL_DELETE           "channel.delete"
#define EVENT_CHANNEL_MODE             "channel.mode"
#define EVENT_CHANNEL_MODE_CHANGE      "channel.mode_change"
#define EVENT_CHANNEL_USER_MODE_CHANGE "channel.user_mode_change"
#define EVENT_CHANNEL_TOPIC            "channel.topic"
#define EVENT_CHANNEL_CLEAR            "channel.clear"
#define EVENT_CHANNEL_SET_TOPIC        "channel.set_topic"

/* Servers.
 *     "server.create": (Server* server)
 *     "server.delete": (Server* server, const char* reason) */
#define EVENT_SERVER_CREATE "server.create"
#define EVENT_SERVER_DELETE "server.delete"

/* Our uplink finished its burst (END_OF_BURST) and we acknowledged it:
 * from here on the network's state is known.  () */
#define EVENT_SERVER_EOB_ACK   "server.eob_ack"

/* A pseudo-client joined the serverinfo channel (service_join_channel()):
 * each module joins its own on EVENT_SERVER_EOB_ACK, and the core joins
 * one introduced after that (its module loaded later, or brought back
 * after a KILL).  ChanServ ops it.
 *     (const char* channel, struct Service* service) */
#define EVENT_PSEUDO_CLIENT_JOINED "server.pseudo_client_joined"

/*************************************************************************/

/* Internals of event_emit(): count the arguments after the event (up to
 * EVENT_MAX_ARGS) and turn each into an intptr_t, so that every argument
 * reaches event_emit_values() with the same size whatever its type. */

#define EVENT_COUNT(...)                                                      \
    EVENT_COUNT_I(__VA_ARGS__, 6, 5, 4, 3, 2, 1, 0, EVENT_TOO_MANY_ARGS)
#define EVENT_COUNT_I(event, a1, a2, a3, a4, a5, a6, count, ...) count

#define EVENT_AS_VALUES(count, ...) EVENT_AS_VALUES_I(count, __VA_ARGS__)
#define EVENT_AS_VALUES_I(count, ...) EVENT_AS_VALUES_##count(__VA_ARGS__)
#define EVENT_V(x)                  ((intptr_t)(x))
#define EVENT_AS_VALUES_0(e)        (e)
#define EVENT_AS_VALUES_1(e, a)     (e), EVENT_V(a)
#define EVENT_AS_VALUES_2(e, a, b)  (e), EVENT_V(a), EVENT_V(b)
#define EVENT_AS_VALUES_3(e, a, b, c)                                         \
    (e), EVENT_V(a), EVENT_V(b), EVENT_V(c)
#define EVENT_AS_VALUES_4(e, a, b, c, d)                                      \
    (e), EVENT_V(a), EVENT_V(b), EVENT_V(c), EVENT_V(d)
#define EVENT_AS_VALUES_5(e, a, b, c, d, f)                                   \
    (e), EVENT_V(a), EVENT_V(b), EVENT_V(c), EVENT_V(d), EVENT_V(f)
#define EVENT_AS_VALUES_6(e, a, b, c, d, f, g)                                \
    (e), EVENT_V(a), EVENT_V(b), EVENT_V(c), EVENT_V(d), EVENT_V(f),          \
        EVENT_V(g)

/* The core (module.c): drop everything `module' attached and retract the
 * events it declared; report handlers waiting for events nobody
 * declares; free everything at exit. */
extern void event_forget_module(Module* module);
extern void event_report_undeclared(void);
extern void event_system_cleanup(void);

#endif /* EVENTS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
