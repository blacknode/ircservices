/* Conversion routines for HybServ.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "convert-db.h"

/*************************************************************************/

/* Encrypted passwords in use? (-crypt option) */
static int crypted_passwords = 0;

/*************************************************************************/
/*************************************************************************/

/* Get the next parameter, which may be a colon-prefixed rest-of-the-line
 * parameter.  As with strtok(), a NULL parameter means "continue from the
 * last call". */

static inline char *next_token(char *s)
{
    static char *str = NULL;
    char *res;

    if (s)
        str = s;
    if (!str)
        return NULL;
    str += strspn(str, " \r\n");
    if (!*str) {
        str = NULL;
        return NULL;
    }
    if (*str == ':') {
        res = str+1;
        str += strcspn(str, "\r\n");
        *str = 0;
        str = NULL;
    } else {
        res = str;
        str += strcspn(str, " \r\n");
        if (*str)
            *str++ = 0;
    }
    return res;
}

/*************************************************************************/

/* Nickname flag conversion table (zero-terminated) */
static const struct {
    int32 hybflag;      /* HybServ flag value */
    int32 nf;           /* Flags to set in ngi->flags */
    int32 ns;           /* Flags to set in ni->status */
} hyb_nickflags[] = {
    { 0x00000001, 0,               0           },  /* NS_IDENTIFIED */
    { 0x00000002, NF_KILLPROTECT,  0           },  /* NS_PROTECTED */
    /* NS_OPERATOR: operator-owned nick, doesn't expire */
    { 0x00000004, 0,               NS_NOEXPIRE },
    /* NS_AUTOMASK: automatically add new hostmasks to ACCESS list */
    { 0x00000008, 0,               0           },
    { 0x00000010, NF_PRIVATE,      0           },  /* NS_PRIVATE */
    { 0x00000020, 0,               0           },  /* NS_COLLIDE */
    { 0x00000040, 0,               0           },  /* NS_RELEASE */
    { 0x00000080, 0,               NS_VERBOTEN },  /* NS_FORBID */
    { 0x00000100, NF_SECURE,       0           },  /* NS_SECURE */
    { 0x00000200, 0,               0           },  /* NS_DELETE */
    /* NS_UNSECURE: like !SECURE, but assume IDENTIFIED on access match */
    { 0x00000400, 0,               0           },
    { 0x00000800, NF_MEMO_SIGNON,  0           },  /* NS_MEMOSIGNON */
    { 0x00001000, NF_MEMO_RECEIVE, 0           },  /* NS_MEMONOTIFY */
    /* NS_MEMOS: allow memos to be sent to us (handled separately) */
    { 0x00002000, 0,               0           },
    { 0x00004000, NF_HIDE_EMAIL|NF_HIDE_MASK|NF_HIDE_QUIT, 0 }, /*NS_HIDEALL*/
    { 0x00008000, NF_HIDE_EMAIL,   0           },  /* NS_HIDEEMAIL */
    { 0x00010000, 0,               0           },  /* NS_HIDEURL */
    { 0x00020000, NF_HIDE_QUIT,    0           },  /* NS_HIDEQUIT */
    { 0x00040000, NF_HIDE_MASK,    0           },  /* NS_HIDEADDR */
    { 0x00080000, NF_KILL_IMMED,   0           },  /* NS_KILLIMMED */
    /* NS_NOREGISTER: not allowed to register channels */
    { 0x00100000, 0,               0           },
    { 0x00200000, NF_NOOP,         0           },  /* NS_NOCHANOPS */
    /* NS_TFORBID: temporary forbid (expiration time in ni->lastseen) */
    { 0x00400000, 0,               0           },
    { 0 }
};

