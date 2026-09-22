/* Multi-language support.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#define LANGSTR_ARRAY  /* define langstrs[] in langstrs.h, via language.h */

#include "services.h"
#include "language.h"

/* Needed for NickGroupInfo structure definition (used by getstring() and
 * strftime_lang()) */
#include "modules/nickserv/nickserv.h"

/*************************************************************************/

/* Indexes of available languages (exported), terminated by -1: */
int langlist[NUM_LANGS+1];

/* List and count of available strings: */
static char **langstrs;
int num_strings;

/* The list of lists of messages. */
static char **langtexts[NUM_LANGS];
/* Pointers to the original data, in case external files are loaded later. */
static char **origtexts[NUM_LANGS];

/* Order in which languages should be displayed: (alphabetical in English) */
static int langorder[] = {
    LANG_EN_US,         /* English (US) */
    LANG_NL,            /* Dutch */
    LANG_FR,            /* French */
    LANG_DE,            /* German */
    LANG_HU,            /* Hungarian */
/*    LANG_IT,*/        /* Italian */
    LANG_JA_EUC,        /* Japanese (EUC encoding) */
    LANG_JA_SJIS,       /* Japanese (SJIS encoding) */
/*    LANG_PT,*/        /* Portugese */
    LANG_RU,            /* Russian */
    LANG_ES,            /* Spanish */
    LANG_TR,            /* Turkish */
};

/* Filenames for language files: */
static struct {
    int num;
    const char *filename;
} filenames[] = {
    { LANG_EN_US,       "en_us" },
    { LANG_NL,          "nl" },
    { LANG_FR,          "fr" },
    { LANG_DE,          "de" },
    { LANG_HU,          "hu" },
    { LANG_IT,          "it" },
    { LANG_JA_EUC,      "ja_euc" },
    { LANG_JA_SJIS,     "ja_sjis" },
    { LANG_PT,          "pt" },
    { LANG_RU,          "ru" },
    { LANG_ES,          "es" },
    { LANG_TR,          "tr" },
    { -1, NULL }
};

/* Mapping of language strings (to allow on-the-fly replacement of strings) */
static int *langmap;

/* Array indicating which languages were actually loaded (needed since NULL
 * langtexts[] pointers are redirected to DEF_LANGUAGE) */
static int is_loaded[NUM_LANGS];

/* Index of the first extra (non-base) string */
#define FIRST_EXTRA_STRING      (NUM_BASE_STRINGS)
/* Is the given string index a base string index? */
#define IS_BASE_STRING(n)       ((n) < FIRST_EXTRA_STRING)

/*************************************************************************/
/*************************************************************************/

/* Helper functions. */

static inline int read_int32(int32 *ptr, FILE *f)
{
    int a = fgetc(f);
    int b = fgetc(f);
    int c = fgetc(f);
    int d = fgetc(f);
    if (a == EOF || b == EOF || c == EOF || d == EOF)
        return -1;
    *ptr = a<<24 | b<<16 | c<<8 | d;
    return 0;
}

static inline int read_uint32(uint32 *ptr, FILE *f)
{
    return read_int32((int32 *)ptr, f);
}

/*************************************************************************/
/*************************************************************************/

/* Load a language file, storing the data in origtexts[index]. */

