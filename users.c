/* Routines to maintain a list of online users.
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

/* Maximum number of tries to randomly select a new guest nick when the
 * first one chosen is in use before giving up.
 */
#define MAKEGUESTNICK_TRIES     1000

/*************************************************************************/

#define add_user  static add_user
#define del_user  static del_user
#include "hash.h"
DEFINE_HASH(user, User, nick)
#undef add_user
#undef del_user

int32 usercnt = 0, opcnt = 0;

static int cb_check = -1;
static int cb_create = -1;
static int cb_servicestamp_change = -1;
static int cb_nickchange1 = -1;
static int cb_nickchange2 = -1;
static int cb_delete = -1;
static int cb_mode = -1;
static int cb_chan_part = -1;
static int cb_chan_kick = -1;

/*************************************************************************/

int user_init(int ac, char **av)
{
    cb_check               = register_callback("user check");
    cb_create              = register_callback("user create");
    cb_servicestamp_change = register_callback("user servicestamp change");
    cb_nickchange1         = register_callback("user nickchange (before)");
    cb_nickchange2         = register_callback("user nickchange (after)");
    cb_delete              = register_callback("user delete");
    cb_mode                = register_callback("user MODE");
    cb_chan_part           = register_callback("channel PART");
    cb_chan_kick           = register_callback("channel KICK");
    if (cb_check < 0 || cb_create < 0 || cb_servicestamp_change < 0
     || cb_nickchange1 < 0 || cb_nickchange2 < 0 || cb_delete < 0
     || cb_mode < 0 || cb_chan_part < 0 || cb_chan_kick < 0
    ) {
        log("user_init: register_callback() failed\n");
        return 0;
    }
    return 1;
}

/*************************************************************************/

void user_cleanup(void)
{
    User *u;

    for (u = first_user(); u; u = next_user())
        del_user(u);
    unregister_callback(cb_chan_kick);
    unregister_callback(cb_chan_part);
    unregister_callback(cb_mode);
    unregister_callback(cb_delete);
    unregister_callback(cb_nickchange2);
    unregister_callback(cb_nickchange1);
    unregister_callback(cb_servicestamp_change);
    unregister_callback(cb_create);
    unregister_callback(cb_check);
}

/*************************************************************************/
/************************* User list management **************************/
/*************************************************************************/

/* Allocate a new User structure, fill in basic values, link it to the
 * overall list, and return it.  Always successful.
 */

static User *new_user(const char *nick)
{
    User *user;

    user = scalloc(sizeof(User), 1);
    if (!nick)
        nick = "";
    strbcpy(user->nick, nick);
    add_user(user);
    usercnt++;
    return user;
}

/*************************************************************************/

/* Change the nickname of a user, and move pointers as necessary. */

static void change_user_nick(User *user, const char *nick)
{
    del_user(user);
    strbcpy(user->nick, nick);
    add_user(user);
}

/*************************************************************************/

/* Remove and free a User structure. */

static void delete_user(User *user)
{
    struct u_chanlist *c, *c2;
    struct u_chaninfolist *ci, *ci2;

    usercnt--;
    if (is_oper(user))
        opcnt--;

    free(user->username);
    free(user->host);
    free(user->ipaddr);
    free(user->realname);
    free(user->fakehost);
    free(user->id_nicks);
    LIST_FOREACH_SAFE (c, user->chans, c2) {
        chan_deluser(user, c->chan);
        free(c);
    }
    LIST_FOREACH_SAFE (ci, user->id_chans, ci2)
        free(ci);
#define next snext
#define prev sprev
    if (user->server)
        LIST_REMOVE(user, user->server->userlist);
#undef next
#undef prev
    del_user(user);
    free(user);
}

/*************************************************************************/
/*************************************************************************/

/* Remove a user on QUIT/KILL.  Calls the user delete callback and then
 * deletes the User structure.
 */