static void hyb_load_nick(const char *dir)
{
    FILE *f;
    char fname[PATH_MAX+1];
    char buf[4096];  /* HybServ uses MAXLINE (510); let's be safe */
    char *s;
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;
    int line;

    snprintf(fname, sizeof(fname), "%s/nick.db", dir);
    f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "Cannot open %s: %s\n", fname, strerror(errno));
        exit(1);
    }

    line = 0;
    while (fgets(buf, sizeof(buf), f)) {
        line++;
        if (!(s = next_token(buf)))
            continue;
        if (*s == ';')
            continue;

        if (strncmp(s, "->", 2) != 0) {
            /* New nickname */

            char *s2, *s3, *nick = s;
            long flags, timereg, lastseen;

#if CLEAN_COMPILE
            flags = timereg = lastseen = 0;
#endif
            ni = NULL;
            ngi = NULL;
            s = next_token(NULL);
            if (s)
                flags = strtol(s, &s, 10);
            s2 = next_token(NULL);
            if (s2)
                timereg = strtol(s2, &s2, 10);
            s3 = next_token(NULL);
            if (s3)
                lastseen = strtol(s3, &s3, 10);
            if (!s || !s2 || !s3 || *s || *s2 || *s3) {
                fprintf(stderr, "%s:%d: Invalid nickname line, ignoring"
                        " nick %s\n", fname, line, nick);
            } else if (get_nickinfo(nick)) {
                fprintf(stderr, "%s:%d: Nickname `%s' already exists,"
                        " skipping\n", fname, line, nick);
            } else {
                int i;
                if (strlen(nick) > NICKMAX-1) {
                    fprintf(stderr, "%s:%d: Nickname %s truncated to %d"
                            " characters\n", fname, line, nick, NICKMAX-1);
                    nick[NICKMAX-1] = 0;
                }
                ni = makenick(nick, &ngi);
                ni->time_registered = (time_t)timereg;
                ni->last_seen = (time_t)lastseen;
                for (i = 0; hyb_nickflags[i].hybflag; i++) {
                    if (flags & hyb_nickflags[i].hybflag) {
                        ngi->flags |= hyb_nickflags[i].nf;
                        ni->status |= hyb_nickflags[i].ns;
                    }
                }
                if (!(flags & 0x2000))  /* NS_MEMOS */
                    ngi->memos.memomax = 0;
                if (ni->status & NS_VERBOTEN) {
                    /* Forbidden nick, so delete the nickgroup */
                    ni->nickgroup = 0;
                    del_nickgroupinfo(ngi);
                    ngi = NULL;
                }
            } /* if valid nickname line */

        } else if (ni) {
            /* Parameter for the current nickname */

            s += 2;
            if (stricmp(s, "PASS") == 0) {
                if (ngi) {
                    if (*ngi->pass.password) {
                        fprintf(stderr, "%s:%d: Duplicate PASS line,"
                                " ignoring\n", fname, line);
                    } else {
                        char *pass = next_token(NULL);
                        if (!pass || !*pass) {
                            fprintf(stderr, "%s:%d: Corrupt PASS line, setting"
                                    " password to nickname for %s\n",
                                    fname, line, ni->nick);
                            if (crypted_passwords) {
                                pass = crypt(ni->nick, "xx");
                                if (!pass) {
                                    perror("FATAL: crypt() failed");
                                    exit(1);
                                }
                            } else {
                                pass = ni->nick;
                            }
                        }
                        if (strlen(pass) > sizeof(ngi->pass.password)-1) {
                            if (crypted_passwords) {
                                fprintf(stderr, "FATAL: PASSMAX too small for"
                                        " encrypted passwords!\n");
                                exit(1);
                            } else {
                                fprintf(stderr, "%s:%d: Password for `%s'"
                                        " truncated to %d characters\n", fname,
                                        line, ni->nick,
                                        sizeof(ngi->pass.password)-1);
                            }
                        }
                        strscpy(ngi->pass.password, pass,
                                sizeof(ngi->pass.password));
                        ngi->pass.cipher = sstrdup("unix-crypt");
                    }
                }

            } else if (stricmp(s, "PERMPASS") == 0) {
                /* ignore */

            } else if (stricmp(s, "FORBIDBY") == 0) {
                /* ignore */

            } else if (stricmp(s, "FORBIDREASON") == 0) {
                /* ignore */

            } else if (stricmp(s, "FREASON") == 0) {
                /* ignore */

            } else if (stricmp(s, "FTIME") == 0) {
                /* ignore */

            } else if (stricmp(s, "EMAIL") == 0) {
                if (ngi) {
                    s = next_token(NULL);
                    if (!s) {
                        fprintf(stderr, "%s:%d: Corrupt EMAIL line,"
                                " ignoring\n", fname, line);
                    } else if (ngi->email) {
                        fprintf(stderr, "%s:%d: Duplicate EMAIL line,"
                                " ignoring\n", fname, line);
                    } else {
                        ngi->email = sstrdup(s);
                    }
                }

            } else if (stricmp(s, "URL") == 0) {
                if (ngi) {
                    s = next_token(NULL);
                    if (!s) {
                        fprintf(stderr, "%s:%d: Corrupt URL line,"
                                " ignoring\n", fname, line);
                    } else if (ngi->url) {
                        fprintf(stderr, "%s:%d: Duplicate URL line,"
                                " ignoring\n", fname, line);
                    } else {
                        ngi->url = sstrdup(s);
                    }
                }

            } else if (stricmp(s, "UIN") == 0
                    || stricmp(s, "GSM") == 0
                    || stricmp(s, "PHONE") == 0
                    || stricmp(s, "ICQ") == 0
            ) {
                /* ignore */

            } else if (stricmp(s, "LANG") == 0) {
                /* ignore */

            } else if (stricmp(s, "LASTUH") == 0) {
                char *user = next_token(NULL);
                char *host = next_token(NULL);
                char buf[1024];
                if (!user || !host) {
                    fprintf(stderr, "%s:%d: Corrupt LASTUH line,"
                            " ignoring\n", fname, line);
                } else {
                    snprintf(buf, sizeof(buf), "%s@%s", user, host);
                    if (ni->last_usermask) {
                        fprintf(stderr, "%s:%d: Duplicate LASTUH line,"
                                " ignoring\n", fname, line);
                    } else {
                        ni->last_usermask = sstrdup(buf);
                        ni->last_realmask = sstrdup(buf);
                    }
                }

            } else if (stricmp(s, "LASTQMSG") == 0) {
                s = next_token(NULL);
                if (!s) {
                    fprintf(stderr, "%s:%d: Corrupt LASTQMSG line,"
                            " ignoring\n", fname, line);
                } else if (ni->last_quit) {
                    fprintf(stderr, "%s:%d: Duplicate LASTQMSG line,"
                            " ignoring\n", fname, line);
                } else {
                    ni->last_quit = sstrdup(s);
                }

            } else if (stricmp(s, "HOST") == 0) {
                if (ngi) {
                    s = next_token(NULL);
                    if (!s) {
                        fprintf(stderr, "%s:%d: Corrupt HOST line, ignoring\n",
                                fname, line);
                    } else {
                        ARRAY_EXTEND(ngi->access);
                        ngi->access[ngi->access_count-1] = sstrdup(s);
                    }
                }

            } else if (stricmp(s, "LINK") == 0) {
                char *master = next_token(NULL);
                NickInfo *ni2;
                NickGroupInfo *ngi2;
                if (strlen(master) > NICKMAX-1)
                    master[NICKMAX-1] = 0;
                /* Link targets are always written before the link itself */
                if (!(ni2 = get_nickinfo(master))) {
                    fprintf(stderr, "%s:%d: Master nick %s for nickname %s"
                            " is missing\n", fname, line, master, ni->nick);
                } else if (!(ngi2 = get_nickgroupinfo(ni2->nickgroup))) {
                    fprintf(stderr, "%s:%d: Nickgroup missing for master"
                            " nick %s, ignoring link\n", fname, line, master);
                } else {
                    del_nickgroupinfo(ngi);
                    ngi = NULL;
                    ni->nickgroup = ngi2->id;
                    ARRAY_EXTEND(ngi2->nicks);
                    strscpy(ngi2->nicks[ngi2->nicks_count-1], ni->nick,
                            sizeof(*ngi2->nicks));
                }

            } else {
                static int warnings = 0;
                switch (warnings) {
                  case 5:
                    fprintf(stderr, "%s:%d: More unrecognized nickname"
                            " options follow\n", fname, line);
                    warnings = -1;
                    /* fall through */
                  case -1:
                    break;
                  default:
                    fprintf(stderr, "%s:%d: Unrecognized nickname option"
                            " `%s'\n", fname, line, s);
                    warnings++;
                    break;
                }
            }
        } /* nick parameters */
    } /* while (fgets()) */

    fclose(f);
}

