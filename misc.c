/* Miscellaneous routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"

#ifdef CONVERT_DB
# undef log
# define log(...) fprintf(stderr, __VA_ARGS__)
#endif

/*************************************************************************/

/* Table used by irc_tolower().  This table follows the RFC 1459
 * requirement that [ \ ] and { | } be considered equivalent.  Protocols
 * which do not follow this requirement should modify the table
 * accordingly.
 */

unsigned char irc_lowertable[256] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
    0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,

    0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x5E,0x5F,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,

    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
    0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,

    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
    0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
    0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
    0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
    0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,
};


/*************************************************************************/

/* irc_tolower:  Like toupper/tolower, but for nicknames and channel names;
 *               the RFC requires that '[' and '{', '\' and '|', ']' and '}'
 *               be pairwise equivalent in such names.  Declared inline for
 *               irc_str[n]icmp()'s benefit.
 */

#undef irc_tolower
inline unsigned char irc_tolower(char c)
{
    return irc_lowertable[(uint8)c];
}

/*************************************************************************/

/* irc_str[n]icmp:  Like str[n]icmp, but for nicknames and channel names. */

int irc_stricmp(const char *s1, const char *s2)
{
    register char c1, c2;

    while ((c1 = (char)irc_tolower(*s1)) == (c2 = (char)irc_tolower(*s2))) {
        if (c1 == 0)
            return 0;
        s1++;
        s2++;
    }
    return c1<c2 ? -1 : 1;
}

int irc_strnicmp(const char *s1, const char *s2, int max)
{
    register char c1, c2;

    if (max <= 0)
        return 0;
    while ((c1 = (char)irc_tolower(*s1)) == (c2 = (char)irc_tolower(*s2))) {
        if (c1 == 0 || --max <= 0)
            return 0;
        s1++;
        s2++;
    }
    return c1<c2 ? -1 : 1;
}

/*************************************************************************/

/* strscpy:  Copy at most len-1 characters from a string to a buffer, and
 *           add a null terminator after the last character copied.
 *           The `strbcpy(d,s)' variant (defined as a macro in extern.h)
 *           assumes `d' is a character array and uses sizeof(d) as the
 *           `len' parameter.
 */

char *strscpy(char *d, const char *s, size_t len)
{
    char *d_orig = d;

    if (!len)
        return d;
    while (--len && (*d++ = *s++))
        ;
    *d = 0;
    return d_orig;
}

/*************************************************************************/

/* strmove:  Like strcpy(), but handles overlapping regions of memory
 *           properly.  The str*() analog of memmove().
 */

char *strmove(char *d, const char *s)
{
    memmove(d, s, strlen(s)+1);
    return d;
}

/*************************************************************************/

/* stristr:  Search case-insensitively for string s2 within string s1,
 *           returning the first occurrence of s2 or NULL if s2 was not
 *           found.
 */

char *stristr(const char *s1, const char *s2)
{
    register const char *s = s1, *d = s2;

    while (*s1) {
        if (tolower(*s1) == tolower(*d)) {
            s1++;
            d++;
            if (*d == 0)
                return (char *)s;
        } else {
            s = ++s1;
            d = s2;
        }
    }
    return NULL;
}

/*************************************************************************/

/* strupper, strlower:  Convert a string to upper or lower case.
 */

char *strupper(char *s)
{
    char *t = s;
    while (*t) {
        *t = toupper(*t);
        t++;
    }
    return s;
}

char *strlower(char *s)
{
    char *t = s;
    while (*t) {
        *t = tolower(*t);
        t++;
    }
    return s;
}

/*************************************************************************/

/* strnrepl:  Replace occurrences of `old' with `new' in string `s'.  Stop
 *            replacing if a replacement would cause the string to exceed
 *            `size' bytes (including the null terminator).  Return the
 *            string.
 */

