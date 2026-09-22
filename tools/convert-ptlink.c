/* Conversion routines for PTlink >= 2.13.x databases.
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

static void ptlink_load_nick(const char *dir)
{
    char filename[PATH_MAX+1];
    dbFILE *f;
    int i, j, c, delete;
    int16 tmp16, ver;
    int32 tmp32, total, count;
    NickInfo *ni;
    NickGroupInfo *ngi;
    char *s;
    signed char ch;

    snprintf(filename, sizeof(filename), "%s/nick.db", dir);
    if (!(f = open_db(filename, "r", 0)))
        return;
    SAFE(read_int8(&ch, f));
    SAFE(read_int16(&ver, f));
    if (ch != -1 || ver < 6 || ver > 11) {
        fprintf(stderr, "Wrong version number on %s\n", filename);
        exit(1);
    }

    SAFE(read_int32(&total, f));
    count = 0;
#if CLEAN_COMPILE
    c = 0;
#endif
    for (i = 0; i < 256; i++) {
        while (ver >= 10 || (c = getc_db(f)) == 1) {
            char nickbuf[32], passbuf[32], authbuf[16];
            if (ver >= 9) {
                int res = read_int32(&tmp32, f);  /* SUID (broken) / SNID */
                if (res < 0) {
                    if (ver >= 10)  /* v10: no more int8 markers */
                        break;
                    else
                        SAFE(res);
                }
            }
            SAFE(read_buffer(nickbuf, f));
            /* The source (v2.26-eol.1) claims that duplicate nicknames can
             * get stored in the database for unknown reasons, so watch for
             * and delete them as needed */
            delete = (get_nickinfo(nickbuf) != NULL);
            ni = makenick(nickbuf, &ngi);
            /* Use the SNID (file version 11) as a nickname group ID only
             * if it's nonzero and unique.  Never use the SUID (versions 9
             * and 10) as the source code claims that it's broken. */
            if (ver >= 11 && tmp32 && !get_nickgroupinfo(tmp32))
                ni->nickgroup = ngi->id = (uint32)tmp32;
            SAFE(read_buffer(passbuf, f));
            init_password(&ngi->pass);
            memcpy(ngi->pass.password, passbuf, sizeof(passbuf));
            if (ver >= 7)
                SAFE(read_buffer(authbuf, f));
            SAFE(read_string(&ngi->url, f));
            if (ver >= 7)
                SAFE(read_string(&s, f));  /* temporary, unauthed E-mail */
            else
                s = NULL;
            SAFE(read_string(&ngi->email, f));
            if (s) {
                ngi->last_email = ngi->email;
                ngi->email = s;
            }
            SAFE(read_string(&s, f));  /* icq_number */
            SAFE(read_string(&s, f));  /* location */
            SAFE(read_string(&ni->last_usermask, f));
            if (!ni->last_usermask)
                ni->last_usermask = (char *)"@";
            SAFE(read_string(&ni->last_realname, f));
            if (!ni->last_realname)
                ni->last_realname = (char *)"";
            SAFE(read_string(&ni->last_quit, f));
            SAFE(read_int32(&tmp32, f));
            ni->time_registered = tmp32;
            SAFE(read_int32(&tmp32, f));  /* last_identify */
            SAFE(read_int32(&tmp32, f));
            ni->last_seen = tmp32;
            if (ver >= 7)
                SAFE(read_int32(&tmp32, f));  /* last_email_request */
            SAFE(read_int32(&tmp32, f));  /* birth_date */
            SAFE(read_int16(&tmp16, f));  /* status */
            if (tmp16 & 0x0002) {
                ni->status |= NS_VERBOTEN;
                del_nickgroupinfo(ngi);
                ni->nickgroup = 0;
            }
            if (tmp16 & 0x0004)
                ni->status |= NS_NOEXPIRE;
            SAFE(read_int16(&tmp16, f));  /* crypt_method */
            switch (tmp16) {
                case 0:                                           break;
                case 2: ngi->pass.cipher = sstrdup("unix-crypt"); break;
                case 3: ngi->pass.cipher = sstrdup("md5");        break;
                case 1:
                    fprintf(stderr, "%s: `%s' password encrypted with"
                            " unsupported method JP2, resetting password"
                            " to nickname", filename, nickbuf);
                    /* fall through */
                default:
                    if (tmp16 != 1) {
                        fprintf(stderr, "%s: `%s' password encrypted with"
                                " unsupported method %d, resetting password"
                                " to nickname", filename, nickbuf, tmp16);
                    }
                    strbcpy(ngi->pass.password, nickbuf);
                    break;
            }
            SAFE(read_int32(&tmp32, f));  /* news_mask */
            SAFE(read_int16(&tmp16, f));  /* news_status */
            SAFE(read_string(&ni->last_realmask, f));  /* link */
            SAFE(read_int16(&tmp16, f));  /* linkcount */
            if (ni->last_realmask) {
                ni->nickgroup = 0;
                SAFE(read_int16(&tmp16, f));  /* channelcount */
            } else {
                SAFE(read_int32(&tmp32, f));  /* flags */
                if (tmp32 & 0x00000001)
                    ngi->flags |= NF_KILLPROTECT;
                if (tmp32 & 0x00004002)
                    ngi->flags |= 0x80000000;  /* suspended */
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
                SAFE(read_int32(&tmp32, f));  /* online */
                if (ngi->flags & 0x80000000) {
                    ngi->flags &= ~0x80000000;
                    SAFE(read_int32(&tmp32, f));  /* expires */
                    strbcpy(ngi->suspend_who, "<unknown>");
                    ngi->suspend_reason =
                        (char *)"Unknown (imported from PTlink IRC Services)";
                    ngi->suspend_time = time(NULL);
                    ngi->suspend_expires = tmp32;
                    ngi->flags |= NF_SUSPENDED;
                }
                SAFE(read_int16(&ngi->ajoin_count, f));
                ngi->ajoin = scalloc(sizeof(char *), ngi->ajoin_count);
                for (j = 0; j < ngi->ajoin_count; j++)
                    SAFE(read_string(&ngi->ajoin[j], f));
                SAFE(read_int16(&ngi->memos.memos_count, f));
                SAFE(read_int16(&ngi->memos.memomax, f));
                if (ngi->memos.memos_count) {
                    Memo *memos;
                    memos = scalloc(sizeof(*memos), ngi->memos.memos_count);
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
                SAFE(read_int16(&tmp16, f));  /* notes.count */
                j = tmp16;
                SAFE(read_int16(&tmp16, f));  /* notes.max */
                while (j--)  /* notes.note[] */
                    SAFE(read_string(&s, f));
                SAFE(read_int16(&tmp16, f));  /* channelcount */
                SAFE(read_int16(&tmp16, f));  /* channelmax */
                SAFE(read_int16(&tmp16, f));
                switch (tmp16) {
                    case 0:  ngi->language = LANG_EN_US;   break;
                    case 1:  ngi->language = LANG_PT;      break;
                    case 2:  ngi->language = LANG_TR;      break;
                    case 3:  ngi->language = LANG_DE;      break;
                    case 4:  ngi->language = LANG_IT;      break;
                }
            }
            if (delete) {
                del_nickgroupinfo(ngi);
                del_nickinfo(ni);
            }
            count++;
        } /* while (getc_db(f) == 1) */
        if (ver < 10 && c != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    } /* for (i) */
    close_db(f);
    if (count != total) {
        fprintf(stderr, "%s: warning: expected %d nicks, got %d\n",
                filename, total, count);
        fprintf(stderr, "    This means that your data files may be corrupt."
                "  It may also be the\n"
                "    result of a bug in some versions of PTlink Services.\n");
    }

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
} ptlink_cmodes[] = {
    { 0x00000001, 'i' },
    { 0x00000002, 'm' },
    { 0x00000004, 'n' },
    { 0x00000008, 'p' },
    { 0x00000010, 's' },
    { 0x00000020, 't' },
    { 0x00000040, 'k' },
    { 0x00000080, 'l' },
    { 0x00000200, 'R' },
    { 0x00000400, 'c' },
    { 0x00001000, 'O' },
    { 0x00002000, 'A' },
    { 0x00008000, 0   }, /* Like Unreal 'f': no dup lines within 10 secs */
    { 0, 0 }
};