/*************************************************************************/

/* Channel flag conversion table (zero-terminated) */
static const struct {
    int32 hybflag;      /* HybServ flag value */
    int32 cf;           /* Flags to set in ci->flags */
} hyb_chanflags[] = {
    { 0x00000001, CF_PRIVATE,   },  /* CS_PRIVATE */
    { 0x00000002, CF_TOPICLOCK  },  /* CS_TOPICLOCK */
    { 0x00000004, CF_SECURE     },  /* CS_SECURE */
    { 0x00000008, CF_SECUREOPS  },  /* CS_SECUREOPS */
    { 0x00000010, CF_SUSPENDED  },  /* CS_SUSPENDED */
    { 0x00000020, CF_VERBOTEN   },  /* CS_FORBID */
    { 0x00000040, CF_RESTRICTED },  /* CS_RESTRICTED */
    { 0x00000080, 0             },  /* CS_FORGET */
    { 0x00000100, 0             },  /* CS_DELETE */
    { 0x00000200, CF_NOEXPIRE   },  /* CS_NOEXPIRE */
    { 0x00000400, 0             },  /* CS_GUARD (ChanServ join) */
    /* 1.7.3 and later: CS_SPLITOPS--same as LEAVEOPS
     * 1.6.1+UniBG: CS_TFORBID--temporary forbid (expiration time in
     *              ci->lastused)
     * We differentiate between these by whether CS_FORBID is also set
     * (+UniBG always sets CS_FORBID with CS_TFORBID) */
    { 0x00000800, 0             },
    { 0 }
};