char *strnrepl(char *s, int32 size, const char *old, const char *new)
{
    char *ptr = s;
    int32 left = strlen(s);
    int32 avail = size - (left+1);
    int32 oldlen = strlen(old);
    int32 newlen = strlen(new);
    int32 diff = newlen - oldlen;

    if (avail < 0)  /* silly parameter */
        avail = 0;
    while (left >= oldlen) {
        if (strncmp(ptr, old, oldlen) != 0) {
            left--;
            ptr++;
            continue;
        }
        if (diff > avail)
            break;
        if (diff != 0)
            memmove(ptr+oldlen+diff, ptr+oldlen, left+1);
        strncpy(ptr, new, newlen);
        ptr += newlen;
        left -= oldlen;
    }
    return s;
}

/*************************************************************************/

/* strtok_remaining:  Return the result of strtok(NULL, "") with any
 *                    leading or trailing whitespace discarded.  If nothing
 *                    but whitespace is left, return NULL.
 */

char *strtok_remaining(void)
{
    char *s = strtok(NULL, ""), *t;
    if (s) {
        while (isspace(*s))
            s++;
        t = s + strlen(s)-1;
        while (t >= s && isspace(*t))
            *t-- = 0;
        if (!*s)
            return NULL;
    }
    return s;
}

/*************************************************************************/
/*************************************************************************/

/* merge_args:  Take an argument count and argument vector and merge them
 *              into a single string in which each argument is separated by
 *              a space.  The returned string is stored in a static buffer
 *              which will be overwritten on the next call.
 */

char *merge_args(int argc, char **argv)
{
    int i;
    static char s[4096];
    char *t;

    t = s;
    for (i = 0; i < argc; i++) {
        t += snprintf(t, sizeof(s)-(t-s), "%s%s", *argv++,
                      (i<argc-1) ? " " : "");
    }
    return s;
}

/*************************************************************************/
/*************************************************************************/

/* match_wild:  Attempt to match a string to a pattern which might contain
 *              '*' or '?' wildcards.  Return 1 if the string matches the
 *              pattern, 0 if not.
 */

static int do_match_wild(const char *pattern, const char *str, int docase)
{
    char c;
    const char *s;

    /* Sanity-check pointer parameters */

    if (pattern == NULL) {
        log("BUG or corrupted database: pattern == NULL in do_match_wild())");
        return 0;
    }
    if (str == NULL) {
        log("BUG or corrupted database: str == NULL in do_match_wild())");
        return 0;
    }

    /* This loop is guaranteed to terminate, either by *pattern == 0 (the
     * end of the pattern string) or a trailing '*' (or "*???..."). */

    for (;;) {
        switch (c = *pattern++) {
          case 0:
            if (!*str)
                return 1;
            return 0;
          case '?':
            if (!*str)
                return 0;
            str++;
            break;
          case '*':
            while (*pattern == '?') {
                if (!*str)
                    return 0;
                str++;          /* skip a character for each '?' */
                pattern++;
            }
            if (!*pattern)
                return 1;       /* trailing '*' matches everything else */
            s = str;
            while (*s) {
                if ((docase ? (*s==*pattern) : (tolower(*s)==tolower(*pattern)))
                                        && do_match_wild(pattern+1, s+1, docase))
                    return 1;
                s++;
            }
            break;
          default:
            if (docase ? (*str != c) : (tolower(*str) != tolower(c)))
                return 0;
            str++;
            break;
        } /* switch */
    }
    /* not reached */
}


int match_wild(const char *pattern, const char *str)
{
    return do_match_wild(pattern, str, 1);
}

int match_wild_nocase(const char *pattern, const char *str)
{
    return do_match_wild(pattern, str, 0);
}

/*************************************************************************/
/*************************************************************************/