static void load_lang(int index, const char *filename)
{
    char buf[256];
    FILE *f;
    uint32 num, size, i;
    char *data = NULL;

    log_debug(1, "Loading language %d from file `languages/%s'",
              index, filename);
    snprintf(buf, sizeof(buf), "languages/%s", filename);
    if (!(f = fopen(buf, "r"))) {
        log_perror("Failed to load language %d (%s)", index, filename);
        return;
    } else if (read_uint32(&num, f) < 0) {
        log("Failed to read number of strings for language %d (%s)",
            index, filename);
        return;
    } else if (read_uint32(&size, f) < 0) {
        log("Failed to read data size for language %d (%s)",
            index, filename);
        return;
    } else if (num != NUM_BASE_STRINGS) {
        log("Warning: Bad number of strings (%d, wanted %d) "
            "for language %d (%s)", num, NUM_BASE_STRINGS, index, filename);
    }
    origtexts[index] = scalloc(sizeof(char *), NUM_BASE_STRINGS+1);
    if (num > NUM_BASE_STRINGS)
        num = NUM_BASE_STRINGS;
    origtexts[index][0] = data = smalloc(size+4);
    *((uint32 *)data) = size;
    data += 4;
    if (fread(data, size, 1, f) != 1) {
        log("Failed to read language data for language %d (%s)",
            index, filename);
        goto fail;
    }
    for (i = 0; i < num; i++) {
        int32 pos;
        if (read_int32(&pos, f) < 0) {
            log("Failed to read entry %d in language %d (%s) TOC",
                i, index, filename);
            goto fail;
        }
        if (pos == -1) {
            origtexts[index][i+1] = NULL;
        } else {
            origtexts[index][i+1] = data + pos;
        }
    }
    fclose(f);
    is_loaded[index] = 1;
    return;

  fail:
    free(data);
    free(origtexts[index]);
    origtexts[index] = NULL;
    return;
}

/*************************************************************************/

/* Initialize list of lists. */

int lang_init(void)
{
    int i, j, n = 0;

    /* Set up the string list */
    langstrs = malloc(sizeof(char *) * NUM_BASE_STRINGS);
    if (!langstrs) {
        log_perror("malloc(langstrs)");
        return 0;
    }
    memcpy(langstrs, base_langstrs, sizeof(char *) * NUM_BASE_STRINGS);
    num_strings = NUM_BASE_STRINGS;

    /* Set up the string remap list */
    langmap = smalloc(sizeof(int) * num_strings);
    for (i = 0; i < num_strings; i++)
        langmap[i] = i;

    /* Load language files */
    memset(is_loaded, 0, sizeof(is_loaded));
    for (i = 0; i < lenof(langorder); i++) {
        for (j = 0; filenames[j].num >= 0; j++) {
            if (filenames[j].num == langorder[i])
                break;
        }
        if (filenames[j].num >= 0) {
            load_lang(langorder[i], filenames[j].filename);
        } else {
            log("BUG: lang_init(): no filename entry for language %d!",
                langorder[i]);
        }
    }

    /* Make sure the default language has all strings available */
    if (!origtexts[DEF_LANGUAGE]) {
        log("Unable to load default language");
        return 0;
    }
    for (i = 0; i < num_strings; i++) {
        if (!origtexts[DEF_LANGUAGE][i+1]) {
            if (is_loaded[LANG_EN_US] && origtexts[LANG_EN_US][i+1]) {
                uint32 oldsize = *((uint32 *)origtexts[DEF_LANGUAGE][0]);
                uint32 newsize = strlen(origtexts[LANG_EN_US][i+1]) + 1;
                origtexts[DEF_LANGUAGE][0] =
                    realloc(origtexts[DEF_LANGUAGE][0], oldsize + newsize + 4);
                if (!origtexts[DEF_LANGUAGE][0]) {
                    log("Out of memory while loading languages");
                    return 0;
                }
                *((uint32 *)origtexts[DEF_LANGUAGE][0]) = oldsize + newsize;
                origtexts[DEF_LANGUAGE][i+1] =
                    origtexts[DEF_LANGUAGE][0] + 4 + oldsize;
                strcpy(origtexts[DEF_LANGUAGE][i+1],
                       origtexts[LANG_EN_US][i+1]);
            } else {
                log("String %s missing from default language", langstrs[i]);
                return 0;
            }
        }
    }

    /* Set up the list of available languages in langlist[] */
    for (i = 0; i < lenof(langorder); i++) {
        if (is_loaded[langorder[i]])
            langlist[n++] = langorder[i];
    }
    while (n < lenof(langlist))
        langlist[n++] = -1;

    /* Initialize the active string tables in langtexts[] */
    reset_ext_lang();

    return 1;
}

/*************************************************************************/

