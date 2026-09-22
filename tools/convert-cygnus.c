/* Conversion routines for Cygnus 0.2.0.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "convert-db.h"

/*************************************************************************/

#define WHAT(a,b) ((a)<<8 | (b))

/* Analog of strtok_remaining() from misc.c that strips newlines. */
static inline char *cyg_strtok_remaining(void) {
    char *s = strtok(NULL, "\r\n");
    while (s && *s == ' ')
        s++;
    return s;
}

/* Short macro to convert a string `str' to a time_t value with error
 * checking and store the result in the variable `var'; if the value is not
 * a valid integer, a message is printed to stderr and the current time is
 * used instead.  `errfmt' must be a string literal. */
#define CVT_TIME(var,str,errfmt,...) do {                               \
    unsigned long time_l = strtoul(str, (char **)&str, 10);             \
    if (str && *str) {                                                  \
        fprintf(stderr, "%s:%d: " errfmt " is not a valid number;"      \
                " setting to current time\n", fname, line , ##__VA_ARGS__);\
        var = time(NULL);                                               \
    } else {                                                            \
        var = time_l;                                                   \
    }                                                                   \
} while (0)

/* The same thing, but defaulting to zero instead of the current time
 * (good for expirations). */
#define CVT_TIME_0(var,str,errfmt,...) do {                             \
    unsigned long time_l = strtoul(str, (char **)&str, 10);             \
    if (str && *str) {                                                  \
        fprintf(stderr, "%s:%d: " errfmt " is not a valid number;"      \
                " setting to zero\n", fname, line , ##__VA_ARGS__);     \
        var = 0;                                                        \
    } else {                                                            \
        var = time_l;                                                   \
    }                                                                   \
} while (0)

/*************************************************************************/

/* Reset memo limits to default? (-reset-memo-limits option) */
static int reset_memo_limits = 0;

/*************************************************************************/
/*************************************************************************/

/* Mapping from timezone IDs to offsets in minutes, based on cygnus.zone
 * distributed with Cygnus 0.2.0 */
static const int default_timezones[] = {
    -720,
    -660,
    -600,
    -600,
    -600,
    -540,
    -540,
    -540,
    -480,
    -480,
    -480,
    -420,
    -420,
    -360,
    -360,
    -300,
    -300,
    -240,
    -240,
    -240,
    -210,
    -210,
    -180,
    -180,
    -180,
    -150,
    -120,
    -60,
    0,
    0,
    0,
    0,
    0,
    60,
    60,
    60,
    60,
    60,
    60,
    60,
    60,
    120,
    120,
    120,
    120,
    120,
    120,
    120,
    180,
    180,
    180,
    180,
    210,
    240,
    300,
    330,
    360,
    390,
    420,
    480,
    480,
    480,
    480,
    540,
    540,
    570,
    600,
    600,
    630,
    660,
    720,
    720,
    720,
    780
};
static int *timezones, ntimezones;