/* Channel mode conversion table */
static struct {
    int32 flag;
    char mode;
} cmodes[] = {
    { 0x00000010, 'l' },
    { 0x00000020, 'k' },
    { 0x00000040, 's' },
    { 0x00000080, 'p' },
    { 0x00000100, 'n' },
    { 0x00000200, 't' },
    { 0x00000400, 'm' },
    { 0x00000800, 'i' },
    { 0, 0 }
};

static void hyb_load_chan(const char *dir)
{
    FILE *f;
    char fname[PATH_MAX+1];
    char buf[4096];  /* HybServ uses MAXLINE (510); let's be safe */
    char *s;
    ChannelInfo *ci = NULL;
    int line;

    snprintf(fname, sizeof(fname), "%s/chan.db", dir);
    f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "Cannot open %s: %s\n", fname, strerror(errno));
        exit(1);
    }

    line = 0;
    while (fgets(buf, sizeof(buf), f)) {
        line++;
        if (!(s = next_token(buf)))
            continue;
        if (*s == ';')
            continue;

        if (strncmp(s, "->", 2) != 0) {
            /* New channel */

            char *s2, *s3, *channel = s;
            long flags, timereg, lastused;

#if CLEAN_COMPILE
            flags = timereg = lastused = 0;
#endif
            if (ci) {
                if (!(ci->flags & CF_VERBOTEN) && !ci->founder) {
                    fprintf(stderr, "Ignoring channel %s (missing founder)\n",
                            ci->name);
                    del_channelinfo(ci);
                }
            }

            ci = NULL;
            s = next_token(NULL);
            if (s)
                flags = strtol(s, &s, 10);
            s2 = next_token(NULL);
            if (s2)
                timereg = strtol(s2, &s2, 10);
            s3 = next_token(NULL);
            if (s3)
                lastused = strtol(s3, &s3, 10);
            if (*channel!='#' || !s || !s2 || !s3 || *s || *s2 || *s3) {
                fprintf(stderr, "%s:%d: Invalid channel line, ignoring"
                        " channel %s\n", fname, line, channel);
            } else if (strcmp(channel, "#") == 0) {
                fprintf(stderr, "%s:%d: Channel `#' not supported in IRC"
                        " Services, ignoring\n", fname, line);
            } else if (get_channelinfo(channel)) {
                fprintf(stderr, "%s:%d: Channel `%s' already exists,"
                        " skipping\n", fname, line, channel);
            } else if (flags & 0x80) {  /* CS_FORGET */
                fprintf(stderr, "%s:%d: Skipping forgotten channel %s\n",
                        fname, line, channel);
            } else {
                int i;
                if (strlen(channel) > CHANMAX-1) {
                    fprintf(stderr, "%s:%d: Channel %s truncated to %d"
                            " characters\n", fname, line, channel, NICKMAX-1);
                    channel[CHANMAX-1] = 0;
                }
                ci = makechan(channel);
                ci->time_registered = (time_t)timereg;
                ci->last_used = (time_t)lastused;
                for (i = 0; hyb_chanflags[i].hybflag; i++) {
                    if (flags & hyb_chanflags[i].hybflag)
                        ci->flags |= hyb_chanflags[i].cf;
                }
                if (flags & 0x10) {  /* CS_SUSPENDED */
                    strbcpy(ci->suspend_who, "<unknown>");
                    ci->suspend_reason =
                        sstrdup("Unknown (imported from HybServ)");
                    ci->suspend_time = time(NULL);
                    ci->suspend_expires = 0;
                }
                if (flags & 0x800) {  /* CS_TFORBID or CS_SPLITOPS */
                    if (flags & 0x20) {
                        /* Forbidden channel, so it must be CS_TFORBID;
                         * we don't support TFORBID, so do nothing */
                    } else {
                        /* Non-forbidden channel, so it must be CS_SPLITOPS */
                        ci->flags |= CF_LEAVEOPS;
                    }
                }
            } /* if valid channel line */

        } else if (ci) {
            /* Parameter for the current channel */

            /* No data is stored for forbidden channels */
            if (ci->flags & CF_VERBOTEN)
                continue;

            s += 2;
            if (stricmp(s, "FNDR") == 0) {
                NickInfo *ni;
                if (ci->founder) {
                    fprintf(stderr, "%s:%d: Duplicate FNDR line, ignoring\n",
                            fname, line);
                } else {
                    s = next_token(NULL);
                    if (!(ni = get_nickinfo(s))) {
                        fprintf(stderr, "%s:%d: Nonexistent nick %s for"
                                " founder of %s\n", fname, line, s, ci->name);
                    } else if (ni->status & NS_VERBOTEN) {
                        fprintf(stderr, "%s:%d: Forbidden nick %s for"
                                " founder of %s\n", fname, line, s, ci->name);
                    } else {
                        ci->founder = ni->nickgroup;
                    }
                }

            } else if (stricmp(s, "SUCCESSOR") == 0) {
                NickInfo *ni;
                if (ci->successor) {
                    fprintf(stderr, "%s:%d: Duplicate SUCCESSOR line,"
                            " ignoring\n", fname, line);
                } else {
                    s = next_token(NULL);
                    if (!(ni = get_nickinfo(s))) {
                        fprintf(stderr, "%s:%d: Nonexistent nick %s for"
                                " successor of %s\n", fname, line, s,
                                ci->name);
                    } else if (ni->status & NS_VERBOTEN) {
                        fprintf(stderr, "%s:%d: Forbidden nick %s for"
                                " successor of %s\n", fname, line, s,
                                ci->name);
                    } else if (ni->nickgroup == ci->founder) {
                        fprintf(stderr, "%s:%d: Successor of %s is the same"
                                " as the founder, ignoring\n", fname, line,
                                ci->name);
                    } else {
                        ci->successor = ni->nickgroup;
                    }
                }

            } else if (stricmp(s, "PASS") == 0) {
                if (*ci->founderpass.password) {
                    fprintf(stderr, "%s:%d: Duplicate PASS line, ignoring\n",
                            fname, line);
                } else {
                    char *pass = next_token(NULL);
                    if (!pass || !*pass) {
                        fprintf(stderr, "%s:%d: Corrupt PASS line, setting"
                                " password to channel name for %s\n",
                                fname, line, ci->name);
                        if (crypted_passwords) {
                            pass = crypt(ci->name, "xx");
                            if (!pass) {
                                perror("FATAL: crypt() failed");
                                exit(1);
                            }
                        } else {
                            pass = ci->name;
                        }
                    }
                    if (strlen(pass) > sizeof(ci->founderpass.password)-1) {
                        if (crypted_passwords) {
                            fprintf(stderr, "FATAL: PASSMAX too small for"
                                    " encrypted passwords!\n");
                            exit(1);
                        } else {
                            fprintf(stderr, "%s:%d: Password for `%s'"
                                    " truncated to %d characters\n", fname,
                                    line, ci->name,
                                    sizeof(ci->founderpass.password)-1);
                        }
                    }
                    strscpy(ci->founderpass.password, pass,
                            sizeof(ci->founderpass.password));
                    ci->founderpass.cipher = sstrdup("unix-crypt");
                }

            } else if (stricmp(s, "PERMPASS") == 0) {
                /* ignore */

            } else if (stricmp(s, "ACCESS") == 0) {
                char *nick = next_token(NULL);
                char *level_s = next_token(NULL);
                /* Next parameter (nick who added entry) might be missing;
                 * currently not used */
                long level;
                NickInfo *ni;
#if CLEAN_COMPILE
                level = 0;
#endif
                if (level_s)
                    level = strtol(level_s, &level_s, 10);
                if (!level_s || *level_s) {
                    fprintf(stderr, "%s:%d: Corrupt ACCESS line, ignoring\n",
                            fname, line);
                } else if (!(ni = get_nickinfo(nick))) {
                    fprintf(stderr, "%s:%d: ACCESS line for nonexistent nick,"
                            " ignoring\n", fname, line);
                } else if (ni->status & NS_VERBOTEN) {
                    fprintf(stderr, "%s:%d: ACCESS line for forbidden nick,"
                            " ignoring\n", fname, line);
                } else if (ni->nickgroup != ci->founder) {
                    ARRAY_EXTEND(ci->access);
                    ci->access[ci->access_count-1].nickgroup = ni->nickgroup;
                    if (level < -9999)
                        level = -999;
                    if (level < -9)
                        level = level/10;
                    else if (level < 0)
                        level = -1;
                    else if (level < 5)
                        level = level*6;
                    else if (level < 10)
                        level = 10 + level*4;
                    else if (level < 15)
                        level = -50 + level*10;
                    else if (level < 20)
                        level = 70 + level*2;
                    else if (level < 200)
                        level = 100 + level/2;
                    else if (level < 1000)
                        level = 175 + level/8;
                    else if (level < 9000)
                        level = 300 + (level-1000)*600/8000;
                    else if (level < 10000)
                        level = level/10;
                    else
                        level = 999;
                    ci->access[ci->access_count-1].level = (int)level;
                }

            } else if (stricmp(s, "AKICK") == 0) {
                char *mask = next_token(NULL);
                char *reason = next_token(NULL);
                if (!reason) {
                    fprintf(stderr, "%s:%d: Corrupt AKICK line, ignoring\n",
                            fname, line);
                } else {
                    ARRAY_EXTEND(ci->akick);
                    ci->akick[ci->akick_count-1].mask = sstrdup(mask);
                    ci->akick[ci->akick_count-1].reason = sstrdup(reason);
                    strscpy(ci->akick[ci->akick_count-1].who, "<unknown>",
                            sizeof(ci->akick[ci->akick_count-1].who));
                    ci->akick[ci->akick_count-1].set = time(NULL);
                    ci->akick[ci->akick_count-1].lastused = 0;
                }

            } else if (stricmp(s, "ALVL") == 0) {
                /* ignore */

            } else if (stricmp(s, "TOPIC") == 0) {
                s = next_token(NULL);
                if (!s) {
                    fprintf(stderr, "%s:%d: Corrupt TOPIC line,"
                            " ignoring\n", fname, line);
                } else if (ci->last_topic) {
                    fprintf(stderr, "%s:%d: Duplicate TOPIC line,"
                            " ignoring\n", fname, line);
                } else {
                    ci->last_topic = sstrdup(s);
                    strscpy(ci->last_topic_setter, "<unknown>",
                            sizeof(ci->last_topic_setter));
                    ci->last_topic_time = time(NULL);
                }

            } else if (stricmp(s, "MON") == 0) {
                long modes;
#if CLEAN_COMPILE
                modes = 0;
#endif
                s = next_token(NULL);
                if (s)
                    modes = strtol(s, &s, 10);
                if (!s || *s) {
                    fprintf(stderr, "%s:%d: Corrupt MON line,"
                            " ignoring\n", fname, line);
                } else if (ci->mlock.on) {
                    fprintf(stderr, "%s:%d: Duplicate MON line,"
                            " ignoring\n", fname, line);
                } else {
                    int i;
                    ci->mlock.on = s = scalloc(64, 1);
                    for (i = 0; cmodes[i].flag != 0; i++) {
                        if (modes & cmodes[i].flag)
                            *s++ = cmodes[i].mode;
                    }
                    *s = 0;
                }

            } else if (stricmp(s, "MOFF") == 0) {
                long modes;
#if CLEAN_COMPILE
                modes = 0;
#endif
                s = next_token(NULL);
                if (s)
                    modes = strtol(s, &s, 10);
                if (!s || *s) {
                    fprintf(stderr, "%s:%d: Corrupt MOFF line,"
                            " ignoring\n", fname, line);
                } else if (ci->mlock.off) {
                    fprintf(stderr, "%s:%d: Duplicate MOFF line,"
                            " ignoring\n", fname, line);
                } else {
                    int i;
                    ci->mlock.off = s = scalloc(64, 1);
                    for (i = 0; cmodes[i].flag != 0; i++) {
                        if (modes & cmodes[i].flag)
                            *s++ = cmodes[i].mode;
                    }
                    *s = 0;
                }

            } else if (stricmp(s, "KEY") == 0) {
                s = next_token(NULL);
                if (!s) {
                    fprintf(stderr, "%s:%d: Corrupt KEY line,"
                            " ignoring\n", fname, line);
                } else if (ci->mlock.key) {
                    fprintf(stderr, "%s:%d: Duplicate KEY line,"
                            " ignoring\n", fname, line);
                } else {
                    ci->mlock.key = sstrdup(s);
                }

            } else if (stricmp(s, "LIMIT") == 0) {
                long limit;
#if CLEAN_COMPILE
                limit = 0;
#endif
                s = next_token(NULL);
                if (s)
                    limit = strtol(s, &s, 10);
                if (!s || *s) {
                    fprintf(stderr, "%s:%d: Corrupt LIMIT line,"
                            " ignoring\n", fname, line);
                } else if (limit < 1) {
                    fprintf(stderr, "%s:%d: Limit out of range,"
                            " ignoring\n", fname, line);
                } else if (ci->mlock.limit) {
                    fprintf(stderr, "%s:%d: Duplicate LIMIT line,"
                            " ignoring\n", fname, line);
                } else {
                    ci->mlock.limit = limit;
                }

            } else if (stricmp(s, "FORWARD") == 0) {
                s = next_token(NULL);
                if (!s) {
                    fprintf(stderr, "%s:%d: Corrupt FORWARD line,"
                            " ignoring\n", fname, line);
                } else if (ci->mlock.link) {
                    fprintf(stderr, "%s:%d: Duplicate FORWARD line,"
                            " ignoring\n", fname, line);
                } else {
                    /* Is this correct?  Dancer IRCd seems to be defunct */
                    ci->mlock.link = sstrdup(s);
                }

            } else if (stricmp(s, "ENTRYMSG") == 0) {
                s = next_token(NULL);
                if (!s) {
                    fprintf(stderr, "%s:%d: Corrupt ENTRYMSG line,"
                            " ignoring\n", fname, line);
                } else if (ci->entry_message) {
                    fprintf(stderr, "%s:%d: Duplicate ENTRYMSG line,"
                            " ignoring\n", fname, line);
                } else {
                    ci->entry_message = sstrdup(s);
                }

            } else if (stricmp(s, "EMAIL") == 0) {
                s = next_token(NULL);
                if (!s) {
                    fprintf(stderr, "%s:%d: Corrupt EMAIL line,"
                            " ignoring\n", fname, line);
                } else if (ci->email) {
                    fprintf(stderr, "%s:%d: Duplicate EMAIL line,"
                            " ignoring\n", fname, line);
                } else {
                    ci->email = sstrdup(s);
                }

            } else if (stricmp(s, "URL") == 0) {
                s = next_token(NULL);
                if (!s) {
                    fprintf(stderr, "%s:%d: Corrupt URL line,"
                            " ignoring\n", fname, line);
                } else if (ci->url) {
                    fprintf(stderr, "%s:%d: Duplicate URL line,"
                            " ignoring\n", fname, line);
                } else {
                    ci->url = sstrdup(s);
                }

            } else if (stricmp(s, "FORBIDBY") == 0) {
                /* ignore */

            } else if (stricmp(s, "FORBIDREASON") == 0) {
                /* ignore */

            } else if (stricmp(s, "FREASON") == 0) {
                /* ignore */

            } else if (stricmp(s, "FTIME") == 0) {
                /* ignore */

            } else {
                static int warnings = 0;
                switch (warnings) {
                  case 5:
                    fprintf(stderr, "%s:%d: More unrecognized channel"
                            " options follow\n", fname, line);
                    warnings = -1;
                    /* fall through */
                  case -1:
                    break;
                  default:
                    fprintf(stderr, "%s:%d: Unrecognized channel option"
                            " `%s'\n", fname, line, s);
                    warnings++;
                    break;
                }
            }
        } /* channel parameters */
    } /* while (fgets()) */

    if (ci) {
        if (!(ci->flags & CF_VERBOTEN) && !ci->founder) {
            fprintf(stderr, "Ignoring channel %s (missing founder)\n",
                    ci->name);
            del_channelinfo(ci);
        }
    }
    fclose(f);
}

