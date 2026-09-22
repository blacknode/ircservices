/* Channel-handling routines.
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

#define HASHFUNC(key) DEFAULT_HASHFUNC(key+1)
#define add_channel  static add_channel
#define del_channel  static del_channel
#include "hash.h"
DEFINE_HASH(channel, Channel, name)
#undef add_channel
#undef del_channel

static int cb_create       = -1;
static int cb_delete       = -1;
static int cb_join         = -1;
static int cb_join_check   = -1;
static int cb_mode         = -1;
static int cb_mode_change  = -1;
static int cb_umode_change = -1;
static int cb_topic        = -1;

/*************************************************************************/
/*************************************************************************/

int channel_init(int ac, char **av)
{
    cb_create       = register_callback("channel create");
    cb_delete       = register_callback("channel delete");
    cb_join         = register_callback("channel JOIN");
    cb_join_check   = register_callback("channel JOIN check");
    cb_mode         = register_callback("channel MODE");
    cb_mode_change  = register_callback("channel mode change");
    cb_umode_change = register_callback("channel umode change");
    cb_topic        = register_callback("channel TOPIC");
    if (cb_create < 0 || cb_delete < 0 || cb_join < 0 || cb_join_check < 0
     || cb_mode < 0 || cb_mode_change < 0 || cb_umode_change < 0
     || cb_topic < 0
    ) {
        log("channel_init: register_callback() failed\n");
        return 0;
    }
    return 1;
}

/*************************************************************************/

void channel_cleanup(void)
{
    Channel *c;

    for (c = first_channel(); c; c = next_channel())
        del_channel(c);
    unregister_callback(cb_topic);
    unregister_callback(cb_umode_change);
    unregister_callback(cb_mode_change);
    unregister_callback(cb_mode);
    unregister_callback(cb_join_check);
    unregister_callback(cb_join);
    unregister_callback(cb_delete);
    unregister_callback(cb_create);
}

/*************************************************************************/

/* Return statistics.  Pointers are assumed to be valid. */

void get_channel_stats(long *nrec, long *memuse)
{
    long count = 0, mem = 0;
    Channel *chan;
    struct c_userlist *cu;
    int i;

    for (chan = first_channel(); chan; chan = next_channel()) {
        count++;
        mem += sizeof(*chan);
        if (chan->topic)
            mem += strlen(chan->topic)+1;
        if (chan->key)
            mem += strlen(chan->key)+1;
        ARRAY_FOREACH (i, chan->bans) {
            mem += sizeof(char *);
            if (chan->bans[i])
                mem += strlen(chan->bans[i])+1;
        }
        ARRAY_FOREACH (i, chan->excepts) {
            mem += sizeof(char *);
            if (chan->excepts[i])
                mem += strlen(chan->excepts[i])+1;
        }
        LIST_FOREACH (cu, chan->users)
            mem += sizeof(*cu);
    }
    *nrec = count;
    *memuse = mem;
}

/*************************************************************************/
/*************************************************************************/

/* Add/remove a user to/from a channel, creating or deleting the channel as
 * necessary.  If creating the channel, restore mode lock and topic as
 * necessary.  Also check for auto-opping and auto-voicing.  If a mode is
 * given, it is assumed to have been set by the remote server.
 * Returns the Channel structure for the given channel, or NULL if the user
 * was refused access to the channel (by the join check callback).
 */

Channel *chan_adduser(User *user, const char *chan, int32 modes)
{
    Channel *c = get_channel(chan);
    int newchan = !c;
    struct c_userlist *u;

    if (call_callback_2(cb_join_check, chan, user) > 0)
        return NULL;
    if (newchan) {
        log_debug(1, "Creating channel %s", chan);
        /* Allocate pre-cleared memory */
        c = scalloc(sizeof(Channel), 1);
        strbcpy(c->name, chan);
        c->creation_time = time(NULL);
        add_channel(c);
        call_callback_3(cb_create, c, user, modes);
    }
    u = smalloc(sizeof(struct c_userlist));
    LIST_INSERT(u, c->users);
    u->user = user;
    u->mode = modes;
    u->flags = 0;
    call_callback_2(cb_join, c, u);
    return c;
}


