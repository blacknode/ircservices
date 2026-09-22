/* Routines to import Services databases from an XML file.  Note that empty
 * tags (of the form "<tag/>") are not supported.
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

/* We include new_xxx() and free_xxx() routines in here directly, so we
 * need to make sure the appropriate preprocessor symbols are defined here
 * to avoid static/non-static warnings. */
#define STANDALONE_NICKSERV
#define STANDALONE_CHANSERV
#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"
#include "modules/memoserv/memoserv.h"
#include "modules/operserv/operserv.h"
#include "modules/operserv/maskdata.h"
#include "modules/operserv/news.h"
#include "modules/statserv/statserv.h"

#include "xml.h"

/*************************************************************************/

/* Flags for xml_import().  Note that the `abort' ones are slightly
 * misleading: the parser will continue to parse to the end of the file
 * even if an `abort' condition is encountered, but will not actually merge
 * the read-in data with the current databases in such a case.
 */

/* Behavior for nickname collisions: default is to skip the entire nickname
 * group when a nickname is found that matches one already registered. */
#define XMLI_NICKCOLL_SKIPGROUP 0x0000  /* Skip the entire group (default) */
#define XMLI_NICKCOLL_SKIPNICK  0x0001  /* Only skip the colliding nick */
#define XMLI_NICKCOLL_OVERWRITE 0x0002  /* Overwrite the registered nick */
#define XMLI_NICKCOLL_ABORT     0x0007  /* Stop importing */
#define XMLI_NICKCOLL_MASK      0x0007

/* Behavior for channel collisions: default is to skip the channel. */
#define XMLI_CHANCOLL_SKIP      0x0000  /* Skip the channel (default) */
#define XMLI_CHANCOLL_OVERWRITE 0x0008  /* Overwrite the registered channel */
#define XMLI_CHANCOLL_ABORT     0x0038  /* Stop importing */
#define XMLI_CHANCOLL_MASK      0x0038

/*************************************************************************/

static int ImportNews = 0;
static int VerboseImport = 0;

/* Import flags, set from configuration options */
static int flags = XMLI_NICKCOLL_SKIPGROUP | XMLI_CHANCOLL_SKIP;

/* File to import from. */
static FILE *import_file;

/* Number of bytes and lines (i.e. \012 characters) read from data so far.
 * lines_read starts at 1 so it can be used as a line number in messages. */
static int bytes_read, lines_read;

/* Macro to read the next byte of data into variable c, returning -1 at EOF. */
#define NEXT_BYTE       do { if ((c = get_byte()) < 0) return -1; } while (0)

/* Value returned by tag handlers to indicate there's no data to return but
 * no fatal error either (as opposed to NULL, which indicates an error). */
#define CONTINUE        ((void *)1)

/* Value returned by parse_tag() to indicate that the current tag's closing
 * tag has been found. */
#define PARSETAG_END    ((void *)-1)

/*************************************************************************/

/* Type for a tag handler. */
typedef void *(*TagHandler)(char *tag, char *attr, char *attrval);

/* Structure for returning text/length pairs.  th_text() returns this. */
typedef struct {
    char *text;
    int len;
} TextInfo;

/* Structure for returning array/length pairs.  th_strarray() and other
 * array-tag handlers return this. */
typedef struct {
    void *array;
    int len;  /* number of elements */
} ArrayInfo;

/* Prototypes for tag handlers. */
static void *th_default(char *tag, char *attr, char *attrval);
static void *th_text(char *tag, char *attr, char *attrval);
static void *th_int32(char *tag, char *attr, char *attrval);
static void *th_uint32(char *tag, char *attr, char *attrval);
static void *th_time(char *tag, char *attr, char *attrval);
static void *th_strarray(char *tag, char *attr, char *attrval);
static void *th_constants(char *tag, char *attr, char *attrval);
static void *th_nickgroupinfo(char *tag, char *attr, char *attrval);
static void *th_password(char *tag, char *attr, char *attrval);
static void *th_memoinfo(char *tag, char *attr, char *attrval);
static void *th_memos(char *tag, char *attr, char *attrval);
static void *th_memo(char *tag, char *attr, char *attrval);
static void *th_nickinfo(char *tag, char *attr, char *attrval);
static void *th_channelinfo(char *tag, char *attr, char *attrval);
static void *th_levels(char *tag, char *attr, char *attrval);
static void *th_chanaccesslist(char *tag, char *attr, char *attrval);
static void *th_chanaccess(char *tag, char *attr, char *attrval);
static void *th_akicklist(char *tag, char *attr, char *attrval);
static void *th_akick(char *tag, char *attr, char *attrval);
static void *th_mlock(char *tag, char *attr, char *attrval);
static void *th_mlock_modes(char *tag, char *attr, char *attrval);
static void *th_news(char *tag, char *attr, char *attrval);
static void *th_maskdata(char *tag, char *attr, char *attrval);
static void *th_serverstats(char *tag, char *attr, char *attrval);

/* List of tags and associated handlers. */
static struct {
    const char *tag;
    TagHandler handler;
} tags[] = {
    { "constants",         th_constants },
    { "LANG_DEFAULT",      th_int32 },
    { "CHANMAX_UNLIMITED", th_int32 },
    { "CHANMAX_DEFAULT",   th_int32 },
    { "TIMEZONE_DEFAULT",  th_int32 },
    { "ACCLEV_FOUNDER",    th_int32 },
    { "ACCLEV_INVALID",    th_int32 },
    { "ACCLEV_SOP",        th_int32 },
    { "ACCLEV_AOP",        th_int32 },
    { "ACCLEV_HOP",        th_int32 },
    { "ACCLEV_VOP",        th_int32 },
    { "ACCLEV_NOP",        th_int32 },
    { "MEMOMAX_UNLIMITED", th_int32 },
    { "MEMOMAX_DEFAULT",   th_int32 },
    { "NEWS_LOGON",        th_int32 },
    { "NEWS_OPER",         th_int32 },
    { "MD_AKILL",          th_int32 },
    { "MD_EXCLUDE",        th_int32 },
    { "MD_EXCLUSION",      th_int32 },  // Old name for MD_EXCLUDE
    { "MD_EXCEPTION",      th_int32 },
    { "MD_SGLINE",         th_int32 },
    { "MD_SQLINE",         th_int32 },
    { "MD_SZLINE",         th_int32 },

    { "maxusercnt",        th_default },  /* ignore */
    { "maxusertime",       th_default },  /* ignore */
    { "supass",            th_default },  /* ignore */

    { "nickgroupinfo",     th_nickgroupinfo },
    { "id",                th_uint32 },
    { "nicks",             th_strarray },
    { "array-element",     th_text },
    { "mainnick",          th_int32 },
    { "pass",              th_password },
    { "url",               th_text },
    { "email",             th_text },
    { "last_email",        th_text },
    { "info",              th_text },
    { "flags",             th_int32 },
    { "os_priv",           th_int32 },
    { "authcode",          th_int32 },
    { "authset",           th_time },
    { "authreason",        th_int32 },
    { "suspend_who",       th_text },
    { "suspend_reason",    th_text },
    { "suspend_time",      th_time },
    { "suspend_expires",   th_time },
    { "language",          th_int32 },
    { "timezone",          th_int32 },
    { "channelmax",        th_int32 },
    { "access",            th_strarray },
    { "ajoin",             th_strarray },
    { "memoinfo",          th_memoinfo },
    { "memos",             th_memos },
    { "memo",              th_memo },
    { "number",            th_int32 },
    { "time",              th_time },
    { "sender",            th_text },
    { "channel",           th_text },
    { "text",              th_text },
    { "memomax",           th_int32 },
    { "ignore",            th_strarray },

    { "nickinfo",          th_nickinfo },
    { "nick",              th_text },
    { "status",            th_int32 },
    { "last_usermask",     th_text },
    { "last_realmask",     th_text },
    { "last_realname",     th_text },
    { "last_quit",         th_text },
    { "time_registered",   th_time },
    { "last_seen",         th_time },
    { "nickgroup",         th_uint32 },

    { "channelinfo",       th_channelinfo },
    { "name",              th_text },
    { "founder",           th_uint32 },
    { "successor",         th_uint32 },
    { "founderpass",       th_password },
    { "desc",              th_text },
    { "last_used",         th_time },
    { "last_topic",        th_text },
    { "last_topic_setter", th_text },
    { "last_topic_time",   th_time },
    { "levels",            th_levels },
    { "CA_INVITE",         th_int32 },
    { "CA_AKICK",          th_int32 },
    { "CA_SET",            th_int32 },
    { "CA_UNBAN",          th_int32 },
    { "CA_AUTOOP",         th_int32 },
    { "CA_AUTODEOP",       th_int32 },
    { "CA_AUTOVOICE",      th_int32 },
    { "CA_OPDEOP",         th_int32 },
    { "CA_ACCESS_LIST",    th_int32 },
    { "CA_CLEAR",          th_int32 },
    { "CA_NOJOIN",         th_int32 },
    { "CA_ACCESS_CHANGE",  th_int32 },
    { "CA_MEMO",           th_int32 },
    { "CA_VOICE",          th_int32 },
    { "CA_AUTOHALFOP",     th_int32 },
    { "CA_HALFOP",         th_int32 },
    { "CA_AUTOPROTECT",    th_int32 },
    { "CA_PROTECT",        th_int32 },
    { "CA_AUTOOWNER",      th_default },  /* obsolete tag from 5.0, ignore */
    { "chanaccesslist",    th_chanaccesslist },
    { "chanaccess",        th_chanaccess },
    { "level",             th_int32 },
    { "akicklist",         th_akicklist },
    { "akick",             th_akick },
    { "mask",              th_text },
    { "reason",            th_text },
    { "who",               th_text },
    { "set",               th_time },
    { "lastused",          th_time },
    { "mlock",             th_mlock },
    { "mlock.on",          th_mlock_modes },
    { "mlock_on",          th_mlock_modes },  // old name
    { "mlock.off",         th_mlock_modes },
    { "mlock_off",         th_mlock_modes },  // old name
    { "mlock.limit",       th_int32 },
    { "mlock_limit",       th_int32 },  // old name
    { "mlock.key",         th_text },
    { "mlock_key",         th_text },   // old name
    { "mlock.link",        th_text },
    { "mlock_link",        th_text },   // old name
    { "mlock.flood",       th_text },
    { "mlock_flood",       th_text },   // old name
    { "mlock.joindelay",   th_int32 },
    { "mlock_joindelay",   th_int32 },  // old name
    { "mlock.joinrate1",   th_int32 },
    { "mlock_joinrate1",   th_int32 },  // old name
    { "mlock.joinrate2",   th_int32 },
    { "mlock_joinrate2",   th_int32 },  // old name
    { "entry_message",     th_text },

    { "news",              th_news },
    { "type",              th_int32 },
    { "num",               th_int32 },

    { "maskdata",          th_maskdata },
    { "limit",             th_int32 },
    { "expires",           th_time },

    { "serverstats",       th_serverstats },
    { "t_join",            th_time },
    { "t_quit",            th_time },
    { "quit_message",      th_text },

    { NULL }
};