static void ptlink_load_chan(const char *dir)
{
    char filename[PATH_MAX+1];
    dbFILE *f;
    int i, j, c, C_MAX;
    ChannelInfo *ci;
    NickInfo *ni;
    signed char ch;
    int16 tmp16, ver;
    int32 tmp32, total, count, mlock_on, mlock_off;
    char *on, *off;
    MemoInfo tmpmi;

    snprintf(filename, sizeof(filename), "%s/chan.db", dir);
    if (!(f = open_db(filename, "r", 0)))
        return;
    SAFE(read_int8(&ch, f));
    SAFE(read_int16(&ver, f));
    if (ch != -1 || ver < 7 || ver > 10) {
        fprintf(stderr, "Wrong version number on %s\n", filename);
        exit(1);
    }

    if (ver >= 9)
        SAFE(read_int32(&total, f));
    count = 0;

    C_MAX = (ver<9 ? 256 : 65535);
    for (i = 0; i < C_MAX; i++) {
        char *s;

        while ((c = getc_db(f)) == 1) {
            char namebuf[64], passbuf[32], nickbuf[32];
            if (ver >= 10)
                SAFE(read_int32(&tmp32, f));  /* SCID */
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
            SAFE(read_int16(&tmp16, f));  /* maxusers */
            SAFE(read_int32(&tmp32, f));  /* maxtime */
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
            if (tmp32 & 0x00000080)
                ci->flags |= CF_VERBOTEN;
            if (tmp32 & 0x00000200)
                ci->flags |= CF_NOEXPIRE;
            if (tmp32 & 0x00000800)
                ci->flags |= CF_OPNOTICE;
            SAFE(read_int16(&tmp16, f));  /* crypt_method */
            switch (tmp16) {
                case 0:                                                 break;
                case 2: ci->founderpass.cipher = sstrdup("unix-crypt"); break;
                case 3: ci->founderpass.cipher = sstrdup("md5");        break;
                case 1:
                    fprintf(stderr, "%s: `%s' password encrypted with"
                            " unsupported method JP2, resetting password"
                            " to channel name", filename, namebuf);
                    /* fall through */
                default:
                    if (tmp16 != 1) {
                        fprintf(stderr, "%s: `%s' password encrypted with"
                                " unsupported method %d, resetting password"
                                " to channel name", filename, namebuf, tmp16);
                    }
                    strbcpy(ci->founderpass.password, namebuf);
                    break;
            }
            if (tmp32 & 0x1000)
                SAFE(read_int32(&tmp32, f));  /* drop_time */

            SAFE(read_int16(&tmp16, f));  /* levels[] */
            for (j = tmp16; j > 0; j--)
                SAFE(read_int16(&tmp16, f));

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
                        SAFE(read_string(&s, f));  /* who */
                    }
                }
            }

            SAFE(read_int16(&ci->akick_count, f));
            if (ci->akick_count) {
                ci->akick = scalloc(ci->akick_count, sizeof(AutoKick));
                for (j = 0; j < ci->akick_count; j++) {
                    SAFE(read_int16(&tmp16, f));  /* in_use */
                    if (tmp16) {
                        SAFE(read_string(&ci->akick[j].mask, f));
                        SAFE(read_string(&ci->akick[j].reason, f));
                        SAFE(read_string(&s, f));       /* who */
                        strbcpy(ci->akick[j].who, s);
                        ci->akick[j].set = time(NULL);
                        SAFE(read_int32(&tmp32, f));    /* last_kick */
                        ci->akick[j].lastused = tmp32;
                    }
                }
            }

            if (ver < 8) {
                SAFE(read_int16(&tmp16, f));
                mlock_on = (int32)tmp16 & 0xFFFF;
                SAFE(read_int16(&tmp16, f));
                mlock_off = (int32)tmp16 & 0xFFFF;
            } else {
                SAFE(read_int32(&mlock_on, f));
                SAFE(read_int32(&mlock_off, f));
            }
            ci->mlock.on = on = scalloc(64, 1);
            ci->mlock.off = off = scalloc(64, 1);
            for (j = 0; ptlink_cmodes[j].flag != 0; j++) {
                if (mlock_on & ptlink_cmodes[j].flag)
                    *on++ = ptlink_cmodes[j].mode;
                if (mlock_off & ptlink_cmodes[j].flag)
                    *off++ = ptlink_cmodes[j].mode;
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

            SAFE(read_string(&ci->entry_message, f));

            if (!(ci->flags & CF_VERBOTEN) && !ci->founder) {
                fprintf(stderr, "Ignoring channel %s (missing founder)\n",
                        ci->name);
                del_channelinfo(ci);
            }

            count++;

        } /* while (getc_db(f) == 1) */

        if (c != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    } /* for (i) */
    close_db(f);

    if (ver >= 9 && count != total) {
        fprintf(stderr, "%s: warning: expected %d channels, got %d\n",
                filename, total, count);
        fprintf(stderr, "    This means that your data files may be corrupt."
                "  It may also be the\n"
                "    result of a bug in some versions of PTlink Services.\n");
    }
}

