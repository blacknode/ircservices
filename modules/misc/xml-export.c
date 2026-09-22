/* Routines to export Services databases in XML format.
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
#include "language.h"
#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"
#include "modules/memoserv/memoserv.h"
#include "modules/operserv/operserv.h"
#include "modules/operserv/maskdata.h"
#include "modules/operserv/news.h"
#include "modules/statserv/statserv.h"

#include "xml.h"

/*************************************************************************/

/* Write a field (either string, password, long, or unsigned long) in XML.
 * `indent' is the indent string to prefix the output with.
 */
#define XML_PUT_STRING(indent,structure,field) do {             \
    if ((structure).field)                                      \
        writefunc(data, "%s<" #field ">%s</" #field ">\n", indent, \
                  xml_quotestr((structure).field));             \
} while (0)
#define XML_PUT_PASS(indent,structure,field)                            \
    writefunc(data, "%s<" #field "%s%s%s>%s</" #field ">\n", indent,    \
              (structure).field.cipher ? " cipher=\"" : "",             \
              (structure).field.cipher ? (structure).field.cipher : "", \
              (structure).field.cipher ? "\"" : "",                     \
              xml_quotebuf((structure).field.password,PASSMAX));
#define XML_PUT_LONG(indent,structure,field)                    \
    writefunc(data, "%s<" #field ">%ld</" #field ">\n", indent, \
              (long)(structure).field)
#define XML_PUT_ULONG(indent,structure,field)                   \
    writefunc(data, "%s<" #field ">%lu</" #field ">\n", indent, \
              (unsigned long)(structure).field)