void quit_user(User *user, const char *quitmsg, int is_kill)
{
    call_callback_3(cb_delete, user, quitmsg, is_kill);
    delete_user(user);
}

/*************************************************************************/

/* Return statistics.  Pointers are assumed to be valid. */

void get_user_stats(long *nusers, long *memuse)
{
    long count = 0, mem = 0;
    User *user;
    struct u_chanlist *uc;
    struct u_chaninfolist *uci;

    for (user = first_user(); user; user = next_user()) {
        count++;
        mem += sizeof(*user);
        if (user->username)
            mem += strlen(user->username)+1;
        if (user->host)
            mem += strlen(user->host)+1;
        if (user->realname)
            mem += strlen(user->realname)+1;
        LIST_FOREACH (uc, user->chans)
            mem += sizeof(*uc);
        LIST_FOREACH (uci, user->id_chans)
            mem += sizeof(*uci);
    }
    *nusers = count;
    *memuse = mem;
}

/*************************************************************************/
/************************* Internal routines *****************************/
/*************************************************************************/

/* Part a user from a channel given the user's u_chanlist entry for the
 * channel. */

static void part_channel_uc(User *user, struct u_chanlist *uc, int callback,
                            const char *param, const char *source)
{
    call_callback_4(callback, uc->chan, user, param, source);
    chan_deluser(user, uc->chan);
    LIST_REMOVE(uc, user->chans);
    free(uc);
}

/*************************************************************************/
/************************* Message handlers ******************************/
/*************************************************************************/

/* Handle a server NICK command.  Parameters must be in the following order.
 *      av[0] = nick
 *      If a new user:
 *              av[1] = hop count
 *              av[2] = signon time
 *              av[3] = username
 *              av[4] = hostname
 *              av[5] = server
 *              av[6] = real name
 *              av[7] = services stamp (if ac >= 8; NULL if none)
 *              av[8] = IP address (if ac >= 9; NULL if unknown)
 *              av[9] = user modes (lf ac >= 10; NULL if unknown.
 *                                  Leading + optional)
 *              av[10..] available for protocol module use
 *      Else:
 *              av[1] = time of change
 * Return 1 if message was accepted, 0 if rejected (AKILL/session limit).
 */

