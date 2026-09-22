/* SJOIN-related functions.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

/* If this file is compiled with UNREAL_HACK defined, the create-channel
 * callback, do_channel_create(), will be modified to force Unreal to
 * update channel times properly.  Defining BAHAMUT_HACK does the same
 * thing for Bahamut.
 */

#include "services.h"
#include "modules.h"
#include "modules/chanserv/chanserv.h"

#include "sjoin.h"

/*************************************************************************/

static Module *sjoin_module_chanserv;

int CSSetChannelTime;

/*************************************************************************/

/* Handle an SJOIN command.
 * Bahamut no-SSJOIN format:
 *      av[0] = TS3 timestamp
 *      av[1] = TS3 timestamp (channel creation time)
 *      av[2] = channel
 *      av[3] = channel modes
 *      av[4] = limit / key (depends on modes in av[3])
 *      av[5] = limit / key (depends on modes in av[3])
 *      av[ac-1] = nickname(s), with modes, joining channel
 * Bahamut SSJOIN (server source) / Hybrid / PTlink / Unreal SJOIN2/SJ3 format:
 *      av[0] = TS3 timestamp (channel creation time)
 *      av[1] = channel
 *      av[2] = modes (limit/key in av[3]/av[4] as needed)
 *      av[ac-1] = users
 *      (Note that Unreal may omit the modes if there aren't any.
 *       Unreal SJ3 also includes bans and exceptions in av[ac-1]
 *       with prefix characters of & and " respectively.)
 * Bahamut SSJOIN format (client source):
 *      av[0] = TS3 timestamp (channel creation time)
 *      av[1] = channel
 */

static void do_sjoin(const char *source, int ac, char **av)
{
    User *user;
    char *t, *nick;
    char *channel;
    Channel *c = NULL, *ctemp;

    if (isdigit(av[1][0])) {
        /* Plain SJOIN format, zap join timestamp */
        memmove(&av[0], &av[1], sizeof(char *) * (ac-1));
        ac--;
    }

    channel = av[1];
    if (ac >= 3) {
        /* SJOIN from server: nick list in last param */
        t = av[ac-1];
    } else {
        /* SJOIN for a single client: source is nick */
        /* We assume the nick has no spaces, so we can discard const and
         * use it in the loop */
        if (strchr(source, ' '))
            fatal("sjoin: source nick contains spaces!");
        t = (char *)source;
    }
    while (*(nick=t)) {
        int32 modes = 0, thismode;

        t = nick + strcspn(nick, " ");
        if (*t)
            *t++ = 0;
        if (*nick == '&' || *nick == '"') {
            /* Ban (&) or exception (") */
            char *av[3];
            av[0] = channel;
            av[1] = (char *)((*nick=='&') ? "+b" : "+e");
            av[2] = nick+1;
            do_cmode(source, 3, av);
            continue;
        }
        do {
            thismode = cumode_prefix_to_flag(*nick);
            modes |= thismode;
        } while (thismode && *nick++); /* increment nick only if thismode!=0 */
        user = get_user(nick);
        if (!user) {
            module_log("sjoin: SJOIN to channel %s for non-existent nick %s"
                       " (%s)", channel, nick, merge_args(ac-1, av));
            continue;
        }
        module_log_debug(1, "%s SJOINs %s", nick, channel);
        if ((ctemp = join_channel(user, channel, modes)) != NULL)
            c = ctemp;
    }

    /* Did anyone actually join the channel? */
    if (c) {
        /* Store channel timestamp in channel structure, unless we set it
         * ourselves. */
        if (!c->ci)
            c->creation_time = strtotime(av[0], NULL);
        /* Set channel modes if there are any.  Note how argument list is
         * conveniently set up for do_cmode(). */
        if (ac > 3)
            do_cmode(source, ac-2, av+1);
    }
}

/*************************************************************************/

/* Clear out all channel modes using an SJOIN (for CLEAR_USERS). */