/*************************************************************************/
/***************************** Error output ******************************/
/*************************************************************************/

/* Write the given error message, along with a line number.  The message
 * will be truncated at 4095 bytes if it exceeds that length.
 */

static void error(const char *fmt, ...) FORMAT(printf,1,2);
static void error(const char *fmt, ...)
{
    char buf[4096];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    fprintf(stderr, "Line %d: %s\n", lines_read, buf);
}

/*************************************************************************/
/************** Utility routines (taken from other modules) **************/
/*************************************************************************/

/* The NickServ/ChanServ/etc. modules may not be loaded at this point, so
 * we need to have the freeing routines locally. */

/*************************************************************************/

#include "modules/nickserv/util.c"
#include "modules/chanserv/util.c"

/*************************************************************************/

/* Free a NewsItem structure. */

static void my_free_newsitem(NewsItem *news)
{
    free(news->text);
    free(news);
}

/*************************************************************************/

/* Free a MaskData structure. */

static void my_free_maskdata(MaskData *md)
{
    free(md->mask);
    free(md->reason);
    free(md);
}

/*************************************************************************/

/* Free a ServerStats structure. */

static void my_free_serverstats(ServerStats *ss)
{
    free(ss->name);
    free(ss->quit_message);
    free(ss);
}

/*************************************************************************/

/* Unlink and free a nickname, and remove it from its associated nickname
 * group, if any.  If the nickname is the last in its group, remove the
 * group as well, along with any channels owned by the group (unless the
 * channels have successors, in which case the successor gets the channel).
 */

static void my_delnick(NickInfo *ni)
{
    if (ni->nickgroup) {
        NickGroupInfo *ngi = get_nickgroupinfo(ni->nickgroup);
        if (ngi) {
            int i;
            ARRAY_SEARCH_PLAIN(ngi->nicks, ni->nick, irc_stricmp, i);
            if (i < ngi->nicks_count) {
                ARRAY_REMOVE(ngi->nicks, i);
                if (ngi->mainnick > i || ngi->mainnick >= ngi->nicks_count)
                    ngi->mainnick--;
                if (!ngi->nicks_count) {
                    ChannelInfo *ci;
                    del_nickgroupinfo(ngi);  /* also frees */
                    for (ci = first_channelinfo(); ci;
                         ci = next_channelinfo()
                    ) {
                        if (ci->successor == ni->nickgroup)
                            ci->successor = 0;
                        if (ci->founder == ni->nickgroup) {
                            if (ci->successor) {
                                NickGroupInfo *ngi2 =
                                    get_nickgroupinfo(ci->successor);
                                if (ngi2) {
                                    error("Giving channel %s (owned by deleted"
                                          " nick %s) to %s", ci->name,
                                          ni->nick, ngi_mainnick(ngi2));
                                    ci->founder = ci->successor;
                                    ci->successor = 0;
                                    put_nickgroupinfo(ngi2);
                                } else {
                                    error("Dropping channel %s (owned by"
                                          " deleted nick %s, invalid successor"
                                          " %u)", ci->name, ni->nick,
                                          ci->successor);
                                    del_channelinfo(ci);
                                }
                            } else {  /* !ci->successor */
                                error("Dropping channel %s (owned by deleted"
                                      " nick %s, no successor)",
                                      ci->name, ni->nick);
                                del_channelinfo(ci);
                            }  /* if (ci->successor) */
                        }  /* if (ci->founder == ni->nickgroup) */
                    }  /* for all channels */
                }  /* if (!ngi->nicks_count) */
                else {
                    put_nickgroupinfo(ngi);
                }
            }  /* if (i < ngi->nicks_count) */
        }  /* if (ngi) */
    }  /* if (ni->nickgroup) */
    del_nickinfo(ni);  /* also frees */
}

/*************************************************************************/
/****************************** XML parsing ******************************/
/*************************************************************************/

/* Read and return the next byte of data; return -1 at end of file. */

static int get_byte(void)
{
    char c;     /* Byte read */

    static char readbuf[4096];  /* Read buffer */
    static int readbuf_end;     /* Amount of data in read buffer */
    static int readbuf_pos;     /* Next byte to read from buffer */

    if (bytes_read == 0) {
        /* First call */
        readbuf_end = readbuf_pos = 0;
    }
    if (readbuf_pos >= readbuf_end) {
        /* Out of data in buffer */
        readbuf_end = fread(readbuf, 1, sizeof(readbuf), import_file);
        if (readbuf_end <= 0) {
            /* End of file or error */
            return -1;
        }
        readbuf_pos = 0;
    }
    /* Retrieve next byte from read buffer and increment "next" index */
    c = readbuf[readbuf_pos++];
    /* Update xxx_read counters */
    bytes_read++;
    if (c == '\012')
        lines_read++;
    /* Return the character */
    return c;
}

/*************************************************************************/

/* Parse the given entity into a character (multiple-character entities not
 * supported).  Return the character, -1 if EOF was reached while parsing
 * the entity, or -2 if the entity was successfully read in but unknown.
 * Assumes the '&' beginning the entity has already been read.  If the
 * entity name is longer than 255 characters, it will not be parsed
 * correctly.
 */

static int parse_entity(void)
{
    char name[256];
    int i, c;

    i = 0;
    NEXT_BYTE;
    while (c != ';') {
        if (i < sizeof(name)-1)
            name[i++] = c;
        NEXT_BYTE;
    }
    name[i] = 0;
    if (stricmp(name, "lt") == 0) {
        return '<';
    } else if (stricmp(name, "gt") == 0) {
        return '>';
    } else if (stricmp(name, "amp") == 0) {
        return '&';
    } else if (*name == '#') {
        if (name[1+strspn(name+1,"0123456789")] == '\0') {
            /* &#NNN; */
            return strtol(name+1, NULL, 10);
        } else if ((name[1] == 'x' || name[1] == 'X')
                   && name[2+strspn(name+2,"0123456789ABCDEFabcdef")] == '\0'){
            /* &#xNN; */
            return strtol(name+2, NULL, 16);
        }
    }
    /* Unknown entity */
    error("Unknown entity `%s'", name);
    return -2;
}

/*************************************************************************/

/* Read the next tag and return it in *tag_ret.  If the tag has attributes,
 * return the name of the first one in *attr_ret and its value in
 * *attrval_ret, otherwise set both *attr_ret and *attrval_ret to NULL.
 * If `text_ret' and `textlen_ret' are both not NULL, return all text found
 * before the opening bracket of the next tag in *text_ret and the length
 * of the text in *textlen_ret; the text will be followed by a null
 * character.
 *
 * The function's return value is as follows:
 *                                                ------ Modified? ------
 * Value| Meaning                                |tag |attr|val |text|len
 * -----+----------------------------------------+----+----+----+----+----
 *   1  | Valid opening tag found                | ** | ** | ** | ** | **
 *   0  | Valid closing tag found                | ** | ** | ** | ** | **
 *  -1  | End of file                            | -- | -- | -- | -- | --
 *  -2  | Non-whitespace chars found before tag  | -- | -- | -- | -- | --
 *      |    (if text_ret or textlen_ret NULL)   |    |    |    |    |
 *  -2  | Not enough memory for text (if both    | -- | -- | -- | -- | --
 *      |    text_ret and textlen_ret not NULL)  |    |    |    |    |
 *  -3  | Invalid syntax in tag                  | ** | -- | -- | ** | **
 *  -4  | Invalid & entity in text before tag    | -- | -- | -- | -- | --
 *
 * Note that all returned strings are stored in static buffers which are
 * overwritten at each call, and all returned strings except *text_ret will
 * be silently truncated at 255 characters.
 *
 * If `tag_ret' is NULL, the function frees its static buffers and returns
 * 0.
 */

static int read_tag(char **tag_ret, char **attr_ret, char **attrval_ret,
                    char **text_ret, int *textlen_ret)
{
    static char tag[256], attr[256], attrval[256];
    static char *text = NULL;
    static int textlen = 0, textsize = 0;
    int is_closing = 0;  /* Is this a closing tag? */
    int c, i;

    /* Special case: free buffers */
    if (!tag_ret) {
        free(text);
        text = NULL;
        textlen = textsize = 0;
        return 0;
    }

    /* Find beginning of tag */
    if (text_ret && textlen_ret) {
        textlen = 0;
        if (!text) {
            textsize = 256;
            text = malloc(textsize);
            if (!text)
                return -2;
        }
    }
    NEXT_BYTE;
    while (c != '<') {
        if (text_ret && textlen_ret) {
            if (textlen+1 >= textsize) {  /* +1 to allow space for \0 at end */
                textsize = textlen + 256;
                text = realloc(text, textsize);
                if (!text) {
                    textsize = 0;
                    return -2;
                }
            }
            if (c == '&') {
                c = parse_entity();
                if (c == -1) {
                    return -1;
                } else if (c == -2) {
                    return -4;
                }
            }
            text[textlen++] = c;
        } else if (!isspace(c)) {
            return -2;
        }
        NEXT_BYTE;
    } while (c != '<');
    if (text_ret && textlen_ret)
        text[textlen] = 0;

    /* Skip over whitespace before tag name */
    do {
        NEXT_BYTE;
    } while (isspace(c));

    /* Check for / to indicate closing tag */
    if (c == '/') {
        is_closing = 1;
        do {
            NEXT_BYTE;
        } while (isspace(c));
    }

    /* Read tag name */
    i = 0;
    while (!isspace(c) && c != '>') {
        if (i < sizeof(tag)-1)
            tag[i++] = c;
        NEXT_BYTE;
    }
    tag[i] = 0;
    /* Skip over whitespace */
    while (isspace(c))
        NEXT_BYTE;

    /* Check for and parse attributes */
    *attr = 0;
    *attrval = 0;
    while (c != '>' && c != '?') {
        int save_attr = (*attr == 0);  /* Only save the first attribute */
        int quote = 0;  /* Quote character to match for attribute value */
        i = 0;
        /* Parse attribute name */
        while (!isspace(c) && c != '=') {
            if (c == '>') {
                /* We don't use any valueless attributes, so ignore this one */
                if (save_attr)
                    *attr = 0;
                break;
            }
            if (save_attr && i < sizeof(attr)-1)
                attr[i++] = c;
            NEXT_BYTE;
        }
        attr[i] = 0;
        while (isspace(c))
            NEXT_BYTE;
        /* As above, we don't use any valueless attributes */
        if (c != '=') {
            if (save_attr)
                *attr = 0;
            continue;
        }
        do {
            NEXT_BYTE;
        } while (isspace(c));
        /* Parse value; allow any sequence of 'string', "string", or
         * unquoted strings */
        i = 0;
        while (quote || (!isspace(c) && c != '>' && c != '?')) {
            if (quote && c == quote) {
                quote = 0;
            } else if (c == '\'' || c == '"') {
                quote = c;
            } else {
                if (c == '&') {
                    c = parse_entity();
                    if (c == -1) {
                        return -1;
                    } else if (c == -2) {
                        *tag_ret = tag;
                        if (text_ret && textlen_ret) {
                            *text_ret = text;
                            *textlen_ret = textlen;
                        }
                        return -3;
                    }
                }
                if (save_attr && i < sizeof(attrval)-1)
                    attrval[i++] = c;
            }
            NEXT_BYTE;
        }
        attrval[i] = 0;
        while (isspace(c)) {
            NEXT_BYTE;
        }
    } /* while more attributes */

    /* If this is a ?XML-type tag, there should be a ? before the > that
     * closes the tag.  Skip it and any subsequent whitespace if it's
     * there, but don't complain if it's not. */
    if (*tag == '?' && c == '?') {
        do {
            NEXT_BYTE;
        } while (isspace(c));
    }

    /* We should now be at the closing > of the tag.  We don't use empty
     * tags, so don't bother handling them. */
    if (c != '>') {
        *tag_ret = tag;
        if (text_ret && textlen_ret) {
            *text_ret = text;
            *textlen_ret = textlen;
        }
        return -3;
    }

    /* Successfully parsed: return tag, attribute and text data. */
    *tag_ret = tag;
    if (*attr) {
        *attr_ret = attr;
        *attrval_ret = attrval;
    } else {
        *attr_ret = NULL;
        *attrval_ret = NULL;
    }
    if (text_ret && textlen_ret) {
        *text_ret = text;
        *textlen_ret = textlen;
    }
    return is_closing ? 0 : 1;
}