int do_nick(const char *source, int ac, char **av)
{
    User *user;

    if (!*source) {
        /* This is a new user; create a User structure for it. */

        int reconnect = 0;  /* Is user reconnecting after a split? */

        log_debug(1, "new user: %s", av[0]);

        /* We used to ignore the ~ which a lot of ircd's use to indicate no
         * identd response.  That caused channel bans to break, so now we
         * just take what the server gives us.  People are still encouraged
         * to read the RFCs and stop doing anything to usernames depending
         * on the result of an identd lookup. */

        /* First check whether the user should be allowed on. */
        if (call_callback_2(cb_check, ac, av))
            return 0;

        /* User was accepted; allocate User structure and fill it in. */
        user = new_user(av[0]);
        user->my_signon = time(NULL);
        user->signon = strtotime(av[2], NULL);
        user->username = sstrdup(av[3]);
        user->host = sstrdup(av[4]);
        user->server = get_server(av[5]);
        user->realname = sstrdup(av[6]);
        if (ac >= 8 && av[7]) {
            user->servicestamp = strtoul(av[7], NULL, 10);
            reconnect = (user->servicestamp != 0);
        } else {
            user->servicestamp = (uint32)user->signon;
            /* Unfortunately, we have no way to tell whether the user is
             * new or not */
        }
        if (ac >= 9 && av[8])
            user->ipaddr = sstrdup(av[8]);
        else
            user->ipaddr = NULL;
#define next snext
#define prev sprev
        if (user->server)
            LIST_INSERT(user, user->server->userlist);
#undef next
#undef prev
        ignore_init(user);

        call_callback_4(cb_create, user, ac, av, reconnect);

        if (ac >= 8 && av[7] && !user->servicestamp) {
            /* A servicestamp was provided, but it was zero, so assign one.
             * Note that we use a random value for the initial Services
             * stamp instead of the current time for the following reason:
             *
             * Suppose you have a network with an average of more than one
             * new user per second; for the sake of argument, assume there
             * are an average of 1.3 new users per second.  If the initial
             * Services stamp is T, the current time, then in 100 seconds
             * (i.e. at T+100) the Services stamp will have gone to T+130.
             * (In reality, it would jump much higher on the initial net
             * burst when no users have Services stamps, but that does not
             * affect this argument.)
             *
             * If Services is now restarted, clearing the last used stamp
             * value, then assuming 5 seconds for restart, Services will
             * receive a network burst at T+105.  However!  While most of
             * the users will already have Services stamps, any new users
             * (as well as any users which connect after the network burst)
             * will be assigned new Services stamps starting with the
             * default value of the current time, in this case T+105.  But
             * other users _already_ have Services stamp values in the
             * range T+105 to T+130 from the previous run--thus you have
             * Services stamp collisions, and all the security problems
             * that go with them.
             *
             * Obviously, this possibility does not disappear entirely by
             * using a random initial value, but it becomes much more
             * unlikely.
             *
             * Note that Unreal 3.1.1 (at least) upper-bounds values at
             * 2^31-1, so we limit ourselves to 31 bits here, even though
             * our field is unsigned.
             */

            static int32 servstamp = 0;

            if (servstamp == 0)
                servstamp = (rand() & 0x7FFFFFFF) | 1;
            user->servicestamp = servstamp++;
            if (servstamp <= 0)
                servstamp = 1;
            call_callback_1(cb_servicestamp_change, user);
        }

        if (ac >= 10 && av[9] && *av[9]) {
            /* Apply modes supplied in av[9].  Current protocol modules all
             * include a '+' before the mode letters, but allow strings
             * without the '+' for robustness. */
            char buf[BUFSIZE];
            char *newav[2];
            newav[0] = user->nick;
            if (*av[9] == '+') {
                newav[1] = av[9];
            } else {
                snprintf(buf, sizeof(buf), "+%s", av[9]);
                newav[1] = buf;
            }
            do_umode(user->nick, 2, newav);
        }

    } else {
        /* An old user changing nicks. */
        char oldnick[NICKMAX];

        user = get_user(source);
        if (!user) {
            log_debug(1, "user: NICK from nonexistent nick %s: %s",
                      source, merge_args(ac, av));
            return 0;
        }
        log_debug(1, "%s changes nick to %s", source, av[0]);

        strbcpy(oldnick, user->nick);
        call_callback_2(cb_nickchange1, user, av[0]);
        /* Flush out all mode changes; necessary to avoid desynch (otherwise
         * we can't find the user when the mode goes out later).  The IRC
         * servers will take care of translating the old nick to the new one */
        set_cmode(NULL, NULL);
        change_user_nick(user, av[0]);
        call_callback_2(cb_nickchange2, user, oldnick);
    }

    return 1;
}

/*************************************************************************/

/* Handle a JOIN command.
 *      av[0] = channels to join
 */

void do_join(const char *source, int ac, char **av)
{
    User *user;
    char *s, *t;

    user = get_user(source);
    if (!user) {
        log_debug(1, "user: JOIN from nonexistent user %s: %s",
                  source, merge_args(ac, av));
        return;
    }
    t = av[0];
    while (*(s=t)) {
        t = s + strcspn(s, ",");
        if (*t)
            *t++ = 0;
        log_debug(1, "%s joins %s", source, s);

        if (*s == '0')
            part_all_channels(user);
        else
            join_channel(user, s, 0);
    }
}

/*************************************************************************/

/* Handle a PART command.
 *      av[0] = channels to leave
 *      av[1] = reason (optional)
 */