static void cyg_load_nick(const char *dir)
{
    FILE *f;
    char fname[PATH_MAX+1];
    char buf[4096];  /* Cygnus uses 2048; let's be safe */
    char *s;
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;
    int ncount = 0, lcount = 0, mcount = 0;  /* to check against DE line */
    int line;

    snprintf(fname, sizeof(fname), "%s/nickserv.db", dir);
    f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "Cannot open %s: %s\n", fname, strerror(errno));
        exit(1);
    }
    fgets(buf, sizeof(buf), f);
    if (!(s = strtok(buf, " \r\n"))
     || strcmp(s,"NV") != 0
     || !(s = strtok(NULL, " \r\n"))
    ) {
        fprintf(stderr, "%s: Version number missing", fname);
        exit(1);
    }
    if (atoi(s) != 3) {
        fprintf(stderr, "%s: Bad version number (expected 3, got %d)\n",
                fname, atoi(s));
        exit(1);
    }

    line = 1;
    while (fgets(buf, sizeof(buf), f)) {
        line++;
        if (!(s = strtok(buf, " \r\n")))
            continue;
        switch (WHAT(s[0],s[1])) {
          case WHAT('S','S'):   /* Previous servicestamp value; ignore */
            break;

          case WHAT('N','I'): { /* Basic nickname info */
            char *pass;  /* may be truncated */
            const char *reg, *seen, *flags, *memolimit, *zone, *key,
                       *usermask, *realname;
            long myflags, tmp;
            int truncated = 0;

            ncount++;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt NI line, ignoring nickname\n",
                        fname, line);
                ni = NULL;
                ngi = NULL;
                break;
            }
            if (strlen(s) > NICKMAX-1) {
                fprintf(stderr, "%s:%d: Truncating nickname `%s' (exceeds"
                        " NICKMAX-1 characters)\n", fname, line, s);
                s[NICKMAX-1] = 0;
                truncated = 1;
            }
            if (get_nickinfo(s)) {
                fprintf(stderr, "%s:%d: Nickname `%s' already exists%s,"
                        " skipping\n", fname, line, s,
                        truncated ? " (due to truncation?)" : "");
                ni = NULL;
                ngi = NULL;
                break;
            }
            pass = strtok(NULL, " \r\n");
            reg = strtok(NULL, " \r\n");
            seen = strtok(NULL, " \r\n");
            (void) strtok(NULL, " \r\n");  /* last ircd timestamp, not used */
            (void) strtok(NULL, " \r\n");  /* last servicestamp, not used */
            flags = strtok(NULL, " \r\n");
            memolimit = strtok(NULL, " \r\n");
            zone = strtok(NULL, " \r\n");
            key = strtok(NULL, " \r\n");
            usermask = strtok(NULL, " \r\n");
            realname = strtok(NULL, " \r\n");
            if (!pass || !reg || !seen || !flags || !memolimit || !zone
             || !key || !usermask || !realname
            ) {
                fprintf(stderr, "%s:%d: Corrupt NI line, ignoring nickname\n",
                        fname, line);
                ni = NULL;
                ngi = NULL;
                break;
            }
            myflags = strtol(flags, (char **)&flags, 10);
            if (flags && *flags) {
                fprintf(stderr, "%s:%d: Flags value for `%s' is not a"
                        " valid number; ignoring nickname\n",
                        fname, line, ni->nick);
                ni = NULL;
                ngi = NULL;
                break;
            }
            ni = makenick(s, &ngi);
            if (!ni) {
                fprintf(stderr, "%s:%d: Unable to add nickname `%s' to"
                        " database, skipping\n", fname, line, s);
                ni = NULL;
                ngi = NULL;
                break;
            }
            if (strlen(pass) > sizeof(ngi->pass)-1) {
                fprintf(stderr, "%s:%d: Password for `%s' truncated to %d"
                        " characters\n", fname, line, ni->nick,
                        sizeof(ngi->pass)-1);
                pass[sizeof(ngi->pass)-1] = 0;
            }
            init_password(&ngi->pass);
            strbcpy(ngi->pass.password, pass);
            CVT_TIME(ni->time_registered, reg,
                     "Registration time for `%s'", ni->nick);
            CVT_TIME(ni->last_seen, seen, "Last-seen time for `%s'", ni->nick);
            tmp = strtol(memolimit, (char **)&memolimit, 10);
            if (reset_memo_limits) {
                ngi->memos.memomax = MEMOMAX_DEFAULT;
            } else if (memolimit && *memolimit) {
                fprintf(stderr, "%s:%d: Memo limit for `%s' is not a valid"
                        " number; setting to default\n",
                        fname, line, ni->nick);
                ngi->memos.memomax = MEMOMAX_DEFAULT;
            } else if (tmp < 0 || tmp > MEMOMAX_MAX) {
                fprintf(stderr, "%s:%d: Memo limit for `%s' is out of range;"
                        " setting to default\n", fname, line, ni->nick);
                ngi->memos.memomax = MEMOMAX_DEFAULT;
            } else {
                ngi->memos.memomax = tmp;
            }
            tmp = strtol(zone, (char **)&zone, 10);
            if (zone && *zone) {
                fprintf(stderr, "%s:%d: Timezone index for `%s' is not a"
                        " valid number; setting to default\n",
                        fname, line, ni->nick);
                ngi->timezone = TIMEZONE_DEFAULT;
            } else if (tmp == 0 || ntimezones <= 0) {
                ngi->timezone = TIMEZONE_DEFAULT;
            } else if (tmp < 1 || tmp > ntimezones) {
                fprintf(stderr, "%s:%d: Timezone index for `%s' is out of"
                        " range; setting to default\n", fname, line, ni->nick);
                ngi->timezone = TIMEZONE_DEFAULT;
            } else {
                ngi->timezone = timezones[tmp-1];
            }
            ngi->authcode = strtol(key, (char **)&key, 10);
            if (key && *key) {
                fprintf(stderr, "%s:%d: Authentication code for `%s' is not a"
                        " valid number; clearing\n", fname, line, ni->nick);
                ngi->authcode = 0;
            } else if (ngi->authcode != 0) {
                ngi->authset = time(NULL);
                /* Assume it's a code for registration; if it's one for an
                 * E-mail change, we'll find that out later */
                ngi->authreason = NICKAUTH_REGISTER;
            }
            ni->last_usermask = sstrdup(usermask);
            ni->last_realname = sstrdup(realname);
            /* Now set flags */
            ngi->flags = 0;
            if (myflags & 0x0001)
                ngi->flags |= NF_KILLPROTECT;
            if (myflags & 0x0002)
                ngi->flags |= NF_SECURE;
            if (myflags & 0x0004)
                ngi->flags |= NF_HIDE_EMAIL;
            if (myflags & 0x0020)   /* NOOP */
                ngi->flags |= NF_NOOP;
            if (myflags & 0x0080)
                ngi->flags |= NF_PRIVATE;
            if (myflags & 0x0400)   /* NOMEMO */
                ngi->memos.memomax = 0;
            if (myflags & 0x1000)   /* MEMOMAIL */
                ngi->flags |= NF_MEMO_FWDCOPY;  /* Cygnus uses forward+copy */
            if (myflags & 0x2000) { /* FROZEN */
                strbcpy(ngi->suspend_who, "<unknown>");
                ngi->suspend_reason =
                    (char *)"Unknown (imported from Cygnus)",
                ngi->suspend_time = time(NULL);
                ngi->suspend_expires = 0;
                ngi->flags |= NF_SUSPENDED;
            }
            if (myflags & 0x4000)   /* HELD, i.e. not expiring */
                ni->status |= NS_NOEXPIRE;
            if (myflags & 0x20000000)
                ngi->os_priv = NP_SERVOPER;
            if (myflags & 0x80000000) {
                if (!ngi->authcode) {
                    fprintf(stderr, "%s:%d: WAITAUTH set for `%s' but no"
                            " authentication code!  Corrupt database?",
                            fname, line, ni->nick);
                }
            } else {
                if (ngi->authcode) {
                    fprintf(stderr, "%s:%d: `%s' has authentication code, but"
                            " WAITAUTH not set (corrupt database?); clearing"
                            " authcode", fname, line, ni->nick);
                    ngi->authcode = 0;
                    ngi->authset = 0;
                }
            }
            break;
          } /* case WHAT('N','I') */

          case WHAT('A','C'):   /* Access list entry */
            if (!ngi)
                break;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt AC line, ignoring\n",
                        fname, line);
                break;
            }
            ARRAY_EXTEND(ngi->access);
            ngi->access[ngi->access_count-1] = sstrdup(s);
            break;

          case WHAT('L','N'):   /* Subnick (link) list */
            while ((s = strtok(NULL, " \r\n")) != NULL) {
                lcount++;
                if (ngi) {
                    int truncated = 0;
                    NickInfo *ni2;
                    if (strlen(s) > NICKMAX-1) {
                        fprintf(stderr, "%s:%d: Truncating nickname `%s'"
                                " (exceeds NICKMAX-1 characters)\n",
                                fname, line, s);
                        s[NICKMAX-1] = 0;
                        truncated = 1;
                    }
                    if (get_nickinfo(s)) {
                        fprintf(stderr, "%s:%d: Nickname `%s' (link to `%s')"
                                " already exists%s, skipping\n",
                                fname, line, s, ni->nick,
                                truncated ? " (due to truncation?)" : "");
                    } else if (!(ni2 = makenick(s, NULL))) {
                        fprintf(stderr, "%s:%d: Unable to add nickname `%s'"
                                " (link to `%s') to database, skipping\n",
                                fname, line, s, ni->nick);
                    } else {
                        ni2->nickgroup = ni->nickgroup;
                        ni2->time_registered = ni->time_registered;
                        ni2->last_seen = ni->last_seen;
                        ARRAY_EXTEND(ngi->nicks);
                        strbcpy(ngi->nicks[ngi->nicks_count-1], s);
                    }
                } else {
                    fprintf(stderr, "%s:%d: Ignoring link `%s' to ignored"
                            " nickname\n", fname, line, s);
                }
            }
            break;

          case WHAT('M','O'): { /* Memo */
            const char *num_s, *flags_s, *time_s, *text;
            char *sender;  /* we modify it */
            int i;
            unsigned long num;
            long flags, rtime;

            mcount++;
            if (!ngi)
                break;
            num_s = strtok(NULL, " \r\n");
            flags_s = strtok(NULL, " \r\n");
            time_s = strtok(NULL, " \r\n");
            sender = strtok(NULL, " \r\n");
            text = cyg_strtok_remaining();
            if (!num_s || !flags_s || !time_s || !sender || !text) {
                fprintf(stderr, "%s:%d: Corrupt MO line, ignoring\n",
                        fname, line);
                break;
            }
            num = strtoul(num_s, (char **)&num_s, 10);
            if (num_s && *num_s) {
                fprintf(stderr, "%s:%d: Memo number is not a valid number;"
                        " ignoring memo\n", fname, line);
                break;
            }
            flags = strtol(flags_s, (char **)&flags_s, 10);
            if (flags_s && *flags_s) {
                fprintf(stderr, "%s:%d: Flag value is not a valid number;"
                        " ignoring memo\n", fname, line);
                break;
            }
            CVT_TIME(rtime, time_s, "Memo timestamp");
            if (strlen(sender) > NICKMAX-1) {
                fprintf(stderr, "%s:%d: Truncating sender nickname `%s'"
                        " (exceeds NICKMAX-1 characters)\n", fname, line,
                        sender);
                sender[NICKMAX-1] = 0;
            }
            ARRAY_EXTEND(ngi->memos.memos);
            i = ngi->memos.memos_count - 1;
            ngi->memos.memos[i].number = num;
            ngi->memos.memos[i].flags = 0;
            if (flags & 1)
                ngi->memos.memos[i].flags |= MF_UNREAD;
            ngi->memos.memos[i].time = rtime;
            strbcpy(ngi->memos.memos[i].sender, sender);
            ngi->memos.memos[i].text = sstrdup(text);
            break;
          } /* case WHAT('M','O') */

          case WHAT('F','W'):   /* Nick to forward memos to; ignore */
            break;

          case WHAT('F','R'):   /* Freeze (suspend) reason */
            if (!ngi)
                break;
            s = cyg_strtok_remaining();
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt FR line, ignoring\n",
                        fname, line);
                break;
            }
            if (!(ngi->flags & NF_SUSPENDED)) {
                fprintf(stderr, "%s:%d: FR line for `%s', but FREEZE flag"
                        " not set (corrupt database?), ignoring\n",
                        fname, line, ni->nick);
                break;
            }
            /* Under normal circumstances si->reason will be a string
             * constant at this point.  If there are ever two FR lines for
             * the same nick group, we leak memory here, but since
             * convert-db doesn't worry about memory leaks in general we
             * don't worry about them here. */
            ngi->suspend_reason = sstrdup(s);
            break;

          case WHAT('E','M'):   /* E-mail address */
            if (!ngi)
                break;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt EM line, ignoring\n",
                        fname, line);
                break;
            }
            if (ngi->email && ngi->authreason == NICKAUTH_SET_EMAIL) {
                /* A new address is waiting for authentication; assume that
                 * this address has been authenticated */
                free(ngi->last_email);
                ngi->last_email = sstrdup(s);
            } else {
                free(ngi->email);
                ngi->email = sstrdup(s);
            }
            break;

          case WHAT('T','E'):   /* Temporary E-mail address */
            /* This is set when a SET EMAIL command has been given and the
             * change has not yet been authenticated.  We discard the old
             * address and store this one instead, then modify the auth
             * code (or set one if needed) appropriately. */
            if (!ngi)
                break;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt TE line, ignoring\n",
                        fname, line);
                break;
            }
            if (!ngi->last_email)  /* in case we get two TE's */
                ngi->last_email = ngi->email;
            ngi->email = sstrdup(s);
            if (!ngi->authcode) {
                fprintf(stderr, "%s:%d: TE line for nickname `%s' without"
                        " authentication code!  Assigning new code.\n",
                        fname, line, ni->nick);
                ngi->authcode = rand()%900000000 + 100000000;
                ngi->authset = time(NULL);
            }
            ngi->authreason = NICKAUTH_SET_EMAIL;
            break;

          case WHAT('U','R'):   /* URL */
            if (!ngi)
                break;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt UR line, ignoring\n",
                        fname, line);
                break;
            }
            ngi->url = sstrdup(s);
            break;

          case WHAT('U','N'):   /* UIN; ignore */
            break;

          case WHAT('N','A'):   /* "Real name" (not ni->last_realname); ignore */
            break;

          case WHAT('A','G'):   /* Age; ignore */
            break;

          case WHAT('S','X'):   /* Sex; ignore (TODO: get some) */
            break;

          case WHAT('L','O'):   /* Location; ignore */
            break;

          case WHAT('D','E'):   /* Database end; check counts */
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "Warning: check line corrupt; database may"
                        " be invalid");
                break;
            }
            if (atoi(s) != ncount) {
                fprintf(stderr, "Warning: bad nickname count (%d expected,"
                        " %d found)--database may be corrupt\n",
                        atoi(s), ncount);
            }
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "Warning: check line corrupt; database may"
                        " be invalid");
                break;
            }
            if (atoi(s) != lcount) {
                fprintf(stderr, "Warning: bad link count (%d expected, %d"
                        " found)--database may be corrupt\n", atoi(s), lcount);
            }
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "Warning: check line corrupt; database may"
                        " be invalid");
                break;
            }
            if (atoi(s) != mcount) {
                fprintf(stderr, "Warning: bad memo count (%d expected, %d"
                        " found)--database may be corrupt\n", atoi(s), mcount);
            }
            break;
        } /* switch (WHAT(s)) */
    } /* while (fgets()) */

    fclose(f);
}

