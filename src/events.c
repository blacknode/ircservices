/* Events: named points the core and the modules announce (events.h).
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"

/*************************************************************************/

/* One handler attached to an event. */
typedef struct {
    EventHandler handler; /* NULL once detached during an emission */
    Module* module;       /* Who attached it (NULL: the core) */
    int priority;
} AttachedHandler;

/* An event, declared or not: a name handlers were attached to exists as
 * soon as either happens, and is only freed at exit, so the pointers the
 * owners hold stay valid. */
struct Event_ {
    Event *next, *prev;
    char* name;
    int declared;           /* Nonzero while its owner provides it */
    Module* owner;          /* Valid while declared (NULL: the core) */
    int emitting;           /* Emissions of it in progress (nesting) */
    int has_detached;       /* A handler was detached while emitting */
    AttachedHandler* handlers;
    int handlers_count;
};

static Event* event_list;

/*************************************************************************/

static Event* find_event(const char* name)
{
    Event* event;

    LIST_SEARCH(event_list, name, name, strcmp, event);
    return event;
}

static Event* find_or_add_event(const char* name)
{
    Event* event = find_event(name);

    if (!event) {
        event = scalloc(sizeof(*event), 1);
        event->name = sstrdup(name);
        LIST_INSERT(event, event_list);
    }
    return event;
}

/* Remove handler `index' of `event', or only mark it if the event is
 * being emitted (the emission compacts the array when it is done). */
static void remove_handler(Event* event, int index)
{
    if (event->emitting) {
        event->handlers[index].handler = NULL;
        event->has_detached = 1;
    }
    else {
        ARRAY_REMOVE(event->handlers, index);
    }
}

/*************************************************************************/
/*************************************************************************/

Event* event_declare(Module* owner, const char* name)
{
    Event* event;

    if (!name || !*name) {
        log("BUG: event_declare() from %s without a name",
            module_name(owner));
        return NULL;
    }
    event = find_or_add_event(name);
    if (event->declared) {
        log("BUG: event_declare(): `%s' (%s) is already declared by %s",
            name, module_name(owner), module_name(event->owner));
        return NULL;
    }
    event->declared = 1;
    event->owner = owner;
    log_debug(2, "event: %s declares `%s'", module_name(owner), name);
    return event;
}

/*************************************************************************/

void event_retract(Event* event)
{
    if (!event)
        return;
    log_debug(2, "event: `%s' retracted", event->name);
    event->declared = 0;
    event->owner = NULL;
}

/*************************************************************************/

int event_emit_values(int count, Event* event, ...)
{
    intptr_t args[EVENT_MAX_ARGS];
    va_list list;
    int i, result = 0;

    if (!event || !event->declared)
        return -1;
    if (count < 0 || count > EVENT_MAX_ARGS) {
        log("BUG: event `%s' emitted with %d arguments (at most %d)",
            event->name, count, EVENT_MAX_ARGS);
        return -1;
    }
    va_start(list, event);
    for (i = 0; i < count; i++)
        args[i] = va_arg(list, intptr_t);
    va_end(list);

    event->emitting++;
    ARRAY_FOREACH(i, event->handlers)
    {
        EventHandler handler = event->handlers[i].handler;
        if (!handler)
            continue;
        /* Called with exactly the arguments the event was emitted with.
         * The handler is declared with the event's own parameter types;
         * see EventHandler in events.h. */
        switch (count) {
            case 0: result = handler(); break;
            case 1: result = handler(args[0]); break;
            case 2: result = handler(args[0], args[1]); break;
            case 3: result = handler(args[0], args[1], args[2]); break;
            case 4:
                result = handler(args[0], args[1], args[2], args[3]);
                break;
            case 5:
                result = handler(args[0], args[1], args[2], args[3], args[4]);
                break;
            default:
                result = handler(args[0], args[1], args[2], args[3], args[4],
                                 args[5]);
                break;
        }
        if (result != 0)
            break;
    }
    if (--event->emitting == 0 && event->has_detached) {
        ARRAY_FOREACH(i, event->handlers)
        {
            if (!event->handlers[i].handler) {
                ARRAY_REMOVE(event->handlers, i);
                i--;
            }
        }
        event->has_detached = 0;
    }
    return result;
}

/*************************************************************************/
/*************************************************************************/

int event_attach(Module* module, const char* name, EventHandler handler)
{
    return event_attach_priority(module, name, handler,
                                 EVENT_PRIORITY_DEFAULT);
}

int event_attach_priority(Module* module, const char* name,
                          EventHandler handler, int priority)
{
    Event* event;
    int i;

    if (!name || !handler) {
        log("BUG: event_attach() from %s without a name or a handler",
            module_name(module));
        return 0;
    }
    if (priority < EVENT_PRIORITY_LAST || priority > EVENT_PRIORITY_FIRST) {
        log("BUG: event_attach(): priority %d out of range for `%s' (%s)",
            priority, name, module_name(module));
        return 0;
    }
    event = find_or_add_event(name);
    ARRAY_FOREACH(i, event->handlers)
    {
        if (event->handlers[i].handler == handler &&
            event->handlers[i].module == module) {
            log("BUG: event_attach(): %s attached the same handler to `%s'"
                " twice",
                module_name(module), name);
            return 0;
        }
    }
    /* After every handler of the same or a higher priority. */
    ARRAY_FOREACH(i, event->handlers)
    {
        if (event->handlers[i].priority < priority)
            break;
    }
    ARRAY_INSERT(event->handlers, i);
    event->handlers[i].handler = handler;
    event->handlers[i].module = module;
    event->handlers[i].priority = priority;
    return 1;
}

/*************************************************************************/

int event_detach(Module* module, const char* name, EventHandler handler)
{
    Event* event = name ? find_event(name) : NULL;
    int i;

    if (!event)
        return 0;
    ARRAY_FOREACH(i, event->handlers)
    {
        if (event->handlers[i].handler == handler &&
            event->handlers[i].module == module) {
            remove_handler(event, i);
            return 1;
        }
    }
    return 0;
}

/*************************************************************************/
/*************************************************************************/

void event_forget_module(Module* module)
{
    Event* event;
    int i;

    LIST_FOREACH(event, event_list)
    {
        ARRAY_FOREACH(i, event->handlers)
        {
            if (event->handlers[i].handler &&
                event->handlers[i].module == module) {
                remove_handler(event, i);
                if (!event->emitting)
                    i--;
            }
        }
        if (event->declared && event->owner == module)
            event_retract(event);
    }
}

/*************************************************************************/

void event_report_undeclared(void)
{
    Event* event;
    int i;

    LIST_FOREACH(event, event_list)
    {
        if (event->declared)
            continue;
        ARRAY_FOREACH(i, event->handlers)
        {
            if (event->handlers[i].handler) {
                log_debug(1,
                          "event: `%s' has handlers (%s, ...) but nothing"
                          " declares it; they will run once it is declared",
                          event->name,
                          module_name(event->handlers[i].module));
                break;
            }
        }
    }
}

/*************************************************************************/

void event_system_cleanup(void)
{
    Event *event, *next;

    LIST_FOREACH_SAFE(event, event_list, next)
    {
        int i;
        ARRAY_FOREACH(i, event->handlers)
        {
            if (event->handlers[i].handler && event->handlers[i].module) {
                log("BUG: event `%s': handler of %s still attached at exit",
                    event->name, module_name(event->handlers[i].module));
            }
        }
        LIST_REMOVE(event, event_list);
        free(event->handlers);
        free(event->name);
        free(event);
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