void do_part(const char *source, int ac, char **av)
{
    User *user;
    char *s, *t;

    user = get_user(source);
    if (!user) {
        log_debug(1, "user: PART from nonexistent user %s: %s",
                  source, merge_args(ac, av));
        return;
    }
    t = av[0];
    while (*(s=t)) {
        t = s + strcspn(s, ",");
        if (*t)
            *t++ = 0;
        log_debug(1, "%s leaves %s", source, s);
        if (!part_channel(user, s, cb_chan_part, av[1], source)) {
            log("user: do_part: no channel record for %s on %s (bug?)",
                user->nick, av[0]);
        }
    }
}

/*************************************************************************/

/* Handle a KICK command.
 *      av[0] = channel
 *      av[1] = nick(s) being kicked
 *      av[2] = reason
 * When called internally to remove a single user (no "," in av[1]) from a
 * channel, callers may assume that the contents of the argument strings
 * will not be modified.
 */

void do_kick(const char *source, int ac, char **av)
{
    User *user;
    char *s, *t;

    t = av[1];
    while (*(s=t)) {
        t = s + strcspn(s, ",");
        if (*t)
            *t++ = 0;
        user = get_user(s);
        if (!user) {
            log_debug(1, "user: KICK for nonexistent user %s on %s: %s",
                      s, av[0], merge_args(ac-2, av+2));
            continue;
        }
        log_debug(1, "kicking %s from %s", s, av[0]);
        if (!part_channel(user, av[0], cb_chan_kick, av[2], source)) {
            log("user: do_kick: no channel record for %s on %s (bug?)",
                user->nick, av[0]);
        }
    }
}

/*************************************************************************/

/* Handle a MODE command for a user.
 *      av[0] = nick to change mode for
 *      av[1] = modes
 */

void do_umode(const char *source, int ac, char **av)
{
    User *user;
    char *modestr, *s;
    int add = 1;                /* 1 if adding modes, 0 if deleting */

    user = get_user(av[0]);
    if (!user) {
        log_debug(1, "user: MODE %s for nonexistent nick %s from %s: %s",
                  av[1], av[0], source, merge_args(ac, av));
        return;
    }
    log_debug(1, "Changing mode for %s to %s", av[0], av[1]);
    modestr = s = av[1];
    av += 2;
    ac -= 2;

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

        flag = mode_char_to_flag(modechar, MODE_USER);
        if (!flag)
            continue;
        if (flag == MODE_INVALID)
            flag = 0;
        params = mode_char_to_params(modechar, MODE_USER);
        params = (params >> (add*8)) & 0xFF;
        if (ac < params) {
            log("user: MODE %s %s: missing parameter(s) for %c%c",
                user->nick, modestr, add ? '+' : '-', modechar);
            break;
        }

        if (call_callback_4(cb_mode, user, modechar, add, av) <= 0) {
            if (modechar == 'o') {
                if (add)
                    opcnt++;
                else
                    opcnt--;
            }
            if (add)
                user->mode |= flag;
            else
                user->mode &= ~flag;
        }
        av += params;
        ac -= params;
    }
}

/*************************************************************************/

/* Handle a QUIT command.
 *      av[0] = reason
 * When called internally, callers may assume that the contents of the
 * argument string will not be modified.
 */

void do_quit(const char *source, int ac, char **av)
{
    User *user;

    user = get_user(source);
    if (!user) {
        log_debug(1, "user: QUIT from nonexistent user %s: %s",
                  source, merge_args(ac, av));
        return;
    }
    log_debug(1, "%s quits", source);
    quit_user(user, av[0], 0);
}

/*************************************************************************/

/* Handle a KILL command.
 *      av[0] = nick being killed
 *      av[1] = reason
 * When called internally, callers may assume that the contents of the
 * argument strings will not be modified.
 */

void do_kill(const char *source, int ac, char **av)
{
    User *user;

    user = get_user(av[0]);
    if (!user)
        return;
    log_debug(1, "%s killed", av[0]);
    quit_user(user, av[1], 1);
}