/*************************************************************************/

/* Parse the next tag, and return a pointer to its contents (the type of
 * pointer depends on the tag); return the tag's name in *found_tag_ret.
 * Returns NULL if either read_tag() returns an error or the tag handler
 * returns NULL; returns PARSETAG_END when the closing tag corresponding to
 * `caller_tag' is found.  (In both of the latter cases, *found_tag_ret
 * will be unmodified.)  If the handler returns PARSETAG_END, signals an
 * error via error() and returns NULL.
 *
 * If `text_ret' and `textlen_ret' are both non-NULL, then for closing
 * tags (PARSETAG_END return value), the text found before the tag will be
 * returned as with read_tag().
 */

static void *parse_tag(const char *caller_tag, char found_tag_ret[256],
                       char **text_ret, int *textlen_ret)
{
    char local_tag[256], local_attr[256], local_attrval[256];
    char *tag, *attr, *attrval, *text;
    int textlen;
    int i;

    /* Read next tag; if it's not an opening tag, return immediately */
    i = read_tag(&tag, &attr, &attrval, &text, &textlen);
    if (i == -1) {
        if (found_tag_ret)
            *found_tag_ret = 0;
        return PARSETAG_END;
    } else if (i < 0) {
        return NULL;
    } else if (i == 0) {
        if (stricmp(tag, caller_tag) != 0) {
            error("Mismatched closing tag: expected </%s>, got </%s>",
                  caller_tag, tag);
            return NULL;
        }
        if (text_ret && textlen_ret) {
            *text_ret = text;
            *textlen_ret = textlen;
        }
        return PARSETAG_END;
    }

    /* Copy tag and attribute data to local buffers */
    strbcpy(local_tag, tag);
    strbcpy(local_attr, attr ? attr : "");
    strbcpy(local_attrval, attrval ? attrval : "");

    /* Copy tag name to return buffer */
    if (found_tag_ret)
        strscpy(found_tag_ret, tag, 256);  /* sizeof(found_tag_ret) == 4 */

    /* Look up tag in table and call handler */
    for (i = 0; tags[i].tag != NULL; i++) {
        if (stricmp(tags[i].tag, tag) == 0) {
            void *retval;
            retval = tags[i].handler(local_tag, local_attr, local_attrval);
            if (retval == PARSETAG_END) {
                error("Internal error: bad return value from <%s> handler",
                      local_tag);
                retval = NULL;
            }
            return retval;
        }
    }

    /* If tag was not found in table, call default handler (which just
     * ignores all text until the end tag and parses intermediate tags) */
    error("Warning: Skipping unknown tag <%s>", local_tag);
    return th_default(local_tag, local_attr, local_attrval);
}

/*************************************************************************/
/***************************** Tag handlers ******************************/
/*************************************************************************/

/* Constants used in this data set. */
static int32
    const_LANG_DEFAULT,
    const_CHANMAX_UNLIMITED,
    const_CHANMAX_DEFAULT,
    const_TIMEZONE_DEFAULT,
    const_ACCLEV_FOUNDER,
    const_ACCLEV_INVALID,
    const_ACCLEV_SOP,
    const_ACCLEV_AOP,
    const_ACCLEV_HOP,
    const_ACCLEV_VOP,
    const_ACCLEV_NOP,
    const_MEMOMAX_UNLIMITED,
    const_MEMOMAX_DEFAULT,
    const_NEWS_LOGON,
    const_NEWS_OPER,
    const_MD_AKILL,
    const_MD_EXCLUDE,
    const_MD_EXCEPTION,
    const_MD_SGLINE,
    const_MD_SQLINE,
    const_MD_SZLINE;

/* Various lists of data. */
static NickGroupInfo *ngi_list;
static NickInfo *ni_list;
static ChannelInfo *ci_list;
static NewsItem *news_list;
static MaskData *md_list[256];
static ServerStats *ss_list;

/*************************************************************************/

/* Default tag handler; simply parses tags until an appropriate end tag is
 * found.  Returns CONTINUE.
 */

static void *th_default(char *tag, char *attr, char *attrval)
{
    void *result;

    while ((result = parse_tag(tag, NULL, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL)
            return NULL;
        else if (result == CONTINUE)
            continue;
    }
    return CONTINUE;
}

/*************************************************************************/

/* Returns the text between the open tag and close tag (if nested tags are
 * present, returns the text between the last nested close tag and this
 * close tag) as a dynamically allocated, null-terminated buffer pointed to
 * by a static TextInfo *.
 */

static void *th_text(char *tag, char *attr, char *attrval)
{
    void *result;
    char *text;
    int textlen;
    static TextInfo ti = {NULL,0};

    while ((result = parse_tag(tag, NULL, &text, &textlen)) != PARSETAG_END) {
        if (result == NULL)
            return NULL;
        else if (result == CONTINUE)
            continue;
    }
    ti.text = malloc(textlen+1);
    if (!ti.text) {
        error("Out of memory for <%s>", tag);
        return NULL;
    }
    memcpy(ti.text, text, textlen);
    ti.text[textlen] = 0;
    ti.len = textlen;
    return &ti;
}

/*************************************************************************/

/* Returns the text between the open tag and close tag (if nested tags are
 * present, returns the text between the last nested close tag and this
 * close tag) as an int32 *.
 */

static void *th_int32(char *tag, char *attr, char *attrval)
{
    void *result;
    char *text;
    int textlen;
    static int32 retval;

    while ((result = parse_tag(tag, NULL, &text, &textlen)) != PARSETAG_END) {
        if (result == NULL)
            return NULL;
        else if (result == CONTINUE)
            continue;
    }
    retval = strtol(text, &text, 0);
    if (*text) {
        error("Invalid integer value for <%s>", tag);
        return CONTINUE;
    }
    return &retval;
}

/*************************************************************************/

/* Returns the text between the open tag and close tag (if nested tags are
 * present, returns the text between the last nested close tag and this
 * close tag) as a uint32 *.
 */

static void *th_uint32(char *tag, char *attr, char *attrval)
{
    void *result;
    char *text;
    int textlen;
    static int32 retval;

    while ((result = parse_tag(tag, NULL, &text, &textlen)) != PARSETAG_END) {
        if (result == NULL)
            return NULL;
        else if (result == CONTINUE)
            continue;
    }
    retval = strtoul(text, &text, 0);
    if (*text) {
        error("Invalid unsigned integer value for <%s>", tag);
        return CONTINUE;
    }
    return &retval;
}

/*************************************************************************/

/* Returns the text between the open tag and close tag (if nested tags are
 * present, returns the text between the last nested close tag and this
 * close tag) as a time_t *.
 */

static void *th_time(char *tag, char *attr, char *attrval)
{
    void *result;
    char *text;
    int textlen;
    static time_t retval;

    while ((result = parse_tag(tag, NULL, &text, &textlen)) != PARSETAG_END) {
        if (result == NULL)
            return NULL;
        else if (result == CONTINUE)
            continue;
    }
    retval = strtol(text, &text, 0);
    if (*text) {
        error("Invalid time value for <%s>", tag);
        return CONTINUE;
    }
    return &retval;
}

/*************************************************************************/

/* Parses all <array-element> tags into an array of NULL-terminated
 * strings.  Returns a dynamically allocated array pointed to by a static
 * ArrayInfo *.
 */

static void *th_strarray(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    static ArrayInfo ai;
    static char **array;
    int i;

    if (!attr || !attrval || stricmp(attr, "count") != 0) {
        error("Missing `count' attribute for <%s>", tag);
        return NULL;
    }
    ai.len = strtol(attrval, &attrval, 0);
    if (*attrval || ai.len < 0) {
        error("Invalid value for `count' attribute for <%s>", tag);
        return NULL;
    }
    if (ai.len == 0) {
        array = NULL;
    } else {
        array = malloc(sizeof(*array) * ai.len);
        if (!array) {
            error("Out of memory for <%s>", tag);
            return NULL;
        }
    }
    i = 0;
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            while (--i >= 0)
                free(array[i]);
            free(array);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "array-element") == 0) {
            if (i >= ai.len) {
                error("Warning: Too many elements for <%s>, extra elements"
                      " ignored", tag);
            } else {
                array[i] = ((TextInfo *)result)->text;
                i++;
            }
        }
    }
    ai.array = array;
    return &ai;
}

/*************************************************************************/

/* Read in constant values.  Returns CONTINUE. */

