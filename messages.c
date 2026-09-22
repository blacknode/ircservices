/* Definitions of IRC message functions and list of messages.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "messages.h"
#include "language.h"
#include "modules.h"
#include "version.h"
#include "modules/operserv/operserv.h"

/*************************************************************************/

/* Enable ignore code for PRIVMSGs? */
int allow_ignore = 1;

/* Callbacks for various messages */
static int cb_privmsg = -1;
static int cb_whois = -1;

/*************************************************************************/
/************************ Basic message handling *************************/
/*************************************************************************/

static void m_nickcoll(char *source, int ac, char **av)
{
    if (ac < 1)
        return;
    if (!readonly)
        introduce_user(av[0]);
}

/*************************************************************************/

static void m_ping(char *source, int ac, char **av)
{
    if (ac < 1)
        return;
    send_cmd(ServerName, "PONG %s %s", ac>1 ? av[1] : ServerName, av[0]);
}

/*************************************************************************/

static void m_info(char *source, int ac, char **av)
{
    int i;
    struct tm *tm;
    char timebuf[64];

    if (!*source) {
        log("Source missing from INFO message");
        return;
    }

    tm = localtime(&start_time);
    strftime(timebuf, sizeof(timebuf), "%a %b %d %H:%M:%S %Y %Z", tm);

    for (i = 0; info_text[i]; i++)
        send_cmd(ServerName, "371 %s :%s", source, info_text[i]);
    send_cmd(ServerName, "371 %s :Version %s (%s)", source,
             version_number, version_build);
    send_cmd(ServerName, "371 %s :On-line since %s", source, timebuf);
    send_cmd(ServerName, "374 %s :End of /INFO list.", source);
}

/*************************************************************************/

static void m_join(char *source, int ac, char **av)
{
    if (!*source) {
        log("Source missing from JOIN message");
        return;
    } else if (ac < 1) {
        return;
    }
    do_join(source, ac, av);
}

/*************************************************************************/

static void m_kick(char *source, int ac, char **av)
{
    if (!*source) {
        log("Source missing from KICK message");
        return;
    } else if (ac != 3) {
        return;
    }
    do_kick(source, ac, av);
}

/*************************************************************************/

static void m_kill(char *source, int ac, char **av)
{
    if (!*source) {
        log("Source missing from KILL message");
        return;
    } else if (ac != 2) {
        return;
    }
    /* Recover if someone kills us.  If introduce_user() returns 0, then
     * the user in question isn't a pseudoclient, so pass it on to the
     * user handling code. */
    if (!introduce_user(av[0]))
        do_kill(source, ac, av);
}

/*************************************************************************/

static void m_mode(char *source, int ac, char **av)
{
    if (!*source) {
        log("Source missing from MODE message");
        return;
    }

    if (*av[0] == '#' || *av[0] == '&') {
        if (ac < 2)
            return;
        do_cmode(source, ac, av);
    } else {
        if (ac != 2) {
            return;
        } else if (irc_stricmp(source,av[0])!=0 && strchr(source,'.')==NULL) {
            log("user: MODE %s %s from different nick %s!", av[0], av[1],
                source);
            wallops(NULL, "%s attempted to change mode %s for %s",
                    source, av[1], av[0]);
            return;
        }
        do_umode(source, ac, av);
    }
}

/*************************************************************************/

static void m_motd(char *source, int ac, char **av)
{
    FILE *f;
    char buf[BUFSIZE];

    if (!*source) {
        log("Source missing from MOTD message");
        return;
    }

    f = fopen(MOTDFilename, "r");
    send_cmd(ServerName, "375 %s :- %s Message of the Day",
             source, ServerName);
    if (f) {
        while (fgets(buf, sizeof(buf), f)) {
            buf[strlen(buf)-1] = 0;
            send_cmd(ServerName, "372 %s :- %s", source, buf);
        }
        fclose(f);
    } else {
        send_cmd(ServerName, "372 %s :- MOTD file not found!  Please "
                 "contact your IRC administrator.", source);
    }
}

/*************************************************************************/

static void m_part(char *source, int ac, char **av)
{
    if (!*source) {
        log("Source missing from PART message");
        return;
    } else if (ac < 1 || ac > 2) {
        return;
    }
    do_part(source, ac, av);
}

/*************************************************************************/

static const char msg_up_inactive[] =
    "Network buffer size exceeded inactive threshold (%d%%), not processing"
    " PRIVMSGs";