/*************************************************************************/

static void ptlink_load_oper(const char *dir)
{
    char filename[PATH_MAX+1];
    dbFILE *f;
    int16 i, n, tmp16;
    int32 tmp32;
    char *s;
    signed char ch;

    snprintf(filename, sizeof(filename), "%s/oper.db", dir);
    if (!(f = open_db(filename, "r", 0)))
        return;
    SAFE(read_int8(&ch, f));
    SAFE(read_int16(&tmp16, f));
    if (ch != -1 || tmp16 < 1 || tmp16 > 2) {
        fprintf(stderr, "Wrong version number on %s\n", filename);
        exit(1);
    }
    SAFE(read_int16(&n, f));
    for (i = 0; i < n; i++) {
        SAFE(read_string(&s, f));
        set_os_priv(s, NP_SERVADMIN);
    }
    SAFE(read_int16(&n, f));
    for (i = 0; i < n; i++) {
        SAFE(read_string(&s, f));
        set_os_priv(s, NP_SERVOPER);
    }
    SAFE(read_int32(&maxusercnt, f));
    SAFE(read_int32(&tmp32, f));
    maxusertime = tmp32;
    close_db(f);
}

/*************************************************************************/

static void ptlink_load_akill(const char *dir)
{
    char filename[PATH_MAX+1];
    dbFILE *f;
    int16 tmp16, i, n;
    int32 tmp32;
    signed char ch;
    MaskData *md;

    snprintf(filename, sizeof(filename), "%s/akill.db", dir);
    if (!(f = open_db(filename, "r", 0)))
        return;
    SAFE(read_int8(&ch, f));
    SAFE(read_int16(&tmp16, f));
    if (ch != -1 || tmp16 != 1) {
        fprintf(stderr, "Wrong version number on %s\n", filename);
        exit(1);
    }
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        char nickbuf[32];
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_buffer(nickbuf, f));
        strbcpy(md[i].who, nickbuf);
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

