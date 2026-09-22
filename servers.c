/* Routines to maintain a list of online servers.
 * Based on code by Andrew Kempe (TheShadow)
 *     E-mail: <theshadow@shadowfire.org>
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

#define add_server  static add_server
#define del_server  static del_server
#include "hash.h"
DEFINE_HASH(server, Server, name)
#undef add_server
#undef del_server

static Server *root_server;     /* Entry for root server (Services) */
static int16 servercnt = 0;     /* Number of online servers */

/* Callback IDs: */
static int cb_create = -1;
static int cb_delete = -1;

/*************************************************************************/
/**************************** Internal functions *************************/
/*************************************************************************/

/* Allocate a new Server structure, fill in basic values, link it to the
 * overall list, and return it. Always successful.
 */

static Server *new_server(const char *servername)
{
    Server *server;

    servercnt++;
    server = scalloc(sizeof(Server), 1);
    server->name = sstrdup(servername);
    add_server(server);
    return server;
}


/* Remove and free a Server structure. */

static void delete_server(Server *server)
{
    del_server(server);
    servercnt--;
    free(server->name);
    free(server);
}

/*************************************************************************/

/* Remove the given server.  This takes care of recursively removing child
 * servers, handling NOQUIT, calling the server delete callback, and
 * actually deleting the server data.
 */

static void recursive_squit(Server *parent, const char *reason);

static void squit_server(Server *server, const char *reason)
{
    User *user, *nextuser;

    recursive_squit(server, reason);
    if (protocol_features & PF_NOQUIT) {
#define next snext
#define prev sprev
        LIST_FOREACH_SAFE (user, server->userlist, nextuser)
            quit_user(user, reason, 0);
#undef next
#undef prev
    }
    if (!server->fake)
        call_callback_2(cb_delete, server, reason);
    delete_server(server);
}


/* "SQUIT" all servers who are linked to us via the specified server by
 * deleting them from the server list. The parent server is not deleted,
 * so this must be done by the calling function.
 */

static void recursive_squit(Server *parent, const char *reason)
{
    Server *server, *nextserver;

    server = parent->child;
    log_debug(2, "recursive_squit, parent: %s", parent->name);
    while (server) {
        nextserver = server->sibling;
        log_debug(2, "recursive_squit, child: %s", server->name);
        squit_server(server, reason);
        server = nextserver;
    }
}

/*************************************************************************/
/**************************** External functions *************************/
/*************************************************************************/

/* Handle a server SERVER command.
 *      source = server's hub; an empty string indicates that this is our hub.
 *      av[0]  = server's name
 * If ac < 0, the server in av[0] is assumed to be a "dummy" server that
 * should not be passed to the callback (e.g. for JUPE).
 *
 * When called internally to add a server (from OperServ JUPE, etc.),
 * callers may assume that the contents of the argument strings will not be
 * modified.
 */

void do_server(const char *source, int ac, char **av)
{
    Server *server, *tmpserver;

    server = new_server(av[0]);
    server->t_join = time(NULL);
    server->child = NULL;
    server->sibling = NULL;

    if (source && *source) {
        server->hub = get_server(source);
        if (!server->hub) {
            /* PARANOIA: This should NEVER EVER happen, but we check anyway.
             *
             * I've heard that on older ircds it is possible for "source"
             * not to be the new server's hub. This will cause problems.
             * -TheShadow
             */
            wallops(ServerName,
                    "WARNING: Could not find server \2%s\2 which is supposed "
                    "to be the hub for \2%s\2", source, av[0]);
            log("server: could not find hub %s for %s", source, av[0]);
            return;
        }
    } else {
        server->hub = root_server;
    }
    if (!server->hub->child) {
        server->hub->child = server;
    } else {
        tmpserver = server->hub->child;
        while (tmpserver->sibling)
            tmpserver = tmpserver->sibling;
        tmpserver->sibling = server;
    }

    if (ac > 0)
        call_callback_1(cb_create, server);
    else
        server->fake = 1;

    return;
}

/*************************************************************************/

/* Handle a server SQUIT command.
 *      av[0] = server's name
 *      av[1] = quit message
 */

void do_squit(const char *source, int ac, char **av)
{
    Server *server;

    server = get_server(av[0]);

    if (server) {
        if (server->hub) {
            if (server->hub->child == server) {
                server->hub->child = server->sibling;
            } else {
                Server *tmpserver;
                for (tmpserver = server->hub->child; tmpserver->sibling;
                                tmpserver = tmpserver->sibling) {
                    if (tmpserver->sibling == server) {
                        tmpserver->sibling = server->sibling;
                        break;
                    }
                }
            }
        }
        squit_server(server, av[1]);

    } else {
        wallops(ServerName,
                "WARNING: Tried to quit non-existent server: \2%s", av[0]);
        log("server: Tried to quit non-existent server: %s", av[0]);
        log("server: Input buffer: %s", inbuf);
        return;
    }
}

/*************************************************************************/
/*************************************************************************/

int server_init(int ac, char **av)
{
    Server *server;

    cb_create = register_callback("server create");
    cb_delete = register_callback("server delete");
    if (cb_create < 0 || cb_delete < 0) {
        log("server_init: register_callback() failed\n");
        return 0;
    }
    server = new_server("");
    server->fake = 1;
    server->t_join = time(NULL);
    server->hub = server->child = server->sibling = NULL;
    root_server = server;
    return 1;
}

/*************************************************************************/

/* Remove all servers; this recursively takes out all users and channels,
 * as well.
 */

void server_cleanup(void)
{
    uint32 pf = protocol_features;
    protocol_features |= PF_NOQUIT;
    squit_server(root_server, "server_cleanup");
    protocol_features = pf;
    unregister_callback(cb_delete);
    unregister_callback(cb_create);
}

/*************************************************************************/

/* Return information on memory use. Assumes pointers are valid. */

void get_server_stats(long *nservers, long *memuse)
{
    Server *server;
    long mem;

    mem = sizeof(Server) * servercnt;
    for (server = first_server(); server; server = next_server())
        mem += strlen(server->name)+1;

    *nservers = servercnt;
    *memuse = mem;
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
