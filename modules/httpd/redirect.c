/* Nick/channel URL redirect module for HTTP server.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "conffile.h"
#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"

#include "http.h"

/*************************************************************************/

static Module *module_httpd;
static Module *module_nickserv;
static Module *module_chanserv;

static char *NicknamePrefix;
static char *ChannelPrefix;


/* Imported from NickServ: */
typeof(get_nickinfo) *p_get_nickinfo;
typeof(put_nickinfo) *p_put_nickinfo;
typeof(_get_ngi) *p__get_ngi;
typeof(put_nickgroupinfo) *p_put_nickgroupinfo;
#define get_nickinfo (*p_get_nickinfo)
#define put_nickinfo (*p_put_nickinfo)
#define _get_ngi (*p__get_ngi)
#define put_nickgroupinfo (*p_put_nickgroupinfo)

/* Imported from ChanServ: */
typeof(get_channelinfo) *p_get_channelinfo;
typeof(put_channelinfo) *p_put_channelinfo;
#define get_channelinfo (*p_get_channelinfo)
#define put_channelinfo (*p_put_channelinfo)

/*************************************************************************/
/*************************** Request callback ****************************/
/*************************************************************************/

static int do_request(Client *c, int *close_ptr)
{
    if (NicknamePrefix && module_nickserv
        && strncmp(c->url,NicknamePrefix,strlen(NicknamePrefix)) == 0
    ) {
        char *nick;
        char newnick[NICKMAX*5];  /* for errors; *5 because of & -> &amp; */
        NickInfo *ni = NULL;
        NickGroupInfo *ngi = NULL;

        nick = c->url + strlen(NicknamePrefix);
        ni = get_nickinfo(nick);
        ngi = (ni && ni->nickgroup) ? get_ngi(ni) : NULL;
        http_quote_html(nick, newnick, sizeof(newnick));

        if (ngi && ngi->url) {
            /* URL registered, so send a redirect there */
            http_send_response(c, HTTP_R_FOUND);
            sockprintf(c->socket, "Location: %s\r\n", ngi->url);
            sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
        } else if (ngi) {
            /* Nick not registered (or forbidden) */
            http_error(c, HTTP_E_NOT_FOUND,
                       "<h1 align=center>URL Not Set</h1>"
                       "The nickname <b>%s</b> does not have a URL set.",
                       newnick);
        } else if (*nick) {
            /* Nick not registered (or forbidden) */
            http_error(c, HTTP_E_NOT_FOUND,
                       "<h1 align=center>Nickname Not Registered</h1>"
                       "The nickname <b>%s</b> is not registered.", newnick);
        } else {
            /* No nick given; just give a standard 404 */
            http_error(c, HTTP_E_NOT_FOUND, NULL);
        }
        put_nickinfo(ni);
        put_nickgroupinfo(ngi);
        return 1;

    } else if (ChannelPrefix && module_chanserv
               && strncmp(c->url,ChannelPrefix,strlen(ChannelPrefix)) == 0) {
        char *chan;
        char newchan[CHANMAX*5];
        ChannelInfo *ci;

        chan = c->url + strlen(ChannelPrefix);
        snprintf(newchan, sizeof(newchan), "#%s", chan);
        ci = get_channelinfo(newchan);
        /* Leave initial "#" in place */
        http_quote_html(chan, newchan+1, sizeof(newchan)-1);
        if (ci && ci->url) {
            http_send_response(c, HTTP_R_FOUND);
            sockprintf(c->socket, "Location: %s\r\n", ci->url);
            sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
        } else if (ci) {
            http_error(c, HTTP_E_NOT_FOUND,
                       "<h1 align=center>URL Not Set</h1>"
                       "The channel <b>%s</b> does not have a URL set.",
                       newchan);
        } else if (*chan) {
            http_error(c, HTTP_E_NOT_FOUND,
                       "<h1 align=center>Channel Not Registered</h1>"
                       "The channel <b>%s</b> is not registered.", newchan);
        } else {
            http_error(c, HTTP_E_NOT_FOUND, NULL);
        }
        put_channelinfo(ci);
        return 1;

    }

    return 0;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "NicknamePrefix",   { { CD_STRING, 0, &NicknamePrefix } } },
    { "ChannelPrefix",    { { CD_STRING, 0, &ChannelPrefix } } },
    { NULL }
};

/*************************************************************************/

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "nickserv/main") == 0) {
        p_get_nickinfo = get_module_symbol(mod, "get_nickinfo");
        p_put_nickinfo = get_module_symbol(mod, "put_nickinfo");
        p__get_ngi = get_module_symbol(mod, "_get_ngi");
        p_put_nickgroupinfo = get_module_symbol(mod, "put_nickgroupinfo");
        if (p_get_nickinfo && p_put_nickinfo
         && p__get_ngi && p_put_nickgroupinfo
        ) {
            module_nickserv = mod;
        } else {
            module_log("Required symbols not found, nickname redirects will"
                       " not be available");
            p_get_nickinfo = NULL;
            p_put_nickinfo = NULL;
            p__get_ngi = NULL;
            p_put_nickgroupinfo = NULL;
            module_nickserv = NULL;
        }
    } else if (strcmp(modname, "chanserv/main") == 0) {
        p_get_channelinfo = get_module_symbol(mod, "get_channelinfo");
        p_put_channelinfo = get_module_symbol(mod, "put_channelinfo");
        if (p_get_channelinfo && p_put_channelinfo) {
            module_chanserv = mod;
        } else {
            module_log("Required symbols not found, channel redirects will"
                       " not be available");
            p_get_channelinfo = NULL;
            p_put_channelinfo = NULL;
        }
    }

    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_nickserv) {
        p_get_nickinfo = NULL;
        p_put_nickinfo = NULL;
        p__get_ngi = NULL;
        p_put_nickgroupinfo = NULL;
        p_get_nickinfo = NULL;
        p_put_nickinfo = NULL;
        p__get_ngi = NULL;
        p_put_nickgroupinfo = NULL;
        module_nickserv = NULL;
        module_nickserv = NULL;
    } else if (mod == module_chanserv) {
        p_get_channelinfo = NULL;
        p_put_channelinfo = NULL;
        module_chanserv = NULL;
    }
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    Module *tmpmod;

    module_httpd = find_module("httpd/main");
    if (!module_httpd) {
        module_log("Main httpd module not loaded");
        exit_module(0);
        return 0;
    }
    use_module(module_httpd);

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(module_httpd, "request", do_request)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    tmpmod = find_module("nickserv/main");
    if (tmpmod)
        do_load_module(tmpmod, "nickserv/main");
    tmpmod = find_module("chanserv/main");
    if (tmpmod)
        do_load_module(tmpmod, "chanserv/main");

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

    if (module_httpd) {
        remove_callback(module_httpd, "request", do_request);
        unuse_module(module_httpd);
        module_httpd = NULL;
    }

    return 1;
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