/*************************************************************************/

static void hyb_load_memo(const char *dir)
{
    FILE *f;
    char fname[PATH_MAX+1];
    char buf[4096];  /* HybServ uses MAXLINE (510); let's be safe */
    char *s;
    MemoInfo *mi = NULL;
    int line;

    snprintf(fname, sizeof(fname), "%s/memo.db", dir);
    f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "Cannot open %s: %s\n", fname, strerror(errno));
        exit(1);
    }

    line = 0;
    while (fgets(buf, sizeof(buf), f)) {
        line++;
        if (!(s = next_token(buf)))
            continue;
        if (*s == ';')
            continue;

        if (strncmp(s, "->", 2) != 0) {
            /* New nickname */

            NickInfo *ni;
            NickGroupInfo *ngi;
            mi = NULL;
            if (!(ni = get_nickinfo(s))) {
                fprintf(stderr, "%s:%d: Nickname `%s' not registered,"
                        " skipping memos\n", fname, line, s);
            } else if (ni->status & NS_VERBOTEN) {
                fprintf(stderr, "%s:%d: Nickname `%s' is forbidden,"
                        " skipping memos\n", fname, line, s);
            } else if (!(ngi = get_nickgroupinfo(ni->nickgroup))) {
                fprintf(stderr, "%s:%d: BUG: Nickname `%s' missing"
                        "nickgroup, skipping memos\n", fname, line, s);
            } else {
                mi = &ngi->memos;
            }

        } else if (mi) {
            /* Memo list contents */

            s += 2;
            if (stricmp(s, "TEXT") == 0) {
                char *sender = next_token(NULL);
                char *time_s = next_token(NULL);
                char *flags_s = next_token(NULL);
                char *text = next_token(NULL);
                long time, flags;
#if CLEAN_COMPILE
                time = flags = 0;
#endif
                if (time_s)
                    time = strtol(time_s, &time_s, 10);
                if (flags_s)
                    flags = strtol(flags_s, &flags_s, 10);
                if (!sender || !time_s || *time_s
                 || !flags_s || *flags_s || !text) {
                    fprintf(stderr, "%s:%d: Corrupt TEXT line, ignoring\n",
                            fname, line);
                } else {
                    Memo *m;
                    ARRAY_EXTEND(mi->memos);
                    m = &mi->memos[mi->memos_count-1];
                    m->number = mi->memos_count;
                    m->flags = (flags & 1) ? 0 : MF_UNREAD;
                    m->time = (time_t)time;
                    strscpy(m->sender, sender, sizeof(m->sender));
                    m->text = sstrdup(text);
                }

            } else {
                static int warnings = 0;
                switch (warnings) {
                  case 5:
                    fprintf(stderr, "%s:%d: More unrecognized channel"
                            " options follow\n", fname, line);
                    warnings = -1;
                    /* fall through */
                  case -1:
                    break;
                  default:
                    fprintf(stderr, "%s:%d: Unrecognized channel option"
                            " `%s'\n", fname, line, s);
                    warnings++;
                    break;
                }
            }
        } /* memo parameters */
    } /* while (fgets()) */

    fclose(f);
}