static const char msg_up_ignore[] =
    "Network buffer size exceeded ignore threshold (%d%%), ignoring PRIVMSGs";
static const char msg_down_inactive[] =
    "Network buffer size dropped below ignore threshold (%d%%), not"
    " processing PRIVMSGs";
static const char msg_down_normal[] =
    "Network buffer size dropped below inactive threshold (%d%%),"
    " processing PRIVMSGs normally";

static void m_privmsg(char *source, int ac, char **av)
{
    /* PRIVMSG handling status based on NetBufferLimit settings */
    static enum {NORMAL,INACTIVE,IGNORE} netbuf_status = NORMAL;

    uint32 start, stop;  /* When processing started and finished */
    User *u = get_user(source);
    char *s;


    if (!*source) {
        log("Source missing from PRIVMSG message");
        return;
    } else if (ac != 2) {
        return;
    }

    /* If a server is specified (nick@server format), make sure it matches
     * us, and strip it off. */
    s = strchr(av[0], '@');
    if (s) {
        *s++ = 0;
        if (stricmp(s, ServerName) != 0)
            return;
    }

    /* Check network buffer status. */
    if (NetBufferLimitInactive) {
        int bufstat = sock_bufstat(servsock, NULL, NULL, NULL, NULL);
        const char *message = NULL;
        int value = 0;
        switch (netbuf_status) {
          case NORMAL:
            if (NetBufferLimitIgnore && bufstat >= NetBufferLimitIgnore) {
                message = msg_up_ignore;
                value = NetBufferLimitIgnore;
                netbuf_status = IGNORE;
            } else if (bufstat >= NetBufferLimitInactive) {
                message = msg_up_inactive;
                value = NetBufferLimitInactive;
                netbuf_status = INACTIVE;
            }
            break;
          case INACTIVE:
            if (NetBufferLimitIgnore && bufstat >= NetBufferLimitIgnore) {
                message = msg_up_ignore;
                value = NetBufferLimitIgnore;
                netbuf_status = IGNORE;
            } else if (bufstat < NetBufferLimitInactive) {
                message = msg_down_normal;
                value = NetBufferLimitInactive;
                netbuf_status = NORMAL;
            }
            break;
          case IGNORE:
            if (bufstat < NetBufferLimitInactive) {
                message = msg_down_normal;
                value = NetBufferLimitInactive;
                netbuf_status = NORMAL;
            } else if (bufstat < NetBufferLimitIgnore) {
                message = msg_down_inactive;
                value = NetBufferLimitIgnore;
                netbuf_status = INACTIVE;
            }
            break;
        } /* switch (netbuf_status) */
        if (message) {
            log(message, value);
            wallops(NULL, message, value);
        }
    }

    /* Check if we should ignore.  Operators always get through. */
    if (u) {
        ignore_update(u, 0);
        if (!is_oper(u)) {
            if (netbuf_status != NORMAL) {
                if (netbuf_status == INACTIVE) {
                    if (u)
                        notice_lang(av[0], u, SERVICES_IS_BUSY);
                    else
                        notice(av[0], source,
                               getstring(NULL, SERVICES_IS_BUSY));
                }
                return;
            } else if (allow_ignore && IgnoreDecay && IgnoreThreshold) {
                if (u->ignore >= IgnoreThreshold) {
                    log("Ignored message from %s: \"%s\"", source, inbuf);
                    return;
                }
            }
        }
    }

    /* Not ignored; actually execute the command, and update ignore data. */
    start = time_msec();
    call_callback_3(cb_privmsg, source, av[0], av[1]);
    stop = time_msec();
    if (stop > start && u && !is_oper(u))
        ignore_update(u, stop-start);
}

/*************************************************************************/

static void m_quit(char *source, int ac, char **av)
{
    if (!*source) {
        log("Source missing from QUIT message");
        return;
    } else if (ac != 1) {
        return;
    }
    do_quit(source, ac, av);
}

/*************************************************************************/

static void m_server(char *source, int ac, char **av)
{
    do_server(source, ac, av);
}

/*************************************************************************/

static void m_squit(char *source, int ac, char **av)
{
    do_squit(source, ac, av);
}

/*************************************************************************/

