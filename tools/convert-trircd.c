/* Conversion routines for trircd Services program (version 4.26).
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
#elif CHANMAX < 64
# error CHANMAX too small (must be >=64)
#elif PASSMAX < 32
# error PASSMAX too small (must be >=32)
#endif

/*************************************************************************/

static void trircd_load_nick(const char *dir)
{
    dbFILE *f;
    int i, j, c;
    int16 tmp16;
    int32 tmp32;
    NickInfo *ni;
    NickGroupInfo *ngi;

    f = open_db_ver(dir, "nick.db", 22, 25, NULL);
    for (i = 0; i < 256; i++) {
        while ((c = getc_db(f)) == 1) {
            char nickbuf[32], passbuf[32];
            SAFE(read_buffer(nickbuf, f));
            ni = makenick(nickbuf, &ngi);
            SAFE(read_buffer(passbuf, f));
            init_password(&ngi->pass);
            memcpy(ngi->pass.password, passbuf, sizeof(passbuf));
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
                void *tmpptr;
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
                if (tmp32 & 0x80000000)
                    ngi->flags |= NF_MEMO_FWD | NF_MEMO_FWDCOPY;
                SAFE(read_ptr(&tmpptr, f));
                if (tmpptr) {
                    SAFE(read_buffer(nickbuf, f));
                    strbcpy(ngi->suspend_who, nickbuf);
                    SAFE(read_string(&ngi->suspend_reason, f));
                    SAFE(read_int32(&tmp32, f));
                    ngi->suspend_time = tmp32;
                    SAFE(read_int32(&tmp32, f));
                    ngi->suspend_expires = tmp32;
                    ngi->flags |= NF_SUSPENDED;
                }
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
                SAFE(read_int16(&ngi->language, f));
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

static void trircd_load_ajoin(const char *dir)
{
    dbFILE *f;
    int c;
    int16 j, tmp16;
    int32 tmp32;
    void *tmpptr;
    char *s;
    NickGroupInfo *ngi;

    f = open_db_ver(dir, "ajoin.db", 25, 25, NULL);
    while ((c = getc_db(f)) == 1) {
        int is_root;  /* is this a root nick (should we store data)? */
        char nickbuf[32];

        SAFE(read_buffer(nickbuf, f));
        ngi = get_nickgroupinfo_by_nick(nickbuf);
        is_root = ngi && stricmp(nickbuf, ngi_mainnick(ngi)) == 0;
        SAFE(read_int16(&tmp16, f));  /* ajoincount */
        if (is_root) {
            ngi->ajoin_count = tmp16;
            ngi->ajoin = scalloc(sizeof(char *), tmp16);
            for (j = 0; j < tmp16; j++)
                SAFE(read_string(&ngi->ajoin[j], f));
        } else {
            for (j = 0; j < tmp16; j++)
                SAFE(read_string(&s, f));
        }
        SAFE(read_ptr((void *)&tmpptr, f));  /* forbidinfo */
        if (tmpptr) {
            SAFE(read_buffer(nickbuf, f));  /* forbidder */
            SAFE(read_string(&s, f));  /* forbid_message */
        }
        SAFE(read_int32(&tmp32, f));  /* authcode */
        SAFE(read_int16(&tmp16, f));  /* authmode */
        if (tmp16 & 3)  /* NH_AUTHENTIC | NH_OLDAUTH */
            tmp32 = 0;
        if (is_root) {
            ngi->authcode = tmp32;
            ngi->authset = time(NULL);
        }
        SAFE(read_int16(&tmp16, f));  /* ignorecount */
        if (is_root) {
            ngi->ignore_count = tmp16;
            ngi->ignore = scalloc(sizeof(char *), tmp16);
            for (j = 0; j < ngi->ignore_count; j++) {
                SAFE(read_int16(&tmp16, f));
                if (tmp16)
                    SAFE(read_string(&ngi->ignore[j], f));
            }
        } else {
            for (j = tmp16; j > 0; j--) {
                SAFE(read_int16(&tmp16, f));
                if (tmp16)
                    SAFE(read_string(&s, f));
            }
        }
        SAFE(read_string(&s, f));  /* infomsg */
        if (is_root)
            ngi->info = s;
        SAFE(read_int32(&tmp32, f));  /* expiredelay */
        SAFE(read_int16(&tmp16, f));  /* count for memo expire */
        for (j = 0; j < tmp16; j++)
            SAFE(read_int32(&tmp32, f));  /* memo.expiration */
    } /* while (getc_db(f) == 1) */
    if (c != -1)
        fprintf(stderr, "Warning: %s may be corrupt.\n", f->filename);
    close_db(f);
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
    { 0x00000400, 'c' },
    { 0x00000800, 'O' },
    { 0x00001000, 'A' },
    { 0x00002000, 'z' },
    { 0x00004000, 'Q' },
    { 0x00008000, 'K' },
    { 0x00010000, 'V' },
    { 0x00020000, 'H' },
    { 0x00040000, 'C' },
    { 0x00080000, 'N' },
    { 0x00100000, 'S' },
    { 0x00200000, 'G' },
    { 0x00400000, 'u' },
    { 0x00800000, 'f' },
    { 0x01000000, 'M' },
    { 0, 0 }
};

static void trircd_load_chan(const char *dir)
{
    dbFILE *f;
    int i, j, c;
    ChannelInfo *ci;
    NickInfo *ni;
    int32 tmp32, mlock_on, mlock_off;
    MemoInfo tmpmi;
    void *tmpptr;

    f = open_db_ver(dir, "chan.db", 22, 25, NULL);

    for (i = 0; i < 256; i++) {
        int16 tmp16;
        char *s, *on, *off;
        char namebuf[64], passbuf[32], nickbuf[32];

        while ((c = getc_db(f)) == 1) {
            SAFE(read_buffer(namebuf, f));
            ci = makechan(namebuf);
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
            SAFE(read_buffer(passbuf, f));
            init_password(&ci->founderpass);
            memcpy(ci->founderpass.password, passbuf, sizeof(passbuf));
            SAFE(read_string(&ci->desc, f));
            if (!ci->desc)
                ci->desc = (char *)"";
            SAFE(read_string(&ci->url, f));
            SAFE(read_string(&ci->email, f));
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
            if (tmp32 & 0x01000000)
                ci->flags |= CF_HIDE_TOPIC;
            if (tmp32 & 0x04000000)
                ci->flags |= CF_HIDE_MLOCK;
            if (tmp32 & 0x80000000)
                ci->flags |= CF_HIDE_EMAIL;

            SAFE(read_ptr((void **)&tmpptr, f));
            if (tmpptr) {
                SAFE(read_buffer(nickbuf, f));
                strbcpy(ci->suspend_who, nickbuf);
                SAFE(read_string(&ci->suspend_reason, f));
                SAFE(read_int32(&tmp32, f));
                ci->suspend_time = tmp32;
                SAFE(read_int32(&tmp32, f));
                ci->suspend_expires = tmp32;
                ci->flags |= CF_SUSPENDED;
            }

            SAFE(read_int16(&tmp16, f));  /* n_levels */
            for (j = tmp16; j > 0; j--)
                SAFE(read_int16(&tmp16, f));  /* levels */

            SAFE(read_int16(&ci->access_count, f));
            if (ci->access_count) {
                ci->access = scalloc(ci->access_count, sizeof(ChanAccess));
                for (j = 0; j < ci->access_count; j++) {
                    SAFE(read_int16(&tmp16, f));  /* in_use */
                    if (tmp16) {
                        SAFE(read_int16(&ci->access[j].level, f));
                        SAFE(read_string(&s, f));
                        ci->access[j].level =
                            convert_acclev(ci->access[j].level);
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
                        SAFE(read_buffer(nickbuf, f));
                        if (ci->akick[j].mask) {
                            ci->akick[j].reason = s;
                            strbcpy(ci->akick[j].who, nickbuf);
                            ci->akick[j].set = time(NULL);
                        }
                    }
                }
            }

            SAFE(read_int32(&mlock_on, f));
            SAFE(read_int32(&mlock_off, f));
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

static void trircd_load_cforbid(const char *dir)
{
    dbFILE *f;
    int c;

    f = open_db_ver(dir, "cforbid.db", 25, 25, NULL);
    while ((c = getc_db(f)) == 1) {
        char chanbuf[64];
        void *tmpptr;
        int8 tmp8;
        int16 i, tmp16;
        int32 tmp32;
        char *s;
        ChannelInfo *ci;

        SAFE(read_buffer(chanbuf, f));
        ci = get_channelinfo(chanbuf);
        if (!ci) {
            fprintf(stderr, "Warning: cforbid data for nonexistent channel"
                    " %s, ignoring\n", chanbuf);
        }
        SAFE(read_ptr(&tmpptr, f));  /* forbidinfo */
        if (tmpptr) {
            char nickbuf[32];
            SAFE(read_buffer(nickbuf, f));  /* forbidder */
            SAFE(read_string(&s, f));  /* forbid_message */
        }
        SAFE(read_int16(&tmp16, f));  /* akickcount */
        if (ci && tmp16 != ci->akick_count) {
            fprintf(stderr, "Warning: autokick count mismatch (chan.db: %d,"
                    " cforbid.db: %d); databases may be corrupt\n",
                    ci->akick_count, tmp16);
        }
        for (i = 0; i < tmp16; i++) {
            int16 in_use;
            SAFE(read_int16(&in_use, f));
            if (in_use) {
                if (ci && i < ci->akick_count) {
                    SAFE(read_int32(&tmp32, f));
                    ci->akick[i].set = tmp32;
                    SAFE(read_int32(&tmp32, f));
                    ci->akick[i].lastused = tmp32;
                } else {
                    SAFE(read_int32(&tmp32, f));
                    SAFE(read_int32(&tmp32, f));
                }
                SAFE(read_int32(&tmp32, f));  /* expires */
            }
        }
        SAFE(read_int16(&tmp16, f));  /* accesscount */
        for (i = tmp16; i > 0; i--) {
            SAFE(read_int16(&tmp16, f));  /* in_use */
            if (tmp16)
                SAFE(read_int32(&tmp32, f));  /* last_used */
        }
        SAFE(read_int8(&tmp8, f));
        if (tmp8 != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    } /* while (getc_db(f) == 1) */
    if (c != -1)
        fprintf(stderr, "Warning: %s may be corrupt.\n", f->filename);
    close_db(f);
}

/*************************************************************************/

static void trircd_load_oper(const char *dir)
{
    dbFILE *f;
    int16 i, n;
    int32 tmp32;
    int8 tmp8;
    char *s;

    f = open_db_ver(dir, "oper.db", 22, 25, NULL);
    /* servadmin */
    SAFE(read_int16(&n, f));
    for (i = 0; i < n; i++) {
        SAFE(read_string(&s, f));
        set_os_priv(s, NP_SERVADMIN);
    }
    /* servoper */
    SAFE(read_int16(&n, f));
    for (i = 0; i < n; i++) {
        SAFE(read_string(&s, f));
        set_os_priv(s, NP_SERVOPER);
    }
    SAFE(read_int32(&maxusercnt, f));
    SAFE(read_int32(&tmp32, f));
    maxusertime = tmp32;
    SAFE(read_int8(&tmp8, f));
    no_supass = tmp8;
    if (!no_supass) {
        char passbuf[32];
        SAFE(read_buffer(passbuf, f));
        memcpy(supass.password, passbuf, sizeof(passbuf));
    }
    close_db(f);
}

/*************************************************************************/

static void trircd_load_akill(const char *dir)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;

    f = open_db_ver(dir, "akill.db", 22, 25, NULL);
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

static void trircd_load_exclude(const char *dir)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;

    f = open_db_ver(dir, "exclude.db", 22, 25, NULL);
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
            add_maskdata(MD_EXCLUDE, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void trircd_load_exception(const char *dir)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;

    f = open_db_ver(dir, "exception.db", 22, 25, NULL);
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

static void trircd_load_news(const char *dir)
{
    dbFILE *f;
    int16 i, n;
    NewsItem *news;

    f = open_db_ver(dir, "news.db", 22, 25, NULL);
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

static void trircd_load_sline(const char *dir, const unsigned char type)
{
    char filenamebuf[16];
    dbFILE *f;
    int32 ver;
    int16 i, n;
    MaskData *md;

    snprintf(filenamebuf, sizeof(filenamebuf), "s%cline.db", type);
    f = open_db_ver(dir, filenamebuf, 22, 25, &ver);
    read_int16(&n, f);
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        char nickbuf[32];
        int32 tmp32;
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_buffer(nickbuf, f));
        strbcpy(md[i].who, nickbuf);
        if (ver >= 23) {
            SAFE(read_int32(&tmp32, f));
            md[i].time = tmp32;
            SAFE(read_int32(&tmp32, f));
            md[i].expires = tmp32;
        } else {
            md[i].time = time(NULL);
        }
        if (md[i].mask)
            add_maskdata(type, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/
/*************************************************************************/

static const char *check_trircd(const char *dir)
{
    char buf[PATH_MAX+1];
    FILE *f;

    snprintf(buf, sizeof(buf), "%s/cforbid.db", dir);
    if ((f = fopen(buf, "rb")) != NULL) {
        int32 ver;
        ver  = fgetc(f)<<24;
        ver |= fgetc(f)<<16;
        ver |= fgetc(f)<< 8;
        ver |= fgetc(f);
        fclose(f);
        if (ver == 25)
            return "trircd-4.26";
    }
    return NULL;
}

static void load_trircd(const char *dir, int verbose, int ac, char **av)
{
    if (ac > 1) {
        fprintf(stderr, "Unrecognized option %s\n", av[1]);
        usage(av[0]);
    }
    if (verbose)
        fprintf(stderr, "Loading nick.db...\n");
    trircd_load_nick(dir);
    if (verbose)
        fprintf(stderr, "Loading ajoin.db...\n");
    trircd_load_ajoin(dir);
    if (verbose)
        fprintf(stderr, "Loading chan.db...\n");
    trircd_load_chan(dir);
    if (verbose)
        fprintf(stderr, "Loading cforbid.db...\n");
    trircd_load_cforbid(dir);
    if (verbose)
        fprintf(stderr, "Loading oper.db...\n");
    trircd_load_oper(dir);
    if (verbose)
        fprintf(stderr, "Loading akill.db...\n");
    trircd_load_akill(dir);
    if (verbose)
        fprintf(stderr, "Loading exclude.db...\n");
    trircd_load_exclude(dir);
    if (verbose)
        fprintf(stderr, "Loading exception.db...\n");
    trircd_load_exception(dir);
    if (verbose)
        fprintf(stderr, "Loading news.db...\n");
    trircd_load_news(dir);
    if (verbose)
        fprintf(stderr, "Loading sgline.db...\n");
    trircd_load_sline(dir, 'g');
    if (verbose)
        fprintf(stderr, "Loading sqline.db...\n");
    trircd_load_sline(dir, 'q');
    if (verbose)
        fprintf(stderr, "Loading szline.db...\n");
    trircd_load_sline(dir, 'z');
}

/*************************************************************************/
/*************************************************************************/

DBTypeInfo dbtype_trircd_4_26 = {
    "trircd-4.26",
    check_trircd,
    load_trircd
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