/*************************************************************************/
/*************************************************************************/

/* Join a user to a channel.  Return a pointer to the channel record if the
 * join succeeded, NULL otherwise.
 */

Channel *join_channel(User *user, const char *channel, int32 modes)
{
    Channel *c = chan_adduser(user, channel, modes);
    struct u_chanlist *uc;

    if (!c)
        return NULL;
    uc = smalloc(sizeof(*uc));
    LIST_INSERT(uc, user->chans);
    uc->chan = c;
    return c;
}

/*************************************************************************/

/* Part a user from a channel. */

int part_channel(User *user, const char *channel, int callback,
                 const char *param, const char *source)
{
    struct u_chanlist *uc;
    LIST_SEARCH(user->chans, chan->name, channel, irc_stricmp, uc);
    if (uc)
        part_channel_uc(user, uc, callback, param, source);
    return uc != NULL;
}

/*************************************************************************/

/* Part a user from all channels s/he is in.  Assumes cb_chan_part, an
 * empty `param' string, and the user as source. */

void part_all_channels(User *user)
{
    struct u_chanlist *uc, *nextuc;
    LIST_FOREACH_SAFE (uc, user->chans, nextuc)
        part_channel_uc(user, uc, cb_chan_part, "", user->nick);
}

/*************************************************************************/
/*************************************************************************/

/* Various check functions.  All of these return false/NULL if any
 * parameter is NULL. */

/*************************************************************************/

/* Is the given user an oper? */

int is_oper(const User *user)
{
    return user != NULL && (user->mode & UMODE_o);
}

/*************************************************************************/

/* Is the given user on the given channel?  Return the Channel * for the
 * channel if so, NULL if not.
 */

Channel *is_on_chan(const User *user, const char *chan)
{
    struct u_chanlist *c;

    if (!user || !chan)
        return NULL;
    LIST_SEARCH(user->chans, chan->name, chan, irc_stricmp, c);
    return c ? c->chan : NULL;
}

/*************************************************************************/

/* Is the given user a channel operator on the given channel? */

int is_chanop(const User *user, const char *chan)
{
    Channel *c = chan ? get_channel(chan) : NULL;
    struct c_userlist *cu;

    if (!user || !chan || !c)
        return 0;
    LIST_SEARCH(c->users, user->nick, user->nick, irc_stricmp, cu);
    return cu != NULL && (cu->mode & CUMODE_o) != 0;
}

/*************************************************************************/

/* Is the given user voiced (channel mode +v) on the given channel? */

int is_voiced(const User *user, const char *chan)
{
    Channel *c = chan ? get_channel(chan) : NULL;
    struct c_userlist *cu;

    if (!user || !chan || !c)
        return 0;
    LIST_SEARCH(c->users, user->nick, user->nick, irc_stricmp, cu);
    return cu != NULL && (cu->mode & CUMODE_v) != 0;
}

/*************************************************************************/
/*************************************************************************/

/* Does the user's usermask match the given mask?  The mask may be in
 * either nick!user@host or just user@host form.  When "fakehosts" or IP
 * addresses are available in the user record, they are also checked
 * against the "host" part of the mask, and a match by any field (real
 * host, fakehost, or IP address) is treated as a match for the host part.
 * Note that CIDR matching on IP addresses is _not_ performed.
 */

int match_usermask(const char *mask, const User *user)
{
    char *mask2;
    char *nick, *username, *host;
    int match_user, match_host, result;

    if (!mask || !user) {
        log_debug(1, "match_usermask: NULL %s!", !mask ? "mask" : "user");
        return 0;
    }
    mask2 = sstrdup(mask);
    if (strchr(mask2, '!')) {
        nick = strtok(mask2, "!");
        username = strtok(NULL, "@");
    } else {
        nick = NULL;
        username = strtok(mask2, "@");
    }
    host = strtok(NULL, "");
    if (!host) {
        free(mask2);
        return 0;
    }
    match_user = match_wild_nocase(username, user->username);
    match_host = match_wild_nocase(host, user->host);
    if (user->fakehost)
        match_host |= match_wild_nocase(host, user->fakehost);
    if (user->ipaddr)
        match_host |= match_wild_nocase(host, user->ipaddr);
    if (nick) {
        result = match_wild_nocase(nick, user->nick) &&
                 match_user && match_host;
    } else {
        result = match_user && match_host;
    }
    free(mask2);
    return result;
}

