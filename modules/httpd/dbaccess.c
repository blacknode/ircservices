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
#include "modules/misc/xml.h"

#include "http.h"

/*************************************************************************/

static Module *module_httpd;
static Module *module_operserv;
static Module *module_operserv_akill;
static Module *module_operserv_news;
static Module *module_operserv_sessions;
static Module *module_operserv_sline;
static Module *module_nickserv;
static Module *module_chanserv;
static Module *module_statserv;
static Module *module_xml_export;

static char *Prefix;
int Prefix_len;


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
typeof(first_nickinfo) *p_first_nickinfo;
typeof(next_nickinfo) *p_next_nickinfo;
static typeof(_get_ngi) *p__get_ngi;
static typeof(_get_ngi_id) *p__get_ngi_id;
typeof(put_nickgroupinfo) *p_put_nickgroupinfo;
#define get_nickinfo (*p_get_nickinfo)
#define put_nickinfo (*p_put_nickinfo)
#define first_nickinfo (*p_first_nickinfo)
#define next_nickinfo (*p_next_nickinfo)
#define _get_ngi (*p__get_ngi)
#define _get_ngi_id (*p__get_ngi_id)
#define put_nickgroupinfo (*p_put_nickgroupinfo)

/* Imported from ChanServ: */
static typeof(CSMaxReg) *p_CSMaxReg;
typeof(get_channelinfo) *p_get_channelinfo;
typeof(put_channelinfo) *p_put_channelinfo;
typeof(first_channelinfo) *p_first_channelinfo;
typeof(next_channelinfo) *p_next_channelinfo;
#define CSMaxReg (*p_CSMaxReg)
#define get_channelinfo (*p_get_channelinfo)
#define put_channelinfo (*p_put_channelinfo)
#define first_channelinfo (*p_first_channelinfo)
#define next_channelinfo (*p_next_channelinfo)

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
    sockprintf((c)->socket, "%s%s%d%s%s%s", prefix,                         \
               (select)==(value) ? "<!--" : "<a href=\"./?select=", (value),\
               (select)==(value) ? "-->(" : "\">", text,                    \
               (select)==(value) ? ")" : "</a>")

/*************************************************************************/

/* Handlers for individual databases: */

static int handle_operserv(Client *c, int *close_ptr, char *path);
static int handle_operserv_akill(Client *c, int *close_ptr, char *path);
static int handle_operserv_exclude(Client *c, int *close_ptr, char *path);
static int handle_operserv_news(Client *c, int *close_ptr, char *path);
static int handle_operserv_sessions(Client *c, int*close_ptr, char *path);
static int handle_operserv_sline(Client *c, int *close_ptr, char *path);
static int handle_nickserv(Client *c, int *close_ptr, char *path);
static int handle_chanserv(Client *c, int *close_ptr, char *path);
static int handle_statserv(Client *c, int *close_ptr, char *path);
static int handle_xml_export(Client *c, int *close_ptr, char *path);

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

static int do_request(Client *c, int *close_ptr)
{
    char *subpath;

    if (strncmp(c->url, Prefix, Prefix_len) != 0)
        return 0;
    subpath = c->url + Prefix_len;
    if (!*subpath) {
        http_send_response(c, HTTP_R_FOUND);
        sockprintf(c->socket, "Location: %s/\r\n", c->url);
        sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
        return 1;
    } else if (*subpath != '/') {
        return 0;
    }
    subpath++;

    if (strncmp(subpath,"operserv",8) == 0)
        return handle_operserv(c, close_ptr, subpath+8);
    if (strncmp(subpath,"nickserv",8) == 0)
        return handle_nickserv(c, close_ptr, subpath+8);
    if (strncmp(subpath,"chanserv",8) == 0)
        return handle_chanserv(c, close_ptr, subpath+8);
    if (strncmp(subpath,"statserv",8) == 0)
        return handle_statserv(c, close_ptr, subpath+8);
    if (strncmp(subpath,"xml-export",10) == 0)
        return handle_xml_export(c, close_ptr, subpath+10);

    if (!*subpath) {
        *close_ptr = 1;  /* Don't bother to count response length */
        http_send_response(c, HTTP_S_OK);
        sockprintf(c->socket, "Content-Type: text/html\r\n");
        sockprintf(c->socket, "Connection: close\r\n\r\n");
        sockprintf(c->socket,
                   "<html><head><title>IRC Services database access</title>"
                   "</head><body><h1 align=center>IRC Services database"
                   " access</h1><p>");
        if (!module_operserv) {  /* this implies !nickserv etc. */
            sockprintf(c->socket,
                       "No service modules are currently loaded.</body>"
                       "</html>");
        } else {
            sockprintf(c->socket, "Please select one of the following:<ul>");
            sockprintf(c->socket,
                       "<li><a href=operserv/>OperServ data</a>");
            if (module_nickserv)
                sockprintf(c->socket,
                           "<li><a href=nickserv/>List of registered"
                           " nicknames</a>");
            if (module_chanserv)
                sockprintf(c->socket,
                           "<li><a href=chanserv/>List of registered"
                           " channels</a>");
            if (module_statserv)
                sockprintf(c->socket,
                           "<li><a href=statserv/>Network statistics</a>");
            if (module_xml_export)
                sockprintf(c->socket, "<li><a href=xml-export/>"
                           "XML database download</a>");
            sockprintf(c->socket, "</ul></body></html>");
        }
        return 1;
    }

    return 0;
}

/*************************************************************************/