#define DEFINE_CONST(name) {#name,&const_##name}
static const struct {
    const char *name;
    int32 *ptr;
} constants[] = {
    DEFINE_CONST(LANG_DEFAULT),
    DEFINE_CONST(CHANMAX_UNLIMITED),
    DEFINE_CONST(CHANMAX_DEFAULT),
    DEFINE_CONST(TIMEZONE_DEFAULT),
    DEFINE_CONST(ACCLEV_FOUNDER),
    DEFINE_CONST(ACCLEV_INVALID),
    DEFINE_CONST(ACCLEV_SOP),
    DEFINE_CONST(ACCLEV_AOP),
    DEFINE_CONST(ACCLEV_HOP),
    DEFINE_CONST(ACCLEV_VOP),
    DEFINE_CONST(ACCLEV_NOP),
    DEFINE_CONST(MEMOMAX_UNLIMITED),
    DEFINE_CONST(MEMOMAX_DEFAULT),
    DEFINE_CONST(NEWS_LOGON),
    DEFINE_CONST(NEWS_OPER),
    DEFINE_CONST(MD_AKILL),
    DEFINE_CONST(MD_EXCLUDE),
    {"MD_EXCLUSION", &const_MD_EXCLUDE},
    DEFINE_CONST(MD_EXCEPTION),
    DEFINE_CONST(MD_SGLINE),
    DEFINE_CONST(MD_SQLINE),
    DEFINE_CONST(MD_SZLINE),
    { NULL }
};
#undef DEFINE_CONST

static void *th_constants(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    int i;

    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL)
            return NULL;
        else if (result == CONTINUE)
            continue;
        for (i = 0; constants[i].name; i++) {
            if (stricmp(constants[i].name, tag2) == 0) {
                *constants[i].ptr = *(int32 *)result;
                break;
            }
        }
        if (!constants[i].name)
            error("Warning: Unknown constant tag <%s> ignored", tag2);
    }
    return CONTINUE;
}

/*************************************************************************/

/* Parse a <nickgroupinfo> tag block, and return a dynamically allocated
 * NickGroupInfo *.
 */

static void *th_nickgroupinfo(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    NickGroupInfo *ngi;

    ngi = new_nickgroupinfo(NULL);
    if (!ngi) {
        error("Out of memory for <%s>", tag);
        return NULL;
    }
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            free_nickgroupinfo(ngi);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "id") == 0) {
            ngi->id = *(int32 *)result;
            if (!ngi->id)
                error("Invalid <id> tag (must not be zero)");
        } else if (stricmp(tag2, "nicks") == 0) {
            int i;
            char **nicks = ((ArrayInfo *)result)->array;
            ngi->nicks_count = ((ArrayInfo *)result)->len;
            ngi->nicks = calloc(ngi->nicks_count, sizeof(*ngi->nicks));
            if (!ngi->nicks) {
                error("Out of memory for <%s>", tag2);
                free_nickgroupinfo(ngi);
                return NULL;
            }
            ARRAY_FOREACH (i, ngi->nicks) {
                strbcpy(ngi->nicks[i], nicks[i]);
                free(nicks[i]);
            }
            free(nicks);
        } else if (stricmp(tag2, "mainnick") == 0) {
            ngi->mainnick = *(int32 *)result;
        } else if (stricmp(tag2, "pass") == 0) {
            Password *pass = result;
            copy_password(&ngi->pass, pass);
            free_password(pass);
        } else if (stricmp(tag2, "url") == 0) {
            ngi->url = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "email") == 0) {
            ngi->email = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "last_email") == 0) {
            ngi->last_email = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "info") == 0) {
            ngi->info = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "flags") == 0) {
            ngi->flags = *(int32 *)result;
        } else if (stricmp(tag2, "os_priv") == 0) {
            ngi->os_priv = *(int32 *)result;
        } else if (stricmp(tag2, "authcode") == 0) {
            ngi->authcode = *(int32 *)result;
        } else if (stricmp(tag2, "authset") == 0) {
            ngi->authset = *(time_t *)result;
        } else if (stricmp(tag2, "authreason") == 0) {
            ngi->authreason = *(int32 *)result;
        } else if (stricmp(tag2, "suspend_who") == 0) {
            strbcpy(ngi->suspend_who, ((TextInfo *)result)->text);
            free(((TextInfo *)result)->text);
        } else if (stricmp(tag2, "suspend_reason") == 0) {
            ngi->suspend_reason = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "suspend_time") == 0) {
            ngi->suspend_time = *(time_t *)result;
        } else if (stricmp(tag2, "suspend_expires") == 0) {
            ngi->suspend_expires = *(time_t *)result;
        } else if (stricmp(tag2, "language") == 0) {
            ngi->language = *(int32 *)result;
            if (ngi->language == const_LANG_DEFAULT)
                ngi->language = LANG_DEFAULT;
        } else if (stricmp(tag2, "timezone") == 0) {
            ngi->timezone = *(int32 *)result;
            if (ngi->timezone == const_TIMEZONE_DEFAULT)
                ngi->timezone = TIMEZONE_DEFAULT;
        } else if (stricmp(tag2, "channelmax") == 0) {
            ngi->channelmax = *(int32 *)result;
            if (ngi->channelmax == const_CHANMAX_DEFAULT)
                ngi->channelmax = CHANMAX_DEFAULT;
            else if (ngi->channelmax == const_CHANMAX_UNLIMITED)
                ngi->channelmax = CHANMAX_UNLIMITED;
        } else if (stricmp(tag2, "access") == 0) {
            ngi->access = ((ArrayInfo *)result)->array;
            ngi->access_count = ((ArrayInfo *)result)->len;
        } else if (stricmp(tag2, "ajoin") == 0) {
            ngi->ajoin = ((ArrayInfo *)result)->array;
            ngi->ajoin_count = ((ArrayInfo *)result)->len;
        } else if (stricmp(tag2, "memoinfo") == 0) {
            ngi->memos = *(MemoInfo *)result;
        } else if (stricmp(tag2, "ignore") == 0) {
            ngi->ignore = ((ArrayInfo *)result)->array;
            ngi->ignore_count = ((ArrayInfo *)result)->len;
        } else {
            error("Warning: Unknown NickGroupInfo tag <%s> ignored", tag2);
        }
    }
    if (!ngi->id) {
        error("<id> tag missing from nick group, ignoring");
        free_nickgroupinfo(ngi);
        return CONTINUE;
    } else if (!ngi->nicks_count) {
        error("Nick group %u has no nicks, ignoring", ngi->id);
        free_nickgroupinfo(ngi);
        return CONTINUE;
    }
    if (ngi->mainnick >= ngi->nicks_count) {
        error("Warning: invalid main nick setting %d for nick group %u,"
              " resetting to 0", ngi->mainnick, ngi->id);
        ngi->mainnick = 0;
    }
    return ngi;
}

/*************************************************************************/

/* Parse a <pass> or <founderpass> tag block, and return a Password *
 * allocated with new_password().
 */

static void *th_password(char *tag, char *attr, char *attrval)
{
    void *result;
    char *text;
    int textlen;
    Password *pass;

    while ((result = parse_tag(tag, NULL, &text, &textlen)) != PARSETAG_END) {
        if (result == NULL)
            return NULL;
        else if (result == CONTINUE)
            continue;
    }
    pass = new_password();
    if (!pass) {
        error("Out of memory for <%s>", tag);
        return NULL;
    }
    if (textlen > sizeof(pass->password)) {
        error("Warning: password truncated");
        textlen = sizeof(pass->password);
    }
    memcpy(pass->password, text, textlen);
    if (attr && strcmp(attr, "cipher") == 0) {
        pass->cipher = strdup(attrval);
        if (!pass->cipher) {
            error("Out of memory for <%s>", tag);
            free_password(pass);
            return NULL;
        }
    }
    return pass;
}

/*************************************************************************/

/* Parse a <memoinfo> tag block, and return dynamically allocated memo
 * data pointed to by a static MemoInfo *.
 */

static void *th_memoinfo(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    static MemoInfo mi;
    int i;

    memset(&mi, 0, sizeof(mi));
    mi.memomax = MEMOMAX_DEFAULT;
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            ARRAY_FOREACH (i, mi.memos)
                free(mi.memos[i].text);
            free(mi.memos);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "memos") == 0) {
            mi.memos = ((ArrayInfo *)result)->array;
            mi.memos_count = ((ArrayInfo *)result)->len;
        } else if (stricmp(tag2, "memomax") == 0) {
            mi.memomax = *(int32 *)result;
            if (mi.memomax == const_MEMOMAX_DEFAULT)
                mi.memomax = MEMOMAX_DEFAULT;
            if (mi.memomax == const_MEMOMAX_UNLIMITED)
                mi.memomax = MEMOMAX_UNLIMITED;
        } else {
            error("Warning: Unknown MemoInfo tag <%s> ignored", tag2);
        }
    }
    return &mi;
}

/*************************************************************************/

/* Parse a <memos> tag block, and return a dynamically allocated Memo array
 * pointed to by a static ArrayInfo *.
 */

static void *th_memos(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    static ArrayInfo ai;
    static Memo *array;
    int i;

    if (!attr || stricmp(attr, "count") != 0) {
        error("Missing `count' attribute for <%s>", tag);
        return NULL;
    }
    ai.len = strtol(attrval, &attrval, 0);
    if (*attrval || ai.len < 0) {
        error("Invalid value for `count' attribute for <%s>", tag);
        return NULL;
    }
    if (ai.len == 0) {
        array = NULL;
    } else {
        array = malloc(sizeof(*array) * ai.len);
        if (!array) {
            error("Out of memory for <%s>", tag);
            return NULL;
        }
    }
    i = 0;
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            while (--i >= 0)
                free(array[i].text);
            free(array);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "memo") == 0) {
            if (i >= ai.len) {
                error("Warning: Too many elements for <%s>, extra elements"
                      " ignored", tag);
            } else {
                array[i] = *(Memo *)result;
                i++;
            }
        }
    }
    ai.array = array;
    return &ai;
}

/*************************************************************************/

/* Parse a <memo> tag block, and return a static Memo * (with dynamically
 * allocated text).
 */

static void *th_memo(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    static Memo memo;

    memset(&memo, 0, sizeof(memo));
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            free(memo.text);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "number") == 0) {
            memo.number = *(int32 *)result;
        } else if (stricmp(tag2, "flags") == 0) {
            memo.flags = *(int32 *)result;
        } else if (stricmp(tag2, "time") == 0) {
            memo.time = *(time_t *)result;
        } else if (stricmp(tag2, "sender") == 0) {
            strbcpy(memo.sender, ((TextInfo *)result)->text);
            free(((TextInfo *)result)->text);
        } else if (stricmp(tag2, "channel") == 0) {
            memo.channel = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "text") == 0) {
            memo.text = ((TextInfo *)result)->text;
        } else {
            error("Warning: Unknown MemoInfo tag <%s> ignored", tag2);
        }
    }
    if (!*memo.sender)
        strbcpy(memo.sender, "<unknown>");
    return &memo;
}

/*************************************************************************/

/* Parse a <nickinfo> tag block, and return a dynamically allocated
 * NickInfo *.
 */