/* Clean up language data. */

void lang_cleanup(void)
{
    int i;

    for (i = 0; i < NUM_LANGS; i++) {
        if (langtexts[i]) {
            free(langtexts[i][0]);
            free(langtexts[i]);
            langtexts[i] = NULL;
        }
        if (origtexts[i]) {
            free(origtexts[i][0]);
            free(origtexts[i]);
            langtexts[i] = NULL;
        }
    }
    free(langmap);
    free(langstrs);
}

/*************************************************************************/

/* Load an external language file.  External language files are formatted
 * the same way as the standard language files, except that a language name
 * follows the string name; for example:
 *
 * UNKNOWN_COMMAND<lwsp+>en_us<lwsp*><nl>
 * <tab>The command %s is not recognized.<nl>
 *
 * where <lwsp> is "linear white space" (space or tab); <nl> is either LF
 * or CRLF; and <tab> is the tab character.  Empty lines and lines
 * beginning with '#' are ignored.  Note that any lines longer than
 * BUFSIZE-1 characters (including the trailing <nl>) will be truncated.
 *
 * Returns 0 on failure, nonzero on success.
 */

int load_ext_lang(const char *filename)
{
    FILE *f;
    char buf[BUFSIZE], *s;
    char **newtexts[NUM_LANGS];
    uint32 newsizes[NUM_LANGS];
    int i, curstr, curlang, line;
    int retval = 1, firstline = 1;

    memset(newtexts, 0, sizeof(newtexts));
    memset(newsizes, 0, sizeof(newsizes));
    f = fopen(filename, "r");
    if (!f) {
        log_perror("load_ext_lang(): Unable to open file %s", filename);
        return 0;
    }

    curstr = curlang = -1;
    line = 0;
    while (fgets(buf, sizeof(buf), f) && *buf) {
        line++;
        s = buf + strlen(buf) - 1;
        if (*s == '\r') {  /* in case we get half a CRLF */
            *s = fgetc(f);
            if (*s != '\n') {
                ungetc(*s, f);
                *s = '\r';
            }
        }
        if (*s == '\n') {
            *s = 0;
            if (s > buf && s[-1] == '\r')
                s[-1] = 0;
        } else {
            char buf2[BUFSIZE];
            log("load_ext_lang(): %s:%d: Line too long (maximum %d"
                " characters)", filename, line, sizeof(buf)-2);
            retval = 0;
            while (fgets(buf2, sizeof(buf2), f)
                   && *buf2 && buf2[strlen(buf2)-1] != '\n')
                ;
        }
        if (*buf == '#')
            continue;
        if (*buf != '\t') {
            curstr = curlang = -1;
            s = strtok(buf, " \t\r\n");
            if (!s)
                continue;  /* empty line */
            if ((curstr = lookup_string(s)) < 0) {
                log("load_ext_lang: %s:%d: Unknown string ID `%s'",
                    filename, line, s);
                retval = 0;
            } else {
                s = strtok(NULL, " \t\r\n");
                if (!s) {
                    log("load_ext_lang: %s:%d: Missing language ID\n",
                        filename, line);
                    retval = 0;
                    curstr = -1;
                } else if ((curlang = lookup_language(s)) < 0) {
                    log("load_ext_lang: %s:%d: Unknown language ID `%s'",
                        filename, line, s);
                    retval = 0;
                    curstr = -1;
                } else if (!is_loaded[curlang]) {
                    log("load_ext_lang: %s:%d: Language `%s' is not"
                        " available", filename, line, s);
                    retval = 0;
                    curstr = -1;
                }
            }
            if (curstr >= 0 && curlang >= 0) {
                /* Valid string/language ID--set up storage space if not
                 * yet done */
                if (!newtexts[curlang]) {
                    newtexts[curlang] = calloc(num_strings+1, sizeof(char *));
                    if (!newtexts[curlang]) {
                        log_perror("load_ext_lang: %s:%d", filename, line);
                        goto fail;
                    }
                    newsizes[curlang] = 0;
                }
                /* Point to location of string in data buffer; we add 1
                 * to the offset to differentiate the value from 0 (not
                 * present) */
                newtexts[curlang][curstr+1] = (char *)newsizes[curlang] + 1;
                /* Set first-line flag (signals whether to insert a
                 * newline) */
                firstline = 1;
            }
        } else {  /* line begins with tab -> text line */
            if (curstr >= 0 && curlang >= 0) {
                int oldsize = newsizes[curlang];
                if (!firstline)
                    oldsize--;  // overwrite the trailing \0 with a newline
                newsizes[curlang] += strlen(buf+1)+1;  // skip initial tab
                newtexts[curlang][0] =
                    realloc(newtexts[curlang][0], newsizes[curlang]);
                sprintf(newtexts[curlang][0]+oldsize, "%s%s",
                        firstline ? "" : "\n", buf+1);
                firstline = 0;
            }
        }
    }  /* for each line */
    fclose(f);

    if (retval) {
        /* Success, install new strings.  First set up new string arrays in
         * newtexts[], then, when all succeeds (i.e. no ENOMEM), move the
         * new data to langtexts[]. */
        for (curlang = 0; curlang < NUM_LANGS; curlang++) {
            char *newbuf;
            uint32 oldlen, newlen;
            if (!newtexts[curlang])
                continue;
            oldlen = *((uint32 *)langtexts[curlang][0]);
            newlen = newsizes[curlang];
            newbuf = malloc(4 + oldlen + newlen);
            if (!newbuf) {
                log_perror("load_ext_lang: %s:%d", filename, line);
                goto fail;
            }
            *((uint32 *)newbuf) = oldlen + newlen;
            memcpy(newbuf+4, langtexts[curlang][0]+4, oldlen);
            memcpy(newbuf+4+oldlen, newtexts[curlang][0], newlen);
            free(newtexts[curlang][0]);
            newtexts[curlang][0] = newbuf;
            for (i = 0; i < num_strings; i++) {
                if (newtexts[curlang][i+1]) {
                    int ofs = (int)newtexts[curlang][i+1] - 1;
                    newtexts[curlang][i+1] = newbuf+4 + oldlen + ofs;
                } else if (langtexts[curlang][i+1]) {
                    int ofs = langtexts[curlang][i+1]
                              - langtexts[curlang][0];
                    newtexts[curlang][i+1] = newbuf + ofs;
                } else {
                    newtexts[curlang][i+1] = NULL;
                }
            }
        }  /* for each language */
        /* All okay, actually install the data */
        for (curlang = 0; curlang < NUM_LANGS; curlang++) {
            if (!newtexts[curlang])
                continue;
            free(langtexts[curlang][0]);
            free(langtexts[curlang]);
            langtexts[curlang] = newtexts[curlang];
        }
    }  /* if (retval) */

    return retval;

  fail:
    for (curlang = 0; curlang < NUM_LANGS; curlang++) {
        if (newtexts[curlang]) {
            free(newtexts[curlang][0]);
            free(newtexts[curlang]);
        }
    }
    fclose(f);
    return 0;
}

