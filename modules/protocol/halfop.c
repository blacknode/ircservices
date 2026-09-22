/* Halfop (+h) related functions.
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

#include "halfop.h"

/*************************************************************************/

static Module *halfop_module_chanserv;

static int halfop_old_XOP_REACHED_LIMIT = -1;
static int halfop_old_XOP_NICKS_ONLY = -1;
static int halfop_old_HELP_SOP_MID2 = -1;
static int halfop_old_HELP_AOP_MID = -1;

static const char **halfop_p_s_ChanServ = NULL;  /* never used if NULL */
#define s_ChanServ (*halfop_p_s_ChanServ)

/*************************************************************************/
/*************************************************************************/

/* Callback to handle ChanServ CLEAR HALFOPS. */

static void clear_halfops(Channel *chan);

static int halfop_cs_clear(User *u, Channel *c, const char *what)
{
    if (stricmp(what, "HALFOPS") == 0) {
        clear_halfops(c);
        set_cmode(NULL, c);
        notice_lang(s_ChanServ, u, CHAN_CLEARED_HALFOPS, c->name);
        return 1;
    }
    return 0;
}


static void clear_halfops(Channel *chan)
{
    struct c_userlist *cu;
    static int32 mode_h = -1;

    if (mode_h < 0)
        mode_h = mode_char_to_flag('h', MODE_CHANUSER);
    LIST_FOREACH (cu, chan->users) {
        if (cu->mode & mode_h)
            set_cmode(s_ChanServ, chan, "-h", cu->user->nick);
    }
}

/*************************************************************************/
/*************************************************************************/

/* Callback to watch for modules being loaded. */

static int halfop_load_module(Module *mod, const char *name)
{
    if (strcmp(name, "chanserv/main") == 0) {
        halfop_module_chanserv = mod;
        halfop_p_s_ChanServ = get_module_symbol(mod, "s_ChanServ");
        if (halfop_p_s_ChanServ) {
            if (!(add_callback(mod, "CLEAR", halfop_cs_clear)))
                module_log("halfop: Unable to add ChanServ CLEAR callback");
        } else {
            module_log("halfop: Symbol `s_ChanServ' not found, CLEAR"
                       " HALFOPS will not be available");
        }
    }
    return 0;
}

/*************************************************************************/

/* Callback to watch for modules being unloaded. */

static int halfop_unload_module(Module *mod)
{
    if (mod == halfop_module_chanserv) {
        halfop_p_s_ChanServ = NULL;
        halfop_module_chanserv = NULL;
    }
    return 0;
}

/*************************************************************************/

int init_halfop(void)
{
    if (!add_callback(NULL, "load module", halfop_load_module)
     || !add_callback(NULL, "unload module", halfop_unload_module)
    ) {
        module_log("halfop: Unable to add callbacks");
        exit_halfop();
        return 0;
    }
    protocol_features |= PF_HALFOP;
    halfop_old_XOP_REACHED_LIMIT =
        mapstring(CHAN_XOP_REACHED_LIMIT, CHAN_XOP_REACHED_LIMIT_HOP);
    halfop_old_XOP_NICKS_ONLY =
        mapstring(CHAN_XOP_NICKS_ONLY, CHAN_XOP_NICKS_ONLY_HOP);
    halfop_old_HELP_SOP_MID2 =
        mapstring(CHAN_HELP_SOP_MID2, CHAN_HELP_SOP_MID2_HALFOP);
    halfop_old_HELP_AOP_MID =
        mapstring(CHAN_HELP_AOP_MID, CHAN_HELP_AOP_MID_HALFOP);
    return 1;
}

/*************************************************************************/

void exit_halfop(void)
{
    if (halfop_module_chanserv)
        halfop_unload_module(halfop_module_chanserv);
    if (halfop_old_HELP_AOP_MID >= 0)
        mapstring(CHAN_HELP_AOP_MID, halfop_old_HELP_AOP_MID);
    if (halfop_old_HELP_SOP_MID2 >= 0)
        mapstring(CHAN_HELP_SOP_MID2, halfop_old_HELP_SOP_MID2);
    if (halfop_old_XOP_NICKS_ONLY >= 0)
        mapstring(CHAN_XOP_NICKS_ONLY, halfop_old_XOP_NICKS_ONLY);
    if (halfop_old_XOP_REACHED_LIMIT >= 0)
        mapstring(CHAN_XOP_REACHED_LIMIT, halfop_old_XOP_REACHED_LIMIT);
    halfop_old_HELP_AOP_MID = -1;
    halfop_old_HELP_SOP_MID2 = -1;
    halfop_old_XOP_NICKS_ONLY = -1;
    halfop_old_XOP_REACHED_LIMIT = -1;
    remove_callback(NULL, "unload module", halfop_unload_module);
    remove_callback(NULL, "load module", halfop_load_module);
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