static void *th_nickinfo(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    NickInfo *ni;

    ni = new_nickinfo();
    if (!ni) {
        error("Out of memory for <%s>", tag);
        return NULL;
    }
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            free_nickinfo(ni);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "nick") == 0) {
            strbcpy(ni->nick, ((TextInfo *)result)->text);
            free(((TextInfo *)result)->text);
            if (!*ni->nick)
                error("Empty <nick> tag");
        } else if (stricmp(tag2, "status") == 0) {
            ni->status = *(int32 *)result;
        } else if (stricmp(tag2, "last_usermask") == 0) {
            ni->last_usermask = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "last_realmask") == 0) {
            ni->last_realmask = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "last_realname") == 0) {
            ni->last_realname = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "last_quit") == 0) {
            ni->last_quit = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "time_registered") == 0) {
            ni->time_registered = *(time_t *)result;
        } else if (stricmp(tag2, "last_seen") == 0) {
            ni->last_seen = *(time_t *)result;
        } else if (stricmp(tag2, "nickgroup") == 0) {
            ni->nickgroup = *(int32 *)result;
        } else {
            error("Warning: Unknown NickInfo tag <%s> ignored", tag2);
        }
    }
    if (!*ni->nick) {
        error("<nick> tag missing from nick, ignoring");
        free_nickinfo(ni);
        return CONTINUE;
    } else if (!(ni->status & NS_VERBOTEN)) {
        if (!ni->nickgroup) {
            error("Nick %s has no nick group, ignoring", ni->nick);
            free_nickinfo(ni);
            return CONTINUE;
        }
        if (!ni->last_usermask) {
            error("Warning: Nick %s has no <last_usermask> tag, setting to"
                  " `@'", ni->nick);
            ni->last_usermask = strdup("@");
            if (!ni->last_usermask) {
                error("Out of memory");
                free_nickinfo(ni);
                return CONTINUE;
            }
        }
        if (!ni->last_realname) {
            error("Warning: Nick %s has no <last_realname> tag, setting to"
                  " `'", ni->nick);
            ni->last_realname = strdup("");
            if (!ni->last_realname) {
                error("Out of memory");
                free_nickinfo(ni);
                return CONTINUE;
            }
        }
    }
    if (!ni->time_registered) {
        /* Don't warn for forbidden nicks--old versions didn't record the
         * forbid time */
        if (!(ni->status & NS_VERBOTEN)) {
            error("Warning: Nick %s has no time of registration, setting"
                  " registration time to current time", ni->nick);
        }
        ni->time_registered = time(NULL);
    }
    if (!ni->last_seen && !(ni->status & NS_VERBOTEN)) {
        error("Warning: Nick %s has no last-seen time, setting last-seen"
              " time to registration time", ni->nick);
        ni->last_seen = ni->time_registered;
    }
    return ni;
}

/*************************************************************************/

/* Parse a <channelinfo> tag block, and return a dynamically allocated
 * ChannelInfo *.
 */

static void *th_channelinfo(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    ChannelInfo *ci;
    int i;

    ci = new_channelinfo();
    if (!ci) {
        error("Out of memory for <%s>", tag);
        return NULL;
    }
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            free_channelinfo(ci);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "name") == 0) {
            strbcpy(ci->name, ((TextInfo *)result)->text);
            free(((TextInfo *)result)->text);
            if (!*ci->name)
                error("Empty <name> tag");
        } else if (stricmp(tag2, "founder") == 0) {
            ci->founder = *(int32 *)result;
        } else if (stricmp(tag2, "successor") == 0) {
            ci->successor = *(int32 *)result;
        } else if (stricmp(tag2, "founderpass") == 0) {
            Password *pass = result;
            copy_password(&ci->founderpass, pass);
            free_password(pass);
        } else if (stricmp(tag2, "desc") == 0) {
            ci->desc = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "url") == 0) {
            ci->url = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "email") == 0) {
            ci->email = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "time_registered") == 0) {
            ci->time_registered = *(time_t *)result;
        } else if (stricmp(tag2, "last_used") == 0) {
            ci->last_used = *(time_t *)result;
        } else if (stricmp(tag2, "last_topic") == 0) {
            ci->last_topic = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "last_topic_setter") == 0) {
            strbcpy(ci->last_topic_setter, ((TextInfo *)result)->text);
            free(((TextInfo *)result)->text);
            if (!*ci->last_topic_setter)
                strbcpy(ci->last_topic_setter, "<unknown>");
        } else if (stricmp(tag2, "last_topic_time") == 0) {
            ci->last_topic_time = *(time_t *)result;
        } else if (stricmp(tag2, "flags") == 0) {
            ci->flags = *(int32 *)result;
        } else if (stricmp(tag2, "suspend_who") == 0) {
            strbcpy(ci->suspend_who, ((TextInfo *)result)->text);
            free(((TextInfo *)result)->text);
        } else if (stricmp(tag2, "suspend_reason") == 0) {
            ci->suspend_reason = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "suspend_time") == 0) {
            ci->suspend_time = *(time_t *)result;
        } else if (stricmp(tag2, "suspend_expires") == 0) {
            ci->suspend_expires = *(time_t *)result;
        } else if (stricmp(tag2, "levels") == 0) {
            memcpy(ci->levels, result, sizeof(ci->levels));
            free(result);
        } else if (stricmp(tag2, "chanaccesslist") == 0) {
            ci->access = ((ArrayInfo *)result)->array;
            ci->access_count = ((ArrayInfo *)result)->len;
        } else if (stricmp(tag2, "akicklist") == 0) {
            ci->akick = ((ArrayInfo *)result)->array;
            ci->akick_count = ((ArrayInfo *)result)->len;
        } else if (stricmp(tag2, "mlock") == 0) {
            ci->mlock = *(ModeLock *)result;
        /* Old mode lock tags (5.0) */
        } else if (stricmp(tag2, "mlock_on") == 0) {
            ci->mlock.on = *(int32 *)result;
        } else if (stricmp(tag2, "mlock_off") == 0) {
            ci->mlock.off = *(int32 *)result;
        } else if (stricmp(tag2, "mlock_limit") == 0) {
            ci->mlock.limit = *(int32 *)result;
        } else if (stricmp(tag2, "mlock_key") == 0) {
            ci->mlock.key = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "mlock_link") == 0) {
            ci->mlock.link = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "mlock_flood") == 0) {
            ci->mlock.flood = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "mlock_joindelay") == 0) {
            ci->mlock.joindelay = *(int32 *)result;
        } else if (stricmp(tag2, "mlock_joinrate1") == 0) {
            ci->mlock.joinrate1 = *(int32 *)result;
        } else if (stricmp(tag2, "mlock_joinrate2") == 0) {
            ci->mlock.joinrate2 = *(int32 *)result;
        } else if (stricmp(tag2, "entry_message") == 0) {
            ci->entry_message = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "memoinfo") == 0) {
            error("Warning: Ignoring channel memos for channel %s",
                  *ci->name ? ci->name : "<unknown>");
        } else {
            error("Warning: Unknown ChannelInfo tag <%s> ignored", tag2);
        }
    }
    if (!*ci->name) {
        error("<name> tag missing from channel, ignoring");
        free_channelinfo(ci);
        return CONTINUE;
    } else if (strcmp(ci->name, "#") == 0) {
        error("Channel \"#\" not supported");
        free_channelinfo(ci);
        return CONTINUE;
    } else if (!ci->founder && !(ci->flags & CF_VERBOTEN)) {
        error("Channel %s has no founder, ignoring", ci->name);
        free_channelinfo(ci);
        return CONTINUE;
    }
    if (ci->founder && ci->successor == ci->founder) {
        error("Warning: Channel %s has founder == successor, clearing"
              " successor", ci->name);
        ci->successor = 0;
    }
    if (!ci->time_registered) {
        /* Don't warn for forbidden channels--old versions didn't record
         * the forbid time */
        if (!(ci->flags & CF_VERBOTEN)) {
            error("Warning: Channel %s has no time of registration, setting"
                  " registration time to current time", ci->name);
        }
        ci->time_registered = time(NULL);
    }
    if (!ci->last_used && !(ci->flags & CF_VERBOTEN)) {
        error("Warning: Channel %s has no last-used time, setting last-used"
              " time to registration time", ci->name);
        ci->last_used = ci->time_registered;
    }
    /* Fill in channel fields for access/autokick entries */
    ARRAY_FOREACH (i, ci->access)
        ci->access[i].channel = ci;
    ARRAY_FOREACH (i, ci->akick)
        ci->akick[i].channel = ci;
    return ci;
}

/*************************************************************************/

/* Parse a <levels> tag block, and return a static array of CA_SIZE level
 * values (int16).
 */

#define DEFINE_LEVEL(name) {#name,name}
static const struct {
    const char *name;
    int index;
} levellist[] = {
    DEFINE_LEVEL(CA_INVITE),
    DEFINE_LEVEL(CA_AKICK),
    DEFINE_LEVEL(CA_SET),
    DEFINE_LEVEL(CA_UNBAN),
    DEFINE_LEVEL(CA_AUTOOP),
    DEFINE_LEVEL(CA_AUTODEOP),
    DEFINE_LEVEL(CA_AUTOVOICE),
    DEFINE_LEVEL(CA_OPDEOP),
    DEFINE_LEVEL(CA_ACCESS_LIST),
    DEFINE_LEVEL(CA_CLEAR),
    DEFINE_LEVEL(CA_NOJOIN),
    DEFINE_LEVEL(CA_ACCESS_CHANGE),
    DEFINE_LEVEL(CA_MEMO),
    DEFINE_LEVEL(CA_VOICE),
    DEFINE_LEVEL(CA_AUTOHALFOP),
    DEFINE_LEVEL(CA_HALFOP),
    DEFINE_LEVEL(CA_AUTOPROTECT),
    DEFINE_LEVEL(CA_PROTECT),
    { NULL }
};
#undef DEFINE_LEVEL

static void *th_levels(char *tag, char *attr, char *attrval)
{
    int16 *levels;
    char tag2[256];
    void *result;
    int i;

    /* Initialize level array */
    levels = malloc(sizeof(*levels) * CA_SIZE);
    if (!levels) {
        error("Out of memory for <%s>", tag);
        return NULL;
    }
    for (i = 0; i < CA_SIZE; i++)
        levels[i] = ACCLEV_DEFAULT;

    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        int32 value;
        if (result == NULL) {
            free(levels);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        }
        value = *(int32 *)result;
        if (value == const_ACCLEV_FOUNDER)
            value = ACCLEV_FOUNDER;
        else if (value == const_ACCLEV_INVALID)
            value = ACCLEV_INVALID;
        else if (value > ACCLEV_MAX)
            value = ACCLEV_MAX;
        else if (value < ACCLEV_MIN)
            value = ACCLEV_MIN;
        for (i = 0; levellist[i].name; i++) {
            if (stricmp(levellist[i].name, tag2) == 0) {
                levels[levellist[i].index] = (int16)value;
                break;
            }
        }
        if (!levellist[i].name)
            error("Warning: Unknown level tag <%s> ignored", tag2);
    }

    return levels;
}

