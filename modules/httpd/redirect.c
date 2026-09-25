/* Nick/channel URL redirect module for HTTP server.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Anybody on the Internet can ask for any name, registered or not, so the
 * records are never read here with the main thread waiting: the request
 * is kept, the record is fetched in the background (prefetch_nickinfo(),
 * prefetch_nickgroupinfo(), prefetch_channelinfo()), and the answer goes
 * when it is in memory.
 */

#include "services.h"
#include "modules.h"
#include "conffile.h"
#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"

#include "modules/httpd/http.h"

/*************************************************************************/

static Module *module_nickserv;
static Module *module_chanserv;

static char *NicknamePrefix;
static char *ChannelPrefix;

/* The prefixes the routes were claimed for (they may change on REHASH). */
static char *claimed_nick, *claimed_chan;

/* Imported from NickServ: */
static typeof(get_nickinfo) *p_get_nickinfo;
static typeof(get_nickinfo_noexpire) *p_get_nickinfo_noexpire;
static typeof(put_nickinfo) *p_put_nickinfo;
static typeof(_get_ngi) *p__get_ngi;
static typeof(put_nickgroupinfo) *p_put_nickgroupinfo;
static typeof(prefetch_nickinfo) *p_prefetch_nickinfo;
static typeof(prefetch_nickgroupinfo) *p_prefetch_nickgroupinfo;
#define get_nickinfo (*p_get_nickinfo)
#define get_nickinfo_noexpire (*p_get_nickinfo_noexpire)
#define put_nickinfo (*p_put_nickinfo)
#define _get_ngi (*p__get_ngi)
#define put_nickgroupinfo (*p_put_nickgroupinfo)
#define prefetch_nickinfo (*p_prefetch_nickinfo)
#define prefetch_nickgroupinfo (*p_prefetch_nickgroupinfo)

/* Imported from ChanServ: */
static typeof(get_channelinfo) *p_get_channelinfo;
static typeof(put_channelinfo) *p_put_channelinfo;
static typeof(prefetch_channelinfo) *p_prefetch_channelinfo;
#define get_channelinfo (*p_get_channelinfo)
#define put_channelinfo (*p_put_channelinfo)
#define prefetch_channelinfo (*p_prefetch_channelinfo)

/* A request waiting for its record. */
typedef struct lookup_ Lookup;
struct lookup_ {
    Lookup *next, *prev;
    http_req_t id;
    char name[CHANMAX + 1]; /* The nickname, or the channel with its # */
};
static Lookup *lookups;

/*************************************************************************/
/*************************************************************************/

static void lookup_free(Lookup *l)
{
    LIST_REMOVE(l, lookups);
    free(l);
}

/* The name in a path: everything after the prefix, up to the next
 * slash.  Returns 0 if it is empty or too long. */
static int name_from_path(const char *path, const char *prefix, char *buf,
                          size_t size)
{
    const char *name = path + strlen(prefix);
    size_t len = strcspn(name, "/");

    if (!len || len >= size)
        return 0;
    memcpy(buf, name, len);
    buf[len] = 0;
    return 1;
}

/* Answer `id' with a redirect to `url', or with a page saying why not. */
static void answer(http_req_t id, const char *url, const char *what,
                   const char *name, int registered)
{
    struct HttpResponse *res = http_response(id);
    char namehtml[CHANMAX * 6];

    if (!res)
        return;  /* timed out meanwhile */
    http_quote_html(name, namehtml, sizeof(namehtml));
    if (url) {
        http_response_redirect(res, HTTP_R_FOUND, url);
    } else if (registered) {
        http_response_error(res, HTTP_E_NOT_FOUND,
                            "<h1 align=center>URL Not Set</h1>"
                            "The %s <b>%s</b> does not have a URL set.",
                            what, namehtml);
    } else {
        http_response_error(res, HTTP_E_NOT_FOUND,
                            "<h1 align=center>%s Not Registered</h1>"
                            "The %s <b>%s</b> is not registered.",
                            *what == 'n' ? "Nickname" : "Channel", what,
                            namehtml);
    }
    http_respond(id, NULL);
}

/*************************************************************************/
/******************************* Nicknames *******************************/
/*************************************************************************/