/*************************************************************************/

/* Split a usermask up into its constitutent parts.  Returned strings are
 * malloc()'d, and should be free()'d when done with.  Returns "*" for
 * missing parts.  Assumes `mask' is a non-empty string.
 */

void split_usermask(const char *mask, char **nick, char **user, char **host)
{
    char *mask2 = sstrdup(mask);
    char *mynick, *myuser, *myhost;

    mynick = mask2;
    myuser = strchr(mask2, '!');
    myhost = myuser ? strchr(myuser, '@') : NULL;
    if (myuser)
        *myuser++ = 0;
    if (myhost)
        *myhost++ = 0;
    /* Handle special case: mask == user@host */
    if (mynick && !myuser && strchr(mynick, '@')) {
        mynick = NULL;
        myuser = mask2;
        myhost = strchr(mask2, '@');
        if (myhost)  /* Paranoia */
            *myhost++ = 0;
    }
    if (!mynick || !*mynick)
        mynick = (char *)"*";
    if (!myuser || !*myuser)
        myuser = (char *)"*";
    if (!myhost || !*myhost)
        myhost = (char *)"*";
    *nick = sstrdup(mynick);
    *user = sstrdup(myuser);
    *host = sstrdup(myhost);
    free(mask2);
}

/*************************************************************************/

/* Given a user, return a mask that will most likely match any address the
 * user will have from that location.  For IP addresses, wildcards the last
 * octet of the address (e.g. 10.1.1.1 -> 10.1.1.*); for named addresses,
 * wildcards the leftmost part of the name unless the name only contains
 * two parts.  The returned character string is malloc'd and should be
 * free'd when done with.
 *
 * Where supported, uses the fake host instead of the real one if
 * use_fakehost is nonzero.
 */

char *create_mask(const User *user, int use_fakehost)
{
    char *mask, *s, *end, *host;

    host = user->host;
    if (use_fakehost && user->fakehost)
        host = user->fakehost;
    /* Get us a buffer the size of the username plus hostname.  The result
     * will never be longer than this (and will often be shorter), thus we
     * can use strcpy() and sprintf() safely.
     */
    end = mask = smalloc(strlen(user->username) + strlen(host) + 2);
    end += sprintf(end, "%s@", user->username);
    if (strspn(host, "0123456789.") == strlen(host)
                && (s = strchr(host, '.'))
                && (s = strchr(s+1, '.'))
                && (s = strchr(s+1, '.'))
                && (   !strchr(s+1, '.'))) {    /* IP addr */
        s = sstrdup(host);
        *strrchr(s, '.') = 0;
        sprintf(end, "%s.*", s);
        free(s);
    } else {
        if ((s = strchr(host+1, '.')) && strchr(s+1, '.')) {
            s = sstrdup(s-1);
            *s = '*';
        } else {
            s = sstrdup(host);
        }
        strcpy(end, s);  /* safe: see above */
        free(s);
    }
    return mask;
}

/*************************************************************************/
/*************************************************************************/

/* Create a new guest nick using GuestNickPrefix and a unique series of
 * digits, and return it.  The returned nick is stored in a static buffer
 * and will be overwritten at the next call.
 *
 * At present, we simply use a rollover counter attached to the nick,
 * initialized to a random value.  This provides for uniqueness as long as
 * Services is not restarted and a reasonable chance of uniqueness even
 * when Services is restarted (unless you have a long prefix and a short
 * maximum nick length; however, this routine will make sure at least 4
 * digits are available, possibly by shortening the prefix).
 *
 * Note that initializing the counter based on the current time would be a
 * bad idea, since if more than one user per second (on average) connected
 * to the network, duplicate nicks would be almost guaranteed if Services
 * restarted.
 */