/*************************************************************************/

/* Parse a <chanaccesslist> tag block, and return a dynamically allocated
 * ChanAccess array pointed to by a static ArrayInfo *.
 */

static void *th_chanaccesslist(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    static ArrayInfo ai;
    static ChanAccess *array;
    int i;

    if (!attr || stricmp(attr, "count") != 0) {
        error("Missing `count' attribute for <%s>", tag);
        return NULL;
    }
    ai.len = strtol(attrval, &attrval, 0);
    if (*attrval || ai.len < 0) {
        error("Invalid value for `count' attribute for <%s>", tag);
        return NULL;
    }
    if (ai.len == 0) {
        array = NULL;
    } else {
        array = malloc(sizeof(*array) * ai.len);
        if (!array) {
            error("Out of memory for <%s>", tag);
            return NULL;
        }
    }
    i = 0;
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            free(array);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "chanaccess") == 0) {
            if (i >= ai.len) {
                error("Warning: Too many elements for <%s>, extra elements"
                      " ignored", tag);
            } else {
                array[i] = *(ChanAccess *)result;
                i++;
            }
        }
    }
    ai.array = array;
    return &ai;
}

/*************************************************************************/

/* Parse a <chanaccess> tag block, and return a static ChanAccess *. */

static void *th_chanaccess(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    static ChanAccess access;

    memset(&access, 0, sizeof(access));
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "nickgroup") == 0) {
            access.nickgroup = *(int32 *)result;
        } else if (stricmp(tag2, "level") == 0) {
            int32 level = *(int32 *)result;
            if (level > ACCLEV_MAX)
                level = ACCLEV_MAX;
            else if (level < ACCLEV_MIN)
                level = ACCLEV_MIN;
            access.level = (int16)level;
        } else {
            error("Warning: Unknown ChanAccess tag <%s> ignored", tag2);
        }
    }
    return &access;
}

/*************************************************************************/

/* Parse a <akicklist> tag block, and return a dynamically allocated
 * AutoKick array pointed to by a static ArrayInfo *.
 */

static void *th_akicklist(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    static ArrayInfo ai;
    static AutoKick *array;
    int i;

    if (!attr || stricmp(attr, "count") != 0) {
        error("Missing `count' attribute for <%s>", tag);
        return NULL;
    }
    ai.len = strtol(attrval, &attrval, 0);
    if (*attrval || ai.len < 0) {
        error("Invalid value for `count' attribute for <%s>", tag);
        return NULL;
    }
    if (ai.len == 0) {
        array = NULL;
    } else {
        array = malloc(sizeof(*array) * ai.len);
        if (!array) {
            error("Out of memory for <%s>", tag);
            return NULL;
        }
    }
    i = 0;
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            while (--i >= 0) {
                free(array[i].mask);
                free(array[i].reason);
            }
            free(array);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "akick") == 0) {
            if (i >= ai.len) {
                error("Warning: Too many elements for <%s>, extra elements"
                      " ignored", tag);
            } else {
                array[i] = *(AutoKick *)result;
                i++;
            }
        }
    }
    ai.array = array;
    return &ai;
}

/*************************************************************************/

/* Parse an <akick> tag block, and return a static AutoKick *. */

static void *th_akick(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    static AutoKick akick;

    memset(&akick, 0, sizeof(akick));
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            free(akick.mask);
            free(akick.reason);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "mask") == 0) {
            akick.mask = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "reason") == 0) {
            akick.reason = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "who") == 0) {
            strbcpy(akick.who, ((TextInfo *)result)->text);
            free(((TextInfo *)result)->text);
        } else if (stricmp(tag2, "set") == 0) {
            akick.set = *(time_t *)result;
        } else if (stricmp(tag2, "lastused") == 0) {
            akick.lastused = *(time_t *)result;
        } else {
            error("Warning: Unknown AutoKick tag <%s> ignored", tag2);
        }
    }
    if (!akick.mask) {
        free(akick.reason);
        memset(&akick, 0, sizeof(akick));
    } else {
        if (!*akick.who)
            strbcpy(akick.who, "<unknown>");
    }
    return &akick;
}

/*************************************************************************/

/* Parse an <mlock> tag block, and return a static ModeLock record. */

static void *th_mlock(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    static ModeLock mlock;

    memset(&mlock, 0, sizeof(mlock));
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "mlock.on") == 0) {
            mlock.on = *(int32 *)result;
        } else if (stricmp(tag2, "mlock.off") == 0) {
            mlock.off = *(int32 *)result;
        } else if (stricmp(tag2, "mlock.limit") == 0) {
            mlock.limit = *(int32 *)result;
        } else if (stricmp(tag2, "mlock.key") == 0) {
            mlock.key = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "mlock.link") == 0) {
            mlock.link = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "mlock.flood") == 0) {
            mlock.flood = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "mlock.joindelay") == 0) {
            mlock.joindelay = *(int32 *)result;
        } else if (stricmp(tag2, "mlock.joinrate1") == 0) {
            mlock.joinrate1 = *(int32 *)result;
        } else if (stricmp(tag2, "mlock.joinrate2") == 0) {
            mlock.joinrate2 = *(int32 *)result;
        } else {
            error("Warning: Unknown ModeLock tag <%s> ignored", tag2);
        }
    }
    return &mlock;
}

/*************************************************************************/

/* Parse an <mlock.on> or <mlock.off> tag, and return a pointer to a static
 * int32.
 */

static void *th_mlock_modes(char *tag, char *attr, char *attrval)
{
    TextInfo *ti;
    char *s;
    static int32 modes;

    ti = th_text(tag, attr, attrval);
    if (!ti)
        return NULL;
    s = ti->text;
    modes = 0;
    while (*s) {
        int32 flag = mode_char_to_flag(*s, MODE_CHANNEL);
        if (flag == 0)                  /* Invalid mode character */
            error("Ignoring unknown mode character `%c' in <%s>", *s, tag);
        else if (flag == MODE_INVALID)  /* No associated flag */
            error("Ignoring non-binary mode character `%c' in <%s>", *s, tag);
        else                            /* Valid mode with flag */
            modes |= flag;
        s++;
    }
    free(ti->text);
    return &modes;
}

/*************************************************************************/

/* Parse a <news> tag block, and return a dynamically allocated NewsItem *.
 */

static void *th_news(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    NewsItem *news;

    news = malloc(sizeof(*news));
    if (!news) {
        error("Out of memory for <%s>", tag);
        return NULL;
    }
    memset(news, 0, sizeof(*news));
    news->type = NEWS_INVALID;
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            my_free_newsitem(news);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "type") == 0) {
            news->type = *(int32 *)result;
            if (news->type == const_NEWS_LOGON)
                news->type = NEWS_LOGON;
            else if (news->type == const_NEWS_OPER)
                news->type = NEWS_OPER;
            else {
                error("Unknown news type %d", news->type);
                news->type = NEWS_INVALID;
            }
        } else if (stricmp(tag2, "num") == 0) {
            news->num = *(int32 *)result;
            if (news->num < 0) {
                error("Warning: Invalid news item number %d, will be"
                      " renumbered later", news->num);
                news->num = 0;
            }
        } else if (stricmp(tag2, "text") == 0) {
            news->text = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "who") == 0) {
            strbcpy(news->who, ((TextInfo *)result)->text);
            free(((TextInfo *)result)->text);
        } else if (stricmp(tag2, "time") == 0) {
            news->time = *(time_t *)result;
        } else {
            error("Warning: Unknown NewsItem tag <%s> ignored", tag2);
        }
    }
    if (news->type == NEWS_INVALID) {
        error("News type missing or invalid, ignoring news item");
        my_free_newsitem(news);
        return CONTINUE;
    } else if (!news->text || !*news->text) {
        error("News item has no text, ignoring");
        my_free_newsitem(news);
        return CONTINUE;
    }
    if (!*news->who)
        strbcpy(news->who, "<unknown>");
    if (!news->time) {
        error("Warning: News item has no creation time, setting to current"
              " time");
        news->time = time(NULL);
    }
    return news;
}

/*************************************************************************/

/* Parse a <maskdata> tag block, and return a dynamically allocated
 * MaskData *.
 */

static void *th_maskdata(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    MaskData *md;
    long type;

    if (!attr || !attrval || stricmp(attr, "type") != 0) {
        error("`type' attribute missing from <%s>", tag);
        return NULL;
    }
    type = strtol(attrval, &attrval, 0);
    if (*attrval || type < 0 || type > 255) {
        error("`Invalid value for `type' attribute for <%s>", tag);
        return NULL;
    }
    if (type == const_MD_AKILL) {
        type = MD_AKILL;
    } else if (type == const_MD_EXCEPTION) {
        type = MD_EXCEPTION;
    } else if (type == const_MD_EXCLUDE) {
        type = MD_EXCLUDE;
    } else if (type == const_MD_SGLINE) {
        type = MD_SGLINE;
    } else if (type == const_MD_SQLINE) {
        type = MD_SQLINE;
    } else if (type == const_MD_SZLINE) {
        type = MD_SZLINE;
    } else {
        error("Unknown type %ld, entry will be ignored", type);
        type = -1;
    }
    md = malloc(sizeof(*md));
    if (!md) {
        error("Out of memory for <%s>", tag);
        return NULL;
    }
    memset(md, 0, sizeof(*md));
    md->type = (uint8)type;
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            my_free_maskdata(md);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "num") == 0) {
            md->num = *(int32 *)result;
            if (md->num < 0) {
                error("Warning: Invalid mask data entry number %d, will be"
                      " renumbered later", md->num);
                md->num = 0;
            }
        } else if (stricmp(tag2, "mask") == 0) {
            md->mask = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "limit") == 0) {
            md->limit = *(int32 *)result;
        } else if (stricmp(tag2, "reason") == 0) {
            md->reason = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "who") == 0) {
            strbcpy(md->who, ((TextInfo *)result)->text);
            free(((TextInfo *)result)->text);
        } else if (stricmp(tag2, "time") == 0) {
            md->time = *(time_t *)result;
        } else if (stricmp(tag2, "expires") == 0) {
            md->expires = *(time_t *)result;
        } else if (stricmp(tag2, "lastused") == 0) {
            md->lastused = *(time_t *)result;
        } else {
            error("Warning: Unknown MaskData tag <%s> ignored", tag2);
        }
    }
    if (type < 0) {
        error("Mask data type unrecognized, ignoring entry");
        my_free_maskdata(md);
        return CONTINUE;
    } else if (!md->mask || !*md->mask) {
        error("Mask data entry has no mask, ignoring");
        my_free_maskdata(md);
        return CONTINUE;
    }
    if (!md->reason) {
        md->reason = strdup("<reason unknown>");
        if (!md->reason) {
            error("Out of memory for <%s>", tag);
            my_free_maskdata(md);
        }
    }
    if (!*md->who)
        strbcpy(md->who, "<unknown>");
    if (!md->time) {
        error("Warning: Mask data entry has no creation time, setting to"
              " current time");
        md->time = time(NULL);
    }
    return md;
}