/*************************************************************************/

/* Clear all data loaded from external language files or set with
 * setstring(), restoring to the default data sets.  Strings added with
 * addstring() are retained but reset to empty.
 */

void reset_ext_lang(void)
{
    int i, j;

    for (i = 0; i < NUM_LANGS; i++) {
        if (is_loaded[i]) {
            uint32 datasize = *((uint32 *)origtexts[i][0]);
            if (langtexts[i]) {
                free(langtexts[i][0]);
                free(langtexts[i]);
            }
            langtexts[i] = smalloc(sizeof(char *) * (num_strings+1));
            langtexts[i][0] = smalloc(datasize+4);
            memcpy(langtexts[i][0], origtexts[i][0], datasize+4);
            for (j = 0; j < num_strings; j++) {
                if (IS_BASE_STRING(j)) {
                    if (origtexts[i][j+1]) {
                        langtexts[i][j+1] = origtexts[i][j+1]
                                          - origtexts[i][0]
                                          + langtexts[i][0];
                    } else {
                        langtexts[i][j+1] = NULL;
                    }
                } else if (i == DEF_LANGUAGE) {
                    langtexts[i][j+1] = langtexts[i][0] + datasize - 1;
                } else {
                    langtexts[i][j+1] = NULL;
                }
            }
        }
    }
}