static void m_stats(char *source, int ac, char **av)
{
    if (!*source) {
        log("Source missing from STATS message");
        return;
    } else if (ac < 1) {
        return;
    }

    switch (*av[0]) {
      case 'u': {
        int uptime = time(NULL) - start_time;
        Module *module_operserv;
        typeof(get_operserv_data) *p_get_operserv_data;
        int32 maxusercnt;

        send_cmd(NULL, "242 %s :Services up %d day%s, %02d:%02d:%02d",
                 source, uptime/86400, (uptime/86400 == 1) ? "" : "s",
                 (uptime/3600) % 24, (uptime/60) % 60, uptime % 60);
        if ((module_operserv = find_module("operserv/main")) != NULL
         && (p_get_operserv_data =
                 get_module_symbol(module_operserv, "get_operserv_data"))
         && p_get_operserv_data(OSDATA_MAXUSERCNT, &maxusercnt)
        ) {
            send_cmd(NULL, "250 %s :Current users: %d (%d ops); maximum %d",
                     source, usercnt, opcnt, maxusercnt);
        } else {
            send_cmd(NULL, "250 %s :Current users: %d (%d ops)",
                     source, usercnt, opcnt);
        }
        send_cmd(NULL, "219 %s u :End of /STATS report.", source);
        break;
      } /* case 'u' */

      case 'l': {
        uint64 read, written;
        sock_rwstat(servsock, &read, &written);
        send_cmd(NULL, "211 %s Server SendBuf SentBytes SentMsgs RecvBuf "
                 "RecvBytes RecvMsgs ConnTime", source);
#if SIZEOF_LONG >= 8
        send_cmd(NULL, "211 %s %s %u %lu %d %u %lu %d %ld",
                 source, RemoteServer,
                 read_buffer_len(servsock), (unsigned long)read, -1,
                 write_buffer_len(servsock), (unsigned long)written, -1,
                 (long)start_time);
#else  // assume long long is available
        send_cmd(NULL, "211 %s %s %u %llu %d %u %llu %d %ld",
                 source, RemoteServer,
                 read_buffer_len(servsock), (unsigned long long)read, -1,
                 write_buffer_len(servsock), (unsigned long long)written, -1,
                 (long)start_time);
#endif
        send_cmd(NULL, "219 %s l :End of /STATS report.", source);
        break;
      }

      case 'c':
      case 'h':
      case 'i':
      case 'k':
      case 'm':
      case 'o':
      case 'y':
        send_cmd(NULL, "219 %s %c :/STATS %c not applicable or not supported.",
                 source, *av[0], *av[0]);
        break;
    }
}

/*************************************************************************/

static void m_time(char *source, int ac, char **av)
{
    time_t t;
    struct tm *tm;
    char buf[64];

    if (!*source) {
        log("Source missing from TIME message");
        return;
    }

    time(&t);
    tm = localtime(&t);
    strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y %Z", tm);
    send_cmd(NULL, "391 %s %s :%s", source, ServerName, buf);
}

/*************************************************************************/

static void m_topic(char *source, int ac, char **av)
{
    if (ac != 4)
        return;
    do_topic(source, ac, av);
}

/*************************************************************************/

static void m_version(char *source, int ac, char **av)
{
    if (!*source) {
        log("Source missing from VERSION message");
        return;
    }
    send_cmd(ServerName, "351 %s %s-%s %s :%s", source,
             program_name, version_number, ServerName, version_build);
}

/*************************************************************************/

static void m_whois(char *source, int ac, char **av)
{
    if (!*source) {
        log("Source missing from WHOIS message");
        return;
    } else if (ac < 1) {
        return;
    }

    if (call_callback_3(cb_whois, source, av[0],
                        ac>1 ? av[1] : NULL) <= 0
    ) {
        send_cmd(ServerName, "401 %s %s :No such service.", source, av[0]);
    }
}

/*************************************************************************/

/* Basic messages (defined above).  Note that NICK and USER are left to the
 * protocol modules, since their usage varies widely between protocols. */

static Message base_messages[] = {

    { "401",       NULL },
    { "436",       m_nickcoll },
    { "AWAY",      NULL },
    { "INFO",      m_info },
    { "JOIN",      m_join },
    { "KICK",      m_kick },
    { "KILL",      m_kill },
    { "MODE",      m_mode },
    { "MOTD",      m_motd },
    { "NOTICE",    NULL },
    { "PART",      m_part },
    { "PASS",      NULL },
    { "PING",      m_ping },
    { "PONG",      NULL },
    { "PRIVMSG",   m_privmsg },
    { "QUIT",      m_quit },
    { "SERVER",    m_server },
    { "SQUIT",     m_squit },
    { "STATS",     m_stats },
    { "TIME",      m_time },
    { "TOPIC",     m_topic },
    { "VERSION",   m_version },
    { "WALLOPS",   NULL },
    { "WHOIS",     m_whois },

    { NULL }

};