/* Tables used by valid_nick() and valid_chan().  These tables follow the
 * RFC 1459 definitions; protocols with different definitions should modify
 * the table accordingly:
 *    Bit 0 (0x01) = valid as a first character
 *    Bit 1 (0x02) = valid as a subsequent character
 *    Bit 2 (0x04) = valid as byte 1 of a 2-byte character
 *    Bit 3 (0x08) = valid as byte 2 of a 2-byte character
 * valid_{nick,chan}_table[0] should never be modified.
 *
 * 2-byte characters are handled by setting bits 2 and 3 on the appropriate
 * single-byte entries (0x80-0xFF), then setting bits 0 and/or 1 on 2-byte
 * entries (0x0100-0xFFFF) as needed, where the first byte of the character
 * is the upper 8 bits of the array index and the second byte is the lower
 * 8 bits.
 *
 * Note:  The grammar for nicknames in RFC 1459 excludes '|', but this
 * contradicts an earlier statement that '|' and '\' are to be considered
 * equivalent when comparing nicknames.  On the other hand, the reference
 * server code (at least as far back as version 2.6.2, September 1991)
 * allows anything from 'A' to '}', and allows all specials other than '-'
 * at the beginning of a nick as well.  We consider the RFC to be in error
 * and go by the reference implementation.
 */

unsigned char valid_nick_table[0x10000] = {
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,2,0,0,  2,2,2,2,2,2,2,2, 2,2,0,0,0,0,0,0,
    0,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,  3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,  3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
};

unsigned char valid_chan_table[0x10000] = {
    0,2,2,2,2,2,2,0, 2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,
    0,2,2,3,2,2,3,2, 2,2,2,2,0,2,2,2,  2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,
};

/*************************************************************************/

/* valid_nick, valid_chan: Check whether the given string is a valid
 *                         nickname or channel name, ignoring length.
 *                         Returns nonzero if valid, zero if not.
 */

static int valid_nickchan(const char *str, const unsigned char *table)
{
    int first = 1;   /* First character? */
    int mbsave = 0;  /* First byte of a 2-byte character */

    while (*str) {
        int ch = *(const unsigned char *)str++;
        if (mbsave) {
            if (!(table[ch] & 8))
                return 0;
            ch |= mbsave<<8;
            mbsave = 0;
        } else if (table[ch] & 4) {
            mbsave = ch;
            continue;
        }
        if (!(table[ch] & (first ? 1 : 2)))
            return 0;
        first = 0;
    }
    return 1;
}

int valid_nick(const char *str) {return valid_nickchan(str, valid_nick_table);}
int valid_chan(const char *str) {return valid_nickchan(str, valid_chan_table);}

/*************************************************************************/

/* Check whether the given string is a valid domain name, according to RFC
 * rules:
 *   - Contains only letters, digits, hyphens, and periods (dots).
 *   - Begins with a letter or digit.
 *   - Has a letter or digit after every dot (except for a trailing dot).
 *   - Is no more than DOMAIN_MAXLEN characters long.
 *   - Has no more than DOMPART_MAXLEN characters between periods.
 *   - Has at least one character and does not end with a dot. (not RFC)
 */

#define DOMAIN_MAXLEN   255
#define DOMPART_MAXLEN  63

int valid_domain(const char *str)
{
    const char *s;
    int i;
    static const char valid_domain_chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-.";

    if (!*str)
        return 0;
    if (str[strspn(str,valid_domain_chars)] != 0)
        return 0;
    s = str;
    while (s-str < DOMAIN_MAXLEN && *s) {
        if (*s == '-' || *s == '.')
            return 0;
        i = strcspn(s, ".");
        if (i > DOMPART_MAXLEN)
            return 0;
        s += i;
        if (*s)
            s++;
    }
    if (s-str > DOMAIN_MAXLEN || *s)
        return 0;
    if (s[-1] == '.')
        return 0;
    return 1;
}

/*************************************************************************/