static int handle_operserv(Client *c, int *close_ptr, char *path)
{
    if (!module_operserv)
        return 0;

    if (!*path) {
        http_send_response(c, HTTP_R_FOUND);
        sockprintf(c->socket, "Location: %s/\r\n", c->url);
        sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    if (strncmp(path,"akill",5) == 0)
        return handle_operserv_akill(c, close_ptr, path+5);
    if (strncmp(path,"exclude",7) == 0)
        return handle_operserv_exclude(c, close_ptr, path+7);
    if (strncmp(path,"news",4) == 0)
        return handle_operserv_news(c, close_ptr, path+4);
    if (strncmp(path,"sessions",6) == 0)
        return handle_operserv_sessions(c, close_ptr, path+8);
    if (strncmp(path,"sline",5) == 0)
        return handle_operserv_sline(c, close_ptr, path+5);

    if (!*path) {
        int32 maxusercnt;
        time_t maxusertime;

        *close_ptr = 1;
        http_send_response(c, HTTP_S_OK);
        sockprintf(c->socket, "Content-Type: text/html\r\n");
        sockprintf(c->socket, "Connection: close\r\n\r\n");
        sockprintf(c->socket,
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
            sockprintf(c->socket, "<li>Maximum user count: <b>%d</b>"
                   " (reached at %s)</ul>", maxusercnt, timebuf);
        }
        sockprintf(c->socket, "Please select one of the following:<ul>");
        if (module_operserv_akill
         || module_operserv_news
         || module_operserv_sessions
         || module_operserv_sline
        ) {
            if (module_operserv_akill)
                sockprintf(c->socket,
                           "<li><a href=akill/>List of autokills</a><li>"
                           "<a href=exclude/>List of autokill exclusions</a>");
            if (module_nickserv)
                sockprintf(c->socket,
                           "<li><a href=news/>List of news items</a>");
            if (module_chanserv)
                sockprintf(c->socket,
                           "<li><a href=sessions/>List of session"
                           " exceptions</a>");
            if (module_statserv)
                sockprintf(c->socket,
                           "<li><a href=sline/>List of S-lines</a>");
        }
        sockprintf(c->socket, "<li><a href=../>Return to previous menu</a>");
        sockprintf(c->socket, "</ul></body></html>");
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

static int handle_maskdata(Client *c, int *close_ptr, char *path, uint8 type,
                           const char *a_an, const char *typename)
{
    char urlbuf[BUFSIZE*3];   /* *3 because of / -> %2F */
    char htmlbuf[BUFSIZE*5];  /* *5 because of & -> &amp; */
    MaskData *md;

    if (!*path) {
        http_send_response(c, HTTP_R_FOUND);
        sockprintf(c->socket, "Location: %s/\r\n", c->url);
        sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    *close_ptr = 1;
    http_send_response(c, HTTP_S_OK);
    sockprintf(c->socket, "Content-Type: text/html\r\n");
    sockprintf(c->socket, "Connection: close\r\n\r\n");
    sockprintf(c->socket, "<html><head><title>%c%s database access</title>"
               "</head><body>", toupper(*typename), typename+1);
    if (!*path) {
        int count = 0;

        sockprintf(c->socket, "<h1 align=center>%c%s database access</h1>"
                   "<p>Click on %s %s for detailed information.<p>"
                   "<a href=../>(Return to previous menu)</a><p><ul>",
                   toupper(*typename), typename+1, a_an, typename);
        for (md = first_maskdata(type); md; md = next_maskdata(type)) {
            http_quote_html(md->mask, htmlbuf, sizeof(htmlbuf));
            http_quote_url(md->mask, urlbuf, sizeof(urlbuf));
            sockprintf(c->socket, "<li><a href=\"%s\">%s</a>",
                       urlbuf, htmlbuf);
            if (type == MD_EXCEPTION)
                sockprintf(c->socket, " (%d)", md->limit);
            count++;
        }
        sockprintf(c->socket, "</ul><p>%d %s%s.</body></html>",
                   count, typename, count==1 ? "" : "s");
        return 1;
    }

    http_unquote_url(path);
    md = get_maskdata(type, path);
    http_quote_html(path, htmlbuf, sizeof(htmlbuf));
    if (!md) {
        sockprintf(c->socket, "<h1 align=center>%c%s not found</h1>"
                   "<p>No %s was found for <b>%s</b>.<p><a href=./>Return"
                   " to %s list</a></body></html>", toupper(*typename),
                   typename+1, typename, htmlbuf, typename);
        return 1;
    }

    sockprintf(c->socket, "<h1 align=center>%c%s database access</h1>"
               "<h2 align=center>%s</h2><div align=center>",
               toupper(*typename), typename+1, htmlbuf);
    sockprintf(c->socket, "<table border=0 cellspacing=4>");
    if (type == MD_EXCEPTION) {
        sockprintf(c->socket, "<tr><th align=right valign=top>Limit:&nbsp;"
                   "<td>%d", md->limit);
    }
    sockprintf(c->socket, "<tr><th align=right valign=top>Set by:&nbsp;<td>");
    http_quote_html(md->who, htmlbuf, sizeof(htmlbuf));
    if (module_nickserv && get_nickinfo(md->who)) {
        http_quote_url(md->who, urlbuf, sizeof(urlbuf));
        sockprintf(c->socket, "<a href=\"../../nickserv/%s\">%s</a>",
                               urlbuf, htmlbuf);
    } else {
        sockprintf(c->socket, "%s", htmlbuf);
    }
    http_quote_html(md->reason ? md->reason : "", htmlbuf, sizeof(htmlbuf));
    sockprintf(c->socket, "<tr><th align=right valign=top>Reason:&nbsp;<td>%s",
               htmlbuf);
    my_strftime(htmlbuf, sizeof(htmlbuf), md->time);
    sockprintf(c->socket, "<tr><th align=right valign=top>Set on:&nbsp;<td>%s",
               htmlbuf);
    sockprintf(c->socket,
               "<tr><th align=right valign=top>Expires on:&nbsp;<td>");
    if (md->expires) {
        my_strftime(htmlbuf, sizeof(htmlbuf), md->expires);
        sockprintf(c->socket, "%s", htmlbuf);
    } else {
        sockprintf(c->socket, "<font color=green>Does not expire</font>");
    }
    sockprintf(c->socket,
               "<tr><th align=right valign=top>Last triggered:&nbsp;<td>");
    if (md->lastused) {
        my_strftime(htmlbuf, sizeof(htmlbuf), md->lastused);
        sockprintf(c->socket, "%s", htmlbuf);
    } else {
        sockprintf(c->socket, "<font color=red>Never</font>");
    }
    sockprintf(c->socket, "</table></div><p><a href=./>Return to %s list"
               "</a></body></html>", typename);
    put_maskdata(md);
    return 1;
}

/*************************************************************************/

static int handle_operserv_akill(Client *c, int *close_ptr, char *path)
{
    if (!module_operserv_akill)
        return 0;
    return handle_maskdata(c, close_ptr, path, MD_AKILL, "an", "autokill");
}

/*************************************************************************/

static int handle_operserv_exclude(Client *c, int *close_ptr, char *path)
{
    if (!module_operserv_akill)
        return 0;
    return handle_maskdata(c, close_ptr, path, MD_EXCLUDE,
                           "an", "autokill exclusion");
}

/*************************************************************************/

static int handle_operserv_news(Client *c, int *close_ptr, char *path)
{
    char htmlbuf[BUFSIZE*5];
    NewsItem *news;

    if (!module_operserv_news)
        return 0;

    if (!*path) {
        http_send_response(c, HTTP_R_FOUND);
        sockprintf(c->socket, "Location: %s/\r\n", c->url);
        sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    *close_ptr = 1;
    http_send_response(c, HTTP_S_OK);
    sockprintf(c->socket, "Content-Type: text/html\r\n");
    sockprintf(c->socket, "Connection: close\r\n\r\n");
    sockprintf(c->socket, "<html><head><title>News database access"
               "</title></head><body>");
    sockprintf(c->socket, "<h1 align=center>News database"
               " access</h1><p><a href=../>(Return to previous menu)</a>");
    sockprintf(c->socket, "<h2 align=center>Logon news</h2><p>"
               "<table border=2><tr><th>Num<th>Added by<th>Date<th>Text");
    for (news = first_news(); news; news = next_news()) {
        if (news->type != NEWS_LOGON)
            continue;
        http_quote_html(news->who, htmlbuf, sizeof(htmlbuf));
        sockprintf(c->socket, "<tr><td>%d<td>%s", news->num, htmlbuf);
        my_strftime(htmlbuf, sizeof(htmlbuf), news->time);
        sockprintf(c->socket, "<td>%s", htmlbuf);
        http_quote_html(news->text ? news->text : "",
                        htmlbuf, sizeof(htmlbuf));
        sockprintf(c->socket, "<td>%s", htmlbuf);
    }
    sockprintf(c->socket, "</table><h2 align=center>Oper news</h2><p>"
               "<table border=2><tr><th>Num<th>Added by<th>Date<th>Text");
    for (news = first_news(); news; news = next_news()) {
        if (news->type != NEWS_OPER)
            continue;
        http_quote_html(news->who, htmlbuf, sizeof(htmlbuf));
        sockprintf(c->socket, "<tr><td>%d<td>%s", news->num, htmlbuf);
        my_strftime(htmlbuf, sizeof(htmlbuf), news->time);
        sockprintf(c->socket, "<td>%s", htmlbuf);
        http_quote_html(news->text ? news->text : "",
                        htmlbuf, sizeof(htmlbuf));
        sockprintf(c->socket, "<td>%s", htmlbuf);
    }
    sockprintf(c->socket, "</table></body></html>");
    return 1;
}

/*************************************************************************/

static int handle_operserv_sessions(Client *c, int *close_ptr, char *path)
{
    if (!module_operserv_sessions)
        return 0;
    return handle_maskdata(c, close_ptr, path, MD_EXCEPTION,
                           "a", "session exception");
}

/*************************************************************************/

static int handle_operserv_sline(Client *c, int *close_ptr, char *path)
{
    char typename[7] = "S.line";

    if (!module_operserv_sline)
        return 0;

    if (!*path) {
        http_send_response(c, HTTP_R_FOUND);
        sockprintf(c->socket, "Location: %s/\r\n", c->url);
        sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    if (!*path) {
        *close_ptr = 1;
        http_send_response(c, HTTP_S_OK);
        sockprintf(c->socket, "Content-Type: text/html\r\n");
        sockprintf(c->socket, "Connection: close\r\n\r\n");
        sockprintf(c->socket, "<html><head><title>S-line database access"
                   "</title></head><body>");
        sockprintf(c->socket, "<p>Please select one of the following:<ul>"
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
    return handle_maskdata(c, close_ptr, path+1, *path, "an", typename);
}

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

static int handle_nickserv(Client *c, int *close_ptr, char *path)
{
    char nickurl[NICKMAX*3];
    char nickhtml[NICKMAX*5];
    NickInfo *ni;
    NickGroupInfo *ngi = NULL;

    if (!module_nickserv)
        return 0;

    if (!*path) {
        http_send_response(c, HTTP_R_FOUND);
        sockprintf(c->socket, "Location: %s/\r\n", c->url);
        sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    *close_ptr = 1;
    http_send_response(c, HTTP_S_OK);
    sockprintf(c->socket, "Content-Type: text/html\r\n");
    sockprintf(c->socket, "Connection: close\r\n\r\n");

    if (!*path) {
        int count = 0;
        char *select_var = http_get_variable(c, "select");
        enum {SEL_ALL, SEL_FORBIDDEN,SEL_SUSPENDED,SEL_NOEXPIRE,SEL_NOAUTH}
            select = SEL_ALL;

        if (select_var)
            select = atoi(select_var);
        sockprintf(c->socket,
                   "<html><head><title>Nickname database access</title>"
                   "</head><body><h1 align=center>Nickname database"
                   " access</h1><p>Click to display:");
        PRINT_SELOPT(c,   " ", select, SEL_ALL, "All nicknames");
        PRINT_SELOPT(c, " | ", select, SEL_FORBIDDEN, "Forbidden nicknames");
        PRINT_SELOPT(c, " | ", select, SEL_SUSPENDED, "Suspended nicknames");
        PRINT_SELOPT(c, " | ", select, SEL_NOEXPIRE, "Non-expiring nicknames");
        PRINT_SELOPT(c, " | ", select, SEL_NOAUTH,
                     "Not-yet-authenticated nicknames");
        sockprintf(c->socket,
                   "<br>Or click on a nickname for detailed information."
                   "<p><a href=../>(Return to previous menu)</a><p><ul>");
        for (ni = first_nickinfo(); ni; ni = next_nickinfo()) {
            NickGroupInfo *ngi = NULL;
            if ((select==SEL_SUSPENDED || select==SEL_NOAUTH) && ni->nickgroup)
                ngi = get_ngi(ni);
            if ((select==SEL_FORBIDDEN && !(ni->status & NS_VERBOTEN))
             || (select==SEL_SUSPENDED && (!ngi || !(ngi->flags&NF_SUSPENDED)))
             || (select==SEL_NOEXPIRE  && !(ni->status & NS_NOEXPIRE))
             || (select==SEL_NOAUTH    && (!ngi || !ngi_unauthed(ngi)))
            ) {
                put_nickgroupinfo(ngi);
                continue;
            }
            http_quote_html(ni->nick, nickhtml, sizeof(nickhtml));
            http_quote_html(ni->nick, nickurl, sizeof(nickurl));
            sockprintf(c->socket, "<li><tt>%s%s%s%s&nbsp;</tt>"
                       "<a href=\"%s\">%s</a>",
                       ni->status & NS_VERBOTEN ? "-" : "&nbsp;",
                       ngi && (ngi->flags & NF_SUSPENDED) ? "*" : "&nbsp;",
                       ni->status & NS_NOEXPIRE ? "!" : "&nbsp;",
                       ngi && ngi->authcode ? "?" : "&nbsp;",
                       nickurl, nickhtml);
            put_nickgroupinfo(ngi);
            count++;
        }
        sockprintf(c->socket,
                   "</ul><p>%d %snickname%s %s.<p>Key:<br>"
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
    sockprintf(c->socket,
               "<html><head><title>Information on nickname \"%s\"</title>"
               "</head><body><h1 align=center>Information on nickname"
               " \"%s\"</h1><div align=center>", nickhtml, nickhtml);

    if (!ni) {
        sockprintf(c->socket, "<p>Nickname \"%s\" is not registered.",
                   nickhtml);
    } else if (ni->status & NS_VERBOTEN) {
        sockprintf(c->socket, "<p>Nickname \"%s\" is <b>forbidden</b>.",
                   nickhtml);
    } else if (!(ngi = get_ngi(ni))) {
        sockprintf(c->socket,
                   "<p>Error retrieving information for nickname \"%s\".",
                   nickhtml);
    } else {
        char buf[BUFSIZE*5], urlbuf[BUFSIZE*3];
        int need_comma = 0, i;

        sockprintf(c->socket, "<table border=0 cellspacing=4>");
        http_quote_html(ni->last_realname ? ni->last_realname : "", buf,
                        sizeof(buf));
        sockprintf(c->socket, "<tr><th align=right valign=top>Registered"
                   " to:&nbsp;<td>%s", buf);
        my_strftime(buf, sizeof(buf), ni->time_registered);
        sockprintf(c->socket, "<tr><th align=right valign=top>Time"
                   " registered:&nbsp;<td>%s", buf);
        http_quote_html(ni->last_realmask ? ni->last_realmask : "", buf,
                        sizeof(buf));
        if (get_user(ni->nick)) {
            sockprintf(c->socket,
                       "<tr><th align=right valign=top><font color=green>Is"
                       " online from:</font>&nbsp;<td>%s", buf);
            sockprintf(c->socket,
                       "<tr><th align=right valign=top>Authorization"
                       " status:&nbsp;<td>%s",
                       nick_identified(ni) ? "Identified" :
                       nick_recognized(ni) ? "Recognized (via access list)" :
                       "Not recognized");
        } else {
            sockprintf(c->socket, "<tr><th align=right valign=top>Last seen"
                       " address:&nbsp;<td>%s", buf);
            my_strftime(buf, sizeof(buf), ni->last_seen);
            sockprintf(c->socket, "<tr><th align=right valign=top>Last seen"
                       " on:&nbsp;<td>%s", buf);
        }
        if (ni->last_quit) {
            http_quote_html(ni->last_quit, buf, sizeof(buf));
            sockprintf(c->socket, "<tr><th align=right valign=top>Last quit"
                       " message:&nbsp;<td>%s", buf);
        }

        sockprintf(c->socket, "<tr><td colspan=2><hr>");

        if (ngi->info) {
            http_quote_html(ngi->info, buf, sizeof(buf));
            sockprintf(c->socket, "<tr><th align=right valign=top>"
                       "Information:&nbsp;<td>%s", buf);
        }
        if (ngi->url) {
            http_quote_html(ngi->url, buf, sizeof(buf));
            http_quote_html(ngi->url, urlbuf, sizeof(urlbuf));
            sockprintf(c->socket,
                       "<tr><th align=right valign=top>URL:&nbsp;"
                       "<td><a href=\"%s\">%s</a>", urlbuf, buf);
        }
        if (ngi->email) {
            http_quote_html(ngi->email, buf, sizeof(buf));
            http_quote_html(ngi->email, urlbuf, sizeof(urlbuf));
            sockprintf(c->socket,
                       "<tr><th align=right valign=top>E-mail address:&nbsp;"
                       "<td><a href=\"mailto:%s\">%s</a>", urlbuf, buf);
        }
        sockprintf(c->socket,
                   "<tr><th align=right valign=top>Options:&nbsp;<td>");
        if (ni->status & NS_NOEXPIRE) {
            sockprintf(c->socket, "<b>Will not expire</b>");
            need_comma++;
        }
        for (i = 0; nickopts[i].mask; i++) {
            if ((ngi->flags & nickopts[i].mask) == nickopts[i].flags) {
                http_quote_html(nickopts[i].text, buf, sizeof(buf));
                if (!need_comma)
                    *buf = toupper(*buf);
                sockprintf(c->socket, "%s%s", need_comma++ ? ", " : "", buf);
            }
        }
        if (!need_comma)
            sockprintf(c->socket, "None");
        sockprintf(c->socket, "<tr><th align=right valign=top>OperServ"
                   " privilege level:");
        if (irc_stricmp(ni->nick, ServicesRoot) == 0)
            sockprintf(c->socket, "<td>Services super-user");
        else if (ngi->os_priv >= NP_SERVADMIN)
            sockprintf(c->socket, "<td>Services administrator");
        else if (ngi->os_priv >= NP_SERVOPER)
            sockprintf(c->socket, "<td>Services operator");
        else 
            sockprintf(c->socket, "<td>None");

        sockprintf(c->socket, "<tr><td colspan=2><hr>");

        if (ngi->authcode) {
            sockprintf(c->socket, "<tr><td colspan=2 align=center>"
                       "<font color=red>This nickname's E-mail address has"
                       " not yet been authenticated.</font>");
            sockprintf(c->socket, "<tr><th align=right>Authenticatation code:"
                       "&nbsp;<td>%d", ngi->authcode);
            my_strftime(buf, sizeof(buf), ngi->authset);
            sockprintf(c->socket, "<tr><th align=right>Code set at:&nbsp;"
                       "<td>%s", buf);
            sockprintf(c->socket, "<tr><td colspan=2><hr>");
        }

        if (ngi->flags & NF_SUSPENDED) {
            sockprintf(c->socket, "<tr><td colspan=2 align=center>"
                       "<font color=red>This nickname group is"
                       " <b>suspended</b>.</font>");
            my_strftime(buf, sizeof(buf), ngi->suspend_time);
            sockprintf(c->socket, "<tr><th align=right valign=top>"
                       "Suspended on:&nbsp;<td>%s", buf);
            http_quote_html(ngi->suspend_who, buf, sizeof(buf));
            http_quote_url(ngi->suspend_who, urlbuf, sizeof(urlbuf));
            sockprintf(c->socket, "<tr><th align=right valign=top>"
                       "Suspended by:&nbsp;<td><a href=\"%s\">%s</a>",
                       urlbuf, buf);
            http_quote_html(ngi->suspend_reason ? ngi->suspend_reason
                            : "", buf, sizeof(buf));
            sockprintf(c->socket, "<tr><th align=right valign=top>"
                       "Reason for suspension:&nbsp;<td>%s", buf);
            if (ngi->suspend_expires)
                my_strftime(buf, sizeof(buf), ngi->suspend_expires);
            else
                strbcpy(buf, "<b>Never</b>");
            sockprintf(c->socket, "<tr><th align=right valign=top>"
                       "Suspension expires on:&nbsp;<td>%s", buf);
            sockprintf(c->socket, "<tr><td colspan=2><hr>");
        }

        sockprintf(c->socket,
                   "<tr><th align=right valign=top>Linked nicks:<td>");
        if (ngi->nicks_count == 1) {
            sockprintf(c->socket, "-");
        } else {
            int count = 0;
            ARRAY_FOREACH (i, ngi->nicks) {
                if (irc_stricmp(ngi->nicks[i], path) == 0)
                    continue;
                if (count > 0)
                    sockprintf(c->socket, "<br>");
                if (i == ngi->mainnick)
                    sockprintf(c->socket, "<b>");
                http_quote_html(ngi->nicks[i], buf, sizeof(buf));
                sockprintf(c->socket, "%s", buf);
                if (i == ngi->mainnick)
                    sockprintf(c->socket, "</b>");
                count++;
            }
        }

        sockprintf(c->socket, "<tr><td colspan=2><hr>");

        sockprintf(c->socket,
                   "<tr><th align=right valign=top>Channels registered:<td>");
        if (!ngi->channels_count) {
            sockprintf(c->socket, "None");
        } else {
            int i;
            ARRAY_FOREACH (i, ngi->channels) {
                if (i > 0)
                    sockprintf(c->socket, "<br>");
                http_quote_html(ngi->channels[i], buf, sizeof(buf));
                if (module_chanserv) {
                    http_quote_url(ngi->channels[i]+1, urlbuf, sizeof(urlbuf));
                    sockprintf(c->socket, "<a href=\"../chanserv/%s\">"
                               "%s</a>", urlbuf, buf);
                } else {
                    sockprintf(c->socket, "%s", buf);
                }
            }
        }
        sockprintf(c->socket, "<tr><th align=right valign=top>Channel"
                   " registration limit:<td>");
        if (ngi->channelmax == CHANMAX_DEFAULT) {
            if (module_chanserv)
                sockprintf(c->socket, "Default (%d)", CSMaxReg);
            else
                sockprintf(c->socket, "Default");
        } else if (ngi->channelmax == CHANMAX_UNLIMITED) {
            sockprintf(c->socket, "None");
        } else {
            sockprintf(c->socket, "%d", ngi->channelmax);
        }

        sockprintf(c->socket, "<tr><td colspan=2><hr>");

        sockprintf(c->socket,
                   "<tr><th align=right valign=top>Access list:<td>");
        if (!ngi->access_count) {
            sockprintf(c->socket, "None");
        } else {
            int i;
            ARRAY_FOREACH (i, ngi->access) {
                if (i > 0)
                    sockprintf(c->socket, "<br>");
                http_quote_html(ngi->access[i], buf, sizeof(buf));
                sockprintf(c->socket, "%s", buf);
            }
        }

        sockprintf(c->socket, "</table>");
    }

    sockprintf(c->socket,
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

static int handle_chanserv(Client *c, int *close_ptr, char *path)
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
        http_send_response(c, HTTP_R_FOUND);
        sockprintf(c->socket, "Location: %s/\r\n", c->url);
        sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    if (!*path) {
        int count = 0;
        char *select_var = http_get_variable(c, "select");
        enum {SEL_ALL, SEL_FORBIDDEN,SEL_SUSPENDED,SEL_NOEXPIRE}
            select = SEL_ALL;

        if (select_var)
            select = atoi(select_var);
        *close_ptr = 1;
        http_send_response(c, HTTP_S_OK);
        sockprintf(c->socket, "Content-Type: text/html\r\n");
        sockprintf(c->socket, "Connection: close\r\n\r\n");
        sockprintf(c->socket,
                   "<html><head><title>Channel database access</title>"
                   "</head><body><h1 align=center>Channel database"
                   " access</h1><p>Click to display:");
        PRINT_SELOPT(c,   " ", select, SEL_ALL, "All channels");
        PRINT_SELOPT(c, " | ", select, SEL_FORBIDDEN, "Forbidden channels");
        PRINT_SELOPT(c, " | ", select, SEL_SUSPENDED, "Suspended channels");
        PRINT_SELOPT(c, " | ", select, SEL_NOEXPIRE, "Non-expiring channels");
        sockprintf(c->socket,
                   "<br>Or click on a channel for detailed information."
                   "<p><a href=../>(Return to previous menu)</a><p><ul>");
        for (ci = first_channelinfo(); ci; ci = next_channelinfo()) {
            if ((select==SEL_FORBIDDEN && !(ci->flags & CF_VERBOTEN))
             || (select==SEL_SUSPENDED && !(ci->flags & CF_SUSPENDED))
             || (select==SEL_NOEXPIRE  && !(ci->flags & CF_NOEXPIRE))
            ) {
                continue;
            }
            http_quote_html(ci->name, chanhtml, sizeof(chanhtml));
            http_quote_html(ci->name+1, chanurl, sizeof(chanurl));
            sockprintf(c->socket, "<li><tt>%s%s%s&nbsp;</tt>"
                       "<a href=\"%s\">%s</a>",
                       ci->flags & CF_VERBOTEN  ? "-" : "&nbsp;",
                       ci->flags & CF_SUSPENDED ? "*" : "&nbsp;",
                       ci->flags & CF_NOEXPIRE  ? "!" : "&nbsp;",
                       chanurl, chanhtml);
            count++;
        }
        sockprintf(c->socket,
                   "</ul><p>%d %schannel%s %s.<p>Key:"
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
            http_send_response(c, HTTP_R_FOUND);
            /* Note that we just modified c->url above */
            sockprintf(c->socket, "Location: %s\r\n", c->url);
            sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
            return 1;
        }
    }

    http_unquote_url(path);
    /* URL has # stripped out, so put it back */
    snprintf(chantmp, sizeof(chantmp), "#%s", path);
    ci = get_channelinfo(chantmp);
    http_quote_html(chantmp, chanhtml, sizeof(chanhtml));
    http_quote_url(chantmp+1, chanurl, sizeof(chanurl));

    *close_ptr = 1;
    http_send_response(c, HTTP_S_OK);
    sockprintf(c->socket, "Content-Type: text/html\r\n");
    sockprintf(c->socket, "Connection: close\r\n\r\n");

    if (!ci) {
        sockprintf(c->socket, "<p>Channel \"%s\" is not registered.",
                   chanhtml);

    } else if (ci->flags & CF_VERBOTEN) {
        sockprintf(c->socket, "<p>Channel \"%s\" is <b>forbidden</b>.",
                   chanhtml);

    } else if (mode == MODE_LEVELS) {
        LevelInfo *levelinfo;  /* from ChanServ */

        levelinfo = get_module_symbol(module_chanserv, "levelinfo");
        sockprintf(c->socket,
                   "<html><head><title>Access levels for channel \"%s\""
                   "</title></head><body><h1 align=center>Access levels"
                   " for channel \"%s\"</h1>", chanhtml, chanhtml);
        if (!levelinfo) {
            sockprintf(c->socket, "<p><font color=red><b>Error accessing"
                       " level data!</b></font>");
        } else {
            sockprintf(c->socket, "<div align=center><table border=1><tr>"
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
                    sockprintf(c->socket, "<tr><td>%s<td align=center>"
                               "(Founder only)<td>%s", buf, buf2);
                else if (level == ACCLEV_INVALID)
                    sockprintf(c->socket, "<tr><td>%s<td align=center>"
                               "(Disabled)<td>%s", buf, buf2);
                else
                    sockprintf(c->socket, "<tr><td>%s<td align=right>%d&nbsp;"
                               "<td>%s", buf, level, buf2);
            }
            sockprintf(c->socket, "</table></div>");
        }
        sockprintf(c->socket, "<p><a href=\"../%s\">Return to channel"
                   " information</a>", chanurl);

    } else if (mode == MODE_ACCESS) {
        sockprintf(c->socket,
                   "<html><head><title>Access list for channel \"%s\"</title>"
                   "</head><body><h1 align=center>Access list for channel"
                   " \"%s\"</h1>", chanhtml, chanhtml);
        ARRAY_FOREACH (i, ci->access) {
            if (ci->access[i].nickgroup)
                break;
        }
        if (i >= ci->access_count) {
            sockprintf(c->socket, "<p>Access list is empty.");
        } else {
            int count = 0;
            sockprintf(c->socket, "<div align=center>");
            sockprintf(c->socket, "<table border=1><tr>"
                       "<th>Nickname<th>Level<tr><td height=2>");
            ARRAY_FOREACH (i, ci->access) {
                if (!ci->access[i].nickgroup)
                    continue;
                sockprintf(c->socket, "<tr><td>");
                ngi = get_ngi_id(ci->access[i].nickgroup);
                if (ngi) {
                    http_quote_html(ngi_mainnick(ngi), buf, sizeof(buf));
                    http_quote_url(ngi_mainnick(ngi), urlbuf, sizeof(urlbuf));
                    sockprintf(c->socket, "<a href=\"../../nickserv/%s\">%s"
                               "</a>", urlbuf, buf);
                } else {
                    sockprintf(c->socket, "<font color=red>(Error)</font>");
                }
                sockprintf(c->socket, "<td>%d", ci->access[i].level);
                count++;
            }
            sockprintf(c->socket, "</table></div><p>%d entries.", count);
        }
        sockprintf(c->socket, "<p><a href=\"../%s\">Return to channel"
                   " information</a>", chanurl);

    } else if (mode == MODE_AUTOKICK) {
        sockprintf(c->socket,
                   "<html><head><title>Autokick list for channel \"%s\""
                   "</title></head><body><h1 align=center>Autokick list"
                   " for channel \"%s\"</h1>", chanhtml, chanhtml);
        ARRAY_FOREACH (i, ci->akick) {
            if (ci->akick[i].mask)
                break;
        }
        if (i >= ci->akick_count) {
            sockprintf(c->socket, "<p>Autokick list is empty.");
        } else {
            int count = 0;
            sockprintf(c->socket, "<div align=center><table border=1><tr>"
                       "<th>Mask<th>Set by<th>Set at<th>Last used<th>Reason"
                       "<tr><td height=2>");
            ARRAY_FOREACH (i, ci->akick) {
                if (!ci->akick[i].mask)
                    continue;
                http_quote_html(ci->akick[i].mask, buf, sizeof(buf));
                sockprintf(c->socket, "<tr><td>%s", buf);
                http_quote_html(ci->akick[i].who, buf, sizeof(buf));
                if (get_nickinfo(ci->akick[i].who)) {
                    http_quote_url(ci->akick[i].who, urlbuf, sizeof(urlbuf));
                    sockprintf(c->socket,
                               "<td><a href=\"../../nickserv/%s\">%s</a>",
                               urlbuf, buf);
                } else {
                    sockprintf(c->socket, "<td>%s", buf);
                }
                my_strftime(buf, sizeof(buf), ci->akick[i].set);
                sockprintf(c->socket, "<td>%s", buf);
                if (ci->akick[i].lastused) {
                    my_strftime(buf, sizeof(buf), ci->akick[i].lastused);
                    sockprintf(c->socket, "<td>%s", buf);
                } else {
                    sockprintf(c->socket, "<td><font color=red>Never</font>");
                }
                if (ci->akick[i].reason)
                    http_quote_html(ci->akick[i].reason, buf, sizeof(buf));
                else
                    strcpy(buf, "&nbsp;");  /* to ensure the cell is drawn */
                sockprintf(c->socket, "<td>%s", buf);
                count++;
            }
            sockprintf(c->socket, "</table></div><p>%d entries.", count);
        }
        sockprintf(c->socket, "<p><a href=\"../%s\">Return to channel"
                   " information</a>", chanurl);

    } else {
        int need_comma = 0;

        sockprintf(c->socket,
                   "<html><head><title>Information on channel \"%s\"</title>"
                   "</head><body><h1 align=center>Information on channel"
                   " \"%s\"</h1><div align=center>", chanhtml, chanhtml);
        sockprintf(c->socket, "<table border=0 cellspacing=4>");
        sockprintf(c->socket,
                   "<tr><th align=right valign=top>Founder:&nbsp;<td>");
        ngi = get_ngi_id(ci->founder);
        if (ngi) {
            http_quote_html(ngi_mainnick(ngi), buf, sizeof(buf));
            http_quote_url(ngi_mainnick(ngi), urlbuf, sizeof(urlbuf));
            sockprintf(c->socket, "<a href=\"../nickserv/%s\">%s</a>",
                       urlbuf, buf);
        } else {
            sockprintf(c->socket, "<font color=red>(Error)</font>");
        }
        sockprintf(c->socket,
                   "<tr><th align=right valign=top>Successor:&nbsp;<td>");
        if (ci->successor) {
            ngi = get_ngi_id(ci->successor);
            if (ngi) {
                http_quote_html(ngi_mainnick(ngi), buf, sizeof(buf));
                http_quote_url(ngi_mainnick(ngi), urlbuf, sizeof(urlbuf));
                sockprintf(c->socket, "<a href=\"../nickserv/%s\">%s</a>",
                           urlbuf, buf);
            } else {
                sockprintf(c->socket, "<font color=red>(Error)</font>");
            }
        } else {
            sockprintf(c->socket, "(None)");
        }
        http_quote_html(ci->desc, buf, sizeof(buf));
        sockprintf(c->socket, "<tr><th align=right valign=top>Description:"
                   "&nbsp;<td>%s", buf);
        if (ci->url) {
            http_quote_html(ci->url, buf, sizeof(buf));
            http_quote_html(ci->url, urlbuf, sizeof(urlbuf));
            sockprintf(c->socket,
                       "<tr><th align=right valign=top>URL:&nbsp;"
                       "<td><a href=\"%s\">%s</a>", urlbuf, buf);
        }
        if (ci->email) {
            http_quote_html(ci->email, buf, sizeof(buf));
            http_quote_html(ci->email, urlbuf, sizeof(urlbuf));
            sockprintf(c->socket,
                       "<tr><th align=right valign=top>E-mail address:&nbsp;"
                       "<td><a href=\"mailto:%s\">%s</a>", urlbuf, buf);
        }
        my_strftime(buf, sizeof(buf), ci->time_registered);
        sockprintf(c->socket, "<tr><th align=right valign=top>Time"
                   " registered:&nbsp;<td>%s", buf);
        my_strftime(buf, sizeof(buf), ci->last_used);
        sockprintf(c->socket, "<tr><th align=right valign=top>Last used:"
                   "&nbsp;<td>%s", buf);
        sockprintf(c->socket,
                   "<tr><th align=right valign=top>Mode lock:&nbsp;<td>");
        if (ci->mlock.on) {
            http_quote_html(mode_flags_to_string(ci->mlock.on,MODE_CHANNEL),
                            buf, sizeof(buf));
            sockprintf(c->socket, "+%s", buf);
        }
        if (ci->mlock.off) {
            http_quote_html(mode_flags_to_string(ci->mlock.off,MODE_CHANNEL),
                            buf, sizeof(buf));
            sockprintf(c->socket, "-%s", buf);
        }
        if (!ci->mlock.on && !ci->mlock.off)
            sockprintf(c->socket, "(None)");
        sockprintf(c->socket,
                   "<tr><th align=right valign=top>Options:&nbsp;<td>");
        if (ci->flags & CF_NOEXPIRE) {
            sockprintf(c->socket, "<b>Will not expire</b>");
            need_comma++;
        }
        for (i = 0; chanopts[i].mask; i++) {
            if ((ci->flags & chanopts[i].mask) == chanopts[i].flags) {
                http_quote_html(chanopts[i].text, buf, sizeof(buf));
                if (!need_comma)
                    *buf = toupper(*buf);
                sockprintf(c->socket, "%s%s", need_comma++ ? ", " : "", buf);
            }
        }
        if (!need_comma)
            sockprintf(c->socket, "None");

        sockprintf(c->socket, "<tr><td colspan=2><hr>");

        if ((ci->flags & CF_KEEPTOPIC) && ci->last_topic) {
            http_quote_html(ci->last_topic, buf, sizeof(buf));
            sockprintf(c->socket, "<tr><th align=right valign=top>Last"
                       " topic:<td>%s", buf);
            http_quote_html(ci->last_topic_setter, buf, sizeof(buf));
            sockprintf(c->socket, "<tr><th align=right valign=top>Topic"
                       " set by:<td>%s", buf);
            my_strftime(buf, sizeof(buf), ci->last_topic_time);
            sockprintf(c->socket, "<tr><th align=right valign=top>Topic"
                       " set on:&nbsp;<td>%s", buf);
            sockprintf(c->socket, "<tr><td colspan=2><hr>");
        }

        if (ci->flags & CF_SUSPENDED) {
            sockprintf(c->socket, "<tr><td colspan=2 align=center>"
                       "<font color=red>This channel is <b>suspended</b>."
                       "</font>");
            my_strftime(buf, sizeof(buf), ci->suspend_time);
            sockprintf(c->socket, "<tr><th align=right valign=top>"
                       "Suspended on:&nbsp;<td>%s", buf);
            http_quote_html(ci->suspend_who, buf, sizeof(buf));
            http_quote_url(ci->suspend_who, urlbuf, sizeof(urlbuf));
            sockprintf(c->socket, "<tr><th align=right valign=top>"
                       "Suspended by:&nbsp;<td><a href=\"%s\">%s</a>",
                       urlbuf, buf);
            http_quote_html(ci->suspend_reason ? ci->suspend_reason
                            : "", buf, sizeof(buf));
            sockprintf(c->socket, "<tr><th align=right valign=top>"
                       "Reason for suspension:&nbsp;<td>%s", buf);
            if (ci->suspend_expires)
                my_strftime(buf, sizeof(buf), ci->suspend_expires);
            else
                strbcpy(buf, "<b>Never</b>");
            sockprintf(c->socket, "<tr><th align=right valign=top>"
                       "Suspension expires on:&nbsp;<td>%s", buf);
            sockprintf(c->socket, "<tr><td colspan=2><hr>");
        }

        sockprintf(c->socket, "<tr><th colspan=2><a href=\"%s/levels\">"
                   "View access level settings</a>", chanurl);
        sockprintf(c->socket, "<tr><th colspan=2><a href=\"%s/access\">"
                   "View access list</a>", chanurl);
        sockprintf(c->socket, "<tr><th colspan=2><a href=\"%s/autokick\">"
                   "View autokick list</a>", chanurl);
        sockprintf(c->socket,
                   "</table></div><p><a href=./>Return to channel list</a>");
    }

    sockprintf(c->socket, "</body></html>");
    put_channelinfo(ci);
    return 1;
}

/*************************************************************************/

static int handle_statserv(Client *c, int *close_ptr, char *path)
{
    ServerStats *ss;
    char servurl[BUFSIZE*3];
    char servhtml[BUFSIZE*5];

    if (!module_statserv)
        return 0;

    if (!*path) {
        http_send_response(c, HTTP_R_FOUND);
        sockprintf(c->socket, "Location: %s/\r\n", c->url);
        sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    *close_ptr = 1;
    http_send_response(c, HTTP_S_OK);
    sockprintf(c->socket, "Content-Type: text/html\r\n");
    sockprintf(c->socket, "Connection: close\r\n\r\n");

    if (!*path) {
        int count = 0;
        sockprintf(c->socket,
                   "<html><head><title>StatServ database access</title>"
                   "</head><body><h1 align=center>StatServ database"
                   " access</h1><p>Click on a server for detailed information."
                   "<p><a href=../>(Return to previous menu)</a><p><ul>");
        for (ss = first_serverstats(); ss; ss = next_serverstats()) {
            http_quote_html(ss->name, servhtml, sizeof(servhtml));
            http_quote_url(ss->name, servurl, sizeof(servurl));
            sockprintf(c->socket, "<li><a href=\"%s\">%s (<font color=%s>"
                       "%sline</font>)</a>", servurl, servhtml,
                       ss->t_join > ss->t_quit ? "green" : "red",
                       ss->t_join > ss->t_quit ? "on" : "off");
            count++;
        }
        sockprintf(c->socket, "</ul><p>%d server%s found.</body></html>",
                   count, count==1 ? "" : "s");
        return 1;
    }

    http_unquote_url(path);
    ss = get_serverstats(path);
    http_quote_html(path, servhtml, sizeof(servhtml));
    sockprintf(c->socket,
               "<html><head><title>Information on server \"%s\"</title>"
               "</head><body><h1 align=center>Information on server"
               " \"%s\"</h1><div align=center>", servhtml, servhtml);

    if (!ss) {
        sockprintf(c->socket, "<p>Server \"%s\" is not known.", servhtml);
    } else {
        sockprintf(c->socket, "<p>Server is currently <font color=%s>%sline"
                   "</font>.", ss->t_join > ss->t_quit ? "green" : "red",
                   ss->t_join > ss->t_quit ? "on" : "off");
        sockprintf(c->socket, "<table border=0 cellspacing=4>");
        if (ss->t_join > ss->t_quit) {
            sockprintf(c->socket, "<tr><th align=right valign=top>Current"
                       " user count:&nbsp;<td>%d", ss->usercnt);
            sockprintf(c->socket, "<tr><th align=right valign=top>Current"
                       " operator count:&nbsp;<td>%d", ss->opercnt);
        }
        my_strftime(servhtml, sizeof(servhtml), ss->t_join);
        sockprintf(c->socket, "<tr><th align=right valign=top>Time of last"
                   " join:&nbsp;<td>%s", ss->t_join ? servhtml : "none");
        my_strftime(servhtml, sizeof(servhtml), ss->t_quit);
        sockprintf(c->socket, "<tr><th align=right valign=top>Time of last"
                   " quit:&nbsp;<td>%s", ss->t_quit ? servhtml : "none");
        http_quote_html(ss->quit_message ? ss->quit_message : "",
                        servhtml, sizeof(servhtml));
        sockprintf(c->socket, "<tr><th align=right valign=top>Last quit"
                   " message:&nbsp;<td>%s", servhtml);
        sockprintf(c->socket, "</table>");
    }

    sockprintf(c->socket,
               "</div><p><a href=./>Return to server list</a></body></html>");
    put_serverstats(ss);
    return 1;
}

/*************************************************************************/

static int handle_xml_export(Client *c, int *close_ptr, char *path)
{
    void (*p_xml_export)(xml_writefunc_t writefunc, void *data);

    if (!module_xml_export)
        return 0;
    if (!(p_xml_export = get_module_symbol(module_xml_export, "xml_export"))) {
        module_xml_export = NULL;
        return 0;
    }

    if (!*path) {
        http_send_response(c, HTTP_R_FOUND);
        sockprintf(c->socket, "Location: %s/\r\n", c->url);
        sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
        return 1;
    } else if (*path != '/') {
        return 0;
    }
    path++;

    if (*path)
        return 0;

    http_send_response(c, HTTP_S_OK);
    /* Use "text/plain" to keep browsers from trying to do anything funny
     * with the data.  MSIE can go to hell. */
    sockprintf(c->socket, "Content-Type: text/plain\r\n");
    sockprintf(c->socket, "Connection: close\r\n\r\n");
    *close_ptr = 1;
    if (c->method != METHOD_HEAD)
        (*p_xml_export)((xml_writefunc_t)sockprintf, c->socket);
    return 1;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "Prefix",           { { CD_STRING, CF_DIRREQ, &Prefix } } },
    { NULL }
};

/*************************************************************************/

#define GET_SYMBOL(sym)  p_##sym = get_module_symbol(mod, #sym)

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "operserv/main") == 0) {
        p_ServicesRoot = get_module_symbol(mod, "ServicesRoot");
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
        GET_SYMBOL(first_nickinfo);
        GET_SYMBOL(next_nickinfo);
        GET_SYMBOL(_get_ngi);
        GET_SYMBOL(_get_ngi_id);
        GET_SYMBOL(put_nickgroupinfo);
        p_get_nickinfo = get_module_symbol(mod, "get_nickinfo");
        p__get_ngi = get_module_symbol(mod, "_get_ngi");
        p__get_ngi_id = get_module_symbol(mod, "_get_ngi_id");
        if (p_get_nickinfo && p_put_nickinfo && p_first_nickinfo
         && p_next_nickinfo && p__get_ngi && p__get_ngi_id
         && p_put_nickgroupinfo
        ) {
            module_nickserv = mod;
        } else {
            module_log("Required symbols not found, nickname information"
                       " will not be available");
            p_get_nickinfo = NULL;
            p_put_nickinfo = NULL;
            p_first_nickinfo = NULL;
            p_next_nickinfo = NULL;
            p__get_ngi = NULL;
            p__get_ngi_id = NULL;
            p_put_nickgroupinfo = NULL;
        }
    } else if (strcmp(modname, "chanserv/main") == 0) {
        GET_SYMBOL(CSMaxReg);
        GET_SYMBOL(get_channelinfo);
        GET_SYMBOL(put_channelinfo);
        GET_SYMBOL(first_channelinfo);
        GET_SYMBOL(next_channelinfo);
        if (p_CSMaxReg && get_channelinfo && put_channelinfo
         && first_channelinfo && next_channelinfo
        ) {
            module_chanserv = mod;
        } else {
            module_log("Required symbols not found, channel information"
                       " will not be available");
            p_CSMaxReg = NULL;
            p_get_channelinfo = NULL;
            p_put_channelinfo = NULL;
            p_first_channelinfo = NULL;
            p_next_channelinfo = NULL;
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
    } else if (strcmp(modname, "misc/xml-export") == 0) {
        module_xml_export = mod;
    }

    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
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
        p_first_nickinfo = NULL;
        p_next_nickinfo = NULL;
        p__get_ngi = NULL;
        p__get_ngi_id = NULL;
        p_put_nickgroupinfo = NULL;
        module_nickserv = NULL;
    } else if (mod == module_chanserv) {
        p_CSMaxReg = NULL;
        p_get_channelinfo = NULL;
        p_put_channelinfo = NULL;
        p_first_channelinfo = NULL;
        p_next_channelinfo = NULL;
        module_chanserv = NULL;
    } else if (mod == module_statserv) {
        p_get_serverstats = NULL;
        p_put_serverstats = NULL;
        p_first_serverstats = NULL;
        p_next_serverstats = NULL;
        module_statserv = NULL;
    } else if (mod == module_xml_export) {
        module_xml_export = NULL;
    }
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    Module *tmpmod;

    Prefix_len = strlen(Prefix);
    while (Prefix_len > 0 && Prefix[Prefix_len-1] == '/')
        Prefix_len--;

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

    tmpmod = find_module("operserv/main");
    if (tmpmod)
        do_load_module(tmpmod, "operserv/main");
    tmpmod = find_module("operserv/akill");
    if (tmpmod)
        do_load_module(tmpmod, "operserv/akill");
    tmpmod = find_module("operserv/news");
    if (tmpmod)
        do_load_module(tmpmod, "operserv/news");
    tmpmod = find_module("operserv/sessions");
    if (tmpmod)
        do_load_module(tmpmod, "operserv/sessions");
    tmpmod = find_module("operserv/sline");
    if (tmpmod)
        do_load_module(tmpmod, "operserv/sline");
    tmpmod = find_module("nickserv/main");
    if (tmpmod)
        do_load_module(tmpmod, "nickserv/main");
    tmpmod = find_module("chanserv/main");
    if (tmpmod)
        do_load_module(tmpmod, "chanserv/main");
    tmpmod = find_module("statserv/main");
    if (tmpmod)
        do_load_module(tmpmod, "statserv/main");
    tmpmod = find_module("misc/xml-export");
    if (tmpmod)
        do_load_module(tmpmod, "misc/xml-export");

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