char *make_guest_nick(void)
{
    static char nickbuf[NICKMAX+1];     /* +1 to check for overrun */
    static uint32 counter = 0;          /* Unique suffix counter */
    int tries;                          /* Tries to find an unused nick */
    int prefixlen;                      /* Length of nick prefix */
    uint32 suffixmod;                   /* Modulo for suffix counter */
    int i;

    /* Sanity checks on nick prefix length */
    prefixlen = strlen(GuestNickPrefix);
    if (protocol_nickmax <= 4) {
        /* This violates RFC1459 as well as common sense, so just blow
         * ourselves out of the water. */
        fatal("make_guest_nick(): protocol_nickmax too small (%d)",
              protocol_nickmax);
    } else if (prefixlen+4 > protocol_nickmax) {
        /* Reserve at least 4 digits for the suffix */
        prefixlen = protocol_nickmax-4;
        log("warning: make_guest_nick(): GuestNickPrefix too long,"
            " shortening to %d characters", prefixlen);
        GuestNickPrefix[prefixlen] = 0;
    }

    /* Calculate number of digits available for suffix -> suffix modulo */
    i = protocol_nickmax - prefixlen;
    if (i < 10) {
        suffixmod = 1;
        while (i-- > 0)
            suffixmod *= 10;
    } else {
        suffixmod = 0;  /* no modulo */
    }

    /* Actually generate the nick.  If the nick already exists, generate a
     * new one, and repeat until finding an unused nick or trying too many
     * times.  If we try too many times, we just kill the last one we end
     * up with. */
    tries = 0;
    for (;;) {
        if (counter == 0)       /* initialize to random the first time */
            counter = rand();
        if (suffixmod)          /* strip down to right number of digits */
            counter %= suffixmod;
        if (counter == 0)       /* if rand() gave us 0 or N*suffixmod... */
            counter = 1;        /* ... use 1 instead */
        i = snprintf(nickbuf, sizeof(nickbuf), "%s%u", GuestNickPrefix, counter);
        if (i > protocol_nickmax) {
            log("BUG: make_guest_nick() generated %s but nickmax == %d!",
                nickbuf, protocol_nickmax);
            nickbuf[protocol_nickmax] = 0;
        }
        if (!get_user(nickbuf)) /* done if user not found */
            break;
        /* Increment try count, and break out if it's too much */
        if (++tries >= MAKEGUESTNICK_TRIES) {
            kill_user(NULL, nickbuf, "Guest nicks may not be used");
            break;
        }
        /* Reset counter to 0 to use a new random number */
        counter = 0;
    }

    /* Increment the unique suffix counter modulo suffixmod, and avoid 0
     * (which would cause a reset to a random value) */
    counter++;
    if (counter == suffixmod)  /* because suffixmod==0 if no modulo */
        counter = 1;

    /* Return the nick */
    return nickbuf;
};

/*************************************************************************/

/* Return nonzero if the given nickname can potentially be a guest nickname
 * (i.e. a nickname that could be generated by make_guest_nick()), zero if
 * not.  Currently this means a nickname consisting of GuestNickPrefix
 * followed by between 1 and 10 digits inclusive.
 */

int is_guest_nick(const char *nick)
{
    int prefixlen = strlen(GuestNickPrefix);
    int nicklen = strlen(nick);
    return nicklen >= prefixlen+1 && nicklen <= prefixlen+10
        && irc_strnicmp(nick, GuestNickPrefix, prefixlen) == 0
        && strspn(nick+prefixlen, "1234567890") == nicklen-prefixlen;
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