/* Check whether the given string is a valid E-mail address.  A valid
 * E-mail address:
 *   - Contains none of the following characters:
 *       + control characters (\000-\037)
 *       + space (\040)
 *       + vertical bar ('|') (because some mailers try to pipe with it)
 *       + RFC-822 specials, except [ ] @ .
 *   - Contains exactly one '@', which may not be the first character.
 *   - Contains a valid domain name or an IP address in square brackets
 *     after the '@'.
 *   - Does not contain [ or ] except when used with an IP address as above.
 */

int valid_email(const char *str)
{
    const unsigned char *s;
    const char *atmark;

    for (s = (const unsigned char *)str; *s; s++) {
        if (*s <= '\040')  // 040 == 0x20 == ' '
            return 0;
        if (strchr("|,:;\\\"()<>", *s))
            return 0;
    }

    /* Find the @, and abort if there isn't one */
    atmark = strchr(str, '@');
    if (!atmark || atmark == str)
        return 0;
    atmark++;

    /* Don't allow [] in username */
    s = (const unsigned char *)strpbrk(str, "[]");
    if (s && (const char *)s < atmark)
        return 0;

    /* Check for a [1.2.3.4] type of domain */
    if (*atmark == '[') {
        unsigned char ipstr[16];
        const char *bracket = strchr(atmark+1, ']');
        int len = bracket - (atmark+1);
        /* Valid IP addresses have no more than 15 characters */
        if (len <= 15 && !bracket[1]) {
            strncpy((char *)ipstr, atmark+1, len);
            ipstr[len] = 0;
            /* Use pack_ip() to see if it's a valid address */
            if (pack_ip((const char *)ipstr))
                return 1;
        }
    }

    /* Don't allow [] in domains except for the above case */
    if (strpbrk(atmark, "[]"))
        return 0;

    /* Valid domain names cannot contain '@' so we just check valid_domain().
     * Also prohibit domains without dots. */
    return strchr(atmark, '.') && valid_domain(atmark);
}

/*************************************************************************/

/* Check whether the given string is a valid URL.  A valid URL:
 *   - Contains neither control characters (\000-\037) nor spaces (\040).
 *   - Contains a series of letters followed by "://" followed by a valid
 *     domain name, possibly followed by a : and a numeric port number in
 *     the range 1-65535, followed either by a slash and possibly more text
 *     or by nothing.
 */