static int sjoin_clear_users(const char *sender, Channel *chan, int what,
                             const void *param)
{
    if (what & CLEAR_USERS) {
        int i;
        send_cmd(ServerName, "SJOIN %ld %s + :",
                 (long)(chan->creation_time - 1), chan->name);
        ARRAY_FOREACH (i, chan->excepts)
            free(chan->excepts[i]);
        chan->excepts_count = 0;
    }
    return 0;
}

/*************************************************************************/
/*************************************************************************/

/* Callback to set the creation time for a registered channel to the
 * channel's registration time.  This callback is added before the main
 * ChanServ module is loaded, so c->ci will not yet be set.
 */

static typeof(get_channelinfo) *sjoin_p_get_channelinfo = NULL;
static typeof(put_channelinfo) *sjoin_p_put_channelinfo = NULL;

static int sjoin_channel_create(Channel *c, User *u, int32 modes)
{
    if (CSSetChannelTime && sjoin_p_get_channelinfo) {
        ChannelInfo *ci = sjoin_p_get_channelinfo(c->name);
        if (ci) {
            c->creation_time = ci->time_registered;
#ifdef UNREAL_HACK
            /* NOTE: this is a bit of a kludge, since Unreal's SJOIN
             * doesn't let us set just the channel creation time while
             * leaving the modes alone. */
            send_cmd(ServerName, "SJOIN %ld %s %co %s :",
                     (long)c->creation_time, c->name,
                     (modes & CUMODE_o ? '+' : '-'), u->nick);
#else
            send_cmd(ServerName, "SJOIN %ld %s + :%s%s",
                     (long)c->creation_time, c->name,
                     (modes & CUMODE_o ? "@" : ""), u->nick);
#endif
#ifdef BAHAMUT_HACK
            if (modes & CUMODE_o) {
                /* Bahamut ignores users in the user list which aren't on
                 * or behind the server sending the SJOIN, so we need an
                 * extra MODE to explicitly give ops back to the initial
                 * joining user. */
                send_cmode_cmd(ServerName, c->name, "+o :%s", u->nick);
            }
#endif
            sjoin_p_put_channelinfo(ci);
        }  /* if (ci) */
    }  /* if (CSSetChannelTime) */
    return 0;
}

/*************************************************************************/

/* Callback to watch for modules being loaded. */

static int sjoin_load_module(Module *mod, const char *name)
{
    if (strcmp(name, "chanserv/main") == 0) {
        sjoin_module_chanserv = mod;
        sjoin_p_get_channelinfo = get_module_symbol(NULL, "get_channelinfo");
        if (!sjoin_p_get_channelinfo)
            module_log("sjoin: symbol `get_channelinfo' not found, channel"
                       " time setting disabled");
        sjoin_p_put_channelinfo = get_module_symbol(NULL, "put_channelinfo");
        if (!sjoin_p_get_channelinfo)
            module_log("sjoin: symbol `put_channelinfo' not found, channel"
                       " time setting disabled");
    }
    return 0;
}

/*************************************************************************/

/* Callback to watch for modules being unloaded. */

static int sjoin_unload_module(Module *mod)
{
    if (mod == sjoin_module_chanserv) {
        sjoin_p_get_channelinfo = NULL;
        sjoin_module_chanserv = NULL;
    }
    return 0;
}

/*************************************************************************/

/* Initialization. */

static void exit_sjoin(void);

static int init_sjoin(void)
{
    sjoin_module_chanserv = NULL;
    sjoin_p_get_channelinfo = NULL;

    if (!add_callback(NULL, "load module", sjoin_load_module)
     || !add_callback(NULL, "unload module", sjoin_unload_module)
     || !add_callback(NULL, "channel create", sjoin_channel_create)
     || !add_callback(NULL, "clear channel", sjoin_clear_users)
    ) {
        module_log("sjoin: Unable to add callbacks");
        exit_sjoin();
        return 0;
    }
    return 1;
}

/*************************************************************************/

/* Cleanup. */

static void exit_sjoin(void)
{
    remove_callback(NULL, "clear channel", sjoin_clear_users);
    remove_callback(NULL, "channel create", sjoin_channel_create);
    remove_callback(NULL, "unload module", sjoin_unload_module);
    remove_callback(NULL, "load module", sjoin_load_module);
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