/*************************************************************************/
/*************************************************************************/

/* Return the language number for the given language name.  If the language
 * is not found, returns -1.
 */

int lookup_language(const char *name)
{
    int i;

    for (i = 0; filenames[i].num >= 0; i++) {
        if (stricmp(filenames[i].filename, name) == 0)
            return filenames[i].num;
    }
    return -1;
}

/*************************************************************************/

/* Return true if the given language is loaded, false otherwise. */

int have_language(int language)
{
    return language >= 0 && language < NUM_LANGS && is_loaded[language];
}

/*************************************************************************/

/* Return the index of the given string name.  If the name is not found,
 * returns -1.  Note that string names are case sensitive.
 */

int lookup_string(const char *name)
{
    int i;

    for (i = 0; i < num_strings; i++) {
        if (strcmp(langstrs[i], name) == 0)
            return i;
    }
    return -1;
}

/*************************************************************************/

/* Retrieve a message text using the language selected for the given
 * NickGroupInfo (if NickGroupInfo is NULL, use DEF_LANGUAGE).
 */

const char *getstring(const NickGroupInfo *ngi, int index)
{
    int language;
    const char *text;

    if (index < 0 || index >= num_strings) {
        log("getstring(): BUG: index (%d) out of range!", index);
        return NULL;
    }
    language = (ngi && ngi != NICKGROUPINFO_INVALID
                    && ngi->language != LANG_DEFAULT)
               ? ngi->language
               : DEF_LANGUAGE;
    text = langtexts[language][langmap[index]+1];
    if (!text)
        text = langtexts[DEF_LANGUAGE][langmap[index]+1];
    return text;
}

/*************************************************************************/

/* Retrieve a message text using the given language. */

const char *getstring_lang(int language, int index)
{
    const char *text;

    if (language < 0 || language >= NUM_LANGS) {
        log("getstring_lang(): BUG: language (%d) out of range!", language);
        language = DEF_LANGUAGE;
    } else if (index < 0 || index >= num_strings) {
        log("getstring_lang(): BUG: index (%d) out of range!", index);
        return NULL;
    }
    text = langtexts[language][langmap[index]+1];
    if (!text)
        text = langtexts[DEF_LANGUAGE][langmap[index]+1];
    return text;
}

/*************************************************************************/

/* Set the contents of the given string in the given language.  Returns
 * nonzero on success, zero on error (invalid parameters or language not
 * available).  The text is copied to a separate location, so does not need
 * to be saved by the caller.
 */

int setstring(int language, int index, const char *text)
{
    uint32 oldlen, newlen;
    char *oldtext, *newtext;
    int i;

    if (language < 0 || language >= NUM_LANGS) {
        log("setstring(): language (%d) out of range!", language);
        return 0;
    } else if (index < 0 || index >= num_strings) {
        log("setstring(): index (%d) out of range!", index);
        return 0;
    } else if (IS_BASE_STRING(index)) {
        log("setstring(): index (%d) is a base string and cannot be set",
            index);
        return 0;
    } else if (!text) {
        log("setstring(): text is NULL!");
        return 0;
    } else if (!is_loaded[language]) {
        log_debug(1, "setstring(): language %d not available", language);
        return 0;
    }
    oldtext = langtexts[language][0];
    oldlen = *((uint32 *)oldtext);
    newlen = oldlen + strlen(text) + 1;
    newtext = malloc(newlen + 4);
    if (!newtext) {
        log("setstring(): out of memory");
        return 0;
    }
    *((uint32 *)newtext) = newlen;
    memcpy(newtext+4, oldtext+4, oldlen);
    strcpy(newtext+4+oldlen, text);
    for (i = 0; i < num_strings; i++) {
        if (i == index) {
            langtexts[language][i+1] = newtext+4 + oldlen;
        } else {
            int ofs = langtexts[language][i+1] - oldtext;
            langtexts[language][i+1] = newtext + ofs;
        }
    }
    langtexts[language][0] = newtext;
    free(oldtext);
    return 1;
}

