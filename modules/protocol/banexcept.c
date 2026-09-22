/* Ban exception related functions.
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

#include "banexcept.h"

/*************************************************************************/

static Module *banexcept_module_chanserv;

static const char **banexcept_p_s_ChanServ = NULL;  /* never used if NULL */
#define s_ChanServ (*banexcept_p_s_ChanServ)

/*************************************************************************/
/*************************************************************************/

/* Callback to handle MODE +/-e. */

static int banexcept_channel_mode(const char *source, Channel *chan,
                                  int modechar, int add, char **av)
{
    if (modechar == 'e') {
        if (add) {
            ARRAY_EXTEND(chan->excepts);
            chan->excepts[chan->excepts_count-1] = sstrdup(av[0]);
        } else {
            int i;
            ARRAY_SEARCH_PLAIN(chan->excepts, av[0], irc_stricmp, i);
            if (i < chan->excepts_count) {
                free(chan->excepts[i]);
                ARRAY_REMOVE(chan->excepts, i);
            } else {
                module_log("banexcept: MODE %s -e %s: exception not found",
                           chan->name, *av);
            }
        }
        return 0;
    }
    return 0;
}

/*************************************************************************/

/* Callback to handle clearing exceptions for clear_channel(). */

static void clear_excepts(const char *sender, Channel *chan, const User *u);

static int banexcept_clear_channel(const char *sender, Channel *chan, int what,
                                   const void *param)
{
    if (what & (CLEAR_USERS | CLEAR_EXCEPTS))
        clear_excepts(sender, chan, (what & CLEAR_EXCEPTS) ? param : NULL);
    return 0;
}


static void clear_excepts(const char *sender, Channel *chan, const User *u)
{
    int i, count;
    char **excepts;

    if (!chan->excepts_count)
        return;
    count = chan->excepts_count;
    excepts = smalloc(sizeof(char *) * count);
    memcpy(excepts, chan->excepts, sizeof(char *) * count);
    for (i = 0; i < count; i++) {
        if (!u || match_usermask(excepts[i], u))
            set_cmode(sender, chan, "-e", excepts[i]);
    }
    free(excepts);
}

/*************************************************************************/

/* Callback to handle ChanServ CLEAR EXCEPTIONS. */

static int banexcept_cs_clear(User *u, Channel *c, const char *what)
{
    if (stricmp(what, "EXCEPTIONS") == 0) {
        clear_excepts(s_ChanServ, c, NULL);
        set_cmode(NULL, c);
        notice_lang(s_ChanServ, u, CHAN_CLEARED_EXCEPTIONS, c->name);
        return 1;
    }
    return 0;
}

/*************************************************************************/
/*************************************************************************/

/* Callback to watch for modules being loaded. */

static int banexcept_load_module(Module *mod, const char *name)
{
    if (strcmp(name, "chanserv/main") == 0) {
        banexcept_module_chanserv = mod;
        banexcept_p_s_ChanServ = get_module_symbol(mod, "s_ChanServ");
        if (banexcept_p_s_ChanServ) {
            if (!(add_callback(mod, "CLEAR", banexcept_cs_clear)))
                module_log("banexcept: Unable to add ChanServ CLEAR callback");
        } else {
            module_log("banexcept: Symbol `s_ChanServ' not found, CLEAR"
                       " EXCEPTIONS will not be available");
        }
    }
    return 0;
}

/*************************************************************************/

/* Callback to watch for modules being unloaded. */

static int banexcept_unload_module(Module *mod)
{
    if (mod == banexcept_module_chanserv) {
        banexcept_p_s_ChanServ = NULL;
        banexcept_module_chanserv = NULL;
    }
    return 0;
}

/*************************************************************************/
/*************************************************************************/

int init_banexcept(void)
{
    if (!add_callback(NULL, "channel MODE", banexcept_channel_mode)
     || !add_callback(NULL, "clear channel", banexcept_clear_channel)
     || !add_callback(NULL, "load module", banexcept_load_module)
     || !add_callback(NULL, "unload module", banexcept_unload_module)
    ) {
        module_log("banexcept: Unable to add callbacks");
        exit_banexcept();
        return 0;
    }
    protocol_features |= PF_BANEXCEPT;
    return 1;
}

/*************************************************************************/

void exit_banexcept(void)
{
    remove_callback(NULL, "unload module", banexcept_unload_module);
    remove_callback(NULL, "load module", banexcept_load_module);
    remove_callback(NULL, "clear channel", banexcept_clear_channel);
    remove_callback(NULL, "channel MODE", banexcept_channel_mode);
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
