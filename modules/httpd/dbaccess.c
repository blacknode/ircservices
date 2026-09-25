/* Database access module for HTTP server.
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
#include "conffile.h"
#include "modules/operserv/operserv.h"
#include "modules/operserv/maskdata.h"
#include "modules/operserv/akill.h"
#include "modules/operserv/news.h"
#include "modules/operserv/sline.h"
#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"
#include "modules/chanserv/access.h"
#include "modules/statserv/statserv.h"

#include "modules/httpd/http.h"

/*************************************************************************/

/* One request, as the handlers below see it. */
typedef struct {
    const struct HttpRequest *req;
    struct HttpResponse *res;
    char url[HTTP_PATH_MAX + 1];  /* The path: the handlers cut it up */
    char vars[4][256];            /* What getvar() returned */
    int nvars;
} Client;

/* A variable of the request (query string or form), or NULL. */
static char *getvar(Client *c, const char *name)
{
    char *buf;

    if (c->nvars >= lenof(c->vars))
        return NULL;
    buf = c->vars[c->nvars];
    if (!http_request_var(c->req, name, buf, sizeof(c->vars[0])))
        return NULL;
    c->nvars++;
    return buf;
}

/* Redirect to `path' followed by `suffix'. */
static void redirect_to(Client *c, const char *path, const char *suffix)
{
    char buf[HTTP_PATH_MAX + 2];

    snprintf(buf, sizeof(buf), "%s%s", path, suffix);
    http_response_redirect(c->res, HTTP_R_FOUND, buf);
}

/*************************************************************************/

static Module *module_operserv;
static Module *module_operserv_akill;
static Module *module_operserv_news;
static Module *module_operserv_sessions;
static Module *module_operserv_sline;
static Module *module_nickserv;
static Module *module_chanserv;
static Module *module_statserv;

static char *Prefix;
static int Prefix_len;

/* The routes claimed: Prefix (without its trailing slash) exactly, and
 * everything under Prefix/. */
static char claimed_exact[HTTP_PATH_MAX + 1];
static char claimed_prefix[HTTP_PATH_MAX + 2];


/* Note that none of the following are used if the respective module is not
 * loaded, so it's safe to reference them directly. */

/* Imported from OperServ: */
static typeof(ServicesRoot) *p_ServicesRoot;
static typeof(get_operserv_data) *p_get_operserv_data;
static typeof(get_maskdata) *p_get_maskdata;
static typeof(put_maskdata) *p_put_maskdata;
static typeof(first_maskdata) *p_first_maskdata;
static typeof(next_maskdata) *p_next_maskdata;
#define ServicesRoot (*p_ServicesRoot)
#define get_operserv_data (*p_get_operserv_data)
#define get_maskdata (*p_get_maskdata)
#define put_maskdata (*p_put_maskdata)
#define first_maskdata (*p_first_maskdata)
#define next_maskdata (*p_next_maskdata)

/* Imported from NickServ: */
typeof(get_nickinfo) *p_get_nickinfo;
typeof(put_nickinfo) *p_put_nickinfo;
typeof(foreach_nickinfo) *p_foreach_nickinfo;
static typeof(_get_ngi) *p__get_ngi;
static typeof(_get_ngi_id) *p__get_ngi_id;
typeof(put_nickgroupinfo) *p_put_nickgroupinfo;
#define get_nickinfo (*p_get_nickinfo)
#define put_nickinfo (*p_put_nickinfo)
#define foreach_nickinfo (*p_foreach_nickinfo)
#define _get_ngi (*p__get_ngi)
#define _get_ngi_id (*p__get_ngi_id)
#define put_nickgroupinfo (*p_put_nickgroupinfo)

/* Imported from ChanServ: */
static typeof(CSMaxReg) *p_CSMaxReg;
typeof(get_channelinfo) *p_get_channelinfo;
typeof(put_channelinfo) *p_put_channelinfo;
typeof(foreach_channelinfo) *p_foreach_channelinfo;
static typeof(update_owned_channels) *p_update_owned_channels;
#define CSMaxReg (*p_CSMaxReg)
#define get_channelinfo (*p_get_channelinfo)
#define put_channelinfo (*p_put_channelinfo)
#define foreach_channelinfo (*p_foreach_channelinfo)
#define update_owned_channels (*p_update_owned_channels)

/* Imported from StatServ: */
typeof(get_serverstats) *p_get_serverstats;
typeof(put_serverstats) *p_put_serverstats;
typeof(first_serverstats) *p_first_serverstats;
typeof(next_serverstats) *p_next_serverstats;
#define get_serverstats (*p_get_serverstats)
#define put_serverstats (*p_put_serverstats)
#define first_serverstats (*p_first_serverstats)
#define next_serverstats (*p_next_serverstats)


/* The following macro is used by the NickServ and ChanServ handlers to
 * make links to various ways of listing nicknames and channels. */

#define PRINT_SELOPT(c,prefix,select,value,text)                            \
    http_response_printf((c)->res, "%s%s%d%s%s%s", prefix,                         \
               (select)==(value) ? "<!--" : "<a href=\"./?select=", (value),\
               (select)==(value) ? "-->(" : "\">", text,                    \
               (select)==(value) ? ")" : "</a>")

/*************************************************************************/

/* Handlers for individual databases: */

static int handle_operserv(Client *c, char *path);
static int handle_operserv_akill(Client *c, char *path);
static int handle_operserv_exclude(Client *c, char *path);
static int handle_operserv_news(Client *c, char *path);
static int handle_operserv_sessions(Client *c, char *path);
static int handle_operserv_sline(Client *c, char *path);
static int handle_nickserv(Client *c, char *path);
static int handle_chanserv(Client *c, char *path);
static int handle_statserv(Client *c, char *path);

/*************************************************************************/
/**************************** Local routines *****************************/
/*************************************************************************/

/* Local utility routine to simplify strftime() calls: */

static int my_strftime(char *buf, int size, time_t t)
{
    char tmp[BUFSIZE];
    int retval = strftime(tmp, sizeof(tmp), "%b %d %H:%M:%S %Y",
                          localtime(&t));
    tmp[sizeof(tmp)-1] = 0;
    if (retval == 0)
        *tmp = 0;
    http_quote_html(tmp, buf, size);
    return strlen(buf);
}


/*************************************************************************/
/******************** Request callback and handlers **********************/
/*************************************************************************/

static int do_request(Client *c)
{
    char *subpath;

    if (strncmp(c->url, Prefix, Prefix_len) != 0)
        return 0;
    subpath = c->url + Prefix_len;
    if (!*subpath) {
        redirect_to(c, c->url, "/");
        return 1;
    } else if (*subpath != '/') {
        return 0;
    }
    subpath++;

    if (strncmp(subpath,"operserv",8) == 0)
        return handle_operserv(c, subpath+8);
    if (strncmp(subpath,"nickserv",8) == 0)
        return handle_nickserv(c, subpath+8);
    if (strncmp(subpath,"chanserv",8) == 0)
        return handle_chanserv(c, subpath+8);
    if (strncmp(subpath,"statserv",8) == 0)
        return handle_statserv(c, subpath+8);

    if (!*subpath) {
        http_response_set(c->res, HTTP_S_OK, "text/html", NULL, 0);
        http_response_printf(c->res,
                   "<html><head><title>IRC Services database access</title>"
                   "</head><body><h1 align=center>IRC Services database"
                   " access</h1><p>");
        if (!module_operserv) {  /* this implies !nickserv etc. */
            http_response_printf(c->res,
                       "No service modules are currently loaded.</body>"
                       "</html>");
        } else {
            http_response_printf(c->res, "Please select one of the following:<ul>");
            http_response_printf(c->res,
                       "<li><a href=operserv/>OperServ data</a>");
            if (module_nickserv)
                http_response_printf(c->res,
                           "<li><a href=nickserv/>List of registered"
                           " nicknames</a>");
            if (module_chanserv)
                http_response_printf(c->res,
                           "<li><a href=chanserv/>List of registered"
                           " channels</a>");
            if (module_statserv)
                http_response_printf(c->res,
                           "<li><a href=statserv/>Network statistics</a>");
            http_response_printf(c->res, "</ul></body></html>");
        }
        return 1;
    }

    return 0;
}

/*************************************************************************/