/* The nickname and its group are in memory: no I/O from here on. */
static void nick_group_ready(void *arg)
{
    Lookup *l = arg;
    NickInfo *ni;
    NickGroupInfo *ngi;

    if (!module_nickserv) {
        answer(l->id, NULL, "nickname", l->name, 0);
        lookup_free(l);
        return;
    }
    ni = get_nickinfo(l->name);
    ngi = (ni && ni->nickgroup) ? get_ngi(ni) : NULL;
    answer(l->id, ngi ? ngi->url : NULL, "nickname", l->name, ngi != NULL);
    put_nickgroupinfo(ngi);
    put_nickinfo(ni);
    lookup_free(l);
}

/* The nickname is in memory (or known not to exist); now its group. */
static void nick_ready(void *arg)
{
    Lookup *l = arg;
    NickInfo *ni;
    uint32 group;

    if (!module_nickserv) {
        nick_group_ready(l);
        return;
    }
    ni = get_nickinfo_noexpire(l->name);
    group = ni ? ni->nickgroup : 0;
    put_nickinfo(ni);
    if (!group || !prefetch_nickgroupinfo(THIS_MODULE, &group, 1,
                                          nick_group_ready, l))
        nick_group_ready(l);
}

static int route_nick(http_req_t id, const struct HttpRequest *req,
                   struct HttpResponse *res, void *user)
{
    Lookup *l;
    const char *keys[1];

    l = scalloc(1, sizeof(*l));
    if (!module_nickserv
     || !name_from_path(req->hreq_path, NicknamePrefix, l->name, NICKMAX)) {
        free(l);
        http_response_error(res, HTTP_E_NOT_FOUND, NULL);
        return 1;
    }
    l->id = id;
    LIST_INSERT(l, lookups);
    keys[0] = l->name;
    /* `nick_ready' may run before this returns, when the record is in
     * memory already: `l' is not touched again here. */
    if (!prefetch_nickinfo(THIS_MODULE, keys, 1, nick_ready, l)) {
        lookup_free(l);
        http_response_error(res, HTTP_F_SERVICE_UNAVAILABLE, NULL);
        return 1;
    }
    return 0;
}

/*************************************************************************/
/******************************* Channels ********************************/
/*************************************************************************/

static void chan_ready(void *arg)
{
    Lookup *l = arg;
    ChannelInfo *ci = module_chanserv ? get_channelinfo(l->name) : NULL;

    answer(l->id, ci ? ci->url : NULL, "channel", l->name, ci != NULL);
    put_channelinfo(ci);
    lookup_free(l);
}

static int route_chan(http_req_t id, const struct HttpRequest *req,
                   struct HttpResponse *res, void *user)
{
    Lookup *l;
    const char *keys[1];

