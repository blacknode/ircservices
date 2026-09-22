/* Invite mask (+I) related functions.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "language.h"
#include "modules/chanserv/chanserv.h"

#include "invitemask.h"

/*************************************************************************/

static Module *invitemask_module_chanserv;

static const char **invitemask_p_s_ChanServ = NULL;  /* never used if NULL */
#define s_ChanServ (*invitemask_p_s_ChanServ)

/*************************************************************************/
/*************************************************************************/

/* Callback to handle MODE +/-I. */

static int invitemask_channel_mode(const char *source, Channel *chan,
                                   int modechar, int add, char **av)
{
    if (modechar == 'I') {
        if (add) {
            ARRAY_EXTEND(chan->invites);
            chan->invites[chan->invites_count-1] = sstrdup(av[0]);
        } else {
            int i;
            ARRAY_SEARCH_PLAIN(chan->invites, av[0], irc_stricmp, i);
            if (i < chan->invites_count) {
                free(chan->invites[i]);
                ARRAY_REMOVE(chan->invites, i);
            } else {
                module_log("invitemask: MODE %s -I %s: mask not found",
                           chan->name, *av);
            }
        }
        return 0;
    }
    return 0;
}

/*************************************************************************/

/* Callback to handle clearing invite masks for clear_channel(). */

static void clear_invites(const char *sender, Channel *chan, const User *u);

static int invitemask_clear_channel(const char *sender, Channel *chan,
                                    int what, const void *param)
{
    if (what & (CLEAR_USERS | CLEAR_INVITES))
        clear_invites(sender, chan, (what & CLEAR_INVITES) ? param : NULL);
    return 0;
}


static void clear_invites(const char *sender, Channel *chan, const User *u)
{
    int i, count;
    char **invites;

    if (!chan->invites_count)
        return;
    count = chan->invites_count;
    invites = smalloc(sizeof(char *) * count);
    memcpy(invites, chan->invites, sizeof(char *) * count);
    for (i = 0; i < count; i++) {
        if (!u || match_usermask(invites[i], u))
            set_cmode(sender, chan, "-I", invites[i]);
    }
    free(invites);
}

/*************************************************************************/

/* Callback to handle ChanServ CLEAR INVITES. */

static int invitemask_cs_clear(User *u, Channel *c, const char *what)
{
    if (stricmp(what, "INVITES") == 0) {
        clear_invites(s_ChanServ, c, NULL);
        set_cmode(NULL, c);
        notice_lang(s_ChanServ, u, CHAN_CLEARED_INVITES, c->name);
        return 1;
    }
    return 0;
}

/*************************************************************************/
/*************************************************************************/

/* Callback to watch for modules being loaded. */

static int invitemask_load_module(Module *mod, const char *name)
{
    if (strcmp(name, "chanserv/main") == 0) {
        invitemask_module_chanserv = mod;
        invitemask_p_s_ChanServ = get_module_symbol(mod, "s_ChanServ");
        if (invitemask_p_s_ChanServ) {
            if (!(add_callback(mod, "CLEAR", invitemask_cs_clear)))
                module_log("invitemask: Unable to add ChanServ CLEAR"
                           " callback");
        } else {
            module_log("invitemask: Symbol `s_ChanServ' not found, CLEAR"
                       " INVITES will not be available");
        }
    }
    return 0;
}

/*************************************************************************/

/* Callback to watch for modules being unloaded. */

static int invitemask_unload_module(Module *mod)
{
    if (mod == invitemask_module_chanserv) {
        invitemask_p_s_ChanServ = NULL;
        invitemask_module_chanserv = NULL;
    }
    return 0;
}

/*************************************************************************/
/*************************************************************************/

int init_invitemask(void)
{
    if (!add_callback(NULL, "channel MODE", invitemask_channel_mode)
     || !add_callback(NULL, "clear channel", invitemask_clear_channel)
     || !add_callback(NULL, "load module", invitemask_load_module)
     || !add_callback(NULL, "unload module", invitemask_unload_module)
    ) {
        module_log("invitemask: Unable to add callbacks");
        exit_invitemask();
        return 0;
    }
    protocol_features |= PF_INVITEMASK;
    return 1;
}

/*************************************************************************/

void exit_invitemask(void)
{
    remove_callback(NULL, "unload module", invitemask_unload_module);
    remove_callback(NULL, "load module", invitemask_load_module);
    remove_callback(NULL, "clear channel", invitemask_clear_channel);
    remove_callback(NULL, "channel MODE", invitemask_channel_mode);
}

/*************************************************************************/

#undef s_ChanServ

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