/*************************************************************************/

static struct {
    int32 flag;
    char mode;
} cmodes[] = {
    { 0x00000001, 't' },
    { 0x00000002, 'n' },
    { 0x00000004, 's' },
    { 0x00000008, 'm' },
    { 0x00000010, 'l' },
    { 0x00000020, 'i' },
    { 0x00000040, 'p' },
    { 0x00000080, 'k' },
    { 0x00000100, 0   }, /* cumode +o */
    { 0x00000200, 0   }, /* cumode +v */
    { 0x00000400, 'R' },
    { 0x00000800, 0   }, /* 'r', never set in mlock */
    { 0x00001000, 'c' },
    { 0x00002000, 'O' },
    { 0x00004000, 'Q' },
    { 0x00008000, 'S' },
    { 0x00010000, 'K' },
    { 0x00020000, 'V' },
    { 0x00040000, 'f' },
    { 0x00080000, 'H' },
    { 0x00100000, 'G' },
    { 0x00200000, 'C' },
    { 0x00400000, 'u' },
    { 0x00800000, 'z' },
    { 0x01000000, 'N' },
    { 0x02000000, 'L' },
    { 0x04000000, 'A' },
    { 0x08000000, 0   }, /* cumode +h */
    { 0x10000000, 0   }, /* cumode +a */
    { 0x20000000, 0   }, /* cumode +q */
    { 0, 0 }
};