/*************************************************************************/

/* Map string number `old' to number `new' and return the number of the
 * message that used to be stored there.  Returns -1 on error (invalid
 * parameter).
 */

int mapstring(int old, int new)
{
    int prev;

    if (old < 0 || old >= num_strings) {
        log("mapstring(): old string index (%d) is out of range!", old);
        return -1;
    } else if (new < 0 || new >= num_strings) {
        log("mapstring(): new string index (%d) is out of range!", new);
        return -1;
    }
    prev = langmap[old];
    langmap[old] = langmap[new];
    return prev;
}

/*************************************************************************/

/* Add a new string to the global list with the given (non-empty) name.
 * The string is initially empty in all languages.  If the string has
 * already been added, clears the string's text but does not add a new copy
 * of the string.  Returns the string index on success, -1 on failure
 * (invalid parameters or attempting to re-add a base string).
 */

int addstring(const char *name)
{
    int index, i;
    char **new_langstrs, **new_langtexts[NUM_LANGS];
    int *new_langmap;

    if (!name) {
        log("addstring(): BUG: name is NULL!");
        return -1;
    }
    if (!*name) {
        log("addstring(): name is the empty string!");
        return -1;
    }
    if ((index = lookup_string(name)) >= 0) {
        if (IS_BASE_STRING(index)) {
            log("addstring(): attempt to re-add base string `%s'!", name);
            return -1;
        } else {
            return index;
        }
    }
    new_langstrs = malloc(sizeof(char *) * (num_strings+1));
    new_langmap = malloc(sizeof(int) * (num_strings+1));
    if (!new_langstrs || !new_langmap) {
        log("addstring(): Out of memory!");
        free(new_langmap);
        free(new_langstrs);
        return -1;
    }
    memcpy(new_langstrs, langstrs, sizeof(char *) * num_strings);
    memcpy(new_langmap, langmap, sizeof(int) * num_strings);
    new_langstrs[num_strings] = strdup(name);
    if (!new_langstrs[num_strings]) {
        log("addstring(): Out of memory!");
        free(new_langmap);
        free(new_langstrs);
        return -1;
    }
    new_langmap[num_strings] = num_strings;
    for (i = 0; i < NUM_LANGS; i++) {
        if (!is_loaded[i])
            continue;
        new_langtexts[i] = malloc(sizeof(char *) * (num_strings+2));
        if (!new_langtexts[i]) {
            log("addstring(): Out of memory!");
            while (--i >= 0)
                free(new_langtexts[i]);
            free(new_langstrs[num_strings]);
            free(new_langmap);
            free(new_langstrs);
            return -1;
        }
        memcpy(new_langtexts[i], langtexts[i],
               sizeof(char *) * (num_strings+1));
        if (i == DEF_LANGUAGE) {
            /* Point at the \0 ending the last string in the text buffer */
            new_langtexts[i][num_strings+1] =
                new_langtexts[i][0]+4 + *((uint32 *)new_langtexts[i][0]) - 1;
        } else {
            new_langtexts[i][num_strings+1] = NULL;
        }
    }
    free(langstrs);
    langstrs = new_langstrs;
    free(langmap);
    langmap = new_langmap;
    for (i = 0; i < NUM_LANGS; i++) {
        if (is_loaded[i]) {
            free(langtexts[i]);
            langtexts[i] = new_langtexts[i];
        }
    }
    return num_strings++;
}

/*************************************************************************/
/*************************************************************************/