/*************************************************************************/

static void hyb_load_stat(const char *dir)
{
    FILE *f;
    char fname[PATH_MAX+1];
    char buf[4096];  /* HybServ uses MAXLINE (510); let's be safe */
    char *s;
    int line;

    snprintf(fname, sizeof(fname), "%s/stat.db", dir);
    f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "Warning: Cannot open %s, skipping: %s\n", fname,
                strerror(errno));
        return;  /* not a critical error */
    }

    line = 0;
    while (fgets(buf, sizeof(buf), f)) {
        line++;
        if (!(s = next_token(buf)))
            continue;
        if (*s == ';')
            continue;

        if (strcmp(s, "->USERS") == 0) {
            char *maxusercnt_s, *maxusertime_s;
            long tmp1, tmp2;

#if CLEAN_COMPILE
            tmp1 = tmp2 = 0;
#endif
            maxusercnt_s = next_token(NULL);
            if (maxusercnt_s)
                tmp1 = strtol(maxusercnt_s, &maxusercnt_s, 10);
            maxusertime_s = next_token(NULL);
            if (maxusertime_s)
                tmp2 = strtol(maxusertime_s, &maxusertime_s, 10);
            if (!maxusercnt_s || !*maxusercnt_s
             || !maxusertime_s || !*maxusertime_s
            ) {
                fprintf(stderr, "%s:%d: Corrupt USERS line, ignoring",
                        fname, line);
                break;
            }
            maxusercnt = (int32)tmp1;
            maxusertime = (time_t)tmp2;
        }
    } /* while (fgets()) */

    fclose(f);
}