static void cyg_load_chan(const char *dir)
{
    FILE *f;
    char fname[PATH_MAX+1];
    char buf[4096];  /* Cygnus uses 2048; let's be safe */
    char *s;
    ChannelInfo *ci = NULL;
    int ccount = 0;  /* to check against DE line */
    int line, i;

    snprintf(fname, sizeof(fname), "%s/chanserv.db", dir);
    f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "Cannot open %s: %s\n", fname, strerror(errno));
        exit(1);
    }
    fgets(buf, sizeof(buf), f);
    if (!(s = strtok(buf, " \r\n"))
     || strcmp(s,"CV") != 0
     || !(s = strtok(NULL, " \r\n"))
    ) {
        fprintf(stderr, "%s: Version number missing", fname);
        exit(1);
    }
    if (atoi(s) != 3) {
        fprintf(stderr, "%s: Bad version number (expected 3, got %d)\n",
                fname, atoi(s));
        exit(1);
    }

    line = 1;
    while (fgets(buf, sizeof(buf), f)) {
        line++;
        if (!(s = strtok(buf, " \r\n")))
            continue;
        switch (WHAT(s[0],s[1])) {
          case WHAT('C','I'): { /* Basic channel info */
            char *pass;  /* may be truncated */
            const char *founder, *reg, *used, *flags, *lockon_s, *lockoff_s,
                       *topiclock, *memolevel;
            char *on, *off;  /* for mode lock */
            long myflags, lockon, lockoff, tmp;
            int truncated = 0;
            NickInfo *ni;

            ccount++;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt CI line, ignoring channel\n",
                        fname, line);
                ci = NULL;
                break;
            }
            if (strlen(s) > CHANMAX-1) {
                fprintf(stderr, "%s:%d: Truncating channel name `%s' (exceeds"
                        " CHANMAX-1 characters)\n", fname, line, s);
                s[CHANMAX-1] = 0;
                truncated = 1;
            }
            if (get_channelinfo(s)) {
                fprintf(stderr, "%s:%d: Channel `%s' already exists%s,"
                        " skipping\n", fname, line, s,
                        truncated ? " (due to truncation?)" : "");
                ci = NULL;
                break;
            }
            founder = strtok(NULL, " \r\n");
            pass = strtok(NULL, " \r\n");
            reg = strtok(NULL, " \r\n");
            used = strtok(NULL, " \r\n");
            flags = strtok(NULL, " \r\n");
            lockon_s = strtok(NULL, " \r\n");
            lockoff_s = strtok(NULL, " \r\n");
            topiclock = strtok(NULL, " \r\n");
            memolevel = strtok(NULL, " \r\n");
            /* key ignored */
            if (!founder || !pass || !reg || !used || !flags || !lockon_s
             || !lockoff_s || !topiclock || !memolevel
            ) {
                fprintf(stderr, "%s:%d: Corrupt CI line, ignoring channel\n",
                        fname, line);
                ci = NULL;
                break;
            }
            ni = get_nickinfo(founder);
            if (!ni) {
                fprintf(stderr, "%s:%d: %s founder `%s' is not a registered"
                        " nickname, ignoring channel\n",
                        fname, line, s, founder);
                ci = NULL;
                break;
            } else if (ni->status & NS_VERBOTEN) {
                fprintf(stderr, "%s:%d: %s founder `%s' is a forbidden"
                        " nickname, ignoring channel\n",
                        fname, line, s, founder);
                ci = NULL;
                break;
            } else if (!ni->nickgroup) {
                fprintf(stderr, "%s:%d: %s fuonder `%s' has an invalid"
                        " nickname record (BUG?), ignoring channel\n",
                        fname, line, s, founder);
                ci = NULL;
                break;
            }
            myflags = strtol(flags, (char **)&flags, 10);
            if (flags && *flags) {
                fprintf(stderr, "%s:%d: Flags value for `%s' is not a"
                        " valid number; ignoring channel\n", fname, line, s);
                ci = NULL;
                break;
            }
            ci = makechan(s);
            if (!ci) {
                fprintf(stderr, "%s:%d: Unable to add channel `%s' to"
                        " database, skipping\n", fname, line, s);
                ci = NULL;
                break;
            }
            ci->founder = ni->nickgroup;
            if (strlen(pass) > sizeof(ci->founderpass)-1) {
                fprintf(stderr, "%s:%d: Password for `%s' truncated to %d"
                        " characters\n", fname, line, ci->name,
                        sizeof(ci->founderpass)-1);
                pass[sizeof(ci->founderpass)-1] = 0;
            }
            init_password(&ci->founderpass);
            strbcpy(ci->founderpass.password, pass);
            CVT_TIME(ci->time_registered, reg,
                     "Registration time for `%s'", ci->name);
            CVT_TIME(ci->last_used, used, "Last-used time for `%s'", ci->name);
            lockon = strtol(lockon_s, (char **)&lockon_s, 10);
            if (lockon_s && *lockon_s) {
                fprintf(stderr, "%s:%d: Locked-on mode value for `%s' is not"
                        " a valid number; clearing\n", fname, line, ci->name);
                lockon = 0;
            }
            lockoff = strtol(lockoff_s, (char **)&lockoff_s, 10);
            if (lockoff_s && *lockoff_s) {
                fprintf(stderr, "%s:%d: Locked-off mode value for `%s' is not"
                        " a valid number; clearing\n", fname, line, ci->name);
                lockoff = 0;
            }
            ci->mlock.on = on = scalloc(64, 1);
            ci->mlock.off = off = scalloc(64, 1);
            for (i = 0; cmodes[i].flag != 0; i++) {
                if (lockon & cmodes[i].flag)
                    *on++ = cmodes[i].mode;
                if (lockoff & cmodes[i].flag)
                    *off++ = cmodes[i].mode;
            }
            *on = 0;
            *off = 0;
            tmp = strtol(topiclock, (char **)&topiclock, 10);
            if (topiclock && *topiclock) {
                fprintf(stderr, "%s:%d: Topic lock value for `%s' is not a"
                        " valid number; clearing\n", fname, line, ci->name);
                tmp = 0;
            } else if (tmp < 0 || tmp > 5) {
                fprintf(stderr, "%s:%d: Topic lock value for `%s' is out of"
                        " range; clearing\n", fname, line, ci->name);
                tmp = 0;
            }
            /* Now set flags */
            ci->flags = 0;
            if (tmp)
                ci->flags |= CF_TOPICLOCK;
            if (myflags & 0x0001)
                ci->flags |= CF_OPNOTICE;
            if (myflags & 0x0002)  /* VOPALL */
                ci->levels[CA_AUTOVOICE] = 0;
            if (myflags & 0x0004)
                ci->flags |= CF_SECURE;
            if (myflags & 0x0008)
                ci->flags |= CF_RESTRICTED;
            if (myflags & 0x0010) { /* FROZEN */
                ci->flags |= CF_SUSPENDED;
                strbcpy(ci->suspend_who, "<unknown>");
                ci->suspend_reason =
                    (char *)"Unknown (imported from Cygnus)";
                ci->suspend_time = time(NULL);
                ci->suspend_expires = 0;
            }
            if (myflags & 0x0020)   /* HELD, i.e. not expiring */
                ci->flags |= CF_NOEXPIRE;
            /* Handle the memolevel setting */
            tmp = strtol(memolevel, (char **)&memolevel, 10);
            if (memolevel && *memolevel) {
                fprintf(stderr, "%s:%d: Memo level value for `%s' is not a"
                        " valid number; using default\n", fname, line,
                        ci->name);
            } else switch (tmp) {
              case 1:
                ci->levels[CA_MEMO] = ACCLEV_VOP;
                break;
              case 2:
                ci->levels[CA_MEMO] = ACCLEV_HOP;
                break;
              case 3:
                ci->levels[CA_MEMO] = ACCLEV_AOP;
                break;
              case 4:
                ci->levels[CA_MEMO] = ACCLEV_SOP;
                break;
              case 5:
                ci->levels[CA_MEMO] = ACCLEV_FOUNDER;
                break;
              default:
                fprintf(stderr, "%s:%d: Memo level value for `%s' is out of"
                        " range; using default\n", fname, line, ci->name);
                break;
            }
            break;
          } /* case WHAT('C','I') */

          case WHAT('C','A'): {   /* Access list entry */
            const char *nick = strtok(NULL, " \r\n");
            const char *level_s = strtok(NULL, " \r\n");
            const NickInfo *ni;
            long level;
            int i;
            if (!ci)
                break;
            if (!nick || !level_s) {
                fprintf(stderr, "%s:%d: Corrupt CA line, ignoring\n",
                        fname, line);
                break;
            }
            ni = get_nickinfo(nick);
            if (!ni) {
                fprintf(stderr, "%s:%d: %s access entry `%s' is not a"
                        " registered nickname, ignoring entry\n",
                        fname, line, ci->name, nick);
                break;
            } else if (ni->status & NS_VERBOTEN) {
                fprintf(stderr, "%s:%d: %s access entry `%s' is a"
                        " forbidden nickname, ignoring entry\n",
                        fname, line, ci->name, nick);
                break;
            } else if (!ni->nickgroup) {
                fprintf(stderr, "%s:%d: %s access entry `%s' has an"
                        " invalid nickname record (BUG?), ignoring entry\n",
                        fname, line, ci->name, nick);
                break;
            } else if (ni->nickgroup == ci->founder) {
                fprintf(stderr, "%s:%d: %s access entry `%s' is the channel"
                        " founder (or is in the same nickname group),"
                        " ignoring entry\n", fname, line, ci->name, nick);
                break;
            }
            for (i = 0; i < ci->access_count; i++) {
                if (ni->nickgroup == ci->access[i].nickgroup) {
                    fprintf(stderr, "%s:%d: %s access entry `%s': nickname"
                            " group is already on the access list, ignoring"
                            " entry\n", fname, line, ci->name, nick);
                    break;
                }
            }
            if (i < ci->access_count)
                break;
            level = strtol(level_s, (char **)&level_s, 10);
            if (level_s && *level_s) {
                fprintf(stderr, "%s:%d: %s access entry `%s': Invalid"
                        " access level, ignoring entry\n",
                        fname, line, ci->name, nick);
                break;
            } else if (level < 0 || level > 5) {
                fprintf(stderr, "%s:%d: %s access entry `%s': Access level"
                        " out of range, ignoring entry\n",
                        fname, line, ci->name, nick);
            }
            switch (level) {
                case 5:
                case 4:  level = ACCLEV_SOP; break;
                case 3:  level = ACCLEV_AOP; break;
                case 2:  level = ACCLEV_HOP; break;
                case 1:  level = ACCLEV_VOP; break;
                default: level = 0;          break;
            }
            ARRAY_EXTEND(ci->access);
            ci->access[ci->access_count-1].nickgroup = ni->nickgroup;
            ci->access[ci->access_count-1].level = level;
            break;
          } /* case WHAT('C','A') */

          case WHAT('A','K'): {   /* Autokick entry */
            const char *mask = strtok(NULL, " \r\n");
            const char *setter = strtok(NULL, " \r\n");
            const char *time_s = strtok(NULL, " \r\n");
            const char *reason = cyg_strtok_remaining();
            time_t stime;
            if (!ci)
                break;
            if (!mask || !setter || !time_s) {
                fprintf(stderr, "%s:%d: Corrupt AK line, ignoring\n",
                        fname, line);
                break;
            }
            if (reason && !*reason)
                reason = NULL;
            CVT_TIME(stime, time_s,
                     "%s autokick `%s': Set time", ci->name, mask);
            ARRAY_EXTEND(ci->akick);
            ci->akick[ci->akick_count-1].mask = sstrdup(mask);
            ci->akick[ci->akick_count-1].reason = reason ? sstrdup(reason)
                                                         : NULL;
            strbcpy(ci->akick[ci->akick_count-1].who, setter);
            ci->akick[ci->akick_count-1].set = stime;
            ci->akick[ci->akick_count-1].lastused = 0;
            break;
          } /* case WHAT('A','K') */

          case WHAT('S','U'): { /* Successor */
            NickInfo *ni;
            if (!ci)
                break;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt SU line, ignoring\n",
                        fname, line);
                break;
            }
            ni = get_nickinfo(s);
            if (!ni) {
                fprintf(stderr, "%s:%d: %s successor `%s' is not a"
                        " registered nickname, ignoring\n",
                        fname, line, ci->name, s);
                break;
            } else if (ni->status & NS_VERBOTEN) {
                fprintf(stderr, "%s:%d: %s successor `%s' is a forbidden"
                        " nickname, ignoring\n", fname, line, ci->name, s);
                break;
            } else if (!ni->nickgroup) {
                fprintf(stderr, "%s:%d: %s successor `%s' has an invalid"
                        " nickname record (BUG?), ignoring\n",
                        fname, line, ci->name, s);
                break;
            } else if (ni->nickgroup == ci->founder) {
                fprintf(stderr, "%s:%d: %s successor `%s' is the channel"
                        " founder (or is in the same nickname group),"
                        " ignoring\n", fname, line, ci->name, s);
                break;
            }
            ci->successor = ni->nickgroup;
            break;
          } /* case WHAT('S','U') */

          case WHAT('G','R'):   /* Entry message */
            if (!ci)
                break;
            s = cyg_strtok_remaining();
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt GR line, ignoring\n",
                        fname, line);
                break;
            }
            ci->entry_message = sstrdup(s);
            break;

          case WHAT('U','R'):   /* URL */
            if (!ci)
                break;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt UR line, ignoring\n",
                        fname, line);
                break;
            }
            ci->url = sstrdup(s);
            break;

          case WHAT('C','T'): { /* Channel topic */
            const char *nick = strtok(NULL, " \r\n");
            const char *time_s = strtok(NULL, " \r\n");
            const char *topic = cyg_strtok_remaining();
            if (!nick || !time_s || !topic) {
                fprintf(stderr, "%s:%d: Corrupt CA line, ignoring\n",
                        fname, line);
                break;
            }
            CVT_TIME(ci->last_topic_time, time_s,
                     "%s topic: Set time", ci->name);
            strbcpy(ci->last_topic_setter, nick);
            ci->last_topic = sstrdup(topic);
            break;
          } /* case WHAT('C','T') */

          case WHAT('K','Y'):   /* Locked key */
            if (!ci)
                break;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt KY line, ignoring\n",
                        fname, line);
                break;
            }
            if (!strchr(ci->mlock.on, 'k')) {
                fprintf(stderr, "%s:%d: Locked key given for channel %s"
                        " without MLOCK +k, ignoring\n",
                        fname, line, ci->name);
                break;
            }
            ci->mlock.key = sstrdup(s);
            break;

          case WHAT('L','M'): { /* Locked limit */
            long limit;
            if (!ci)
                break;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt LM line, ignoring\n",
                        fname, line);
                break;
            }
            if (!strchr(ci->mlock.on, 'l')) {
                fprintf(stderr, "%s:%d: Locked limit given for channel %s"
                        " without MLOCK +l, ignoring\n",
                        fname, line, ci->name);
                break;
            }
            limit = strtol(s, (char **)&s, 10);
            if (s && *s) {
                fprintf(stderr, "%s:%d: Locked limit for channel %s is"
                        " invalid, ignoring\n", fname, line, ci->name);
                s = strchr(ci->mlock.on, 'l');
                if (s)  /* always true, from check above */
                    memmove(s, s+1, strlen(s+1)+1);
                break;
            }
            ci->mlock.limit = limit;
            break;
          } /* case WHAT('L','M') */

          case WHAT('F','L'):   /* Locked flood setting */
            if (!ci)
                break;
            s = cyg_strtok_remaining();
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt FL line, ignoring\n",
                        fname, line);
                break;
            }
            if (!strchr(ci->mlock.on, 'f')) {
                fprintf(stderr, "%s:%d: Locked flood setting given for"
                        " channel %s without MLOCK +f, ignoring\n",
                        fname, line, ci->name);
                break;
            }
            ci->mlock.flood = sstrdup(s);
            break;

          case WHAT('L','K'):   /* Locked link */
            if (!ci)
                break;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt LK line, ignoring\n",
                        fname, line);
                break;
            }
            if (!strchr(ci->mlock.on, 'L')) {
                fprintf(stderr, "%s:%d: Locked link given for channel %s"
                        " without MLOCK +L, ignoring\n",
                        fname, line, ci->name);
                break;
            }
            ci->mlock.link = sstrdup(s);
            break;

          case WHAT('V','F'):   /* Channel verify; ignore */
            break;

          case WHAT('D','E'):   /* Database end; check counts */
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "Warning: check line corrupt; database may"
                        " be invalid");
                break;
            }
            if (atoi(s) != ccount) {
                fprintf(stderr, "Warning: bad channel count (%d expected,"
                        " %d found)--database may be corrupt\n",
                        atoi(s), ccount);
            }
            break;
        } /* switch (WHAT(s)) */
    } /* while (fgets()) */

    fclose(f);

    /* Sanity check for locked modes vs. their parameters */
    for (ci = first_channelinfo(); ci; ci = next_channelinfo()) {
        s = strchr(ci->mlock.on, 'k');
        if (s && !ci->mlock.key) {
            fprintf(stderr, "%s: Channel %s has MLOCK +k but no locked key,"
                    " removing MLOCK +k", fname, ci->name);
            memmove(s, s+1, strlen(s+1)+1);
        }
        s = strchr(ci->mlock.on, 'l');
        if (s && !ci->mlock.limit) {
            fprintf(stderr, "%s: Channel %s has MLOCK +l but no locked limit,"
                    " removing MLOCK +l", fname, ci->name);
            memmove(s, s+1, strlen(s+1)+1);
        }
        s = strchr(ci->mlock.on, 'f');
        if (s && !ci->mlock.flood) {
            fprintf(stderr, "%s: Channel %s has MLOCK +k but no locked flood"
                    " setting, removing MLOCK +f", fname, ci->name);
            memmove(s, s+1, strlen(s+1)+1);
        }
        s = strchr(ci->mlock.on, 'L');
        if (s && !ci->mlock.link) {
            fprintf(stderr, "%s: Channel %s has MLOCK +L but no locked link,"
                    " removing MLOCK +L", fname, ci->name);
            memmove(s, s+1, strlen(s+1)+1);
        }
    }
}