void chan_deluser(User *user, Channel *c)
{
    struct c_userlist *u;
    int i;

    LIST_SEARCH_SCALAR(c->users, user, user, u);
    if (!u) {
        log("channel: BUG: chan_deluser() called for %s in %s but they "
            "were not found on the channel's userlist.",
            user->nick, c->name);
        return;
    }
    LIST_REMOVE(u, c->users);
    free(u);

    if (!c->users) {
        log_debug(1, "Deleting channel %s", c->name);
        call_callback_1(cb_delete, c);
        set_cmode(NULL, c);  /* make sure nothing's left buffered */
        free(c->topic);
        free(c->key);
        free(c->link);
        free(c->flood);
        for (i = 0; i < c->bans_count; i++)
            free(c->bans[i]);
        free(c->bans);
        for (i = 0; i < c->excepts_count; i++)
            free(c->excepts[i]);
        free(c->excepts);
        del_channel(c);
        free(c);
    }
}

/*************************************************************************/

/* Search for the given ban on the given channel, and return the index into
 * chan->bans[] if found, -1 otherwise.  Nicknames are compared using
 * irc_stricmp(), usernames and hostnames using stricmp().
 */
static int find_ban(const Channel *chan, const char *ban)
{
    char *s, *t;
    int i;

    t = strchr(ban, '!');
    i = 0;
    ARRAY_FOREACH (i, chan->bans) {
        s = strchr(chan->bans[i], '!');
        if (s && t) {
            if (s-(chan->bans[i]) == t-ban
             && irc_strnicmp(chan->bans[i], ban, s-(chan->bans[i])) == 0
             && stricmp(s+1, t+1) == 0
            ) {
                return i;
            }
        } else if (!s && !t) {
            /* Bans without '!' should be impossible; just
             * do a case-insensitive compare */
            if (stricmp(chan->bans[i], ban) == 0)
                return i;
        }
    }
    return -1;
}

/*************************************************************************/

/* Search for the given ban (case-insensitive) on the channel; return 1 if
 * it exists, 0 if not.
 */

int chan_has_ban(const char *chan, const char *ban)
{
    Channel *c = get_channel(chan);
    if (!c)
        return 0;
    return find_ban(c, ban) >= 0;
}

/*************************************************************************/

/* Handle a channel MODE command.
 * When called internally to modify channel modes, callers may assume that
 * the contents of the argument strings will not be modified.
 */

/* Hack to allow -o+v to work without having to search the whole channel
 * user list for changed modes. */
#define MAX_CUMODES     16
static struct {
    struct c_userlist *user;
    int32 add, remove;
} cumode_changes[MAX_CUMODES];
static int cumode_count = 0;

static void do_cumode(const char *source, Channel *chan, int32 flag, int add,
                      const char *nick);
static void finish_cumode(const char *source, Channel *chan);

void do_cmode(const char *source, int ac, char **av)
{
    Channel *chan;
    char *s;
    int add = 1;                /* 1 if adding modes, 0 if deleting */
    char *modestr;

    chan = get_channel(av[0]);
    if (!chan) {
        log_debug(1, "channel: MODE %s for nonexistent channel %s",
                  merge_args(ac-1, av+1), av[0]);
        return;
    }
    if ((protocol_features & PF_MODETS_FIRST) && isdigit(av[1][0])) {
        ac--;
        av++;
    }
    modestr = av[1];

    if (!NoBouncyModes) {
        /* Count identical server mode changes per second (mode bounce check)*/
        /* Doesn't trigger on +/-[bov] or other multiply-settable modes */
        static char multimodes[BUFSIZE];
        if (!*multimodes) {
            int i = 0;
            i = snprintf(multimodes, sizeof(multimodes), "%s",
                         chanmode_multiple);
            snprintf(multimodes+i, sizeof(multimodes)-i, "%s",
                     mode_flags_to_string(MODE_ALL,MODE_CHANUSER));
        }
        if (strchr(source, '.') && strcmp(source, ServerName) != 0
         && !modestr[strcspn(modestr, multimodes)]
        ) {
            static char lastmodes[BUFSIZE];
            if (time(NULL) != chan->server_modetime
             || strcmp(modestr, lastmodes) != 0
            ) {
                chan->server_modecount = 0;
                chan->server_modetime = time(NULL);
                strbcpy(lastmodes, modestr);
            }
            chan->server_modecount++;
        }
    }

    s = modestr;
    ac -= 2;
    av += 2;
    cumode_count = 0;

    while (*s) {
        char modechar = *s++;
        int32 flag;
        int params;

        if (modechar == '+') {
            add = 1;
            continue;
        } else if (modechar == '-') {
            add = 0;
            continue;
        } else if (add < 0) {
            continue;
        }

        /* Check for it as a channel user mode */

        flag = mode_char_to_flag(modechar, MODE_CHANUSER);
        if (flag) {
            if (--ac < 0) {
                log("channel: MODE %s %s: missing parameter for %c%c",
                    chan->name, modestr, add ? '+' : '-', modechar);
                break;
            }
            do_cumode(source, chan, flag, add, *av++);
            continue;
        }

        /* Nope, must be a regular channel mode */

        flag = mode_char_to_flag(modechar, MODE_CHANNEL);
        if (!flag)
            continue;
        if (flag == MODE_INVALID)
            flag = 0;
        params = mode_char_to_params(modechar, MODE_CHANNEL);
        params = (params >> (add*8)) & 0xFF;
        if (ac < params) {
            log("channel: MODE %s %s: missing parameter(s) for %c%c",
                chan->name, modestr, add ? '+' : '-', modechar);
            break;
        }

        if (call_callback_5(cb_mode, source, chan, modechar, add, av) <= 0) {

            if (add)
                chan->mode |= flag;
            else
                chan->mode &= ~flag;

            switch (modechar) {
              case 'k':
                free(chan->key);
                if (add)
                    chan->key = sstrdup(av[0]);
                else
                    chan->key = NULL;
                break;

              case 'l':
                if (add)
                    chan->limit = atoi(av[0]);
                else
                    chan->limit = 0;
                break;

              case 'b': {
                int i = find_ban(chan, av[0]);
                if (add) {
                    if (i < 0) {  /* Don't add if it already exists */
                        ARRAY_EXTEND(chan->bans);
                        chan->bans[chan->bans_count-1] = sstrdup(av[0]);
                    }
                } else {
                    if (i >= 0) {
                        free(chan->bans[i]);
                        ARRAY_REMOVE(chan->bans, i);
                    } else {
                        log("channel: MODE %s -b %s: ban not found",
                            chan->name, *av);
                    }
                }
                break;
              }  /* case 'b' */

            } /* switch (modechar) */

        } /* if (callback() <= 0) */

        ac -= params;
        av += params;

    } /* while (*s) */

    call_callback_2(cb_mode_change, source, chan);
    finish_cumode(source, chan);
}