/*************************************************************************/

/* Parse a <ServerStats> tag block, and return a dynamically allocated
 * ServerStats *.
 */

static void *th_serverstats(char *tag, char *attr, char *attrval)
{
    void *result;
    char tag2[256];
    ServerStats *ss;

    ss = malloc(sizeof(*ss));
    if (!ss) {
        error("Out of memory for <%s>", tag);
        return NULL;
    }
    memset(ss, 0, sizeof(*ss));
    while ((result = parse_tag(tag, tag2, NULL, NULL)) != PARSETAG_END) {
        if (result == NULL) {
            my_free_serverstats(ss);
            return NULL;
        } else if (result == CONTINUE) {
            continue;
        } else if (stricmp(tag2, "name") == 0) {
            ss->name = ((TextInfo *)result)->text;
        } else if (stricmp(tag2, "t_join") == 0) {
            ss->t_join = *(time_t *)result;
        } else if (stricmp(tag2, "t_quit") == 0) {
            ss->t_quit = *(time_t *)result;
        } else if (stricmp(tag2, "quit_message") == 0) {
            ss->quit_message = ((TextInfo *)result)->text;
        } else {
            error("Warning: Unknown ServerStats tag <%s> ignored", tag2);
        }
    }
    if (!ss->name || !*ss->name) {
        error("ServerStats entry has no server name, ignoring");
        my_free_serverstats(ss);
        return CONTINUE;
    }
    return ss;
}

/*************************************************************************/
/************************* Overall data handling *************************/
/*************************************************************************/

/* Free read-in data. */

static void free_data(void)
{
    NickGroupInfo *ngi, *ngi2;
    NickInfo *ni, *ni2;
    ChannelInfo *ci, *ci2;
    NewsItem *news, *news2;
    MaskData *md, *md2;
    ServerStats *ss, *ss2;
    int i;

    LIST_FOREACH_SAFE (ngi, ngi_list, ngi2)
        free_nickgroupinfo(ngi);
    ngi_list = NULL;
    LIST_FOREACH_SAFE (ni, ni_list, ni2)
        free_nickinfo(ni);
    ni_list = NULL;
    LIST_FOREACH_SAFE (ci, ci_list, ci2)
        free_channelinfo(ci);
    ci_list = NULL;
    LIST_FOREACH_SAFE (news, news_list, news2)
        my_free_newsitem(news);
    news_list = NULL;
    for (i = 0; i < 256; i++) {
        LIST_FOREACH_SAFE (md, md_list[i], md2)
            my_free_maskdata(md);
        md_list[i] = NULL;
    }
    LIST_FOREACH_SAFE (ss, ss_list, ss2)
        my_free_serverstats(ss);
    ss_list = NULL;
}

/*************************************************************************/

/* Read in all data; return nonzero on success, zero if an error occurred
 * that prevents the data from being merged.
 */

static int read_data(int flags)
{
    int flags_nickcoll = flags & XMLI_NICKCOLL_MASK;
    int flags_chancoll = flags & XMLI_CHANCOLL_MASK;
    NickGroupInfo *ngi_discarded = NULL;  /* for discarded nickgroups */
    int failed = 0;  /* set to 1 on a fatal error */
    void *result;
    char tag[256];

    /* Make sure we start with a clean slate */
    free_data();

    while ((result = parse_tag("ircservices-db", tag, NULL, NULL))
           != PARSETAG_END) {
        if (!result)
            return 0;
        else if (result == CONTINUE)
            continue;

        /* Do something with the returned data */

        if (stricmp(tag, "nickgroupinfo") == 0) {
            NickGroupInfo *ngi = result;
            int i;
            /* Check for nick collisions now if we're in the default mode
             * (discard entire group on collision) or abort-on-collision
             * mode.  For discard-colliding-nick-only mode, we check when
             * we get the NickInfo. */
            if (!flags_nickcoll || flags_nickcoll == XMLI_NICKCOLL_ABORT) {
                ARRAY_FOREACH (i, ngi->nicks) {
                    /* See if the nick already exists in the database */
                    NickInfo *ni = get_nickinfo(ngi->nicks[i]);
                    if (ni) {
                        int j;
                        printf("Line %d: %sImported nick %s collides with %s"
                               " nick, discarding nick group with nicks:",
                               lines_read,
                               flags_nickcoll==XMLI_NICKCOLL_ABORT
                                   ? "" : "Warning: ",
                               ngi->nicks[i],
                               ni->status & NS_VERBOTEN
                                   ? "forbidden" : "registered");
                        /* Let them know what nicks get discarded. */
                        ARRAY_FOREACH (j, ngi->nicks)
                            printf(" %s", ngi->nicks[j]);
                        printf("\n");
                        LIST_INSERT(ngi, ngi_discarded);
                        ngi = NULL;
                        put_nickinfo(ni);
                        if (flags_nickcoll == XMLI_NICKCOLL_ABORT)
                            failed = 1;
                        break;
                    }
                } /* for each nick */
            } /* if default mode or abort mode */
            if (ngi)  /* it might be NULL if we handled it above */
                LIST_INSERT(ngi, ngi_list);

        } else if (stricmp(tag, "nickinfo") == 0) {
            NickInfo *ni = result;
            NickGroupInfo *ngi;
            int i;

            /* Special handling for forbidden nicks (which have no groups) */
            if (!ni->nickgroup) {
                if (flags_nickcoll != XMLI_NICKCOLL_OVERWRITE
                 && put_nickinfo(get_nickinfo(ni->nick))
                ) {
                    free_nickinfo(ni);
                    continue;
                }
                LIST_INSERT(ni, ni_list);
                continue;
            }

            /* Make sure nick has a valid (possibly discarded) nickgroup */
            LIST_SEARCH_SCALAR(ngi_discarded, id, ni->nickgroup, ngi);
            if (ngi) {
                /* It's from a discarded nickgroup, drop it and continue */
                free_nickinfo(ni);
                continue;
            }
            LIST_SEARCH_SCALAR(ngi_list, id, ni->nickgroup, ngi);
            if (!ngi) {
                error("Nick %s has invalid nick group %u, discarding",
                      ni->nick, ni->nickgroup);
                free_nickinfo(ni);
                continue;
            }
            /* Also make sure nickgroup has nick listed */
            ARRAY_SEARCH_PLAIN(ngi->nicks, ni->nick, irc_stricmp, i);
            if (i >= ngi->nicks_count) {
                error("Nick %s not listed in nick array for nick group %u"
                      " (corrupt database?), discarding", ni->nick, ngi->id);
                free_nickinfo(ni);
                continue;
            }
            /* Check for nick collisions if in discard-colliding-nick-only
             * mode. */
            if (flags_nickcoll == XMLI_NICKCOLL_SKIPNICK) {
                NickInfo *ni2 = get_nickinfo(ngi->nicks[i]);
                if (ni2) {
                    error("Warning: Imported nick %s collides with %s nick,"
                          " discarding imported nick", ni->nick,
                          ni2->status & NS_VERBOTEN
                              ? "forbidden" : "registered");
                    free_nickinfo(ni);
                    put_nickinfo(ni2);
                    if (ngi->nicks_count == 1) {
                        /* Only nick in group, delete group */
                        LIST_REMOVE(ngi, ngi_list);
                        free_nickgroupinfo(ngi);
                    } else {
                        /* Delete nick from group's nick list; `i' still
                         * holds nicks[] index from above */
                        ARRAY_REMOVE(ngi->nicks, i);
                        /* Update mainnick if necessary */
                        if (ngi->mainnick > i
                         || (ngi->mainnick == i && i >= ngi->nicks_count))
                            ngi->mainnick--;
                    }
                    /* Continue on to next tag */
                    continue;
                } /* if nick is registered */
            } /* if discard-colliding-nick-only mode */
            /* Everything okay, insert nick into list. */
            LIST_INSERT(ni, ni_list);

        } else if (stricmp(tag, "channelinfo") == 0) {
            ChannelInfo *ci = result;
            if (flags_chancoll != XMLI_CHANCOLL_OVERWRITE) {
                if (put_channelinfo(get_channelinfo(ci->name))) {
                    error("%sImported channel %s collides with %s channel,"
                          " discarding imported channel",
                          flags_chancoll==XMLI_CHANCOLL_ABORT ? "":"Warning: ",
                          ci->name,
                          ci->flags&CF_VERBOTEN ? "forbidden" : "registered");
                    free_channelinfo(ci);
                    ci = NULL;
                    if (flags_chancoll == XMLI_CHANCOLL_ABORT)
                        failed = 1;
                }
            }
            if (ci)
                LIST_INSERT(ci, ci_list);

        } else if (stricmp(tag, "news") == 0) {
            if (ImportNews) {
                /* Don't add if it's a duplicate */
                NewsItem *iterator;
                for (iterator=first_news(); iterator; iterator=next_news()) {
                    if (strcmp(iterator->text,((NewsItem *)result)->text) == 0)
                        break;
                }
                if (iterator)
                    my_free_newsitem(result);
                else
                    add_news(result);
            } else {
                my_free_newsitem(result);
            }

        } else if (stricmp(tag, "maskdata") == 0) {
            MaskData *md = result;
            if (put_maskdata(get_maskdata(md->type, md->mask))) {
                char typebuf[16];
                const char *s;
                switch (md->type) {
                  case MD_AKILL:        s = "autokill";                 break;
                  case MD_EXCEPTION:    s = "session exception";        break;
                  case MD_SGLINE:       s = "SGline";                   break;
                  case MD_SQLINE:       s = "SQline";                   break;
                  case MD_SZLINE:       s = "SZline";                   break;
                  default:
                    snprintf(typebuf, sizeof(typebuf), "type %u", md->type);
                    s = typebuf;
                    break;
                }
                error("MaskData entry for `%s' (%s) already exists in"
                      " database, skipping", md->mask, s);
                my_free_maskdata(md);
            } else {
                LIST_APPEND(md, md_list[md->type]);
            }

        } else if (stricmp(tag, "serverstats") == 0) {
            ServerStats *ss = result;
            if (put_serverstats(get_serverstats(ss->name))) {
                error("ServerStats entry for `%s' already exists in"
                      " database, skipping", ss->name);
                my_free_serverstats(ss);
            } else {
                LIST_INSERT(ss, ss_list);
            }
        }

    } /* while not PARSETAG_END */

    /* Free discarded nickgroups */
    {
        NickGroupInfo *ngi, *ngi2;
        LIST_FOREACH_SAFE (ngi, ngi_discarded, ngi2)
            free_nickgroupinfo(ngi);
    }

    return !failed;
}