/*************************************************************************/
/******************** Message registration and lookup ********************/
/*************************************************************************/

/* Structure to link tables together */
typedef struct messagetable_ MessageTable;
struct messagetable_ {
    MessageTable *next, *prev;
    Message *table;
};
static MessageTable *msgtable = NULL;

/* List of known messages (for speed-lookup list) */
typedef struct messagenode_ MessageNode;
struct messagenode_ {
    MessageNode *next, *prev;
    Message *msg;
};
static MessageNode *msglist = NULL;

/*************************************************************************/
/*************************************************************************/

/* (Re)generate the speed-lookup list.  This list is used to reduce the
 * time spent searching for messages; every time a message is seen, its
 * entry in this list is moved one place closer to the head of the list,
 * allowing frequently-seen messages to "percolate" to the top of the list
 * so that they will be found more quickly by searches.
 */

static void init_message_list(void)
{
    MessageNode *mn, *mn2;
    MessageTable *mt;
    Message *m;

    LIST_FOREACH_SAFE(mn, msglist, mn2)
        free(mn);
    msglist = NULL;

    LIST_FOREACH (mt, msgtable) {
        for (m = mt->table; m->name; m++) {
            LIST_SEARCH(msglist, msg->name, m->name, stricmp, mn);
            if (!mn) {
                mn = smalloc(sizeof(*mn));
                mn->msg = m;
                LIST_INSERT(mn, msglist);
            }
        }
    }
}

/*************************************************************************/

/* Register the given table of messages.  Returns 1 on success, 0 on
 * failure (`table' == NULL, `table' already registered, or out of memory).
 */

int register_messages(Message *table)
{
    MessageTable *mt;

    if (!table)
        return 0;
    LIST_SEARCH_SCALAR(msgtable, table, table, mt);
    if (mt)   /* if it's already on the list, abort */
        return 0;
    mt = malloc(sizeof(*mt));
    if (!mt)  /* out of memory */
        return 0;
    mt->table = table;
    LIST_INSERT(mt, msgtable);
    init_message_list();
    return 1;
}

/*************************************************************************/

/* Unregister the given table of messages.  Returns 1 on success, 0 on
 * failure (`table' not registered).
 */

int unregister_messages(Message *table)
{
    MessageTable *mt;

    LIST_SEARCH_SCALAR(msgtable, table, table, mt);
    if (!mt)
        return 0;
    LIST_REMOVE(mt, msgtable);
    free(mt);
    init_message_list();
    return 1;
}

/*************************************************************************/

/* Return the Message structure for the given message name, or NULL if none
 * exists.  If there are multiple tables with entries for the message,
 * returns the entry in the most recently registered table.
 */

Message *find_message(const char *name)
{
    MessageNode *mn;

    LIST_SEARCH(msglist, msg->name, name, stricmp, mn);
    if (mn) {
        MessageNode *prev = mn->prev;
        if (prev) {
            MessageNode *pprev = prev->prev;
            MessageNode *next = mn->next;
            /* Current order: pprev -> prev -> mn -> next */
            /*     New order: pprev -> mn -> prev -> next */
            if (pprev)
                pprev->next = mn;
            else
                msglist = mn;
            mn->prev = pprev;
            mn->next = prev;
            prev->prev = mn;
            prev->next = next;
            if (next)
                next->prev = prev;
        }
        return mn->msg;
    }
    return NULL;
}

/*************************************************************************/
/************************ Initialization/cleanup *************************/
/*************************************************************************/

int messages_init(int ac, char **av)
{
    if (!register_messages(base_messages)) {
        log("messages_init: Unable to register base messages\n");
        return 0;
    }
    cb_privmsg = register_callback("m_privmsg");
    cb_whois = register_callback("m_whois");
    if (cb_privmsg < 0 || cb_whois < 0) {
        log("messages_init: register_callback() failed\n");
        return 0;
    }
    return 1;
}

/*************************************************************************/

void messages_cleanup(void)
{
    unregister_callback(cb_whois);
    unregister_callback(cb_privmsg);
    unregister_messages(base_messages);
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