/* Format a string in a strftime()-like way, but heed the nick group's
 * language setting for month and day names and adjust the time for the
 * nick group's time zone setting.  The string stored in the buffer will
 * always be null-terminated, even if the actual string was longer than the
 * buffer size.  Also note that the format parameter is a message number
 * rather than a literal string, and the time parameter is a time_t, not a
 * struct tm *.
 * Assumption: No month or day name has a length (including trailing null)
 * greater than BUFSIZE or contains the '%' character.
 */

int strftime_lang(char *buf, int size, const NickGroupInfo *ngi,
                  int format, time_t time)
{
    int ngi_is_valid = (ngi && ngi != NICKGROUPINFO_INVALID);
    int language = (ngi_is_valid && ngi->language != LANG_DEFAULT)
                   ? ngi->language
                   : DEF_LANGUAGE;
    char tmpbuf[BUFSIZE], buf2[BUFSIZE];
    char *s;
    int i, ret;
    struct tm *tm;

    strbcpy(tmpbuf, getstring_lang(language,format));
    if (ngi_is_valid && ngi->timezone != TIMEZONE_DEFAULT) {
        time += ngi->timezone*60;
        tm = gmtime(&time);
        /* Remove "%Z" (timezone) specifiers */
        while ((s = strstr(tmpbuf, "%Z")) != NULL) {
            char *end = s+2;
            while (s > tmpbuf && s[-1] == ' ')
                s--;
            strmove(s, end);
        }
    } else {
        tm = localtime(&time);
    }
    if ((s = langtexts[language][STRFTIME_DAYS_SHORT+1]) != NULL) {
        for (i = 0; i < tm->tm_wday; i++)
            s += strcspn(s, "\n")+1;
        i = strcspn(s, "\n");
        strncpy(buf2, s, i);
        buf2[i] = 0;
        strnrepl(tmpbuf, sizeof(tmpbuf), "%a", buf2);
    }
    if ((s = langtexts[language][STRFTIME_DAYS_LONG+1]) != NULL) {
        for (i = 0; i < tm->tm_wday; i++)
            s += strcspn(s, "\n")+1;
        i = strcspn(s, "\n");
        strncpy(buf2, s, i);
        buf2[i] = 0;
        strnrepl(tmpbuf, sizeof(tmpbuf), "%A", buf2);
    }
    if ((s = langtexts[language][STRFTIME_MONTHS_SHORT+1]) != NULL) {
        for (i = 0; i < tm->tm_mon; i++)
            s += strcspn(s, "\n")+1;
        i = strcspn(s, "\n");
        strncpy(buf2, s, i);
        buf2[i] = 0;
        strnrepl(tmpbuf, sizeof(tmpbuf), "%b", buf2);
    }
    if ((s = langtexts[language][STRFTIME_MONTHS_LONG+1]) != NULL) {
        for (i = 0; i < tm->tm_mon; i++)
            s += strcspn(s, "\n")+1;
        i = strcspn(s, "\n");
        strncpy(buf2, s, i);
        buf2[i] = 0;
        strnrepl(tmpbuf, sizeof(tmpbuf), "%B", buf2);
    }
    ret = strftime(buf, size, tmpbuf, tm);
    if (ret >= size)  // buffer overflow, should be impossible
        return 0;
    return ret;
}

/*************************************************************************/

/* Generates a string describing the given length of time to one unit
 * (e.g. "3 days" or "10 hours"), or two units (e.g. "5 hours 25 minutes")
 * if the MT_DUALUNIT flag is specified.  The minimum resolution is one
 * minute, unless the MT_SECONDS flag is specified; the returned time is
 * rounded up if in the minimum unit, else rounded to the nearest integer.
 * The returned buffer is a static buffer which will be overwritten on the
 * next call to this routine.
 *
 * The MT_* flags (passed in the `flags' parameter) are defined in
 * language.h.
 */