/*************************************************************************/

static void cyg_load_root(const char *dir)
{
    FILE *f;
    char fname[PATH_MAX+1];
    char buf[4096];  /* Cygnus uses 2048; let's be safe */
    char *s;
    MaskData *md;
    int acount = 0, tcount = 0, ecount = 0;  /* to check against DE line */
    int line, i;

    snprintf(fname, sizeof(fname), "%s/rootserv.db", dir);
    f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "Cannot open %s: %s\n", fname, strerror(errno));
        exit(1);
    }
    fgets(buf, sizeof(buf), f);
    if (!(s = strtok(buf, " \r\n"))
     || strcmp(s,"RV") != 0
     || !(s = strtok(NULL, " \r\n"))
    ) {
        fprintf(stderr, "%s: Version number missing", fname);
        exit(1);
    }
    if (atoi(s) != 3) {
        fprintf(stderr, "%s: Bad version number (expected 3, got %d)\n",
                fname, atoi(s));
        exit(1);
    }

    line = 1;
    while (fgets(buf, sizeof(buf), f)) {
        line++;
        if (!(s = strtok(buf, " \r\n")))
            continue;
        switch (WHAT(s[0],s[1])) {
          case WHAT('A','K'): { /* Autokills / SGlines */
            const char *mask = strtok(NULL, " \r\n");
            const char *setter = strtok(NULL, " \r\n");
            const char *realname = strtok(NULL, " \r\n");
            const char *set = strtok(NULL, " \r\n");
            const char *expires = strtok(NULL, " \r\n");
            const char *reason = cyg_strtok_remaining();
            acount++;
            if (!mask || !setter || !realname || !set || !expires || !reason) {
                fprintf(stderr, "%s:%d: Corrupt AK line, ignoring\n",
                        fname, line);
                break;
            }
            i = strtol(realname, (char **)&realname, 10);
            if (realname && *realname) {
                fprintf(stderr, "%s:%d: Autokill `%s' realname value is"
                        " invalid, ignoring autokill", fname, line, mask);
                break;
            }
            md = scalloc(sizeof(*md), 1);
            md->mask = sstrdup(mask);
            md->reason = sstrdup(reason);
            strbcpy(md->who, setter);
            CVT_TIME(md->time, set, "Autokill `%s' set-time value", mask);
            CVT_TIME_0(md->expires, expires,
                       "Autokill `%s' expires value", mask);
            if (md->expires)
                md->expires += md->time;
            md->lastused = 0;
            add_maskdata(i ? MD_SGLINE : MD_AKILL, md);
            break;
          } /* case WHAT('A','K') */

          case WHAT('T','R'): { /* Triggers (limited exceptions) */
            const char *mask = strtok(NULL, " \r\n");
            const char *limit_s = strtok(NULL, " \r\n");
            int limit;
            tcount++;
            if (!mask || !limit_s) {
                fprintf(stderr, "%s:%d: Corrupt TR line, ignoring\n",
                        fname, line);
                break;
            }
            limit = strtol(limit_s, (char **)&limit_s, 10);
            if (limit_s && *limit_s) {
                fprintf(stderr, "%s:%d: Trigger `%s' limit value is invalid,"
                        " ignoring trigger", fname, line, mask);
                break;
            } else if (limit < 0) {
                fprintf(stderr, "%s:%d: Trigger `%s' limit value is negative,"
                        " ignoring trigger", fname, line, mask);
                break;
            } else if (limit == 0) {
                fprintf(stderr, "%s:%d: Trigger `%s' limit value is zero,"
                        " converting to autokill", fname, line, mask);
            } else if (limit > MAX_MASKDATA_LIMIT) {
                limit = MAX_MASKDATA_LIMIT;
                fprintf(stderr, "%s:%d: Trigger `%s' limit value is too large,"
                        " setting to %d", fname, line, mask, limit);
            }
            md = scalloc(sizeof(*md), 1);
            md->mask = sstrdup(mask);
            md->limit = limit;
            md->reason = sstrdup("Unknown (imported from Cygnus)");
            strbcpy(md->who, "<unknown>");
            md->time = time(NULL);
            md->expires = 0;
            md->lastused = 0;
            add_maskdata(limit ? MD_EXCEPTION : MD_AKILL, md);
            break;
          } /* case WHAT('T','R') */

          case WHAT('E','X'):   /* Exceptions (unlimited exceptions) */
            ecount++;
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "%s:%d: Corrupt EX line, ignoring\n",
                        fname, line);
                break;
            }
            md = scalloc(sizeof(*md), 1);
            md->mask = sstrdup(s);
            md->limit = 0;
            md->reason = sstrdup("Unknown (imported from Cygnus)");
            strbcpy(md->who, "<unknown>");
            md->time = time(NULL);
            md->expires = 0;
            md->lastused = 0;
            add_maskdata(MD_EXCEPTION, md);
            break;

          case WHAT('N','W'):   /* Last news update; ignore */
            break;

          case WHAT('R','S'):   /* Maximum uptime; ignore */
            break;

          case WHAT('D','E'):   /* Database end; check counts */
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "Warning: check line corrupt; database may"
                        " be invalid");
                break;
            }
            if (atoi(s) != acount) {
                fprintf(stderr, "Warning: bad autokill count (%d expected, %d"
                        " found)--database may be corrupt\n", atoi(s), acount);
            }
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "Warning: check line corrupt; database may"
                        " be invalid");
                break;
            }
            if (atoi(s) != tcount) {
                fprintf(stderr, "Warning: bad trigger count (%d expected, %d"
                        " found)--database may be corrupt\n", atoi(s), tcount);
            }
            s = strtok(NULL, " \r\n");
            if (!s) {
                fprintf(stderr, "Warning: check line corrupt; database may"
                        " be invalid");
                break;
            }
            if (atoi(s) != ecount) {
                fprintf(stderr, "Warning: bad exception count (%d expected, %d"
                        " found)--database may be corrupt\n", atoi(s), ecount);
            }
            break;
        } /* switch (WHAT(s)) */
    } /* while (fgets()) */

    fclose(f);
}