/*************************************************************************/

/* Merge read-in data with database. */

static void merge_data(int flags)
{
    NickGroupInfo *ngi, *ngi2;
    NickInfo *ni, *ni2;
    ChannelInfo *ci, *ci2;
    MaskData *md, *md2;
    ServerStats *ss, *ss2;
    int i;

    /* All colliding nick groups have already been removed, so just add all
     * the new ones. */
    LIST_FOREACH_SAFE (ngi, ngi_list, ngi2) {
        uint32 newid = ngi->id;
        while (put_nickgroupinfo(get_nickgroupinfo(newid))) {
            newid++;
            if (newid == 0)
                newid++;
            if (newid == ngi->id) {
                fatal("No available nick group IDs for ID %u in XML import",
                      ngi->id);
            }
        }
        if (newid != ngi->id) {
            if (VerboseImport)
                error("Nick group %u imported as group %u", ngi->id, newid);
            LIST_FOREACH (ni, ni_list) {
                if (ni->nickgroup == ngi->id)
                    ni->nickgroup = newid;
            }
            LIST_FOREACH (ci, ci_list) {
                if (ci->founder == ngi->id)
                    ci->founder = newid;
                if (ci->successor == ngi->id)
                    ci->successor = newid;
                ARRAY_FOREACH (i, ci->access) {
                    if (ci->access[i].nickgroup == ngi->id)
                        ci->access[i].nickgroup = newid;
                }
            }
            ngi->id = newid;
        } else if (VerboseImport) {
            error("Nick group %u imported", ngi->id);
        }
        LIST_REMOVE(ngi, ngi_list);
        add_nickgroupinfo(ngi);
    }

    LIST_FOREACH_SAFE (ni, ni_list, ni2) {
        NickInfo *oldni = get_nickinfo(ni->nick);
        if (oldni) {
            if ((flags & XMLI_NICKCOLL_MASK) == XMLI_NICKCOLL_OVERWRITE) {
                error("Overwriting nick %s", oldni->nick);
                my_delnick(oldni);
            } else {
                fatal("BUG: Colliding nick %s not removed!", ni->nick);
            }
            put_nickinfo(oldni);
        }
        LIST_REMOVE(ni, ni_list);
        add_nickinfo(ni);
        error("Nick %s imported", ni->nick);
    }

    LIST_FOREACH_SAFE (ci, ci_list, ci2) {
        LIST_REMOVE(ci, ci_list);
        if (ci->founder) {
            NickGroupInfo *ngi = get_nickgroupinfo(ci->founder);
            if (!ngi) {
                error("Warning: Founder nickgroup missing for channel %s,"
                      " deleting channel", ci->name);
                free_channelinfo(ci);
                ci = NULL;
            }
            put_nickgroupinfo(ngi);
        }
        if (ci) {
            ChannelInfo *oldci = get_channelinfo(ci->name);
            if (oldci) {
                if ((flags & XMLI_CHANCOLL_MASK) == XMLI_CHANCOLL_OVERWRITE) {
                    error("Overwriting channel %s", oldci->name);
                    del_channelinfo(oldci);  /* also frees */
                } else {
                    fatal("BUG: Colliding nick %s not removed!", ni->nick);
                }
                put_channelinfo(oldci);
            }
            add_channelinfo(ci);
            error("Channel %s imported", ci->name);
        }
    }

    for (i = 0; i < 256; i++) {
        LIST_FOREACH_SAFE (md, md_list[i], md2) {
            MaskData *newmd;  // Because the pointer gets changed
            LIST_REMOVE(md, md_list[i]);
            newmd = add_maskdata(i, md);
            error("Mask data %d/%s imported", i, newmd->mask);
        }
    }

    LIST_FOREACH_SAFE (ss, ss_list, ss2) {
        LIST_REMOVE(ss, ss_list);
        add_serverstats(ss);
        error("ServerStats %s imported", ss->name);
    }
}

/*************************************************************************/
/************************** Main import routine **************************/
/*************************************************************************/

static int xml_import(FILE *f)
{
    char *tag, *attr, *attrval;

    import_file = f;
    bytes_read = 0;
    lines_read = 1;
    const_LANG_DEFAULT      = LANG_DEFAULT;
    const_CHANMAX_UNLIMITED = CHANMAX_UNLIMITED;
    const_CHANMAX_DEFAULT   = CHANMAX_DEFAULT;
    const_TIMEZONE_DEFAULT  = TIMEZONE_DEFAULT;
    const_ACCLEV_FOUNDER    = ACCLEV_FOUNDER;
    const_ACCLEV_INVALID    = ACCLEV_INVALID;
    const_ACCLEV_SOP        = ACCLEV_SOP;
    const_ACCLEV_AOP        = ACCLEV_AOP;
    const_ACCLEV_HOP        = ACCLEV_HOP;
    const_ACCLEV_VOP        = ACCLEV_VOP;
    const_ACCLEV_NOP        = ACCLEV_NOP;
    const_MEMOMAX_UNLIMITED = MEMOMAX_UNLIMITED;
    const_MEMOMAX_DEFAULT   = MEMOMAX_DEFAULT;
    const_NEWS_LOGON        = NEWS_LOGON;
    const_NEWS_OPER         = NEWS_OPER;
    const_MD_AKILL          = MD_AKILL;
    const_MD_EXCEPTION      = MD_EXCEPTION;
    const_MD_SGLINE         = MD_SGLINE;
    const_MD_SQLINE         = MD_SQLINE;
    const_MD_SZLINE         = MD_SZLINE;

    /* Read in initial tag(s) */
    if (read_tag(&tag, &attr, &attrval, NULL, NULL) != 1) {
        error("Error reading initial tag");
        return 0;
    } else if (stricmp(tag, "?xml") == 0) {
        if (attr && stricmp(attr, "version") == 0) {
            char *s = strchr(attrval, '.');
            if (s)
                *s++ = 0;
            if (!s || atoi(attrval) != 1 || atoi(s) != 0) {
                error("Invalid XML version");
                return 0;
            }
        }
        if (read_tag(&tag, &attr, &attrval, NULL, NULL) != 1) {
            error("Error reading initial tag");
            return 0;
        }
    }
    if (stricmp(tag, "ircservices-db") != 0) {
        error("Initial tag is not <ircservices-db>");
        return 0;
    }

    /* Read data */
    if (!read_data(flags)) {
        printf("Import aborted.\n");
        free_data();
        return 0;
    }

    /* Free read_tag() data */
    (void) read_tag(NULL, NULL, NULL, NULL, NULL);

    /* Merge data into our database and free it */
    merge_data(flags);
    free_data();

    /* Return success */
    return 1;
}

/*************************************************************************/

/* Command-line option callback. */

static int do_command_line(const char *option, const char *value)
{
    FILE *f;

    if (!option || strcmp(option, "import") != 0)
        return 0;
    if (!value || !*value) {
        fprintf(stderr, "-import option requires a parameter (filename to"
                " import)\n");
        return 2;
    }
    f = fopen(value, "r");
    if (!f) {
        perror(value);
        return 2;
    }
    if (!xml_import(f))
        return 2;
    return 3;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int do_OnNicknameCollision(const char *filename, int linenum,
                                  const char *value);
static int do_OnChannelCollision(const char *filename, int linenum,
                                 const char *value);

ConfigDirective module_config[] = {
    { "OnChannelCollision",  { { CD_FUNC, 0, do_OnChannelCollision } } },
    { "OnNicknameCollision", { { CD_FUNC, 0, do_OnNicknameCollision } } },
    { "ImportNews",          { { CD_SET, 0, &ImportNews } } },
    { "VerboseImport",       { { CD_SET, 0, &VerboseImport } } },
    { NULL }
};

/*************************************************************************/

static int do_OnNicknameCollision(const char *filename, int linenum,
                                  const char *value)
{
    static int value_to_set = 0;

    if (!value) {
        if (linenum == CDFUNC_SET) {
            flags &= ~XMLI_NICKCOLL_MASK;
            flags |= value_to_set;
        } else if (linenum == CDFUNC_DECONFIG) {
            flags &= ~XMLI_NICKCOLL_MASK;
        }
        return 1;
    }
    if (stricmp(value, "skipgroup") == 0) {
        value_to_set = XMLI_NICKCOLL_SKIPGROUP;
    } else if (stricmp(value, "skipnick") == 0) {
        value_to_set = XMLI_NICKCOLL_SKIPNICK;
    } else if (stricmp(value, "overwrite") == 0) {
        value_to_set = XMLI_NICKCOLL_OVERWRITE;
    } else if (stricmp(value, "abort") == 0) {
        value_to_set = XMLI_NICKCOLL_ABORT;
    } else {
        config_error(filename, linenum,
                     "Invalid setting for OnNicknameCollision: `%s'", value);
        return 0;
    }
    return 1;
}

/*************************************************************************/

static int do_OnChannelCollision(const char *filename, int linenum,
                                 const char *value)
{
    static int value_to_set = 0;

    if (!value) {
        if (linenum == CDFUNC_SET) {
            flags &= ~XMLI_CHANCOLL_MASK;
            flags |= value_to_set;
        } else if (linenum == CDFUNC_DECONFIG) {
            flags &= ~XMLI_CHANCOLL_MASK;
        }
        return 1;
    }
    if (stricmp(value, "skip") == 0) {
        value_to_set = XMLI_CHANCOLL_SKIP;
    } else if (stricmp(value, "overwrite") == 0) {
        value_to_set = XMLI_CHANCOLL_OVERWRITE;
    } else if (stricmp(value, "abort") == 0) {
        value_to_set = XMLI_CHANCOLL_ABORT;
    } else {
        config_error(filename, linenum,
                     "Invalid setting for OnChannelCollision: `%s'", value);
        return 0;
    }
    return 1;
}

/*************************************************************************/

int init_module()
{
    int i, j;

    for (i = 1; tags[i].tag; i++) {
        for (j = 0; j < i; j++) {
            if (stricmp(tags[i].tag, tags[j].tag) == 0)
                module_log("BUG: duplicate entry for tag `%s'", tags[i].tag);
        }
    }

    if (!add_callback(NULL, "command line", do_command_line)) {
        module_log("Unable to add callback");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown)
{
    free_data();
    remove_callback(NULL, "command line", do_command_line);
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