/* Modify a user's CUMODE. */
static void do_cumode(const char *source, Channel *chan, int32 flag, int add,
                      const char *nick)
{
    struct c_userlist *u;
    User *user;
    int i;

    user = get_user(nick);
    if (!user) {
        log_debug(1, "channel: MODE %s %c%c for nonexistent user %s",
                  chan->name, add ? '+' : '-',
                  mode_flag_to_char(flag, MODE_CHANUSER), nick);
        return;
    }
    LIST_SEARCH_SCALAR(chan->users, user, user, u);
    if (!u) {
        log("channel: MODE %s %c%c for user %s not on channel",
            chan->name, add ? '+' : '-',
            mode_flag_to_char(flag, MODE_CHANUSER), nick);
        return;
    }

    if (flag == MODE_INVALID)
        return;
    for (i = 0; i < cumode_count; i++) {
        if (cumode_changes[i].user == u)
            break;
    }
    if (i >= MAX_CUMODES) {
        finish_cumode(source, chan);
        i = cumode_count = 0;
    }
    cumode_changes[i].user = u;
    if (i >= cumode_count) {
        cumode_changes[i].add = cumode_changes[i].remove = 0;
    }
    if (add) {
        cumode_changes[i].add |= flag;
        cumode_changes[i].remove &= ~flag;
    } else {
        cumode_changes[i].remove |= flag;
        cumode_changes[i].add &= ~flag;
    }
    if (i >= cumode_count)
        cumode_count = i+1;
}

static void finish_cumode(const char *source, Channel *chan)
{
    int i;

    for (i = 0; i < cumode_count; i++) {
        struct c_userlist *u = cumode_changes[i].user;
        int32 oldmode = u->mode;
        u->mode |= cumode_changes[i].add;
        u->mode &= ~cumode_changes[i].remove;
        if (u->mode != oldmode)
            call_callback_4(cb_umode_change, source, chan, u, oldmode);
    }
    cumode_count = 0;
}

/*************************************************************************/

/* Handle a TOPIC command. */

void do_topic(const char *source, int ac, char **av)
{
    Channel *c = get_channel(av[0]);
    const char *topic;
    char *s;

    if (!c) {
        log_debug(1, "channel: TOPIC %s for nonexistent channel %s",
                  merge_args(ac-1, av+1), av[0]);
        return;
    }
    s = strchr(av[1], '!');
    if (s)
        *s = 0;
    if (ac > 3)
        topic = av[3];
    else
        topic = "";
    if (call_callback_4(cb_topic, c, topic, av[1], strtotime(av[2],NULL)) > 0)
        return;
    strbcpy(c->topic_setter, av[1]);
    if (c->topic) {
        free(c->topic);
        c->topic = NULL;
    }
    if (ac > 3 && *av[3])
        c->topic = sstrdup(av[3]);
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