/*************************************************************************/
/*************************************************************************/

static const char *check_cygnus(const char *dir)
{
    char buf[PATH_MAX+1];
    FILE *f;

    snprintf(buf, sizeof(buf), "%s/rootserv.db", dir);
    f = fopen(buf, "r");
    if (f) {
        char *s;
        fgets(buf, sizeof(buf), f);
        fclose(f);
        s = strtok(buf, " \r\n");
        if (strcmp(s,"RV") == 0) {
            s = strtok(NULL, " \r\n");
            if (atoi(s) == 3)
                return "Cygnus 0.2.0";
        }
    }
    return NULL;
}

static void load_cygnus(const char *dir, int verbose, int ac, char **av)
{
    int i;

    timezones = (int *)default_timezones;
    ntimezones = lenof(default_timezones);
    reset_memo_limits = 0;
    for (i = 1; i < ac; i++) {
        if (strncmp(av[i],"-tzfile=",8) == 0) {
            const char *filename = av[i]+8;
            FILE *f = fopen(filename, "r");
            if (!f) {
                fprintf(stderr, "Cannot open time zone file `%s': %s\n",
                        filename, strerror(errno));
            } else {
                char buf[BUFSIZE];
                fprintf(stderr, "Using time zone file `%s'.\n", filename);
                ntimezones = 0;
                while (fgets(buf, sizeof(buf), f)) {
                    int n = atoi(buf);
                    if (n > ntimezones)
                        ntimezones = n;
                }
                if (ntimezones <= 0) {
                    fprintf(stderr, "Warning: time zone file `%s' is empty\n",
                            filename);
                    timezones = NULL;
                } else {
                    int line = 0;
                    timezones = smalloc(sizeof(*timezones) * ntimezones);
                    memset(timezones, -1, sizeof(timezones));
                    fseek(f, 0, SEEK_SET);
                    while (fgets(buf, sizeof(buf), f)) {
                        char *nstr, *ofsstr;
                        int n, ofs;
                        line++;
                        nstr   = strtok(buf,  " \t\r\n");
                        (void)   strtok(NULL, " \t\r\n");
                        (void)   strtok(NULL, " \t\r\n");
                        ofsstr = strtok(NULL, " \t\r\n");
                        if (!nstr || !ofsstr) {
                            fprintf(stderr, "Warning: invalid format in time"
                                    " zone file `%s' line %d\n",
                                    filename, line);
                            continue;
                        }
                        n = strtol(nstr, &nstr, 10);
                        ofs = strtol(ofsstr, &ofsstr, 10);
                        if ((nstr && *nstr) || (ofsstr && *ofsstr)) {
                            fprintf(stderr, "Warning: invalid format in time"
                                    " zone file `%s' line %d\n",
                                    filename, line);
                        } else if (n < 1 || n > ntimezones) {
                            fprintf(stderr, "Warning: invalid time zone index"
                                    " in time zone file `%s' line %d\n",
                                    filename, line);
                        } else if (ofs < -12*60*60 || ofs > 13*60*60) {
                            fprintf(stderr, "Warning: time zone offset out of"
                                    " range in time zone file `%s' line %d\n",
                                    filename, line);
                        } else {
                            timezones[n-1] = ofs/60;
                        }
                    }
                }
            }
        } else if (strcmp(av[i],"-no-timezones") == 0) {
            ntimezones = 0;
            if (timezones != default_timezones)
                free(timezones);
            timezones = NULL;
        } else if (strcmp(av[i],"-reset-memo-limits") == 0) {
            reset_memo_limits = 1;
        } else {
            fprintf(stderr, "Unrecognized option %s\n", av[i]);
            usage(av[0]);
        }
    }

    if (verbose)
        fprintf(stderr, "Loading nickserv.db...\n");
    cyg_load_nick(dir);
    if (verbose)
        fprintf(stderr, "Loading chanserv.db...\n");
    cyg_load_chan(dir);
    if (verbose)
        fprintf(stderr, "Loading rootserv.db...\n");
    cyg_load_root(dir);
}

/*************************************************************************/
/*************************************************************************/

DBTypeInfo dbtype_cygnus = {
    "cygnus",
    check_cygnus,
    load_cygnus
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