int valid_url(const char *str)
{
    const unsigned char *s, *colon, *host;
    static const char letters[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char domainbuf[DOMAIN_MAXLEN+1];

    for (s = (const unsigned char *)str; *s; s++) {
        if (*s <= '\040')  // 040 == 0x20 == ' '
            return 0;
    }
    s = (const unsigned char *)strstr(str, "://");
    if (!s)
        return 0;  /* No "://" */
    if (strspn(str, letters) != s - (const unsigned char *)str)
        return 0;  /* Protocol has non-alphabetic characters */
    host = s+3;
    colon = (const unsigned char *)strchr((const char *)host, ':');
    /* s will eventually point to the expected end of the host string */
    s = host + strcspn((const char *)host, "/");
    /* Make sure the port is valid if present. */
    if (colon && colon < s) {
        int port = (int)atolsafe((const char *)colon+1, 1, 65535);
        if (port < 1)
            return 0;  /* Invalid port number or non-numeric characters */
        s = colon;
    }
    /* The string from host through s-1 must be a valid domain name.
     * Check length (must be >=1, <=DOMAIN_MAXLEN), then copy into
     * temporary buffer and check.  Also discard domain names without
     * dots in them. */
    if (s-host < 1 || s-host > DOMAIN_MAXLEN)
        return 0;
    memcpy(domainbuf, host, s-host);
    domainbuf[s-host] = 0;
    return strchr(domainbuf, '.') && valid_domain(domainbuf);
}

/*************************************************************************/

/* Check whether the given E-mail address is rejected by a RejectEmail
 * configuration directive.
 */

int rejected_email(const char *email)
{
    int i;

    if (!email) {
        return 0;
    }
    ARRAY_FOREACH (i, RejectEmail) {
        if (match_wild_nocase(RejectEmail[i], email)) {
            return 1;
        }
    }
    return 0;
}

/*************************************************************************/
/*************************************************************************/

/* time_msec:  Return the current time to millisecond resolution. */

uint32 time_msec(void)
{
#if HAVE_GETTIMEOFDAY
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
#else
    return time(NULL) * 1000;
#endif
}

/*************************************************************************/

/* strtotime:  Convert a string into a time_t.  Essentially the same as
 *             strtol(), but assumes base 10 and returns a time_t.
 */

time_t strtotime(const char *str, char **endptr)
{
    time_t t = 0;

    while (*str >= '0' && *str <= '9') {
        if (t > MAX_TIME_T/10 || MAX_TIME_T - t*10 < *str-'0') {
            t = MAX_TIME_T;
            errno = ERANGE;
        } else {
            t = t*10 + *str-'0';
        }
        str++;
    }
    if (endptr)
        *endptr = (char *)str;
    return t;
}

/*************************************************************************/

/* dotime:  Return the number of seconds corresponding to the given time
 *          string.  If the given string does not represent a valid time,
 *          return -1.
 *
 *          A time string is either a plain integer (representing a number
 *          of seconds), an integer followed by one of these characters:
 *          "s" (seconds), "m" (minutes), "h" (hours), or "d" (days), or a
 *          sequence of such integer-character pairs (without separators,
 *          e.g. "1h30m").
 */

int dotime(const char *s)
{
    int amount;

    amount = strtol(s, (char **)&s, 10);
    if (*s) {
        char c = *s++;
        int rest = dotime(s);
        if (rest < 0)
            return -1;
        switch (c) {
            case 's': return rest + amount;
            case 'm': return rest + amount*60;
            case 'h': return rest + amount*3600;
            case 'd': return rest + amount*86400;
            default : return -1;
        }
    } else {
        return amount;
    }
}

/*************************************************************************/
/*************************************************************************/

/* Translate an IPv4 dotted-quad address into binary (4 bytes).  Returns
 * NULL if the given string is not in dotted-quad format or `ipaddr' is
 * NULL.  The returned buffer is static and will be overwritten on
 * subsequent calls.
 */

uint8 *pack_ip(const char *ipaddr)
{
    static uint8 ipbuf[4];
    const char *s, *s2;
    int i;
    long tmp;

    if (!ipaddr)
        return NULL;
    s = ipaddr;
    for (i = 0; i < 4; i++) {
        if (i > 0 && *s++ != '.')
            return NULL;
        if (isspace(*s))  /* because strtol() will skip whitespace */
            return NULL;
        tmp = strtol(s, (char **)&s2, 10);
        if (s2 == s || tmp < 0 || tmp > 255)
            return NULL;
        ipbuf[i] = (uint8)tmp;
        s = s2;
    }
    if (*s)
        return NULL;
    return ipbuf;
}

/*************************************************************************/

/* Translate a 4-byte binary IPv4 to its dotted-quad (ASCII) representation.
 * Always succeeds and always returns a null-terminated string <= 15
 * characters in length, unless `ip' is NULL, in which case the function
 * returns NULL.  The returned buffer is static and will be overwritten on
 * subsequent calls.
 */

char *unpack_ip(const uint8 *ip)
{
    static char ipbuf[16];

    if (!ip)
        return NULL;
    snprintf(ipbuf, sizeof(ipbuf), "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    return ipbuf;
}

/*************************************************************************/

/* Translate an IPv6 ASCII address into binary (16 bytes).  Return NULL if
 * the given string is not in IPv6 format or `ipaddr' is NULL.  The
 * returned buffer is static and will be overwritten on subsequent calls.
 */

uint8 *pack_ip6(const char *ipaddr)
{
    static uint8 ipbuf[16];
    int words[8];
    int wordnum;
    const char *s, *t;
    int i;

    if (!ipaddr)
        return NULL;

    /* Parse the address into 16-bit words */
    s = ipaddr;
    wordnum = 0;
    if (*s == ':') {
        words[wordnum++] = 0;
        s++;
    }
    while (*s) {
        if (wordnum >= 8) {
            /* too many words, abort */
            return NULL;
        }
        if (*s == ':') {
            /* mark "::" with a -1 */
            words[wordnum++] = -1;
            s++;
        } else {
            words[wordnum++] = (int)strtol(s, (char **)&t, 16);
            if (*t && *t != ':') {
                /* invalid syntax */
                return NULL;
            }
            if (*t) {
                t++;  /* skip past delimiter */
                if (!*t) {
                    /* trailing ":", convert to :0000 */
                    if (wordnum >= 8)
                        return NULL;
                    words[wordnum++] = 0;
                }
            }
            s = t;
        }
    }

    /* Expand "::" into as many zeros as needed, and make sure there
     * aren't multiple occurrences of "::" */
    for (i = 0; i < wordnum; i++) {
        if (words[i] == -1)
            break;
    }
    if (i < wordnum) {  /* found a "::" */
        int j, offset;
        for (j = i+1; j < wordnum; j++) {
            if (words[j] == -1) {
                /* multiple "::" */
                return NULL;
            }
        }
        offset = 8-wordnum;
        for (j = 7; j >= i; j--) {
            if (j-offset > i)
                words[j] = words[j-offset];
            else
                words[j] = 0;
        }
        wordnum = 8;
    }

    /* Make sure we have exactly 8 words */
    if (wordnum != 8)
        return NULL;

    /* Convert to binary and return */
    for (i = 0; i < 8; i++) {
        ipbuf[i*2  ] = words[i] >> 8;
        ipbuf[i*2+1] = words[i] & 255;
    }
    return ipbuf;
}

/*************************************************************************/

/* Translate a 16-byte binary IPv6 address to its ASCII representation.
 * Always succeeds and always returns a null-terminated string <= 39
 * characters in length, unless `ip' is NULL, in which case the function
 * returns NULL.  The returned buffer is static and will be overwritten on
 * subsequent calls.
 *
 * The address string returned by this function will always use 4-digit
 * hexadecimal numbers for each word in the address, but will omit all-zero
 * words when possible.
 */

char *unpack_ip6(const uint8 *ip)
{
    static char ipbuf[40];
    char *out, *s;
    int i;

    if (!ip)
        return NULL;

    out = ipbuf;
    for (i = 0; i < 8; i++) {
        /* Skip 0000 at beginning or end */
        if ((i != 0 && i != 7) || ip[i*2] || ip[i*2+1]) {
            out += snprintf(out, sizeof(ipbuf)-(out-ipbuf), "%02X%02X",
                            ip[i*2], ip[i*2+1]);
        }
        if (i != 7)
            *out++ = ':';
    }
    if ((s = strstr(ipbuf,":0000:")) != NULL) {
        /* Compress zeros */
        memmove(s+1, s+5, strlen(s+5)+1);
        s++;
        while (strncmp(s,":0000:",6) == 0)
            memmove(s, s+5, strlen(s+5)+1);
    }
    return ipbuf;
}

/*************************************************************************/
/*************************************************************************/

/* Encode buffer `in' of size `insize' to buffer `out' of size `outsize'
 * using base64, appending a trailing null.  Returns the number of bytes
 * required by the encoded string.  (The amount of space needed to store
 * the encoded string can be found with encode_base64(...,NULL,0).)
 * Returns -1 if the input or output buffer is NULL (except when the
 * corresponding size is zero) or if the input or output size is negative.
 * If `out' is too small to hold the entire encoded string, it will contain
 * the first `outsize'-1 bytes of the encoded string followed by a null.
 */

static const char base64_chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int encode_base64(const void *in, int insize, char *out, int outsize)
{
    int required = ((insize+2)/3*4)+1;
    const uint8 *inp = in;
    int inpos, outpos;

    if (insize < 0 || outsize < 0)
        return -1;
    if (outsize == 0)
        return required;
    if (!out)
        return -1;
    if (insize == 0) {
        /* outsize is at least 1 */
        *out = 0;
        return 1;
    }
    if (!in)
        return -1;

    outsize--;  /* leave room for trailing \0 */

    /* Actually do the encoding */
    outpos = 0;
    for (inpos = 0; inpos < insize; inpos += 3) {
        uint8 i0, i1, i2;
        char o0, o1, o2, o3;
        i0 = inp[inpos];
        o0 = base64_chars[i0>>2];
        if (inpos+1 < insize) {
            i1 = inp[inpos+1];
            o1 = base64_chars[(i0&3)<<4 | i1>>4];
            if (inpos+2 < insize) {
                i2 = inp[inpos+2];
                o2 = base64_chars[(i1&15)<<2 | i2>>6];
                o3 = base64_chars[i2&63];
            } else {
                o2 = base64_chars[(i1&15)<<2];
                o3 = '=';
            }
        } else {
            o1 = base64_chars[(i0&3)<<4];
            o2 = '=';
            o3 = '=';
        }
        if (outpos < outsize)
            out[outpos++] = o0;
        if (outpos < outsize)
            out[outpos++] = o1;
        if (outpos < outsize)
            out[outpos++] = o2;
        if (outpos < outsize)
            out[outpos++] = o3;
    }

    /* Terminate the string and return; outpos is constrained above to
     * still be within the buffer (remember that outsize was decremented
     * before the loop) */
    out[outpos] = 0;
    return required;
}

/*************************************************************************/

/* Decode base64-encoded string `in' (null-terminated) to buffer `out' of
 * size `outsize'.  Returns the number of bytes required by the decoded
 * data.  (The amount of space needed to store the decoded data can be
 * found with decode_base64(...,NULL,0).)  Returns -1 if the input string
 * is NULL, or if the output buffer is NULL (except when the size is zero)
 * or the size is negative.  If `out' is too small to hold all of the
 * decoded data, it will contain the first `outsize' bytes.
 */

static const char base64_array[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 0x00 */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 0x40 */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 0x80 */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 0xC0 */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
};

int decode_base64(const char *in, void *out, int outsize)
{
    uint8 *outp = out;
    int outpos;

    if (!in || outsize < 0)
        return -1;

    outpos = 0;
    while (*in) {
        int i0, i1, i2, i3;
        i0 = *in++;
        if (*in) {
            i1 = *in++;
            if (*in) {
                i2 = *in++;
                if (*in) {
                    i3 = *in++;
                } else {
                    i3 = 0;
                }
            } else {
                i2 = 0;
                i3 = 0;
            }
        } else {
            i1 = 0;
            i2 = 0;
            i3 = 0;
        }
        i0 = base64_array[(int)i0 & 255];
        i1 = base64_array[(int)i1 & 255];
        i2 = base64_array[(int)i2 & 255];
        i3 = base64_array[(int)i3 & 255];
        if (i0 < 0 || i1 < 0)
            break;
        /* Only store if buffer space is available; increment outpos anyway
         * to keep track of total space required (for return value) */
        if (outpos < outsize)
            outp[outpos] = i0<<2 | i1>>4;
        outpos++;
        if (i2 < 0)
            break;
        if (outpos < outsize)
            outp[outpos] = (i1&15)<<4 | i2>>2;
        outpos++;
        if (i3 < 0)
            break;
        if (outpos < outsize)
            outp[outpos] = (i2&3)<<6 | i3;
        outpos++;
    }

    return outpos;
}

/*************************************************************************/
/*************************************************************************/

/* Process a string containing a number/range list in the form
 * "n1[-n2][,n3[-n4]]...", calling a caller-specified routine for each
 * number in the list.  If the callback returns -1, stop immediately.
 * Returns the sum of all nonnegative return values from the callback.
 * If `count_ret' is non-NULL, *count_ret will be set to the total number
 * of times the callback was called.
 *
 * The number list will never contain duplicates and will always be sorted
 * ascendingly. This means the callback routines don't have to worry about
 * being called twice for the same index. -TheShadow
 * Also, the only values accepted are 0-65536 (inclusive), to avoid someone
 * giving us 0-2^31 and causing freezes or out-of-memory.  Numbers outside
 * this range will be ignored.
 *
 * The callback should be of type range_callback_t, which is defined as:
 *      int (*range_callback_t)(User *u, int num, va_list args)
 */

int process_numlist(const char *numstr, int *count_ret,
                    range_callback_t callback, ...)
{
    int n1, n2, min, max, i;
    int retval = 0;
    int numcount = 0;
    va_list args;
    static uint8 numflag[65536/8+1];  /* 1 bit per index 0-65536 inclusive */

    memset(numflag, 0, sizeof(numflag));
    min = 65536;
    max = 0;
    va_start(args, callback);

    /* This algorithm ignores invalid characters, ignores a dash
     * when it precedes a comma, and ignores everything from the
     * end of a valid number or range to the next comma or null.
     */
    while (*numstr) {
        n1 = n2 = strtol(numstr, (char **)&numstr, 10);
        numstr += strcspn(numstr, "0123456789,-");
        if (*numstr == '-') {
            numstr++;
            numstr += strcspn(numstr, "0123456789,");
            if (isdigit(*numstr)) {
                n2 = strtol(numstr, (char **)&numstr, 10);
                numstr += strcspn(numstr, "0123456789,-");
            }
        }
        if (n1 < 0)
            n1 = 0;
        if (n2 > 65536)
            n2 = 65536;
        if (n1 < min)
            min = n1;
        if (n2 > max)
            max = n2;
        while (n1 <= n2) {
            if ((n1&7) == 0 && n1+7 <= n2) {
                /* Set a whole byte at once */
                numflag[n1>>3] = 0xFF;
                n1 += 8;
            } else {
                /* Set just a single bit */
                numflag[n1>>3] |= 1 << (n1&7);
                n1++;
            }
        }
        numstr += strcspn(numstr, ",");
        if (*numstr)
            numstr++;
    }

    /* Now call the callback routine for each index. */
    numcount = 0;
    for (i = min; i <= max; i++) {
        va_list args_copy;
        int res;
        if (!(numflag[i>>3] & (1 << (i&7))))
            continue;
        numcount++;
        va_copy(args_copy, args);
        res = callback(i, args_copy);
        va_end(args_copy);
        if (res < 0)
            break;
        retval += res;
    }

    va_end(args);
    if (count_ret)
        *count_ret = numcount;
    return retval;
}

/*************************************************************************/
/*************************************************************************/

/* atolsafe:  Convert a string in base 10 to a long int, ensuring that the
 *            string contains no invalid characters and that the result is
 *            within a specified range (>=min && <=max).  Returns min-1 on
 *            error.  errno is set to EINVAL if the string is not a valid
 *            number, ERANGE if the value is outside the range [min..max],
 *            and left unmodified otherwise.
 */

long atolsafe(const char *s, long min, long max)
{
    int errno_save = errno;
    long v;

    if (!s || !*s)
        return min-1;
    errno = 0;
    v = strtol(s, (char **)&s, 10);
    if (*s) {
        errno = EINVAL;
        return min-1;
    }
    if (errno == ERANGE || v < min || v > max) {
        errno = ERANGE;
        return min-1;
    }
    errno = errno_save;
    return v;
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