static int handle_operserv(Client *c, char *path)
{
    if (!module_operserv)
        return 0;

    if (!*path) {
        redirect_to(c, c->url, "/");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    if (strncmp(path,"akill",5) == 0)
        return handle_operserv_akill(c, path+5);
    if (strncmp(path,"exclude",7) == 0)
        return handle_operserv_exclude(c, path+7);
    if (strncmp(path,"news",4) == 0)
        return handle_operserv_news(c, path+4);
    if (strncmp(path,"sessions",6) == 0)
        return handle_operserv_sessions(c, path+8);
    if (strncmp(path,"sline",5) == 0)
        return handle_operserv_sline(c, path+5);

    if (!*path) {
        int32 maxusercnt;
        time_t maxusertime;

        http_response_set(c->res, HTTP_S_OK, "text/html", NULL, 0);
        http_response_printf(c->res,
                   "<html><head><title>OperServ database access</title>"
                   "</head><body><h1 align=center>OperServ database"
                   " access</h1><p><ul><li>Current number of users:"
                   " <b>%d</b> (%d ops)", usercnt, opcnt);
        if (get_operserv_data
         && get_operserv_data(OSDATA_MAXUSERCNT, &maxusercnt)
         && get_operserv_data(OSDATA_MAXUSERTIME, &maxusertime)
        ) {
            char timebuf[BUFSIZE];
            my_strftime(timebuf, sizeof(timebuf), maxusertime);
            http_response_printf(c->res, "<li>Maximum user count: <b>%d</b>"
                   " (reached at %s)</ul>", maxusercnt, timebuf);
        }
        http_response_printf(c->res, "Please select one of the following:<ul>");
        if (module_operserv_akill
         || module_operserv_news
         || module_operserv_sessions
         || module_operserv_sline
        ) {
            if (module_operserv_akill)
                http_response_printf(c->res,
                           "<li><a href=akill/>List of autokills</a><li>"
                           "<a href=exclude/>List of autokill exclusions</a>");
            if (module_nickserv)
                http_response_printf(c->res,
                           "<li><a href=news/>List of news items</a>");
            if (module_chanserv)
                http_response_printf(c->res,
                           "<li><a href=sessions/>List of session"
                           " exceptions</a>");
            if (module_statserv)
                http_response_printf(c->res,
                           "<li><a href=sline/>List of S-lines</a>");
        }
        http_response_printf(c->res, "<li><a href=../>Return to previous menu</a>");
        http_response_printf(c->res, "</ul></body></html>");
        return 1;
    }

    return 0;
}

/*************************************************************************/

/* Common code for handling MaskData structures.  `typename' should be in
 * all lower case (except for letters that are always capitalized); `a_an'
 * should be the proper indefinite article for the type name ("a" or "an").
 * Presently the code assumes that the plural is formed by simply adding an
 * "s"; this works for the currently available options (autokill, exclusion,
 * exception, S-line).
 */

static int handle_maskdata(Client *c, char *path, uint8 type,
                           const char *a_an, const char *typename)
{
    char urlbuf[BUFSIZE*3];   /* *3 because of / -> %2F */
    char htmlbuf[BUFSIZE*5];  /* *5 because of & -> &amp; */
    MaskData *md;

    if (!*path) {
        redirect_to(c, c->url, "/");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    http_response_set(c->res, HTTP_S_OK, "text/html", NULL, 0);
    http_response_printf(c->res, "<html><head><title>%c%s database access</title>"
               "</head><body>", toupper(*typename), typename+1);
    if (!*path) {
        int count = 0;

        http_response_printf(c->res, "<h1 align=center>%c%s database access</h1>"
                   "<p>Click on %s %s for detailed information.<p>"
                   "<a href=../>(Return to previous menu)</a><p><ul>",
                   toupper(*typename), typename+1, a_an, typename);
        for (md = first_maskdata(type); md; md = next_maskdata(type)) {
            http_quote_html(md->mask, htmlbuf, sizeof(htmlbuf));
            http_quote_url(md->mask, urlbuf, sizeof(urlbuf));
            http_response_printf(c->res, "<li><a href=\"%s\">%s</a>",
                       urlbuf, htmlbuf);
            if (type == MD_EXCEPTION)
                http_response_printf(c->res, " (%d)", md->limit);
            count++;
        }
        http_response_printf(c->res, "</ul><p>%d %s%s.</body></html>",
                   count, typename, count==1 ? "" : "s");
        return 1;
    }

    http_unquote_url(path);
    md = get_maskdata(type, path);
    http_quote_html(path, htmlbuf, sizeof(htmlbuf));
    if (!md) {
        http_response_printf(c->res, "<h1 align=center>%c%s not found</h1>"
                   "<p>No %s was found for <b>%s</b>.<p><a href=./>Return"
                   " to %s list</a></body></html>", toupper(*typename),
                   typename+1, typename, htmlbuf, typename);
        return 1;
    }

    http_response_printf(c->res, "<h1 align=center>%c%s database access</h1>"
               "<h2 align=center>%s</h2><div align=center>",
               toupper(*typename), typename+1, htmlbuf);
    http_response_printf(c->res, "<table border=0 cellspacing=4>");
    if (type == MD_EXCEPTION) {
        http_response_printf(c->res, "<tr><th align=right valign=top>Limit:&nbsp;"
                   "<td>%d", md->limit);
    }
    http_response_printf(c->res, "<tr><th align=right valign=top>Set by:&nbsp;<td>");
    http_quote_html(md->who, htmlbuf, sizeof(htmlbuf));
    if (module_nickserv && get_nickinfo(md->who)) {
        http_quote_url(md->who, urlbuf, sizeof(urlbuf));
        http_response_printf(c->res, "<a href=\"../../nickserv/%s\">%s</a>",
                               urlbuf, htmlbuf);
    } else {
        http_response_printf(c->res, "%s", htmlbuf);
    }
    http_quote_html(md->reason ? md->reason : "", htmlbuf, sizeof(htmlbuf));
    http_response_printf(c->res, "<tr><th align=right valign=top>Reason:&nbsp;<td>%s",
               htmlbuf);
    my_strftime(htmlbuf, sizeof(htmlbuf), md->time);
    http_response_printf(c->res, "<tr><th align=right valign=top>Set on:&nbsp;<td>%s",
               htmlbuf);
    http_response_printf(c->res,
               "<tr><th align=right valign=top>Expires on:&nbsp;<td>");
    if (md->expires) {
        my_strftime(htmlbuf, sizeof(htmlbuf), md->expires);
        http_response_printf(c->res, "%s", htmlbuf);
    } else {
        http_response_printf(c->res, "<font color=green>Does not expire</font>");
    }
    http_response_printf(c->res,
               "<tr><th align=right valign=top>Last triggered:&nbsp;<td>");
    if (md->lastused) {
        my_strftime(htmlbuf, sizeof(htmlbuf), md->lastused);
        http_response_printf(c->res, "%s", htmlbuf);
    } else {
        http_response_printf(c->res, "<font color=red>Never</font>");
    }
    http_response_printf(c->res, "</table></div><p><a href=./>Return to %s list"
               "</a></body></html>", typename);
    put_maskdata(md);
    return 1;
}

/*************************************************************************/

static int handle_operserv_akill(Client *c, char *path)
{
    if (!module_operserv_akill)
        return 0;
    return handle_maskdata(c, path, MD_AKILL, "an", "autokill");
}

/*************************************************************************/

static int handle_operserv_exclude(Client *c, char *path)
{
    if (!module_operserv_akill)
        return 0;
    return handle_maskdata(c, path, MD_EXCLUDE,
                           "an", "autokill exclusion");
}

/*************************************************************************/

static int handle_operserv_news(Client *c, char *path)
{
    char htmlbuf[BUFSIZE*5];
    NewsItem *news;

    if (!module_operserv_news)
        return 0;

    if (!*path) {
        redirect_to(c, c->url, "/");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    http_response_set(c->res, HTTP_S_OK, "text/html", NULL, 0);
    http_response_printf(c->res, "<html><head><title>News database access"
               "</title></head><body>");
    http_response_printf(c->res, "<h1 align=center>News database"
               " access</h1><p><a href=../>(Return to previous menu)</a>");
    http_response_printf(c->res, "<h2 align=center>Logon news</h2><p>"
               "<table border=2><tr><th>Num<th>Added by<th>Date<th>Text");
    for (news = first_news(); news; news = next_news()) {
        if (news->type != NEWS_LOGON)
            continue;
        http_quote_html(news->who, htmlbuf, sizeof(htmlbuf));
        http_response_printf(c->res, "<tr><td>%d<td>%s", news->num, htmlbuf);
        my_strftime(htmlbuf, sizeof(htmlbuf), news->time);
        http_response_printf(c->res, "<td>%s", htmlbuf);
        http_quote_html(news->text ? news->text : "",
                        htmlbuf, sizeof(htmlbuf));
        http_response_printf(c->res, "<td>%s", htmlbuf);
    }
    http_response_printf(c->res, "</table><h2 align=center>Oper news</h2><p>"
               "<table border=2><tr><th>Num<th>Added by<th>Date<th>Text");
    for (news = first_news(); news; news = next_news()) {
        if (news->type != NEWS_OPER)
            continue;
        http_quote_html(news->who, htmlbuf, sizeof(htmlbuf));
        http_response_printf(c->res, "<tr><td>%d<td>%s", news->num, htmlbuf);
        my_strftime(htmlbuf, sizeof(htmlbuf), news->time);
        http_response_printf(c->res, "<td>%s", htmlbuf);
        http_quote_html(news->text ? news->text : "",
                        htmlbuf, sizeof(htmlbuf));
        http_response_printf(c->res, "<td>%s", htmlbuf);
    }
    http_response_printf(c->res, "</table></body></html>");
    return 1;
}

/*************************************************************************/

static int handle_operserv_sessions(Client *c, char *path)
{
    if (!module_operserv_sessions)
        return 0;
    return handle_maskdata(c, path, MD_EXCEPTION,
                           "a", "session exception");
}

/*************************************************************************/

static int handle_operserv_sline(Client *c, char *path)
{
    char typename[7] = "S.line";

    if (!module_operserv_sline)
        return 0;

    if (!*path) {
        redirect_to(c, c->url, "/");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    if (!*path) {
        http_response_set(c->res, HTTP_S_OK, "text/html", NULL, 0);
        http_response_printf(c->res, "<html><head><title>S-line database access"
                   "</title></head><body>");
        http_response_printf(c->res, "<p>Please select one of the following:<ul>"
                   "<li><a href=G/>List of SGlines</a>"
                   "<li><a href=Q/>List of SQlines</a>"
                   "<li><a href=Z/>List of SZlines</a>"
                   "<li><a href=../>Return to previous menu</a>"
                   "</ul></body></html>");
        return 1;
    } else if (*path != 'G' && *path != 'Q' && *path != 'Z') {
        return 0;
    }

    typename[1] = *path;
    return handle_maskdata(c, path+1, *path, "an", typename);
}

/* Listing nicknames and channels, a page at a time. */

#define LIST_PAGE 500

/* A name as its key in the database (IRC case folding). */
static void irc_lowercase_key(const char *name, char *buf, int size)
{
    int n = 0;

    while (*name && n < size-1)
        buf[n++] = irc_tolower(*name++);
    buf[n] = 0;
}

typedef struct {
    Client *c;
    int count;
    int more;                   /* Another page follows */
    int want_ngi;               /* Show nickgroup flags (costs a lookup) */
    char last[NICKMAX*2+CHANMAX*2];  /* Key of the last one shown */
} NickListArg, ChanListArg;

static int nick_list_one(NickInfo *ni, void *arg_)
{
    NickListArg *arg = arg_;
    NickGroupInfo *ngi = NULL;
    char nickhtml[NICKMAX*6], nickurl[NICKMAX*6];

    if (arg->count >= LIST_PAGE) {
        arg->more = 1;
        return 1;
    }
    if (arg->want_ngi && ni->nickgroup)
        ngi = get_ngi(ni);
    http_quote_html(ni->nick, nickhtml, sizeof(nickhtml));
    http_quote_url(ni->nick, nickurl, sizeof(nickurl));
    http_response_printf(arg->c->res, "<li><tt>%s%s%s%s&nbsp;</tt>"
               "<a href=\"%s\">%s</a>",
               ni->status & NS_VERBOTEN ? "-" : "&nbsp;",
               ngi && (ngi->flags & NF_SUSPENDED) ? "*" : "&nbsp;",
               ni->status & NS_NOEXPIRE ? "!" : "&nbsp;",
               ngi && ngi->authcode ? "?" : "&nbsp;",
               nickurl, nickhtml);
    put_nickgroupinfo(ngi);
    irc_lowercase_key(ni->nick, arg->last, sizeof(arg->last));
    arg->count++;
    return 0;
}

static int chan_list_one(ChannelInfo *ci, void *arg_)
{
    ChanListArg *arg = arg_;
    char chanhtml[CHANMAX*6], chanurl[CHANMAX*6];

    if (arg->count >= LIST_PAGE) {
        arg->more = 1;
        return 1;
    }
    http_quote_html(ci->name, chanhtml, sizeof(chanhtml));
    http_quote_url(ci->name+1, chanurl, sizeof(chanurl));
    http_response_printf(arg->c->res, "<li><tt>%s%s%s&nbsp;</tt>"
               "<a href=\"%s\">%s</a>",
               ci->flags & CF_VERBOTEN  ? "-" : "&nbsp;",
               ci->flags & CF_SUSPENDED ? "*" : "&nbsp;",
               ci->flags & CF_NOEXPIRE  ? "!" : "&nbsp;",
               chanurl, chanhtml);
    irc_lowercase_key(ci->name, arg->last, sizeof(arg->last));
    arg->count++;
    return 0;
}

/*************************************************************************/

/*************************************************************************/

static struct {
    int32 mask, flags;
    const char *text;
} nickopts[] = {
    { NF_KILLPROTECT | NF_KILL_QUICK | NF_KILL_IMMED,
        NF_KILLPROTECT | NF_KILL_QUICK | NF_KILL_IMMED,
        "kill protection (immediate)" },
    { NF_KILLPROTECT | NF_KILL_QUICK | NF_KILL_IMMED,
        NF_KILLPROTECT | NF_KILL_QUICK,
        "kill protection (quick)" },
    { NF_KILLPROTECT | NF_KILL_QUICK | NF_KILL_IMMED, NF_KILLPROTECT,
        "kill protection" },
    { NF_SECURE,       NF_SECURE,       "secure" },
    { NF_MEMO_SIGNON,  NF_MEMO_SIGNON,  "memo notify on logon" },
    { NF_MEMO_RECEIVE, NF_MEMO_RECEIVE, "memo notify on receive" },
    { NF_PRIVATE,      NF_PRIVATE,      "private" },
    { NF_HIDE_EMAIL,   NF_HIDE_EMAIL,   "hide E-mail address" },
    { NF_HIDE_MASK,    NF_HIDE_MASK,    "hide user@host mask" },
    { NF_HIDE_QUIT,    NF_HIDE_QUIT,    "hide quit message" },
    { NF_MEMO_FWD | NF_MEMO_FWDCOPY, NF_MEMO_FWD | NF_MEMO_FWDCOPY,
        "copy and forward memos" },
    { NF_MEMO_FWD | NF_MEMO_FWDCOPY, NF_MEMO_FWD, "forward memos" },
    { 0 }
};

static int handle_nickserv(Client *c, char *path)
{
    char nickhtml[NICKMAX*5];
    NickInfo *ni;
    NickGroupInfo *ngi = NULL;

    if (!module_nickserv)
        return 0;

    if (!*path) {
        redirect_to(c, c->url, "/");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    http_response_set(c->res, HTTP_S_OK, "text/html", NULL, 0);

    if (!*path) {
        int count = 0;
        char *select_var = getvar(c, "select");
        char *start_var = getvar(c, "start");
        enum {SEL_ALL, SEL_FORBIDDEN,SEL_SUSPENDED,SEL_NOEXPIRE,SEL_NOAUTH}
            select = SEL_ALL;

        if (select_var)
            select = atoi(select_var);
        http_response_printf(c->res,
                   "<html><head><title>Nickname database access</title>"
                   "</head><body><h1 align=center>Nickname database"
                   " access</h1><p>Click to display:");
        PRINT_SELOPT(c,   " ", select, SEL_ALL, "All nicknames");
        PRINT_SELOPT(c, " | ", select, SEL_FORBIDDEN, "Forbidden nicknames");
        PRINT_SELOPT(c, " | ", select, SEL_SUSPENDED, "Suspended nicknames");
        PRINT_SELOPT(c, " | ", select, SEL_NOEXPIRE, "Non-expiring nicknames");
        PRINT_SELOPT(c, " | ", select, SEL_NOAUTH,
                     "Not-yet-authenticated nicknames");
        http_response_printf(c->res,
                   "<br>Or click on a nickname for detailed information."
                   "<p><a href=../>(Return to previous menu)</a><p><ul>");
        {
            /* The nicknames are in the database: a page at a time, in
             * order, from where the previous page stopped. */
            static const char *const conds[] = {
                /* SEL_ALL */       "",
                /* SEL_FORBIDDEN */ " and t.status & 2 <> 0",
                /* SEL_SUSPENDED */ " and exists (select 1 from nickgroups g"
                                    " where g.id = t.nickgroup"
                                    " and g.flags & 16384 <> 0)",
                /* SEL_NOEXPIRE */  " and t.status & 4 <> 0",
                /* SEL_NOAUTH */    " and exists (select 1 from nickgroups g"
                                    " where g.id = t.nickgroup"
                                    " and g.authcode <> 0)",
            };
            char where[256];
            const char *params[1];
            NickListArg arg;

            if (select < SEL_ALL || select > SEL_NOAUTH)
                select = SEL_ALL;
            snprintf(where, sizeof(where), "t.nick_key > $2%s",
                     conds[select]);
            params[0] = start_var ? start_var : "";
            arg.c = c;
            arg.count = 0;
            arg.more = 0;
            arg.want_ngi = (select == SEL_SUSPENDED || select == SEL_NOAUTH);
            *arg.last = 0;
            foreach_nickinfo(where, params, 1, nick_list_one, &arg);
            count = arg.count;
            if (arg.more) {
                char urlbuf[BUFSIZE*3];
                http_quote_url(arg.last, urlbuf, sizeof(urlbuf));
                http_response_printf(c->res, "</ul><p><a href=\"./?select=%d&start="
                           "%s\">Next page</a><ul>", select, urlbuf);
            }
        }
        http_response_printf(c->res,
                   "</ul><p>%d %snickname%s %s shown.<p>Key:<br>"
                   "<tt>&nbsp;&nbsp;-&nbsp;</tt>Nickname is forbidden<br>"
                   "<tt>&nbsp;&nbsp;*&nbsp;</tt>Nickname is suspended<br>"
                   "<tt>&nbsp;&nbsp;!&nbsp;</tt>Nickname is non-expiring<br>"
                   "<tt>&nbsp;&nbsp;?&nbsp;</tt>Nickname is not yet"
                   " authenticated</body></html>", count,
                   select==SEL_NOEXPIRE ? "non-expiring " : "",
                   count==1 ? "" : "s",
                   select==SEL_FORBIDDEN ? "forbidden" :
                       select==SEL_SUSPENDED ? "suspended" :
                       select==SEL_NOAUTH ? "not-yet-authenticated" : 
                       "registered");
        return 1;
    }

    http_unquote_url(path);
    ni = get_nickinfo(path);
    http_quote_html(path, nickhtml, sizeof(nickhtml));
    http_response_printf(c->res,
               "<html><head><title>Information on nickname \"%s\"</title>"
               "</head><body><h1 align=center>Information on nickname"
               " \"%s\"</h1><div align=center>", nickhtml, nickhtml);

    if (!ni) {
        http_response_printf(c->res, "<p>Nickname \"%s\" is not registered.",
                   nickhtml);
    } else if (ni->status & NS_VERBOTEN) {
        http_response_printf(c->res, "<p>Nickname \"%s\" is <b>forbidden</b>.",
                   nickhtml);
    } else if (!(ngi = get_ngi(ni))) {
        http_response_printf(c->res,
                   "<p>Error retrieving information for nickname \"%s\".",
                   nickhtml);
    } else {
        char buf[BUFSIZE*5], urlbuf[BUFSIZE*3];
        int need_comma = 0, i;

        http_response_printf(c->res, "<table border=0 cellspacing=4>");
        http_quote_html(ni->last_realname ? ni->last_realname : "", buf,
                        sizeof(buf));
        http_response_printf(c->res, "<tr><th align=right valign=top>Registered"
                   " to:&nbsp;<td>%s", buf);
        my_strftime(buf, sizeof(buf), ni->time_registered);
        http_response_printf(c->res, "<tr><th align=right valign=top>Time"
                   " registered:&nbsp;<td>%s", buf);
        http_quote_html(ni->last_realmask ? ni->last_realmask : "", buf,
                        sizeof(buf));
        if (get_user(ni->nick)) {
            http_response_printf(c->res,
                       "<tr><th align=right valign=top><font color=green>Is"
                       " online from:</font>&nbsp;<td>%s", buf);
            http_response_printf(c->res,
                       "<tr><th align=right valign=top>Authorization"
                       " status:&nbsp;<td>%s",
                       nick_identified(ni) ? "Identified" :
                       nick_recognized(ni) ? "Recognized (via access list)" :
                       "Not recognized");
        } else {
            http_response_printf(c->res, "<tr><th align=right valign=top>Last seen"
                       " address:&nbsp;<td>%s", buf);
            my_strftime(buf, sizeof(buf), ni->last_seen);
            http_response_printf(c->res, "<tr><th align=right valign=top>Last seen"
                       " on:&nbsp;<td>%s", buf);
        }
        if (ni->last_quit) {
            http_quote_html(ni->last_quit, buf, sizeof(buf));
            http_response_printf(c->res, "<tr><th align=right valign=top>Last quit"
                       " message:&nbsp;<td>%s", buf);
        }

        http_response_printf(c->res, "<tr><td colspan=2><hr>");

        if (ngi->info) {
            http_quote_html(ngi->info, buf, sizeof(buf));
            http_response_printf(c->res, "<tr><th align=right valign=top>"
                       "Information:&nbsp;<td>%s", buf);
        }
        if (ngi->url) {
            http_quote_html(ngi->url, buf, sizeof(buf));
            http_quote_html(ngi->url, urlbuf, sizeof(urlbuf));
            http_response_printf(c->res,
                       "<tr><th align=right valign=top>URL:&nbsp;"
                       "<td><a href=\"%s\">%s</a>", urlbuf, buf);
        }
        if (ngi->email) {
            http_quote_html(ngi->email, buf, sizeof(buf));
            http_quote_html(ngi->email, urlbuf, sizeof(urlbuf));
            http_response_printf(c->res,
                       "<tr><th align=right valign=top>E-mail address:&nbsp;"
                       "<td><a href=\"mailto:%s\">%s</a>", urlbuf, buf);
        }
        http_response_printf(c->res,
                   "<tr><th align=right valign=top>Options:&nbsp;<td>");
        if (ni->status & NS_NOEXPIRE) {
            http_response_printf(c->res, "<b>Will not expire</b>");
            need_comma++;
        }
        for (i = 0; nickopts[i].mask; i++) {
            if ((ngi->flags & nickopts[i].mask) == nickopts[i].flags) {
                http_quote_html(nickopts[i].text, buf, sizeof(buf));
                if (!need_comma)
                    *buf = toupper(*buf);
                http_response_printf(c->res, "%s%s", need_comma++ ? ", " : "", buf);
            }
        }
        if (!need_comma)
            http_response_printf(c->res, "None");
        http_response_printf(c->res, "<tr><th align=right valign=top>OperServ"
                   " privilege level:");
        if (irc_stricmp(ni->nick, ServicesRoot) == 0)
            http_response_printf(c->res, "<td>Services super-user");
        else if (ngi->os_priv >= NP_SERVADMIN)
            http_response_printf(c->res, "<td>Services administrator");
        else if (ngi->os_priv >= NP_SERVOPER)
            http_response_printf(c->res, "<td>Services operator");
        else 
            http_response_printf(c->res, "<td>None");

        http_response_printf(c->res, "<tr><td colspan=2><hr>");

        if (ngi->authcode) {
            http_response_printf(c->res, "<tr><td colspan=2 align=center>"
                       "<font color=red>This nickname's E-mail address has"
                       " not yet been authenticated.</font>");
            http_response_printf(c->res, "<tr><th align=right>Authenticatation code:"
                       "&nbsp;<td>%d", ngi->authcode);
            my_strftime(buf, sizeof(buf), ngi->authset);
            http_response_printf(c->res, "<tr><th align=right>Code set at:&nbsp;"
                       "<td>%s", buf);
            http_response_printf(c->res, "<tr><td colspan=2><hr>");
        }

        if (ngi->flags & NF_SUSPENDED) {
            http_response_printf(c->res, "<tr><td colspan=2 align=center>"
                       "<font color=red>This nickname group is"
                       " <b>suspended</b>.</font>");
            my_strftime(buf, sizeof(buf), ngi->suspend_time);
            http_response_printf(c->res, "<tr><th align=right valign=top>"
                       "Suspended on:&nbsp;<td>%s", buf);
            http_quote_html(ngi->suspend_who, buf, sizeof(buf));
            http_quote_url(ngi->suspend_who, urlbuf, sizeof(urlbuf));
            http_response_printf(c->res, "<tr><th align=right valign=top>"
                       "Suspended by:&nbsp;<td><a href=\"%s\">%s</a>",
                       urlbuf, buf);
            http_quote_html(ngi->suspend_reason ? ngi->suspend_reason
                            : "", buf, sizeof(buf));
            http_response_printf(c->res, "<tr><th align=right valign=top>"
                       "Reason for suspension:&nbsp;<td>%s", buf);
            if (ngi->suspend_expires)
                my_strftime(buf, sizeof(buf), ngi->suspend_expires);
            else
                strbcpy(buf, "<b>Never</b>");
            http_response_printf(c->res, "<tr><th align=right valign=top>"
                       "Suspension expires on:&nbsp;<td>%s", buf);
            http_response_printf(c->res, "<tr><td colspan=2><hr>");
        }

        http_response_printf(c->res,
                   "<tr><th align=right valign=top>Linked nicks:<td>");
        if (ngi->nicks_count == 1) {
            http_response_printf(c->res, "-");
        } else {
            int count = 0;
            ARRAY_FOREACH (i, ngi->nicks) {
                if (irc_stricmp(ngi->nicks[i], path) == 0)
                    continue;
                if (count > 0)
                    http_response_printf(c->res, "<br>");
                if (i == ngi->mainnick)
                    http_response_printf(c->res, "<b>");
                http_quote_html(ngi->nicks[i], buf, sizeof(buf));
                http_response_printf(c->res, "%s", buf);
                if (i == ngi->mainnick)
                    http_response_printf(c->res, "</b>");
                count++;
            }
        }

        http_response_printf(c->res, "<tr><td colspan=2><hr>");

        http_response_printf(c->res,
                   "<tr><th align=right valign=top>Channels registered:<td>");
        if (module_chanserv)
            update_owned_channels(ngi);
        if (!ngi->channels_count) {
            http_response_printf(c->res, "None");
        } else {
            int i;
            ARRAY_FOREACH (i, ngi->channels) {
                if (i > 0)
                    http_response_printf(c->res, "<br>");
                http_quote_html(ngi->channels[i], buf, sizeof(buf));
                if (module_chanserv) {
                    http_quote_url(ngi->channels[i]+1, urlbuf, sizeof(urlbuf));
                    http_response_printf(c->res, "<a href=\"../chanserv/%s\">"
                               "%s</a>", urlbuf, buf);
                } else {
                    http_response_printf(c->res, "%s", buf);
                }
            }
        }
        http_response_printf(c->res, "<tr><th align=right valign=top>Channel"
                   " registration limit:<td>");
        if (ngi->channelmax == CHANMAX_DEFAULT) {
            if (module_chanserv)
                http_response_printf(c->res, "Default (%d)", CSMaxReg);
            else
                http_response_printf(c->res, "Default");
        } else if (ngi->channelmax == CHANMAX_UNLIMITED) {
            http_response_printf(c->res, "None");
        } else {
            http_response_printf(c->res, "%d", ngi->channelmax);
        }

        http_response_printf(c->res, "<tr><td colspan=2><hr>");

        http_response_printf(c->res,
                   "<tr><th align=right valign=top>Access list:<td>");
        if (!ngi->access_count) {
            http_response_printf(c->res, "None");
        } else {
            int i;
            ARRAY_FOREACH (i, ngi->access) {
                if (i > 0)
                    http_response_printf(c->res, "<br>");
                http_quote_html(ngi->access[i], buf, sizeof(buf));
                http_response_printf(c->res, "%s", buf);
            }
        }

        http_response_printf(c->res, "</table>");
    }

    http_response_printf(c->res,
               "</div><p><a href=./>Return to nickname list</a></body>"
               "</html>");
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
    return 1;
}

/*************************************************************************/

static struct {
    int32 mask, flags;
    const char *text;
} chanopts[] = {
    { CF_KEEPTOPIC,  CF_KEEPTOPIC,  "topic retention" },
    { CF_SECUREOPS,  CF_SECUREOPS,  "secure ops"      },
    { CF_PRIVATE,    CF_PRIVATE,    "private"         },
    { CF_TOPICLOCK,  CF_TOPICLOCK,  "topic lock"      },
    { CF_RESTRICTED, CF_RESTRICTED, "restricted"      },
    { CF_LEAVEOPS,   CF_LEAVEOPS,   "leave ops"       },
    { CF_SECURE,     CF_SECURE,     "secure"          },
    { CF_OPNOTICE,   CF_OPNOTICE,   "op-notice"       },
    { CF_ENFORCE,    CF_ENFORCE,    "enforce"         },
    { 0 }
};

static int handle_chanserv(Client *c, char *path)
{
    char chanurl[CHANMAX*3];
    char chanhtml[CHANMAX*5];
    char chantmp[CHANMAX];
    char buf[BUFSIZE*5], urlbuf[BUFSIZE*3];
    int i;
    char *s;
    ChannelInfo *ci;
    NickGroupInfo *ngi;
    enum {MODE_INFO, MODE_LEVELS, MODE_ACCESS, MODE_AUTOKICK} mode = MODE_INFO;

    if (!module_chanserv)
        return 0;

    if (!*path) {
        redirect_to(c, c->url, "/");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    if (!*path) {
        int count = 0;
        char *select_var = getvar(c, "select");
        char *start_var = getvar(c, "start");
        enum {SEL_ALL, SEL_FORBIDDEN,SEL_SUSPENDED,SEL_NOEXPIRE}
            select = SEL_ALL;

        if (select_var)
            select = atoi(select_var);
        http_response_set(c->res, HTTP_S_OK, "text/html", NULL, 0);
        http_response_printf(c->res,
                   "<html><head><title>Channel database access</title>"
                   "</head><body><h1 align=center>Channel database"
                   " access</h1><p>Click to display:");
        PRINT_SELOPT(c,   " ", select, SEL_ALL, "All channels");
        PRINT_SELOPT(c, " | ", select, SEL_FORBIDDEN, "Forbidden channels");
        PRINT_SELOPT(c, " | ", select, SEL_SUSPENDED, "Suspended channels");
        PRINT_SELOPT(c, " | ", select, SEL_NOEXPIRE, "Non-expiring channels");
        http_response_printf(c->res,
                   "<br>Or click on a channel for detailed information."
                   "<p><a href=../>(Return to previous menu)</a><p><ul>");
        {
            static const char *const conds[] = {
                /* SEL_ALL */       "",
                /* SEL_FORBIDDEN */ " and t.flags & 128 <> 0",
                /* SEL_SUSPENDED */ " and t.flags & 65536 <> 0",
                /* SEL_NOEXPIRE */  " and t.flags & 512 <> 0",
            };
            char where[256];
            const char *params[1];
            ChanListArg arg;

            if (select < SEL_ALL || select > SEL_NOEXPIRE)
                select = SEL_ALL;
            snprintf(where, sizeof(where), "t.name_key > $2%s",
                     conds[select]);
            params[0] = start_var ? start_var : "";
            arg.c = c;
            arg.count = 0;
            arg.more = 0;
            arg.want_ngi = 0;
            *arg.last = 0;
            foreach_channelinfo(where, params, 1, chan_list_one, &arg);
            count = arg.count;
            if (arg.more) {
                char urlbuf[BUFSIZE*3];
                http_quote_url(arg.last, urlbuf, sizeof(urlbuf));
                http_response_printf(c->res, "</ul><p><a href=\"./?select=%d&start="
                           "%s\">Next page</a><ul>", select, urlbuf);
            }
        }
        http_response_printf(c->res,
                   "</ul><p>%d %schannel%s %s shown.<p>Key:"
                   "<tt>&nbsp;&nbsp;-&nbsp;</tt>Channel is forbidden<br>"
                   "<tt>&nbsp;&nbsp;*&nbsp;</tt>Channel is suspended<br>"
                   "<tt>&nbsp;&nbsp;!&nbsp;</tt>Channel is non-expiring<br>"
                   "</body></html>", count,
                   select==SEL_NOEXPIRE ? "non-expiring " : "",
                   count==1 ? "" : "s",
                   select==SEL_FORBIDDEN ? "forbidden" :
                       select==SEL_SUSPENDED ? "suspended" : "registered");
        return 1;
    }

    s = strchr(path, '/');
    if (s) {
        *s++ = 0;
        if (strcmp(s, "levels") == 0)
            mode = MODE_LEVELS;
        else if (strcmp(s, "access") == 0)
            mode = MODE_ACCESS;
        else if (strcmp(s, "autokick") == 0)
            mode = MODE_AUTOKICK;
        else if (*s)
            return 0;
        else {  /* ".../chanserv/channel-name/" */
            /* Note that we just modified c->url above */
            redirect_to(c, c->url, "");
            return 1;
        }
    }

    http_unquote_url(path);
    /* URL has # stripped out, so put it back */
    snprintf(chantmp, sizeof(chantmp), "#%s", path);
    ci = get_channelinfo(chantmp);
    http_quote_html(chantmp, chanhtml, sizeof(chanhtml));
    http_quote_url(chantmp+1, chanurl, sizeof(chanurl));

    http_response_set(c->res, HTTP_S_OK, "text/html", NULL, 0);

    if (!ci) {
        http_response_printf(c->res, "<p>Channel \"%s\" is not registered.",
                   chanhtml);

    } else if (ci->flags & CF_VERBOTEN) {
        http_response_printf(c->res, "<p>Channel \"%s\" is <b>forbidden</b>.",
                   chanhtml);

    } else if (mode == MODE_LEVELS) {
        LevelInfo *levelinfo;  /* from ChanServ */

        levelinfo = module_symbol(module_chanserv, "levelinfo");
        http_response_printf(c->res,
                   "<html><head><title>Access levels for channel \"%s\""
                   "</title></head><body><h1 align=center>Access levels"
                   " for channel \"%s\"</h1>", chanhtml, chanhtml);
        if (!levelinfo) {
            http_response_printf(c->res, "<p><font color=red><b>Error accessing"
                       " level data!</b></font>");
        } else {
            http_response_printf(c->res, "<div align=center><table border=1><tr>"
                       "<th>Name<th>Level<th>Description<tr><td height=2>");
            for (i = 0; levelinfo[i].what >= 0; i++) {
                char buf2[BUFSIZE*5];
                int level = ci->levels[levelinfo[i].what];
                if (level == ACCLEV_DEFAULT)
                    level = levelinfo[i].defval;
                if (!*levelinfo[i].name)  /* Empty name -> dummy level */
                    continue;
                http_quote_html(levelinfo[i].name, buf, sizeof(buf));
                http_quote_html(getstring(NULL,levelinfo[i].desc),
                                buf2, sizeof(buf2));
                if (level == ACCLEV_FOUNDER)
                    http_response_printf(c->res, "<tr><td>%s<td align=center>"
                               "(Founder only)<td>%s", buf, buf2);
                else if (level == ACCLEV_INVALID)
                    http_response_printf(c->res, "<tr><td>%s<td align=center>"
                               "(Disabled)<td>%s", buf, buf2);
                else
                    http_response_printf(c->res, "<tr><td>%s<td align=right>%d&nbsp;"
                               "<td>%s", buf, level, buf2);
            }
            http_response_printf(c->res, "</table></div>");
        }
        http_response_printf(c->res, "<p><a href=\"../%s\">Return to channel"
                   " information</a>", chanurl);

    } else if (mode == MODE_ACCESS) {
        http_response_printf(c->res,
                   "<html><head><title>Access list for channel \"%s\"</title>"
                   "</head><body><h1 align=center>Access list for channel"
                   " \"%s\"</h1>", chanhtml, chanhtml);
        ARRAY_FOREACH (i, ci->access) {
            if (ci->access[i].nickgroup)
                break;
        }
        if (i >= ci->access_count) {
            http_response_printf(c->res, "<p>Access list is empty.");
        } else {
            int count = 0;
            http_response_printf(c->res, "<div align=center>");
            http_response_printf(c->res, "<table border=1><tr>"
                       "<th>Nickname<th>Level<tr><td height=2>");
            ARRAY_FOREACH (i, ci->access) {
                if (!ci->access[i].nickgroup)
                    continue;
                http_response_printf(c->res, "<tr><td>");
                ngi = get_ngi_id(ci->access[i].nickgroup);
                if (ngi) {
                    http_quote_html(ngi_mainnick(ngi), buf, sizeof(buf));
                    http_quote_url(ngi_mainnick(ngi), urlbuf, sizeof(urlbuf));
                    http_response_printf(c->res, "<a href=\"../../nickserv/%s\">%s"
                               "</a>", urlbuf, buf);
                } else {
                    http_response_printf(c->res, "<font color=red>(Error)</font>");
                }
                http_response_printf(c->res, "<td>%d", ci->access[i].level);
                count++;
            }
            http_response_printf(c->res, "</table></div><p>%d entries.", count);
        }
        http_response_printf(c->res, "<p><a href=\"../%s\">Return to channel"
                   " information</a>", chanurl);

    } else if (mode == MODE_AUTOKICK) {
        http_response_printf(c->res,
                   "<html><head><title>Autokick list for channel \"%s\""
                   "</title></head><body><h1 align=center>Autokick list"
                   " for channel \"%s\"</h1>", chanhtml, chanhtml);
        ARRAY_FOREACH (i, ci->akick) {
            if (ci->akick[i].mask)
                break;
        }
        if (i >= ci->akick_count) {
            http_response_printf(c->res, "<p>Autokick list is empty.");
        } else {
            int count = 0;
            http_response_printf(c->res, "<div align=center><table border=1><tr>"
                       "<th>Mask<th>Set by<th>Set at<th>Last used<th>Reason"
                       "<tr><td height=2>");
            ARRAY_FOREACH (i, ci->akick) {
                if (!ci->akick[i].mask)
                    continue;
                http_quote_html(ci->akick[i].mask, buf, sizeof(buf));
                http_response_printf(c->res, "<tr><td>%s", buf);
                http_quote_html(ci->akick[i].who, buf, sizeof(buf));
                if (get_nickinfo(ci->akick[i].who)) {
                    http_quote_url(ci->akick[i].who, urlbuf, sizeof(urlbuf));
                    http_response_printf(c->res,
                               "<td><a href=\"../../nickserv/%s\">%s</a>",
                               urlbuf, buf);
                } else {
                    http_response_printf(c->res, "<td>%s", buf);
                }
                my_strftime(buf, sizeof(buf), ci->akick[i].set);
                http_response_printf(c->res, "<td>%s", buf);
                if (ci->akick[i].lastused) {
                    my_strftime(buf, sizeof(buf), ci->akick[i].lastused);
                    http_response_printf(c->res, "<td>%s", buf);
                } else {
                    http_response_printf(c->res, "<td><font color=red>Never</font>");
                }
                if (ci->akick[i].reason)
                    http_quote_html(ci->akick[i].reason, buf, sizeof(buf));
                else
                    strcpy(buf, "&nbsp;");  /* to ensure the cell is drawn */
                http_response_printf(c->res, "<td>%s", buf);
                count++;
            }
            http_response_printf(c->res, "</table></div><p>%d entries.", count);
        }
        http_response_printf(c->res, "<p><a href=\"../%s\">Return to channel"
                   " information</a>", chanurl);

    } else {
        int need_comma = 0;

        http_response_printf(c->res,
                   "<html><head><title>Information on channel \"%s\"</title>"
                   "</head><body><h1 align=center>Information on channel"
                   " \"%s\"</h1><div align=center>", chanhtml, chanhtml);
        http_response_printf(c->res, "<table border=0 cellspacing=4>");
        http_response_printf(c->res,
                   "<tr><th align=right valign=top>Founder:&nbsp;<td>");
        ngi = get_ngi_id(ci->founder);
        if (ngi) {
            http_quote_html(ngi_mainnick(ngi), buf, sizeof(buf));
            http_quote_url(ngi_mainnick(ngi), urlbuf, sizeof(urlbuf));
            http_response_printf(c->res, "<a href=\"../nickserv/%s\">%s</a>",
                       urlbuf, buf);
        } else {
            http_response_printf(c->res, "<font color=red>(Error)</font>");
        }
        http_response_printf(c->res,
                   "<tr><th align=right valign=top>Successor:&nbsp;<td>");
        if (ci->successor) {
            ngi = get_ngi_id(ci->successor);
            if (ngi) {
                http_quote_html(ngi_mainnick(ngi), buf, sizeof(buf));
                http_quote_url(ngi_mainnick(ngi), urlbuf, sizeof(urlbuf));
                http_response_printf(c->res, "<a href=\"../nickserv/%s\">%s</a>",
                           urlbuf, buf);
            } else {
                http_response_printf(c->res, "<font color=red>(Error)</font>");
            }
        } else {
            http_response_printf(c->res, "(None)");
        }
        http_quote_html(ci->desc, buf, sizeof(buf));
        http_response_printf(c->res, "<tr><th align=right valign=top>Description:"
                   "&nbsp;<td>%s", buf);
        if (ci->url) {
            http_quote_html(ci->url, buf, sizeof(buf));
            http_quote_html(ci->url, urlbuf, sizeof(urlbuf));
            http_response_printf(c->res,
                       "<tr><th align=right valign=top>URL:&nbsp;"
                       "<td><a href=\"%s\">%s</a>", urlbuf, buf);
        }
        if (ci->email) {
            http_quote_html(ci->email, buf, sizeof(buf));
            http_quote_html(ci->email, urlbuf, sizeof(urlbuf));
            http_response_printf(c->res,
                       "<tr><th align=right valign=top>E-mail address:&nbsp;"
                       "<td><a href=\"mailto:%s\">%s</a>", urlbuf, buf);
        }
        my_strftime(buf, sizeof(buf), ci->time_registered);
        http_response_printf(c->res, "<tr><th align=right valign=top>Time"
                   " registered:&nbsp;<td>%s", buf);
        my_strftime(buf, sizeof(buf), ci->last_used);
        http_response_printf(c->res, "<tr><th align=right valign=top>Last used:"
                   "&nbsp;<td>%s", buf);
        http_response_printf(c->res,
                   "<tr><th align=right valign=top>Mode lock:&nbsp;<td>");
        if (ci->mlock.on) {
            http_quote_html(mode_flags_to_string(ci->mlock.on,MODE_CHANNEL),
                            buf, sizeof(buf));
            http_response_printf(c->res, "+%s", buf);
        }
        if (ci->mlock.off) {
            http_quote_html(mode_flags_to_string(ci->mlock.off,MODE_CHANNEL),
                            buf, sizeof(buf));
            http_response_printf(c->res, "-%s", buf);
        }
        if (!ci->mlock.on && !ci->mlock.off)
            http_response_printf(c->res, "(None)");
        http_response_printf(c->res,
                   "<tr><th align=right valign=top>Options:&nbsp;<td>");
        if (ci->flags & CF_NOEXPIRE) {
            http_response_printf(c->res, "<b>Will not expire</b>");
            need_comma++;
        }
        for (i = 0; chanopts[i].mask; i++) {
            if ((ci->flags & chanopts[i].mask) == chanopts[i].flags) {
                http_quote_html(chanopts[i].text, buf, sizeof(buf));
                if (!need_comma)
                    *buf = toupper(*buf);
                http_response_printf(c->res, "%s%s", need_comma++ ? ", " : "", buf);
            }
        }
        if (!need_comma)
            http_response_printf(c->res, "None");

        http_response_printf(c->res, "<tr><td colspan=2><hr>");

        if ((ci->flags & CF_KEEPTOPIC) && ci->last_topic) {
            http_quote_html(ci->last_topic, buf, sizeof(buf));
            http_response_printf(c->res, "<tr><th align=right valign=top>Last"
                       " topic:<td>%s", buf);
            http_quote_html(ci->last_topic_setter, buf, sizeof(buf));
            http_response_printf(c->res, "<tr><th align=right valign=top>Topic"
                       " set by:<td>%s", buf);
            my_strftime(buf, sizeof(buf), ci->last_topic_time);
            http_response_printf(c->res, "<tr><th align=right valign=top>Topic"
                       " set on:&nbsp;<td>%s", buf);
            http_response_printf(c->res, "<tr><td colspan=2><hr>");
        }

        if (ci->flags & CF_SUSPENDED) {
            http_response_printf(c->res, "<tr><td colspan=2 align=center>"
                       "<font color=red>This channel is <b>suspended</b>."
                       "</font>");
            my_strftime(buf, sizeof(buf), ci->suspend_time);
            http_response_printf(c->res, "<tr><th align=right valign=top>"
                       "Suspended on:&nbsp;<td>%s", buf);
            http_quote_html(ci->suspend_who, buf, sizeof(buf));
            http_quote_url(ci->suspend_who, urlbuf, sizeof(urlbuf));
            http_response_printf(c->res, "<tr><th align=right valign=top>"
                       "Suspended by:&nbsp;<td><a href=\"%s\">%s</a>",
                       urlbuf, buf);
            http_quote_html(ci->suspend_reason ? ci->suspend_reason
                            : "", buf, sizeof(buf));
            http_response_printf(c->res, "<tr><th align=right valign=top>"
                       "Reason for suspension:&nbsp;<td>%s", buf);
            if (ci->suspend_expires)
                my_strftime(buf, sizeof(buf), ci->suspend_expires);
            else
                strbcpy(buf, "<b>Never</b>");
            http_response_printf(c->res, "<tr><th align=right valign=top>"
                       "Suspension expires on:&nbsp;<td>%s", buf);
            http_response_printf(c->res, "<tr><td colspan=2><hr>");
        }

        http_response_printf(c->res, "<tr><th colspan=2><a href=\"%s/levels\">"
                   "View access level settings</a>", chanurl);
        http_response_printf(c->res, "<tr><th colspan=2><a href=\"%s/access\">"
                   "View access list</a>", chanurl);
        http_response_printf(c->res, "<tr><th colspan=2><a href=\"%s/autokick\">"
                   "View autokick list</a>", chanurl);
        http_response_printf(c->res,
                   "</table></div><p><a href=./>Return to channel list</a>");
    }

    http_response_printf(c->res, "</body></html>");
    put_channelinfo(ci);
    return 1;
}

/*************************************************************************/

static int handle_statserv(Client *c, char *path)
{
    ServerStats *ss;
    char servurl[BUFSIZE*3];
    char servhtml[BUFSIZE*5];

    if (!module_statserv)
        return 0;

    if (!*path) {
        redirect_to(c, c->url, "/");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    http_response_set(c->res, HTTP_S_OK, "text/html", NULL, 0);

    if (!*path) {
        int count = 0;
        http_response_printf(c->res,
                   "<html><head><title>StatServ database access</title>"
                   "</head><body><h1 align=center>StatServ database"
                   " access</h1><p>Click on a server for detailed information."
                   "<p><a href=../>(Return to previous menu)</a><p><ul>");
        for (ss = first_serverstats(); ss; ss = next_serverstats()) {
            http_quote_html(ss->name, servhtml, sizeof(servhtml));
            http_quote_url(ss->name, servurl, sizeof(servurl));
            http_response_printf(c->res, "<li><a href=\"%s\">%s (<font color=%s>"
                       "%sline</font>)</a>", servurl, servhtml,
                       ss->t_join > ss->t_quit ? "green" : "red",
                       ss->t_join > ss->t_quit ? "on" : "off");
            count++;
        }
        http_response_printf(c->res, "</ul><p>%d server%s found.</body></html>",
                   count, count==1 ? "" : "s");
        return 1;
    }

    http_unquote_url(path);
    ss = get_serverstats(path);
    http_quote_html(path, servhtml, sizeof(servhtml));
    http_response_printf(c->res,
               "<html><head><title>Information on server \"%s\"</title>"
               "</head><body><h1 align=center>Information on server"
               " \"%s\"</h1><div align=center>", servhtml, servhtml);

    if (!ss) {
        http_response_printf(c->res, "<p>Server \"%s\" is not known.", servhtml);
    } else {
        http_response_printf(c->res, "<p>Server is currently <font color=%s>%sline"
                   "</font>.", ss->t_join > ss->t_quit ? "green" : "red",
                   ss->t_join > ss->t_quit ? "on" : "off");
        http_response_printf(c->res, "<table border=0 cellspacing=4>");
        if (ss->t_join > ss->t_quit) {
            http_response_printf(c->res, "<tr><th align=right valign=top>Current"
                       " user count:&nbsp;<td>%d", ss->usercnt);
            http_response_printf(c->res, "<tr><th align=right valign=top>Current"
                       " operator count:&nbsp;<td>%d", ss->opercnt);
        }
        my_strftime(servhtml, sizeof(servhtml), ss->t_join);
        http_response_printf(c->res, "<tr><th align=right valign=top>Time of last"
                   " join:&nbsp;<td>%s", ss->t_join ? servhtml : "none");
        my_strftime(servhtml, sizeof(servhtml), ss->t_quit);
        http_response_printf(c->res, "<tr><th align=right valign=top>Time of last"
                   " quit:&nbsp;<td>%s", ss->t_quit ? servhtml : "none");
        http_quote_html(ss->quit_message ? ss->quit_message : "",
                        servhtml, sizeof(servhtml));
        http_response_printf(c->res, "<tr><th align=right valign=top>Last quit"
                   " message:&nbsp;<td>%s", servhtml);
        http_response_printf(c->res, "</table>");
    }

    http_response_printf(c->res,
               "</div><p><a href=./>Return to server list</a></body></html>");
    put_serverstats(ss);
    return 1;
}

/*************************************************************************/

/* The route: every request under Prefix comes here. */
static int do_route(http_req_t id, const struct HttpRequest *req,
                    struct HttpResponse *res, void *user)
{
    Client c;

    memset(&c, 0, sizeof(c));
    c.req = req;
    c.res = res;
    strbcpy(c.url, req->hreq_path);
    if (!do_request(&c))
        http_response_error(res, HTTP_E_NOT_FOUND, NULL);
    return 1;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static ConfigDirective dbaccess_config[] = {
    { "Prefix",           { { CD_STRING, CF_DIRREQ, &Prefix } } },
    { NULL }
};

/*************************************************************************/

/* Every module this one reads from is optional: its symbols are looked
 * up while it is loaded. */

#define GET_SYMBOL(sym)  p_##sym = module_symbol(mod, #sym)

static int do_module_loaded(Module *mod, const char *modname)
{
    if (strcmp(modname, "operserv/main") == 0) {
        p_ServicesRoot = module_symbol(mod, "ServicesRoot");
        if (!p_ServicesRoot) {
            static char *dummy_ServicesRoot = "";
            p_ServicesRoot = &dummy_ServicesRoot;
        }
        GET_SYMBOL(get_operserv_data);
        GET_SYMBOL(get_maskdata);
        GET_SYMBOL(put_maskdata);
        GET_SYMBOL(first_maskdata);
        GET_SYMBOL(next_maskdata);
        if (get_operserv_data && get_maskdata && put_maskdata
         && first_maskdata && next_maskdata
        ) {
            module_operserv = mod;
        } else {
            module_log("Required symbols not found, OperServ information"
                       " will not be available");
            p_ServicesRoot = NULL;
            p_get_operserv_data = NULL;
            p_get_maskdata = NULL;
            p_put_maskdata = NULL;
            p_first_maskdata = NULL;
            p_next_maskdata = NULL;
        }
    } else if (strcmp(modname, "operserv/akill") == 0) {
        module_operserv_akill = mod;
    } else if (strcmp(modname, "operserv/news") == 0) {
        module_operserv_news = mod;
    } else if (strcmp(modname, "operserv/sessions") == 0) {
        module_operserv_sessions = mod;
    } else if (strcmp(modname, "operserv/sline") == 0) {
        module_operserv_sline = mod;
    } else if (strcmp(modname, "nickserv/main") == 0) {
        GET_SYMBOL(get_nickinfo);
        GET_SYMBOL(put_nickinfo);
        GET_SYMBOL(foreach_nickinfo);
        GET_SYMBOL(_get_ngi);
        GET_SYMBOL(_get_ngi_id);
        GET_SYMBOL(put_nickgroupinfo);
        p_get_nickinfo = module_symbol(mod, "get_nickinfo");
        p__get_ngi = module_symbol(mod, "_get_ngi");
        p__get_ngi_id = module_symbol(mod, "_get_ngi_id");
        if (p_get_nickinfo && p_put_nickinfo && p_foreach_nickinfo
         && p__get_ngi && p__get_ngi_id
         && p_put_nickgroupinfo
        ) {
            module_nickserv = mod;
        } else {
            module_log("Required symbols not found, nickname information"
                       " will not be available");
            p_get_nickinfo = NULL;
            p_put_nickinfo = NULL;
            p_foreach_nickinfo = NULL;
            p__get_ngi = NULL;
            p__get_ngi_id = NULL;
            p_put_nickgroupinfo = NULL;
        }
    } else if (strcmp(modname, "chanserv/main") == 0) {
        GET_SYMBOL(CSMaxReg);
        GET_SYMBOL(get_channelinfo);
        GET_SYMBOL(put_channelinfo);
        GET_SYMBOL(foreach_channelinfo);
        GET_SYMBOL(update_owned_channels);
        if (p_CSMaxReg && p_get_channelinfo && p_put_channelinfo
         && p_foreach_channelinfo && p_update_owned_channels
        ) {
            module_chanserv = mod;
        } else {
            module_log("Required symbols not found, channel information"
                       " will not be available");
            p_CSMaxReg = NULL;
            p_get_channelinfo = NULL;
            p_put_channelinfo = NULL;
            p_foreach_channelinfo = NULL;
            p_update_owned_channels = NULL;
        }
    } else if (strcmp(modname, "statserv/main") == 0) {
        GET_SYMBOL(get_serverstats);
        GET_SYMBOL(put_serverstats);
        GET_SYMBOL(first_serverstats);
        GET_SYMBOL(next_serverstats);
        if (p_CSMaxReg && get_serverstats && put_serverstats
         && first_serverstats && next_serverstats
        ) {
            module_statserv = mod;
        } else {
            module_log("Required symbols not found, channel information"
                       " will not be available");
            p_CSMaxReg = NULL;
            p_get_serverstats = NULL;
            p_put_serverstats = NULL;
            p_first_serverstats = NULL;
            p_next_serverstats = NULL;
        }
    }

    return 0;
}

/*************************************************************************/

static int do_module_unloaded(Module *mod)
{
    if (mod == module_operserv) {
        p_ServicesRoot = NULL;
        p_get_operserv_data = NULL;
        p_get_maskdata = NULL;
        p_put_maskdata = NULL;
        p_first_maskdata = NULL;
        p_next_maskdata = NULL;
        module_operserv = NULL;
    } else if (mod == module_operserv_akill) {
        module_operserv_akill = NULL;
    } else if (mod == module_operserv_news) {
        module_operserv_news = NULL;
    } else if (mod == module_operserv_sessions) {
        module_operserv_sessions = NULL;
    } else if (mod == module_operserv_sline) {
        module_operserv_sline = NULL;
    } else if (mod == module_nickserv) {
        p_get_nickinfo = NULL;
        p_put_nickinfo = NULL;
        p_foreach_nickinfo = NULL;
        p__get_ngi = NULL;
        p__get_ngi_id = NULL;
        p_put_nickgroupinfo = NULL;
        module_nickserv = NULL;
    } else if (mod == module_chanserv) {
        p_CSMaxReg = NULL;
        p_get_channelinfo = NULL;
        p_put_channelinfo = NULL;
        p_foreach_channelinfo = NULL;
        p_update_owned_channels = NULL;
        module_chanserv = NULL;
    } else if (mod == module_statserv) {
        p_get_serverstats = NULL;
        p_put_serverstats = NULL;
        p_first_serverstats = NULL;
        p_next_serverstats = NULL;
        module_statserv = NULL;
    }
    return 0;
}

/*************************************************************************/

/* Claim the routes for Prefix (again, if it changed). */
static int claim(void)
{
    char exact[HTTP_PATH_MAX + 1], prefix[HTTP_PATH_MAX + 2];

    Prefix_len = strlen(Prefix);
    while (Prefix_len > 0 && Prefix[Prefix_len-1] == '/')
        Prefix_len--;
    if (Prefix[0] != '/' || Prefix_len == 0 || Prefix_len + 1 > HTTP_PATH_MAX) {
        module_log("Prefix `%s' must be a path below / (e.g. /dbaccess)",
                   Prefix);
        return 0;
    }
    snprintf(exact, sizeof(exact), "%.*s", Prefix_len, Prefix);
    snprintf(prefix, sizeof(prefix), "%s/", exact);
    if (strcmp(exact, claimed_exact) == 0)
        return 1;
    if (*claimed_exact) {
        http_del_route(THIS_MODULE, "GET", claimed_exact);
        http_del_route(THIS_MODULE, "GET", claimed_prefix);
        *claimed_exact = *claimed_prefix = 0;
    }
    if (!http_add_route(THIS_MODULE, "GET", exact, do_route, NULL)
     || !http_add_route(THIS_MODULE, "GET", prefix, do_route, NULL)
    ) {
        module_log("Unable to claim %s (already claimed)", prefix);
        http_del_routes(THIS_MODULE);
        return 0;
    }
    strbcpy(claimed_exact, exact);
    strbcpy(claimed_prefix, prefix);
    return 1;
}

static void dbaccess_rehash(Module *module)
{
    claim();
}

/*************************************************************************/

static int dbaccess_init(Module *module)
{
    static const char *const readable[] = {
        "operserv/main", "operserv/akill", "operserv/news",
        "operserv/sessions", "operserv/sline", "nickserv/main",
        "chanserv/main", "statserv/main", NULL
    };
    Module *other;
    int i;

    if (!event_attach(module, EVENT_MODULE_LOADED, do_module_loaded)
     || !event_attach(module, EVENT_MODULE_UNLOADED, do_module_unloaded)
     || !claim()
    ) {
        return 0;
    }
    for (i = 0; readable[i]; i++) {
        if ((other = module_find(readable[i])) != NULL)
            do_module_loaded(other, readable[i]);
    }
    return 1;
}

/*************************************************************************/

static int dbaccess_fini(Module *module, int shutdown)
{
    http_del_routes(module);
    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "HTTP: read-only pages over the Services databases",
    .requires = MODULE_REQUIRES("httpd/main"),
    .config = dbaccess_config,
    .init = dbaccess_init,
    .fini = dbaccess_fini,
    .rehash = dbaccess_rehash,
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