static void ptlink_load_sqline(const char *dir)
{
    char filename[PATH_MAX+1];
    dbFILE *f;
    int16 tmp16, i, n;
    int32 tmp32;
    signed char ch;
    MaskData *md;

    snprintf(filename, sizeof(filename), "%s/sqline.db", dir);
    if (!(f = open_db(filename, "r", 0)))
        return;
    SAFE(read_int8(&ch, f));
    SAFE(read_int16(&tmp16, f));
    if (ch != -1 || tmp16 != 1) {
        fprintf(stderr, "Wrong version number on %s\n", filename);
        exit(1);
    }
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        char nickbuf[32];
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_buffer(nickbuf, f));
        strbcpy(md[i].who, nickbuf);
        SAFE(read_int32(&tmp32, f));
        md[i].time = tmp32;
        SAFE(read_int32(&tmp32, f));
        md[i].expires = tmp32;
        if (md[i].mask)
            add_maskdata(MD_SQLINE, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void ptlink_load_sxline(const char *dir)
{
    char filename[PATH_MAX+1];
    dbFILE *f;
    int16 tmp16, i, n;
    int32 tmp32;
    signed char ch;
    MaskData *md;

    snprintf(filename, sizeof(filename), "%s/sxline.db", dir);
    if (!(f = open_db(filename, "r", 0)))
        return;
    SAFE(read_int8(&ch, f));
    SAFE(read_int16(&tmp16, f));
    if (ch != -1 || tmp16 != 1) {
        fprintf(stderr, "Wrong version number on %s\n", filename);
        exit(1);
    }
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        char nickbuf[32];
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_buffer(nickbuf, f));
        strbcpy(md[i].who, nickbuf);
        SAFE(read_int32(&tmp32, f));
        md[i].time = tmp32;
        SAFE(read_int32(&tmp32, f));
        md[i].expires = tmp32;
        if (md[i].mask)
            add_maskdata(MD_SGLINE, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void ptlink_load_news(const char *dir)
{
    char filename[PATH_MAX+1];
    dbFILE *f;
    int16 tmp16, i, n;
    int32 tmp32;
    signed char ch;
    NewsItem *news;

    snprintf(filename, sizeof(filename), "%s/news.db", dir);
    if (!(f = open_db(filename, "r", 0)))
        return;
    SAFE(read_int8(&ch, f));
    SAFE(read_int16(&tmp16, f));
    if (ch != -1 || tmp16 != 1) {
        fprintf(stderr, "Wrong version number on %s\n", filename);
        exit(1);
    }
    SAFE(read_int16(&n, f));
    news = scalloc(sizeof(*news), n);
    for (i = 0; i < n; i++) {
        char nickbuf[32];
        SAFE(read_int16(&news[i].type, f));
        SAFE(read_int32(&news[i].num, f));
        SAFE(read_string(&news[i].text, f));
        SAFE(read_buffer(nickbuf, f));
        strbcpy(news[i].who, nickbuf);
        SAFE(read_int32(&tmp32, f));
        news[i].time = tmp32;
        add_news(&news[i]);
    }
    close_db(f);
}

/*************************************************************************/
/*************************************************************************/

static const char *check_ptlink(const char *dir)
{
    char buf[PATH_MAX+1];

    snprintf(buf, sizeof(buf), "%s/vline.db", dir);
    if (access(buf, R_OK) == 0)
        return "PTlink";
    return NULL;
}

static void load_ptlink(const char *dir, int verbose, int ac, char **av)
{
    if (ac > 1) {
        fprintf(stderr, "Unrecognized option %s\n", av[1]);
        usage(av[0]);
    }
    if (verbose)
        fprintf(stderr, "Loading nick.db...\n");
    ptlink_load_nick(dir);
    if (verbose)
        fprintf(stderr, "Loading chan.db...\n");
    ptlink_load_chan(dir);
    if (verbose)
        fprintf(stderr, "Loading oper.db...\n");
    ptlink_load_oper(dir);
    if (verbose)
        fprintf(stderr, "Loading akill.db...\n");
    ptlink_load_akill(dir);
    if (verbose)
        fprintf(stderr, "Loading sqline.db...\n");
    ptlink_load_sqline(dir);
    if (verbose)
        fprintf(stderr, "Loading sxline.db...\n");
    ptlink_load_sxline(dir);
    if (verbose)
        fprintf(stderr, "Loading news.db...\n");
    ptlink_load_news(dir);
}

/*************************************************************************/
/*************************************************************************/

DBTypeInfo dbtype_ptlink = {
    "ptlink",
    check_ptlink,
    load_ptlink
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