char *maketime(const NickGroupInfo *ngi, time_t time, int flags)
{
    static char buf[BUFSIZE];
    int unit;

    if (time < 1)  /* Enforce a minimum of one second */
        time = 1;

    if ((flags & MT_SECONDS) && time <= 59) {

        unit = (time==1 ? STR_SECOND : STR_SECONDS);
        snprintf(buf, sizeof(buf), "%ld%s", (long)time, getstring(ngi,unit));

    } else if (!(flags & MT_SECONDS) && time <= 59*60) {

        time = (time+59) / 60;
        unit = (time==1 ? STR_MINUTE : STR_MINUTES);
        snprintf(buf, sizeof(buf), "%ld%s", (long)time, getstring(ngi,unit));


    } else if (flags & MT_DUALUNIT) {

        time_t time2;
        int unit2;

        if (time <= 59*60+59) {  /* 59 minutes, 59 seconds */
            time2 = time % 60;
            unit2 = (time2==1 ? STR_SECOND : STR_SECONDS);
            time = time / 60;
            unit = (time==1 ? STR_MINUTE : STR_MINUTES);
        } else if (time <= (23*60+59)*60+30) {  /* 23 hours, 59.5 minutes */
            time = (time+30) / 60;
            time2 = time % 60;
            unit2 = (time2==1 ? STR_MINUTE : STR_MINUTES);
            time = time / 60;
            unit = (time==1 ? STR_HOUR : STR_HOURS);
        } else {
            time = (time+(30*60)) / (60*60);
            time2 = time % 24;
            unit2 = (time2==1 ? STR_HOUR : STR_HOURS);
            time = time / 24;
            unit = (time==1 ? STR_DAY : STR_DAYS);
        }
        if (time2)
            snprintf(buf, sizeof(buf), "%ld%s%s%ld%s", (long)time,
                     getstring(ngi,unit), getstring(ngi,STR_TIMESEP),
                     (long)time2, getstring(ngi,unit2));
        else
            snprintf(buf, sizeof(buf), "%ld%s", (long)time,
                     getstring(ngi,unit));

    } else {  /* single unit */

        if (time <= 59*60+30) {  /* 59 min 30 sec; MT_SECONDS known true */
            time = (time+30) / 60;
            unit = (time==1 ? STR_MINUTE : STR_MINUTES);
        } else if (time <= (23*60+30)*60) {  /* 23 hours, 30 minutes */
            time = (time+(30*60)) / (60*60);
            unit = (time==1 ? STR_HOUR : STR_HOURS);
        } else {
            time = (time+(12*60*60)) / (24*60*60);
            unit = (time==1 ? STR_DAY : STR_DAYS);
        }
        snprintf(buf, sizeof(buf), "%ld%s", (long)time, getstring(ngi,unit));

    }

    return buf;
}

/*************************************************************************/

/* Generates a description for the given expiration time in the form of
 * days, hours, minutes, seconds and/or a combination thereof.  May also
 * return "does not expire" or "already expired" messages if the expiration
 * time given is zero or earlier than the current time, respectively.
 * String is truncated if it would exceed `size' bytes (including trailing
 * null byte).
 */

void expires_in_lang(char *buf, int size, const NickGroupInfo *ngi,
                     time_t expires)
{
    time_t seconds = expires - time(NULL);

    if (expires == 0) {
        strscpy(buf, getstring(ngi,EXPIRES_NONE), size);
    } else if (seconds <= 0 && noexpire) {
        strscpy(buf, getstring(ngi,EXPIRES_NOW), size);
    } else {
        if (seconds <= 0) {
            /* Already expired--it will be cleared out by the next get() */
            seconds = 1;
        }
        snprintf(buf, size, getstring(ngi,EXPIRES_IN),
                 maketime(ngi,seconds,MT_DUALUNIT));
    }
}

/*************************************************************************/
/*************************************************************************/

/* Send a syntax-error message to the user. */

void syntax_error(const char *service, const User *u, const char *command,
                  int msgnum)
{
    char buf[BUFSIZE];
    snprintf(buf, sizeof(buf), getstring(u->ngi, msgnum), command);
    notice_lang(service, u, SYNTAX_ERROR, buf);
    notice_lang(service, u, MORE_INFO, service, command);
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