    l = scalloc(1, sizeof(*l));
    l->name[0] = '#';
    if (!module_chanserv
     || !name_from_path(req->hreq_path, ChannelPrefix, l->name + 1,
                        CHANMAX - 1)) {
        free(l);
        http_response_error(res, HTTP_E_NOT_FOUND, NULL);
        return 1;
    }
    l->id = id;
    LIST_INSERT(l, lookups);
    keys[0] = l->name;
    if (!prefetch_channelinfo(THIS_MODULE, keys, 1, chan_ready, l)) {
        lookup_free(l);
        http_response_error(res, HTTP_F_SERVICE_UNAVAILABLE, NULL);
        return 1;
    }
    return 0;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static ConfigDirective redirect_config[] = {
    { "NicknamePrefix",   { { CD_STRING, 0, &NicknamePrefix } } },
    { "ChannelPrefix",    { { CD_STRING, 0, &ChannelPrefix } } },
    { NULL }
};

/*************************************************************************/

/* Claim (or give up) the route for one prefix.  A prefix route must end
 * in a slash: that is what makes it match everything under it. */
static void claim(char **claimed, const char *prefix, HttpHandlerFn fn,
                  const char *what)
{
    if (*claimed && (!prefix || strcmp(*claimed, prefix) != 0)) {
        http_del_route(THIS_MODULE, "GET", *claimed);
        free(*claimed);
        *claimed = NULL;
    }
    if (!prefix || *claimed)
        return;
    if (prefix[0] != '/' || prefix[strlen(prefix) - 1] != '/') {
        module_log("%s `%s' must begin and end with a slash; %s redirects"
                   " are off", what, prefix,
                   fn == route_nick ? "nickname" : "channel");
        return;
    }
    if (!http_add_route(THIS_MODULE, "GET", prefix, fn, NULL)) {
        module_log("Unable to claim %s (already claimed)", prefix);
        return;
    }
    *claimed = sstrdup(prefix);
}

static void claim_all(void)
{
    claim(&claimed_nick, NicknamePrefix, route_nick, "NicknamePrefix");
    claim(&claimed_chan, ChannelPrefix, route_chan, "ChannelPrefix");
}

static void redirect_rehash(Module *module)
{
    claim_all();
}

/*************************************************************************/

/* NickServ and ChanServ are optional: their records are looked up
 * through their symbols while they are loaded. */

static int do_module_loaded(Module *mod, const char *modname)
{
    if (strcmp(modname, "nickserv/main") == 0) {
        p_get_nickinfo = module_symbol(mod, "get_nickinfo");
        p_get_nickinfo_noexpire =
            module_symbol(mod, "get_nickinfo_noexpire");
        p_put_nickinfo = module_symbol(mod, "put_nickinfo");
        p__get_ngi = module_symbol(mod, "_get_ngi");
        p_put_nickgroupinfo = module_symbol(mod, "put_nickgroupinfo");
        p_prefetch_nickinfo = module_symbol(mod, "prefetch_nickinfo");
        p_prefetch_nickgroupinfo =
            module_symbol(mod, "prefetch_nickgroupinfo");
        if (p_get_nickinfo && p_get_nickinfo_noexpire && p_put_nickinfo
         && p__get_ngi && p_put_nickgroupinfo && p_prefetch_nickinfo
         && p_prefetch_nickgroupinfo
        ) {
            module_nickserv = mod;
        } else {
            module_log("Required symbols not found, nickname redirects will"
                       " not be available");
            module_nickserv = NULL;
        }
    } else if (strcmp(modname, "chanserv/main") == 0) {
        p_get_channelinfo = module_symbol(mod, "get_channelinfo");
        p_put_channelinfo = module_symbol(mod, "put_channelinfo");
        p_prefetch_channelinfo =
            module_symbol(mod, "prefetch_channelinfo");
        if (p_get_channelinfo && p_put_channelinfo
         && p_prefetch_channelinfo) {
            module_chanserv = mod;
        } else {
            module_log("Required symbols not found, channel redirects will"
                       " not be available");
            module_chanserv = NULL;
        }
    }

    return 0;
}

/*************************************************************************/

static int do_module_unloaded(Module *mod)
{
    /* Its fetches in flight are dropped with it (the store cancels them
     * by type); the requests they were for time out. */
    if (mod == module_nickserv)
        module_nickserv = NULL;
    else if (mod == module_chanserv)
        module_chanserv = NULL;
    return 0;
}

/*************************************************************************/

static int redirect_init(Module *module)
{
    Module *other;

    if (!event_attach(module, EVENT_MODULE_LOADED, do_module_loaded)
     || !event_attach(module, EVENT_MODULE_UNLOADED, do_module_unloaded)
    ) {
        module_log("Unable to attach event handlers");
        return 0;
    }

    if ((other = module_find("nickserv/main")) != NULL)
        do_module_loaded(other, "nickserv/main");
    if ((other = module_find("chanserv/main")) != NULL)
        do_module_loaded(other, "chanserv/main");

    claim_all();
    return 1;
}

/*************************************************************************/

static int redirect_fini(Module *module, int shutdown)
{
    /* Our fetches in flight are dropped with this module; so are the
     * requests they were for. */
    while (lookups)
        lookup_free(lookups);
    http_del_routes(module);
    free(claimed_nick);
    free(claimed_chan);
    claimed_nick = claimed_chan = NULL;
    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "HTTP: redirects to the URL of a nickname or a channel",
    .requires = MODULE_REQUIRES("httpd/main"),
    .config = redirect_config,
    .init = redirect_init,
    .fini = redirect_fini,
    .rehash = redirect_rehash,
};

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
