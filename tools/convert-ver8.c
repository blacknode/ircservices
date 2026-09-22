/* Conversion routines for datafile version 8 based databases (Daylight and
 * IRCS 1.2).
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "convert-db.h"

#if NICKMAX < 32
# error NICKMAX too small (must be >=32)
#elif CHANMAX < 204
# error CHANMAX too small (must be >=204)
#elif PASSMAX < 32
# error PASSMAX too small (must be >=32)
#endif

#define TYPE_DAYLIGHT   0
#define TYPE_IRCS       1

/*************************************************************************/

static void ver8_load_nick(const char *dir, int type)
{
    dbFILE *f;
    int i, j, c;
    int16 tmp16;
    int32 tmp32;
    NickInfo *ni;
    NickGroupInfo *ngi;

    f = open_db_ver(dir, "nick.db", 8, 8, NULL);
    for (i = 0; i < 256; i++) {
        while ((c = getc_db(f)) == 1) {
            char nickbuf[32];
            SAFE(read_buffer(nickbuf, f));
            ni = makenick(nickbuf, &ngi);
            init_password(&ngi->pass);
            if (type == TYPE_IRCS) {
                char passbuf[16];
                SAFE(read_buffer(passbuf, f));
                memcpy(ngi->pass.password, passbuf, sizeof(passbuf));
            } else {
                char passbuf[32];
                SAFE(read_buffer(passbuf, f));
                memcpy(ngi->pass.password, passbuf, sizeof(passbuf));
            }
            SAFE(read_string(&ngi->url, f));
            SAFE(read_string(&ngi->email, f));
            SAFE(read_string(&ni->last_usermask, f));
            if (!ni->last_usermask)
                ni->last_usermask = (char *)"@";
            SAFE(read_string(&ni->last_realname, f));
            if (!ni->last_realname)
                ni->last_realname = (char *)"";
            SAFE(read_string(&ni->last_quit, f));
            SAFE(read_int32(&tmp32, f));
            ni->time_registered = tmp32;
            SAFE(read_int32(&tmp32, f));
            ni->last_seen = tmp32;
            SAFE(read_int16(&tmp16, f));  /* status */
            if (tmp16 & 0x0001)
                ngi->pass.cipher = sstrdup("md5");
            if (tmp16 & 0x0002) {
                ni->status |= NS_VERBOTEN;
                del_nickgroupinfo(ngi);
                ni->nickgroup = 0;
            }
            if (tmp16 & 0x0004)
                ni->status |= NS_NOEXPIRE;
            SAFE(read_string(&ni->last_realmask, f));  /* link */
            SAFE(read_int16(&tmp16, f));  /* linkcount */
            if (ni->last_realmask) {
                ni->nickgroup = 0;
                SAFE(read_int16(&tmp16, f));  /* channelcount */
            } else {
                SAFE(read_int32(&tmp32, f));  /* flags */
                if (tmp32 & 0x00000001)
                    ngi->flags |= NF_KILLPROTECT;
                if (tmp32 & 0x00000002)
                    ngi->flags |= NF_SECURE;
                if (tmp32 & 0x00000008)
                    ngi->flags |= NF_MEMO_HARDMAX;
                if (tmp32 & 0x00000010)
                    ngi->flags |= NF_MEMO_SIGNON;
                if (tmp32 & 0x00000020)
                    ngi->flags |= NF_MEMO_RECEIVE;
                if (tmp32 & 0x00000040)
                    ngi->flags |= NF_PRIVATE;
                if (tmp32 & 0x00000080)
                    ngi->flags |= NF_HIDE_EMAIL;
                if (tmp32 & 0x00000100)
                    ngi->flags |= NF_HIDE_MASK;
                if (tmp32 & 0x00000200)
                    ngi->flags |= NF_HIDE_QUIT;
                if (tmp32 & 0x00000400)
                    ngi->flags |= NF_KILL_QUICK;
                if (tmp32 & 0x00000800)
                    ngi->flags |= NF_KILL_IMMED;
                SAFE(read_int16(&ngi->access_count, f));
                if (ngi->access_count) {
                    char **access;
                    access = scalloc(sizeof(char *), ngi->access_count);
                    ngi->access = access;
                    for (j = 0; j < ngi->access_count; j++, access++)
                        SAFE(read_string(access, f));
                }
                SAFE(read_int16(&ngi->memos.memos_count, f));
                SAFE(read_int16(&ngi->memos.memomax, f));
                if (ngi->memos.memos_count) {
                    Memo *memos;
                    memos = scalloc(sizeof(Memo), ngi->memos.memos_count);
                    ngi->memos.memos = memos;
                    for (j = 0; j < ngi->memos.memos_count; j++, memos++) {
                        SAFE(read_uint32(&memos->number, f));
                        SAFE(read_int16(&tmp16, f));
                        if (tmp16 & 1)
                            memos->flags |= MF_UNREAD;
                        SAFE(read_int32(&tmp32, f));
                        memos->time = tmp32;
                        SAFE(read_buffer(nickbuf, f));
                        strbcpy(memos->sender, nickbuf);
                        SAFE(read_string(&memos->text, f));
                    }
                }
                SAFE(read_int16(&tmp16, f));  /* channelcount */
                SAFE(read_int16(&tmp16, f));  /* channelmax */
                SAFE(read_int16(&tmp16, f));
                switch (tmp16) {
                    case  0: ngi->language = LANG_EN_US;   break;
                    case  2: ngi->language = LANG_JA_EUC;  break;
                    case  3: ngi->language = LANG_JA_SJIS; break;
                    case  4: ngi->language = LANG_ES;      break;
                    case  5: ngi->language = LANG_PT;      break;
                    case  6: ngi->language = LANG_FR;      break;
                    case  7: ngi->language = LANG_TR;      break;
                    case  8: ngi->language = LANG_IT;      break;
                }
            }
        } /* while (getc_db(f) == 1) */
        if (c != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    } /* for (i) */
    close_db(f);

    /* Resolve links */
    for (ni = first_nickinfo(); ni; ni = next_nickinfo()) {
        NickInfo *ni2;
        const char *last_nick;
        if (ni->last_realmask) {
            ni2 = ni;
            /* Find root nick (this will actually stop at the first nick
             * in the path to the root that isn't marked as linked, but
             * that's okay because such a nick will already have its
             * nickgroup ID set correctly) */
            do {
                last_nick = ni2->last_realmask;
                ni2 = get_nickinfo(last_nick);
            } while (ni2 && ni2 != ni && ni2->last_realmask);
            ni->last_realmask = NULL;
            /* Set nickgroup, or delete nick if an error occurred */
            if (ni2 == ni) {
                fprintf(stderr,
                        "Warning: dropping nick %s with circular link\n",
                        ni->nick);
                del_nickinfo(ni);
            } else if (!ni2) {
                fprintf(stderr, "Warning: dropping nick %s linked to"
                        " nonexistent nick %s\n", ni->nick, last_nick);
                del_nickinfo(ni);
            } else {
                ngi = get_nickgroupinfo(ni->nickgroup);
                if (ngi)
                    del_nickgroupinfo(ngi);
                ni->nickgroup = ni2->nickgroup;
                ngi = get_nickgroupinfo(ni->nickgroup);
                if (ngi) {
                    ARRAY_EXTEND(ngi->nicks);
                    strbcpy(ngi->nicks[ngi->nicks_count-1], ni->nick);
                } else if (ni->nickgroup != 0) {
                    fprintf(stderr, "Warning: Nick group %d for nick %s not"
                            " found -- program bug?  Output may be corrupt.",
                            ni->nickgroup, ni->nick);
                }
            }
        }
    }
}

/*************************************************************************/

static struct {
    int32 flag;
    char mode;
} cmodes[] = {
    { 0x00000001, 'i' },
    { 0x00000002, 'm' },
    { 0x00000004, 'n' },
    { 0x00000008, 'p' },
    { 0x00000010, 's' },
    { 0x00000020, 't' },
    { 0x00000040, 'k' },
    { 0x00000080, 'l' },
    { 0x00000100, 'R' },
    { 0x00000200, 0   }, /* 'r', never set in mlock */
    { 0x00000400, 'K' },
    { 0x00000800, 'V' },
    { 0x00001000, 'Q' },
    { 0x00002000, 'c' },
    { 0x00004000, 'O' },
    { 0x00008000, 'A' },
    { 0x00010000, 'S' },
    { 0x00020000, 'H' },
    { 0x00040000, 'C' },
    { 0x00080000, 'u' },
    { 0x00100000, 'N' },
    { 0x00200000, 'f' },
    { 0x00400000, 'z' },
    { 0, 0 }
};

static void ver8_load_chan(const char *dir, int type)
{
    dbFILE *f;
    int i, j, c;
    ChannelInfo *ci, dummy_ci;
    NickInfo *ni;
    int32 tmp32, mlock_on, mlock_off;
    MemoInfo tmpmi;

    f = open_db_ver(dir, "chan.db", 8, type==TYPE_DAYLIGHT?9:8, NULL);

    for (i = 0; i < 256; i++) {
        int16 tmp16;
        char *s, *on, *off;

        while ((c = getc_db(f)) == 1) {
            char nickbuf[32];
            if (type == TYPE_IRCS) {
                char namebuf[204];
                SAFE(read_buffer(namebuf, f));
#if CHANMAX < 204
                if (CHANMAX-1 < strlen(namebuf)) {
                    fprintf(stderr, "Warning: Channel %s name truncated\n",
                            namebuf);
                    namebuf[CHANMAX-1] = 0;
                }
#endif
                if (get_channelinfo(namebuf)) {
                    fprintf(stderr, "Channel %s conflicts with existing"
                            " channel, ignoring\n", namebuf);
                    ci = &dummy_ci;
                } else {
                    ci = makechan(namebuf);
                }
            } else {
                char namebuf[64];
                SAFE(read_buffer(namebuf, f));
                if (get_channelinfo(namebuf)) {
                    fprintf(stderr, "Channel %s conflicts with existing"
                            " channel, ignoring\n", namebuf);
                    ci = &dummy_ci;
                } else {
                    ci = makechan(namebuf);
                }
            }
            SAFE(read_string(&s, f));
            if (s) {
                ni = get_nickinfo(s);
                if (!ni) {
                    fprintf(stderr,
                            "Warning: Founder %s for channel %s not found\n",
                            s, ci->name);
                } else if (!ni->nickgroup) {
                    fprintf(stderr, "Warning: Founder %s for channel %s is a"
                            " forbidden nick\n", s, ci->name);
                } else {
                    ci->founder = ni->nickgroup;
                }
            }
            if (type != TYPE_IRCS) {
                SAFE(read_string(&s, f));
                if (s) {
                    ni = get_nickinfo(s);
                    if (!ni) {
                        fprintf(stderr, "Warning: Successor %s for channel %s"
                                " not found\n", s, ci->name);
                    } else if (!ni->nickgroup) {
                        fprintf(stderr, "Warning: Successor %s for channel %s"
                                " is a forbidden nick\n", s, ci->name);
                    } else if (ni->nickgroup == ci->founder) {
                        fprintf(stderr, "Warning: Successor %s for channel %s"
                                " is the same as the founder, clearing\n",
                                s, ci->name);
                    } else {
                        ci->successor = ni->nickgroup;
                    }
                }
            }
            init_password(&ci->founderpass);
            if (type == TYPE_IRCS) {
                char passbuf[16];
                SAFE(read_buffer(passbuf, f));
                memcpy(ci->founderpass.password, passbuf, sizeof(passbuf));
            } else {
                char passbuf[32];
                SAFE(read_buffer(passbuf, f));
                memcpy(ci->founderpass.password, passbuf, sizeof(passbuf));
            }
            SAFE(read_string(&ci->desc, f));
            if (!ci->desc)
                ci->desc = (char *)"";
            SAFE(read_string(&ci->url, f));
            SAFE(read_string(&ci->email, f));
            if (type == TYPE_IRCS)
                SAFE(read_int16(&tmp16, f));  /* autoop */
            SAFE(read_int32(&tmp32, f));
            ci->time_registered = tmp32;
            SAFE(read_int32(&tmp32, f));
            ci->last_used = tmp32;
            SAFE(read_string(&ci->last_topic, f));
            SAFE(read_buffer(nickbuf, f));
            strbcpy(ci->last_topic_setter, nickbuf);
            SAFE(read_int32(&tmp32, f));
            ci->last_topic_time = tmp32;

            SAFE(read_int32(&tmp32, f));  /* flags */
            if (tmp32 & 0x00000001)
                ci->flags |= CF_KEEPTOPIC;
            if (tmp32 & 0x00000002)
                ci->flags |= CF_SECUREOPS;
            if (tmp32 & 0x00000004)
                ci->flags |= CF_PRIVATE;
            if (tmp32 & 0x00000008)
                ci->flags |= CF_TOPICLOCK;
            if (tmp32 & 0x00000010)
                ci->flags |= CF_RESTRICTED;
            if (tmp32 & 0x00000020)
                ci->flags |= CF_LEAVEOPS;
            if (tmp32 & 0x00000040)
                ci->flags |= CF_SECURE;
            if (tmp32 & 0x00000080)
                ci->flags |= CF_VERBOTEN;
            if (tmp32 & 0x00000100)
                ci->founderpass.cipher = sstrdup("md5");
            if (tmp32 & 0x00000200)
                ci->flags |= CF_NOEXPIRE;

            /* Levels data is ignored */
            SAFE(read_int16(&tmp16, f));
            for (j = tmp16; j > 0; j--)
                SAFE(read_int16(&tmp16, f));

            SAFE(read_int16(&ci->access_count, f));
            if (ci->access_count) {
                ci->access = scalloc(ci->access_count, sizeof(ChanAccess));
                for (j = 0; j < ci->access_count; j++) {
                    SAFE(read_int16(&tmp16, f));  /* in_use */
                    if (tmp16) {
                        SAFE(read_int16(&ci->access[j].level, f));
                        if (type == TYPE_IRCS) {
                            if (ci->access[j].level >= 10)
                                ci->access[j].level = 15;
                            else if (ci->access[j].level >= 7)
                                ci->access[j].level += 3;
                            ci->access[j].level =
                                convert_acclev(ci->access[j].level);
                            SAFE(read_int16(&tmp16, f));  /* autoop */
                        }
                        SAFE(read_string(&s, f));
                        if (s) {
                            ni = get_nickinfo(s);
                            if (ni)
                                ci->access[j].nickgroup = ni->nickgroup;
                        }
                    }
                }
            }

            SAFE(read_int16(&ci->akick_count, f));
            if (ci->akick_count) {
                ci->akick = scalloc(ci->akick_count, sizeof(AutoKick));
                for (j = 0; j < ci->akick_count; j++) {
                    SAFE(read_int16(&tmp16, f));  /* in_use */
                    if (tmp16) {
                        SAFE(read_int16(&tmp16, f));  /* is_nick */
                        SAFE(read_string(&s, f));
                        if (tmp16 && s) {
                            ci->akick[j].mask = smalloc(strlen(s)+5);
                            sprintf(ci->akick[j].mask, "%s!*@*", s);
                        } else {
                            ci->akick[j].mask = s;
                        }
                        SAFE(read_string(&s, f));
                        if (ci->akick[j].mask) {
                            ci->akick[j].reason = s;
                            strbcpy(ci->akick[j].who, "<unknown>");
                            ci->akick[j].set = time(NULL);
                        }
                    }
                }
            }

            if (type == TYPE_DAYLIGHT) {
                SAFE(read_int32(&mlock_on, f));
                SAFE(read_int32(&mlock_off, f));
            } else {
                SAFE(read_int16(&tmp16, f));
                mlock_on = tmp16 & 0x00FF;
                SAFE(read_int16(&tmp16, f));
                mlock_off = tmp16 & 0x00FF;
            }
            ci->mlock.on = on = scalloc(64, 1);
            ci->mlock.off = off = scalloc(64, 1);
            for (j = 0; cmodes[j].flag != 0; j++) {
                if (mlock_on & cmodes[j].flag)
                    *on++ = cmodes[j].mode;
                if (mlock_off & cmodes[j].flag)
                    *off++ = cmodes[j].mode;
            }
            *on = 0;
            *off = 0;
            SAFE(read_int32(&ci->mlock.limit, f));
            SAFE(read_string(&ci->mlock.key, f));

            SAFE(read_int16(&tmpmi.memos_count, f));
            SAFE(read_int16(&tmp16, f));  /* memomax */
            if (tmpmi.memos_count) {
                Memo *memos;
                memos = scalloc(sizeof(Memo), tmpmi.memos_count);
                tmpmi.memos = memos;
                for (j = 0; j < tmpmi.memos_count; j++, memos++) {
                    SAFE(read_uint32(&memos->number, f));
                    SAFE(read_int16(&memos->flags, f));
                    SAFE(read_int32(&tmp32, f));
                    memos->time = tmp32;
                    SAFE(read_buffer(nickbuf, f));
                    strbcpy(memos->sender, nickbuf);
                    SAFE(read_string(&memos->text, f));
                }
            }

            SAFE(read_string(&ci->entry_message, f));

            if (!(ci->flags & CF_VERBOTEN) && !ci->founder) {
                fprintf(stderr, "Ignoring channel %s (missing founder)\n",
                        ci->name);
                del_channelinfo(ci);
            }

        } /* while (getc_db(f) == 1) */

        if (c != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    } /* for (i) */
    close_db(f);
}

/*************************************************************************/

static void ver8_load_oper(const char *dir, int type)
{
    dbFILE *f;
    int16 i, n;
    int32 tmp32;
    char *s;

    f = open_db_ver(dir, "oper.db", 8, 8, NULL);
    /* servadmin */
    SAFE(read_int16(&n, f));
    for (i = 0; i < n; i++) {
        SAFE(read_string(&s, f));
        set_os_priv(s, NP_SERVADMIN);
    }
    if (type == TYPE_IRCS) {
        /* senior */
        SAFE(read_int16(&n, f));
        for (i = 0; i < n; i++) {
            SAFE(read_string(&s, f));
            set_os_priv(s, NP_SERVADMIN);
        }
        /* junior */
        SAFE(read_int16(&n, f));
        for (i = 0; i < n; i++) {
            SAFE(read_string(&s, f));
            set_os_priv(s, NP_SERVOPER);
        }
        /* helper */
        SAFE(read_int16(&n, f));
        for (i = 0; i < n; i++) {
            SAFE(read_string(&s, f));
            set_os_priv(s, NP_SERVOPER);
        }
    }
    /* servoper */
    SAFE(read_int16(&n, f));
    for (i = 0; i < n; i++) {
        SAFE(read_string(&s, f));
        set_os_priv(s, NP_SERVOPER);
    }
    SAFE(read_int32(&maxusercnt, f));
    if (type != TYPE_IRCS) {
        SAFE(read_int32(&tmp32, f));
        maxusertime = tmp32;
    }
    close_db(f);
}

/*************************************************************************/

static void ver8_load_akill(const char *dir, int type)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;

    f = open_db_ver(dir, "akill.db", 8, 8, NULL);
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        char nick[32];
        int32 tmp32;
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_buffer(nick, f));
        strbcpy(md[i].who, nick);
        SAFE(read_int32(&tmp32, f));
        md[i].time = tmp32;
        SAFE(read_int32(&tmp32, f));
        md[i].expires = tmp32;
        if (md[i].mask)
            add_maskdata(MD_AKILL, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void ver8_load_exception(const char *dir, int type)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;

    f = open_db_ver(dir, "exception.db", 8, 8, NULL);
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        char nick[32];
        int32 tmp32;
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_int16(&md[i].limit, f));
        SAFE(read_buffer(nick, f));
        strbcpy(md[i].who, nick);
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_int32(&tmp32, f));
        md[i].time = tmp32;
        SAFE(read_int32(&tmp32, f));
        md[i].expires = tmp32;
        if (md[i].mask)
            add_maskdata(MD_AKILL, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void ver8_load_news(const char *dir, int type)
{
    dbFILE *f;
    int16 i, n;
    NewsItem *news;

    f = open_db_ver(dir, "news.db", 8, 8, NULL);
    SAFE(read_int16(&n, f));
    news = scalloc(sizeof(*news), n);
    for (i = 0; i < n; i++) {
        char nick[32];
        int32 tmp32;
        SAFE(read_int16(&news[i].type, f));
        SAFE(read_int32(&news[i].num, f));
        SAFE(read_string(&news[i].text, f));
        SAFE(read_buffer(nick, f));
        strbcpy(news[i].who, nick);
        SAFE(read_int32(&tmp32, f));
        news[i].time = tmp32;
        add_news(&news[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void ircs_load_gline(const char *dir)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;

    f = open_db_ver(dir, "gline.db", 8, 8, NULL);
    read_int16(&n, f);
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        char *user, *host;
        char nick[32];
        int32 tmp32;
        SAFE(read_string(&user, f));
        SAFE(read_string(&host, f));
        if (user && host) {
            md[i].mask = smalloc(strlen(user)+strlen(host)+2);
            sprintf(md[i].mask, "%s@%s", user, host);
        } else {
            md[i].mask = NULL;
        }
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_buffer(nick, f));
        strbcpy(md[i].who, nick);
        SAFE(read_int32(&tmp32, f));
        md[i].time = tmp32;
        SAFE(read_int32(&tmp32, f));
        md[i].expires = tmp32;
        if (md[i].mask)
            add_maskdata(MD_AKILL, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/
/*************************************************************************/

static const char *check_daylight(const char *dir)
{
    char buf[PATH_MAX+1];

    snprintf(buf, sizeof(buf), "%s/services.conn", dir);
    if (access(buf, R_OK) == 0)
        return "Daylight";
    return NULL;
}

static void load_daylight(const char *dir, int verbose, int ac,
                          char **av)
{
    if (ac > 1) {
        fprintf(stderr, "Unrecognized option %s\n", av[1]);
        usage(av[0]);
    }
    if (verbose)
        fprintf(stderr, "Loading nick.db...\n");
    ver8_load_nick(dir, TYPE_DAYLIGHT);
    if (verbose)
        fprintf(stderr, "Loading chan.db...\n");
    ver8_load_chan(dir, TYPE_DAYLIGHT);
    if (verbose)
        fprintf(stderr, "Loading oper.db...\n");
    ver8_load_oper(dir, TYPE_DAYLIGHT);
    if (verbose)
        fprintf(stderr, "Loading akill.db...\n");
    ver8_load_akill(dir, TYPE_DAYLIGHT);
    if (verbose)
        fprintf(stderr, "Loading exception.db...\n");
    ver8_load_exception(dir, TYPE_DAYLIGHT);
    if (verbose)
        fprintf(stderr, "Loading news.db...\n");
    ver8_load_news(dir, TYPE_DAYLIGHT);
}

/*************************************************************************/
/*************************************************************************/

static const char *check_ircs_1_2(const char *dir)
{
    char buf[PATH_MAX+1];

    snprintf(buf, sizeof(buf), "%s/gline.db", dir);
    if (access(buf, R_OK) == 0)
        return "IRCS 1.2";
    return NULL;
}

static void load_ircs_1_2(const char *dir, int verbose, int ac,
                          char **av)
{
    if (ac > 1) {
        fprintf(stderr, "Unrecognized option %s\n", av[1]);
        usage(av[0]);
    }
    if (verbose)
        fprintf(stderr, "Loading nick.db...\n");
    ver8_load_nick(dir, TYPE_IRCS);
    if (verbose)
        fprintf(stderr, "Loading chan.db...\n");
    ver8_load_chan(dir, TYPE_IRCS);
    if (verbose)
        fprintf(stderr, "Loading oper.db...\n");
    ver8_load_oper(dir, TYPE_IRCS);
    if (verbose)
        fprintf(stderr, "Loading gline.db...\n");
    ircs_load_gline(dir);
}

/*************************************************************************/
/*************************************************************************/

DBTypeInfo dbtype_daylight = {
    "daylight",
    check_daylight,
    load_daylight
};

DBTypeInfo dbtype_ircs_1_2 = {
    "ircs-1.2",
    check_ircs_1_2,
    load_ircs_1_2
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