/*************************************************************************/
/*************************************************************************/

static const char *check_hybserv(const char *dir)
{
    static char buf[PATH_MAX+1];
    FILE *f;

    snprintf(buf, sizeof(buf), "%s/nick.db", dir);
    f = fopen(buf, "r");
    if (f) {
        char *s;
        fgets(buf, sizeof(buf), f);
        fclose(f);
        if (strncmp(buf, "; HybServ", 9) == 0) {
            s = strchr(buf+10, ' ');
            if (s)
                *s = 0;
            return buf+2;
        }
    }
    return NULL;
}

static void load_hybserv(const char *dir, int verbose, int ac, char **av)
{
    int i;

    crypted_passwords = 0;
    for (i = 1; i < ac; i++) {
        if (strcmp(av[i],"-crypt") == 0) {
            crypted_passwords = 1;
        } else {
            fprintf(stderr, "Unrecognized option %s\n", av[i]);
            usage(av[0]);
        }
    }

    if (verbose)
        fprintf(stderr, "Loading nick.db...\n");
    hyb_load_nick(dir);
    if (verbose)
        fprintf(stderr, "Loading chan.db...\n");
    hyb_load_chan(dir);
    if (verbose)
        fprintf(stderr, "Loading memo.db...\n");
    hyb_load_memo(dir);
    if (verbose)
        fprintf(stderr, "Loading stat.db...\n");
    hyb_load_stat(dir);
}

/*************************************************************************/
/*************************************************************************/

DBTypeInfo dbtype_hybserv = {
    "hybserv",
    check_hybserv,
    load_hybserv
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