/* Write an array of strings (with corresponding count) in XML. */
#define XML_PUT_STRARR(indent,structure,field) do {             \
    int i;                                                      \
    writefunc(data, "%s<" #field " count='%lu'>\n", indent,     \
            (unsigned long)(structure).field##_count);          \
    ARRAY_FOREACH (i, (structure).field) {                      \
        writefunc(data, "%s\t<array-element>%s</array-element>\n", \
                  indent, xml_quotestr((structure).field[i]));  \
    }                                                           \
    writefunc(data, "%s</" #field ">\n", indent);               \
} while (0)

/*************************************************************************/
/*************************** Internal routines ***************************/
/*************************************************************************/

/* Quote any special characters in the given buffer and return the result
 * in a static buffer.  Trailing null characters are removed.
 */

static char *xml_quotebuf(const char *buf_, int size)
{
    const unsigned char *buf = (unsigned char *)buf_;
    static char retbuf[BUFSIZE*6+1];
    uint32 i;
    char *d;

    d = retbuf;
    while (size > 0 && !buf[size-1])
        size--;
    for (i = 0; i < size; i++, buf++) {
        if (d - retbuf >= sizeof(retbuf)-6) {
#ifdef CONVERT_DB
            fprintf(stderr, "warning: xml_quotebuf(%p,%d) result too long,"
                    " truncated", buf, size);
#else
            module_log("warning: xml_quotebuf(%p,%d) result too long,"
                       " truncated", buf, size);
#endif
            break;
        }
        if (*buf < 32 || *buf > 126) {
            sprintf(d, "&#%u;", *buf);
            if (*buf < 10)
                d += 4;
            else if (*buf < 100)
                d += 5;
            else
                d += 6;
        } else switch (*buf) {
          case '<':
            memcpy(d, "&lt;", 4);
            d += 4;
            break;
          case '>':
            memcpy(d, "&gt;", 4);
            d += 4;
            break;
          case '&':
            memcpy(d, "&amp;", 5);
            d += 5;
            break;
          default:
            *d++ = *buf;
            break;
        }
    }
    *d = 0;
    return retbuf;
}


/* Do the same thing to a \0-terminated string. */

#define xml_quotestr(s)  xml_quotebuf((s), strlen((s)))

/*************************************************************************/
/*************************** Database output *****************************/
/*************************************************************************/

/* Write all (major) Services constants to the file. */

static int export_constants(xml_writefunc_t writefunc, void *data)
{
    writefunc(data, "\t<constants>\n");
#define XML_PUT_CONST(c) \
writefunc(data, "\t\t<" #c ">%ld</" #c ">\n", (long)c)
    XML_PUT_CONST(LANG_DEFAULT);
    XML_PUT_CONST(CHANMAX_UNLIMITED);
    XML_PUT_CONST(CHANMAX_DEFAULT);
    XML_PUT_CONST(TIMEZONE_DEFAULT);
    XML_PUT_CONST(ACCLEV_FOUNDER);
    XML_PUT_CONST(ACCLEV_INVALID);
    XML_PUT_CONST(ACCLEV_SOP);
    XML_PUT_CONST(ACCLEV_AOP);
    XML_PUT_CONST(ACCLEV_HOP);
    XML_PUT_CONST(ACCLEV_VOP);
    XML_PUT_CONST(ACCLEV_NOP);
    XML_PUT_CONST(MEMOMAX_UNLIMITED);
    XML_PUT_CONST(MEMOMAX_DEFAULT);
    XML_PUT_CONST(NEWS_LOGON);
    XML_PUT_CONST(NEWS_OPER);
    XML_PUT_CONST(MD_AKILL);
    XML_PUT_CONST(MD_EXCLUDE);
    XML_PUT_CONST(MD_EXCEPTION);
    XML_PUT_CONST(MD_SGLINE);
    XML_PUT_CONST(MD_SQLINE);
    XML_PUT_CONST(MD_SZLINE);
#undef XML_PUT_CONST
    writefunc(data, "\t</constants>\n");
    return 1;
}

/*************************************************************************/

static int export_operserv_data(xml_writefunc_t writefunc, void *data)
{
    int32 maxusercnt;
    time_t maxusertime;
    Password *supass;

    if (!get_operserv_data(OSDATA_MAXUSERCNT, &maxusercnt)
     || !get_operserv_data(OSDATA_MAXUSERTIME, &maxusertime)
     || !get_operserv_data(OSDATA_SUPASS, &supass))
        return 0;
    writefunc(data, "\t<maxusercnt>%d</maxusercnt>\n", maxusercnt);
    writefunc(data, "\t<maxusertime>%ld</maxusertime>\n", (long)maxusertime);
    if (supass) {
        writefunc(data, "\t<supass%s%s%s>%s</supass>\n",
                  supass->cipher ? " cipher=\"" : "",
                  supass->cipher ? supass->cipher : "",
                  supass->cipher ? "\"" : "",
                  xml_quotebuf(supass->password, PASSMAX));
    }
    return 1;
}

/*************************************************************************/

static int export_nick_db(xml_writefunc_t writefunc, void *data)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    int i;

    for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {
        writefunc(data, "\t<nickgroupinfo>\n");
        XML_PUT_ULONG ("\t\t", *ngi, id);
        XML_PUT_STRARR("\t\t", *ngi, nicks);
        XML_PUT_ULONG ("\t\t", *ngi, mainnick);
        XML_PUT_PASS  ("\t\t", *ngi, pass);
        XML_PUT_STRING("\t\t", *ngi, url);
        XML_PUT_STRING("\t\t", *ngi, email);
        XML_PUT_STRING("\t\t", *ngi, last_email);
        XML_PUT_STRING("\t\t", *ngi, info);
        XML_PUT_LONG  ("\t\t", *ngi, flags);
        XML_PUT_LONG  ("\t\t", *ngi, os_priv);
        XML_PUT_LONG  ("\t\t", *ngi, authcode);
        if (ngi->authcode) {
            XML_PUT_LONG  ("\t\t", *ngi, authset);
            XML_PUT_LONG  ("\t\t", *ngi, authreason);
        }
        if (ngi->flags & NF_SUSPENDED) {
            XML_PUT_STRING("\t\t", *ngi, suspend_who);
            XML_PUT_STRING("\t\t", *ngi, suspend_reason);
            XML_PUT_LONG  ("\t\t", *ngi, suspend_time);
            XML_PUT_LONG  ("\t\t", *ngi, suspend_expires);
        }
        XML_PUT_LONG  ("\t\t", *ngi, language);
        XML_PUT_LONG  ("\t\t", *ngi, timezone);
        XML_PUT_LONG  ("\t\t", *ngi, channelmax);
        XML_PUT_STRARR("\t\t", *ngi, access);
        XML_PUT_STRARR("\t\t", *ngi, ajoin);
        writefunc(data, "\t\t<memoinfo>\n\t\t\t<memos count='%d'>\n",
                  ngi->memos.memos_count);
        ARRAY_FOREACH (i, ngi->memos.memos) {
            writefunc(data, "\t\t\t\t<memo>\n");
            XML_PUT_LONG  ("\t\t\t\t\t", ngi->memos.memos[i], number);
            XML_PUT_LONG  ("\t\t\t\t\t", ngi->memos.memos[i], flags);
            XML_PUT_LONG  ("\t\t\t\t\t", ngi->memos.memos[i], time);
            XML_PUT_STRING("\t\t\t\t\t", ngi->memos.memos[i], sender);
            if (ngi->memos.memos[i].channel)
                XML_PUT_STRING("\t\t\t\t\t", ngi->memos.memos[i], channel);
            XML_PUT_STRING("\t\t\t\t\t", ngi->memos.memos[i], text);
            writefunc(data, "\t\t\t\t</memo>\n");
        }
        writefunc(data, "\t\t\t</memos>\n");
        XML_PUT_LONG  ("\t\t\t", ngi->memos, memomax);
        writefunc(data, "\t\t</memoinfo>\n");
        XML_PUT_STRARR("\t\t", *ngi, ignore);
        writefunc(data, "\t</nickgroupinfo>\n");
    }
    for (ni = first_nickinfo(); ni; ni = next_nickinfo()) {
        writefunc(data, "\t<nickinfo>\n");
        XML_PUT_STRING("\t\t", *ni, nick);
        writefunc(data, "\t\t<status>%d</status>\n",
                  ni->status & ~NS_TEMPORARY);
        XML_PUT_STRING("\t\t", *ni, last_usermask);
        XML_PUT_STRING("\t\t", *ni, last_realmask);
        XML_PUT_STRING("\t\t", *ni, last_realname);
        XML_PUT_STRING("\t\t", *ni, last_quit);
        XML_PUT_LONG  ("\t\t", *ni, time_registered);
        XML_PUT_LONG  ("\t\t", *ni, last_seen);
        XML_PUT_ULONG ("\t\t", *ni, nickgroup);
        writefunc(data, "\t</nickinfo>\n");
    }
    return 1;
}

/*************************************************************************/

static int export_channel_db(xml_writefunc_t writefunc, void *data)
{
    int i;
    ChannelInfo *ci;

    for (ci = first_channelinfo(); ci; ci = next_channelinfo()) {
        writefunc(data, "\t<channelinfo>\n");
        XML_PUT_STRING("\t\t", *ci, name);
        XML_PUT_ULONG ("\t\t", *ci, founder);
        XML_PUT_ULONG ("\t\t", *ci, successor);
        XML_PUT_PASS  ("\t\t", *ci, founderpass);
        XML_PUT_STRING("\t\t", *ci, desc);
        XML_PUT_STRING("\t\t", *ci, url);
        XML_PUT_STRING("\t\t", *ci, email);
        XML_PUT_LONG  ("\t\t", *ci, time_registered);
        XML_PUT_LONG  ("\t\t", *ci, last_used);
        XML_PUT_STRING("\t\t", *ci, last_topic);
        XML_PUT_STRING("\t\t", *ci, last_topic_setter);
        XML_PUT_LONG  ("\t\t", *ci, last_topic_time);
        XML_PUT_LONG  ("\t\t", *ci, flags);
        if (ci->flags & CF_SUSPENDED) {
            XML_PUT_STRING("\t\t", *ci, suspend_who);
            XML_PUT_STRING("\t\t", *ci, suspend_reason);
            XML_PUT_LONG  ("\t\t", *ci, suspend_time);
            XML_PUT_LONG  ("\t\t", *ci, suspend_expires);
        }
        writefunc(data, "\t\t<levels>\n");
#define XML_PUT_LEVEL(lev) \
  if (ci->levels[lev] != ACCLEV_DEFAULT) \
    writefunc(data, "\t\t\t<" #lev ">%d</" #lev ">\n", ci->levels[lev])
        XML_PUT_LEVEL(CA_INVITE);
        XML_PUT_LEVEL(CA_AKICK);
        XML_PUT_LEVEL(CA_SET);
        XML_PUT_LEVEL(CA_UNBAN);
        XML_PUT_LEVEL(CA_AUTOOP);
        XML_PUT_LEVEL(CA_AUTODEOP);
        XML_PUT_LEVEL(CA_AUTOVOICE);
        XML_PUT_LEVEL(CA_OPDEOP);
        XML_PUT_LEVEL(CA_ACCESS_LIST);
        XML_PUT_LEVEL(CA_CLEAR);
        XML_PUT_LEVEL(CA_NOJOIN);
        XML_PUT_LEVEL(CA_ACCESS_CHANGE);
        XML_PUT_LEVEL(CA_MEMO);
        XML_PUT_LEVEL(CA_VOICE);
        XML_PUT_LEVEL(CA_AUTOHALFOP);
        XML_PUT_LEVEL(CA_HALFOP);
        XML_PUT_LEVEL(CA_AUTOPROTECT);
        XML_PUT_LEVEL(CA_PROTECT);
#undef XML_PUT_LEVEL
        writefunc(data, "\t\t</levels>\n");
        writefunc(data, "\t\t<chanaccesslist count='%d'>\n", ci->access_count);
        ARRAY_FOREACH (i, ci->access) {
            if (ci->access[i].nickgroup) {  // Skip empty entries
                writefunc(data, "\t\t\t<chanaccess>\n");
                XML_PUT_ULONG ("\t\t\t\t", ci->access[i], nickgroup);
                XML_PUT_LONG  ("\t\t\t\t", ci->access[i], level);
                writefunc(data, "\t\t\t</chanaccess>\n");
            }
        }
        writefunc(data, "\t\t</chanaccesslist>\n");
        writefunc(data, "\t\t<akicklist count='%d'>\n", ci->akick_count);
        ARRAY_FOREACH (i, ci->akick) {
            writefunc(data, "\t\t\t<akick>\n");
            XML_PUT_STRING("\t\t\t\t", ci->akick[i], mask);
            XML_PUT_STRING("\t\t\t\t", ci->akick[i], reason);
            XML_PUT_STRING("\t\t\t\t", ci->akick[i], who);
            XML_PUT_LONG  ("\t\t\t\t", ci->akick[i], set);
            XML_PUT_LONG  ("\t\t\t\t", ci->akick[i], lastused);
            writefunc(data, "\t\t\t</akick>\n");
        }
        writefunc(data, "\t\t</akicklist>\n");
        writefunc(data, "\t\t<mlock>\n");
#ifdef CONVERT_DB
        XML_PUT_STRING("\t\t\t", *ci, mlock.on);
        XML_PUT_STRING("\t\t\t", *ci, mlock.off);
#else
        writefunc(data, "\t\t\t<mlock.on>%s</mlock.on>\n",
                  mode_flags_to_string(ci->mlock.on, MODE_CHANNEL));
        writefunc(data, "\t\t\t<mlock.off>%s</mlock.off>\n",
                  mode_flags_to_string(ci->mlock.off, MODE_CHANNEL));
#endif
        XML_PUT_LONG  ("\t\t\t", *ci, mlock.limit);
        XML_PUT_STRING("\t\t\t", *ci, mlock.key);
        XML_PUT_STRING("\t\t\t", *ci, mlock.link);
        XML_PUT_STRING("\t\t\t", *ci, mlock.flood);
        XML_PUT_LONG  ("\t\t\t", *ci, mlock.joindelay);
        XML_PUT_LONG  ("\t\t\t", *ci, mlock.joinrate1);
        XML_PUT_LONG  ("\t\t\t", *ci, mlock.joinrate2);
        writefunc(data, "\t\t</mlock>\n");
        XML_PUT_STRING("\t\t", *ci, entry_message);
        writefunc(data, "\t</channelinfo>\n");
    }
    return 1;
}

/*************************************************************************/

static int export_news_db(xml_writefunc_t writefunc, void *data)
{
    NewsItem *news;

    for (news = first_news(); news; news = next_news()) {
        writefunc(data, "\t<news>\n");
        XML_PUT_LONG  ("\t\t", *news, type);
        XML_PUT_LONG  ("\t\t", *news, num);
        XML_PUT_STRING("\t\t", *news, text);
        XML_PUT_STRING("\t\t", *news, who);
        XML_PUT_LONG  ("\t\t", *news, time);
        writefunc(data, "\t</news>\n");
    }
    return 1;
}

/*************************************************************************/

static int export_maskdata(xml_writefunc_t writefunc, void *data)
{
    int i;
    MaskData *md;

    for (i = 0; i < 256; i++) {
        for (md = first_maskdata(i); md; md = next_maskdata(i)) {
            writefunc(data, "\t<maskdata type='%d'>\n", i);
            XML_PUT_LONG  ("\t\t", *md, num);
            XML_PUT_STRING("\t\t", *md, mask);
            if (md->limit)
                XML_PUT_LONG("\t\t",*md, limit);
            XML_PUT_STRING("\t\t", *md, reason);
            XML_PUT_STRING("\t\t", *md, who);
            XML_PUT_LONG  ("\t\t", *md, time);
            XML_PUT_LONG  ("\t\t", *md, expires);
            XML_PUT_LONG  ("\t\t", *md, lastused);
            writefunc(data, "\t</maskdata>\n");
        }
    }
    return 1;
}

/*************************************************************************/

static int export_statserv_db(xml_writefunc_t writefunc, void *data)
{
    ServerStats *ss;

    for (ss = first_serverstats(); ss; ss = next_serverstats()) {
        writefunc(data, "\t<serverstats>\n");
        XML_PUT_STRING("\t\t", *ss, name);
        XML_PUT_LONG  ("\t\t", *ss, t_join);
        XML_PUT_LONG  ("\t\t", *ss, t_quit);
        XML_PUT_STRING("\t\t", *ss, quit_message);
        writefunc(data, "\t</serverstats>\n");
    }
    return 1;
}

/*************************************************************************/
/**************************** Global routines ****************************/
/*************************************************************************/

EXPORT_FUNC(xml_export)
int xml_export(xml_writefunc_t writefunc, void *data)
{
    writefunc(data, "<?xml version='1.0'?>\n<ircservices-db>\n");
    return export_constants(writefunc, data)
        && export_operserv_data(writefunc, data)
        && export_nick_db(writefunc, data)
        && export_channel_db(writefunc, data)
        && export_news_db(writefunc, data)
        && export_maskdata(writefunc, data)
        && export_statserv_db(writefunc, data)
        && (writefunc(data, "</ircservices-db>\n"), 1);
}

/*************************************************************************/
/*************************************************************************/

#ifndef CONVERT_DB

/* Command-line option callback. */

static int do_command_line(const char *option, const char *value)
{
    FILE *f;

    if (!option || strcmp(option, "export") != 0)
        return 0;
    if (!value || !*value || strcmp(value,"-") == 0) {
        f = stdout;
    } else {
        f = fopen(value, "w");
        if (!f) {
            perror(value);
            return 2;
        }
    }
    if (!xml_export((xml_writefunc_t)fprintf, f))
        return 2;
    return 3;
}

#endif  /* CONVERT_DB */

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

#ifndef CONVERT_DB

/*************************************************************************/

ConfigDirective module_config[] = {
    { NULL }
};

/*************************************************************************/

int init_module(void)
{
    if (!add_callback(NULL, "command line", do_command_line)) {
        module_log("Unable to add callback");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    remove_callback(NULL, "command line", do_command_line);
    return 1;
}

/*************************************************************************/

#endif  /* CONVERT_DB */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
