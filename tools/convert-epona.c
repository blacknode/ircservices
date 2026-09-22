/* Conversion routines for Epona/Anope databases (version 1.3.0 and later).
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

/* Server type selected with -ircd=XXX */
static enum servertype_enum {
    IRCD_UNKNOWN = 0,
    IRCD_BAHAMUT,  /* or dreamforge, solidircd */
    IRCD_INSPIRCD,
    IRCD_PLEXUS,
    IRCD_PTLINK,
    IRCD_RAGE,
    IRCD_SHADOW,
    IRCD_ULTIMATE2,
    IRCD_ULTIMATE3,
    IRCD_UNREAL,
    IRCD_VIAGRA,
} servertype = IRCD_UNKNOWN;

/* Encryption type selected with -encryption=XXXX */
static enum enctype_enum {
    ENC_UNKNOWN = 0,  /* Pre-1.7.18: use nick/channel flags to select cipher */
    ENC_NONE,
    ENC_MD5,
    ENC_SHA1,
} enctype = ENC_UNKNOWN;

/* Option names for -ircd=XXXX */
static struct {
    const char *name;
    enum servertype_enum type;
} servertype_names[] = {
    { "bahamut",    IRCD_BAHAMUT   },
    { "dreamforge", IRCD_BAHAMUT   },  /* +R/+r only */
    { "hybrid",     IRCD_UNKNOWN   },  /* no special modes */
    { "inspircd",   IRCD_INSPIRCD  },
    { "plexus",     IRCD_PLEXUS    },
    { "ptlink",     IRCD_PTLINK    },
    { "rageircd",   IRCD_RAGE      },
    { "ratbox",     IRCD_UNKNOWN   },  /* no special modes */
    { "shadowircd", IRCD_SHADOW    },
    { "solidircd",  IRCD_BAHAMUT   },  /* shared modes */
    { "ultimate2",  IRCD_ULTIMATE2 },
    { "ultimate3",  IRCD_ULTIMATE3 },
    { "unreal31",   IRCD_UNREAL    },
    { "unreal32",   IRCD_UNREAL    },
    { "viagra",     IRCD_VIAGRA    },
    { NULL }
};

/*************************************************************************/

static void epona_load_nick(const char *dir)
{
    dbFILE *f;
    int32 ver;
    int i, j, c;
    int16 tmp16;
    int32 tmp32;
    NickInfo *ni;
    NickGroupInfo *ngi;
    char *s;

    f = open_db_ver(dir, "nick.db", 13, 13, &ver);

    /* Nick cores (nick core = nickname + NickGroupInfo) */
    for (i = 0; i < 1024; i++) {
        while ((c = getc_db(f)) == 1) {
            char nickbuf[32];
            SAFE(read_string(&s, f));
            ni = makenick(s, &ngi);
            SAFE(read_string(&s, f));
            /* For some reason nick cores can get null passwords (???) */
            if (!s) {
                fprintf(stderr, "Warning: nick `%s' has null password."
                                "  Setting password to nickname.\n", ni->nick);
                s = ni->nick;
            }
            init_password(&ngi->pass);
            strbcpy(ngi->pass.password, s);
            SAFE(read_string(&ngi->email, f));
            SAFE(read_string(&s, f));  /* greet */
            SAFE(read_int32(&tmp32, f));  /* icq */
            SAFE(read_string(&ngi->url, f));
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
            if (tmp32 & 0x0000A000)             /* NI_SERVICES_{ADMIN,ROOT} */
                ngi->os_priv = NP_SERVADMIN;
            else if (tmp32 & 0x00001000)        /* NI_SERVICES_OPER */
                ngi->os_priv = NP_SERVOPER;
            if (enctype == ENC_UNKNOWN && (tmp32 & 0x00004000))
                ngi->pass.cipher = sstrdup("md5");
            else if (enctype == ENC_MD5)
                ngi->pass.cipher = sstrdup("md5");
            else if (enctype == ENC_SHA1)
                ngi->pass.cipher = sstrdup("sha1");
            else
                ngi->pass.cipher = NULL;
            if (tmp32 & 0x00080000)
                ngi->flags |= NF_NOOP;
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
                case  9: ngi->language = LANG_DE;      break;
                case 10: /* Catalan */                 break;
                case 11: /* Greek */                   break;
                case 12: ngi->language = LANG_NL;      break;
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
        } /* while (getc_db(f) == 1) */
        if (c != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    } /* for (i) */

    /* Nick aliases */
    for (i = 0; i < 1024; i++) {
        while ((c = getc_db(f)) == 1) {
            int islink = 0;
            SAFE(read_string(&s, f));  /* nick */
            ni = get_nickinfo(s);
            if (!ni) {
                islink = 1;
                ni = makenick(s, NULL);
            }
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
            if (tmp16 & 0x0002) {
                ni->status |= NS_VERBOTEN;
                ni->nickgroup = 0;
            }
            if (tmp16 & 0x0004)
                ni->status |= NS_NOEXPIRE;
            SAFE(read_string(&s, f));
            if (islink) {
                NickInfo *root = get_nickinfo(s);
                if (!root) {
                    fprintf(stderr,
                            "Warning: nick alias %s has no core, discarding\n",
                            ni->nick);
                    continue;
                }
                ni->nickgroup = root->nickgroup;
                ngi = get_nickgroupinfo(ni->nickgroup);
                if (ngi) {
                    ARRAY_EXTEND(ngi->nicks);
                    strbcpy(ngi->nicks[ngi->nicks_count-1], ni->nick);
                } else {
                    fprintf(stderr, "Warning: Nick group %d for nick %s not"
                            " found -- program bug?  Output may be corrupt.",
                            ni->nickgroup, ni->nick);
                }
            } else {
                if (stricmp(s, ni->nick) != 0) {
                    fprintf(stderr, "Warning: display %s for nick alias %s"
                            " different from nick core\n", s, ni->nick);
                }
            }
        }
        if (c != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    }

    close_db(f);
}

/*************************************************************************/

static struct {
    int32 flag;
    char mode;
} epona_cmodes_common[] = {
    { 0x00000001, 'i' },
    { 0x00000002, 'm' },
    { 0x00000004, 'n' },
    { 0x00000008, 'p' },
    { 0x00000010, 's' },
    { 0x00000020, 't' },
    { 0x00000040, 'k' },
    { 0x00000080, 'l' },
    { 0, 0 }
}, epona_cmodes_none[] = {  /* for servers with no special modes */
    { 0, 0 }
}, epona_cmodes_bahamut[] = {  /* also for dreamforge, solidircd */
    { 0x00000100, 'R' },
    { 0x00000200, 0   }, /* 'r', never set in mlock */
    { 0x00000400, 'c' },
    { 0x00000800, 'M' },
    { 0x00001000, 'j' },
    { 0x00002000, 'S' },
    { 0x00004000, 'N' },
    { 0x00008000, 'O' },
    { 0, 0 }
}, epona_cmodes_inspircd[] = {
    { 0x00000100, 'R' },
    { 0x00000200, 0   }, /* 'r', never set in mlock */
    { 0x00000400, 'c' },
    { 0x00000800, 'A' },
    { 0x00001000, 'H' },
    { 0x00002000, 'K' },
    { 0x00004000, 'L' },
    { 0x00008000, 'O' },
    { 0x00010000, 'Q' },
    { 0x00020000, 'S' },
    { 0x00040000, 'V' },
    { 0x00080000, 'f' },
    { 0x00100000, 'G' },
    { 0x00200000, 'C' },
    { 0x00400000, 'u' },
    { 0x00800000, 'z' },
    { 0x01000000, 'N' },
    { 0, 0 }
}, epona_cmodes_plexus[] = {
    { 0x00000400, 'a' },
    { 0x00000800, 'Z' },
    { 0x00001000, 'M' },
    { 0x00002000, 'c' },
    { 0x00004000, 'O' },
    { 0x00008000, 'R' },
    { 0x00010000, 'N' },
    { 0, 0 }
}, epona_cmodes_ptlink[] = {
    { 0x00000100, 'R' },
    { 0x00000200, 0   }, /* 'r', never set in mlock */
    { 0x00000400, 'A' },
    { 0x00000800, 'B' },
    { 0x00001000, 'c' },
    { 0x00002000, 'd' },
    { 0x00004000, 'f' },
    { 0x00008000, 'K' },
    { 0x00010000, 'O' },
    { 0x00020000, 'q' },
    { 0x00040000, 'S' },
    { 0x00080000, 'N' },
    { 0, 0 }
}, epona_cmodes_rage[] = {
    { 0x00000100, 'R' },
    { 0x00000200, 0   }, /* 'r', never set in mlock */
    { 0x00000400, 'c' },
    { 0x00000800, 'M' },
    { 0x00001000, 'N' },
    { 0x00002000, 'S' },
    { 0x00004000, 'C' },
    { 0x00008000, 'A' },
    { 0x00010000, 'O' },
    { 0, 0 }
}, epona_cmodes_shadow[] = {
    { 0x00000100, 'K' },
    { 0x00000200, 'A' },
    { 0x00000400, 0   }, /* 'r', never set in mlock */
    { 0x00000800, 'z' },
    { 0x00001000, 'S' },
    { 0x00002000, 'c' },
    { 0x00004000, 'E' },
    { 0x00008000, 'F' },
    { 0x00010000, 'G' },
    { 0x00020000, 'L' },
    { 0x00040000, 'N' },
    { 0x00080000, 'O' },
    { 0x00100000, 'P' },
    { 0x00200000, 'R' },
    { 0x00400000, 'T' },
    { 0x00800000, 'V' },
    { 0, 0 }
}, epona_cmodes_ultimate2[] = {
    { 0x00000100, 'R' },
    { 0x00000200, 0   }, /* 'r', never set in mlock */
    { 0x00000400, 'f' },
    { 0x00000800, 'x' },
    { 0x00001000, 'A' },
    { 0x00002000, 'I' },
    { 0x00004000, 'K' },
    { 0x00008000, 'L' },
    { 0x00010000, 'O' },
    { 0x00020000, 'S' },
    { 0, 0 }
}, epona_cmodes_ultimate3[] = {
    { 0x00000100, 'R' },
    { 0x00000200, 0   }, /* 'r', never set in mlock */
    { 0x00000400, 'c' },
    { 0x00000800, 'A' },
    { 0x00001000, 'N' },
    { 0x00002000, 'S' },
    { 0x00004000, 'K' },
    { 0x00008000, 'O' },
    { 0x00010000, 'q' },
    { 0x00020000, 'M' },
    { 0, 0 }
}, epona_cmodes_unreal[] = {
    { 0x00000100, 'R' },
    { 0x00000200, 0   }, /* 'r', never set in mlock */
    { 0x00000400, 'c' },
    { 0x00000800, 'A' },
    { 0x00001000, 'H' },
    { 0x00002000, 'K' },
    { 0x00004000, 'L' },
    { 0x00008000, 'O' },
    { 0x00010000, 'Q' },
    { 0x00020000, 'S' },
    { 0x00040000, 'V' },
    { 0x00080000, 'f' },
    { 0x00100000, 'G' },
    { 0x00200000, 'C' },
    { 0x00800000, 'z' },
    { 0x01000000, 'N' },
    { 0x02000000, 'M' },
    { 0x04000000, 'T' },
    { 0, 0 }
}, epona_cmodes_viagra[] = {
    { 0x00000100, 'R' },
    { 0x00000200, 0   }, /* 'r', never set in mlock */
    { 0x00000400, 'c' },
    { 0x00000800, 'M' },
    { 0x00001000, 'H' },
    { 0x00008000, 'O' },
    { 0x00020000, 'S' },
    { 0x01000000, 'N' },
    { 0x02000000, 'P' },
    { 0x04000000, 'x' },
    { 0, 0 }
}, *epona_cmode_index[] = {
    /* IRCD_UNKNOWN   */ epona_cmodes_none,
    /* IRCD_BAHAMUT   */ epona_cmodes_bahamut,
    /* IRCD_INSPIRCD  */ epona_cmodes_inspircd,
    /* IRCD_PLEXUS    */ epona_cmodes_plexus,
    /* IRCD_PTLINK    */ epona_cmodes_ptlink,
    /* IRCD_RAGE      */ epona_cmodes_rage,
    /* IRCD_SHADOW    */ epona_cmodes_shadow,
    /* IRCD_ULTIMATE2 */ epona_cmodes_ultimate2,
    /* IRCD_ULTIMATE3 */ epona_cmodes_ultimate3,
    /* IRCD_UNREAL    */ epona_cmodes_unreal,
    /* IRCD_VIAGRA    */ epona_cmodes_viagra,
};

/************************************/

static void epona_load_chan(const char *dir)
{
    dbFILE *f;
    int32 ver;
    int i, j, c;
    ChannelInfo *ci;
    NickInfo *ni;
    int32 tmp32, mlock_on, mlock_off;
    char *on, *off;
    MemoInfo tmpmi;

    f = open_db_ver(dir, "chan.db", 14, 17, &ver);

    for (i = 0; i < 256; i++) {
        int16 tmp16;
        char *s;

        while ((c = getc_db(f)) == 1) {
            char namebuf[64], passbuf[32], nickbuf[32];
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
            if (tmp32 & 0x00000040)
                ci->flags |= CF_SECURE;
            if (tmp32 & 0x00000080)
                ci->flags |= CF_VERBOTEN;
            if (enctype == ENC_UNKNOWN && (tmp32 & 0x00000100))
                ci->founderpass.cipher = sstrdup("md5");
            else if (enctype == ENC_MD5)
                ci->founderpass.cipher = sstrdup("md5");
            else if (enctype == ENC_SHA1)
                ci->founderpass.cipher = sstrdup("sha1");
            else
                ci->founderpass.cipher = NULL;
            if (tmp32 & 0x00000200)
                ci->flags |= CF_NOEXPIRE;
            if (tmp32 & 0x00000800)
                ci->flags |= CF_OPNOTICE;
            if (tmp32 & 0x00010000) {
                ci->flags |= CF_SUSPENDED;
                strbcpy(ci->suspend_who, "<unknown>");
                ci->suspend_reason =
                    (char *)"Unknown (imported from Anope Services)";
                ci->suspend_time = time(NULL);
                ci->suspend_expires = 0;
            }
            SAFE(read_string(&s, f));           /* forbidby */
            SAFE(read_string(&s, f));           /* forbidreason */
            SAFE(read_int16(&tmp16, f));        /* bantype */

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
                        SAFE(read_string(&s, f));
                        SAFE(read_int32(&tmp32, f));    /* last used */
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
                    int16 is_nick = 0;
                    SAFE(read_int16(&tmp16, f));  /* in_use */
                    if (ver > 14) {
                        is_nick = tmp16 & 2;
                        tmp16 &= 1;
                    }
                    if (tmp16) {
                        if (ver == 14)
                            SAFE(read_int16(&is_nick, f));
                        SAFE(read_string(&s, f));
                        if (is_nick && s) {
                            ci->akick[j].mask = smalloc(strlen(s)+5);
                            sprintf(ci->akick[j].mask, "%s!*@*", s);
                        } else {
                            ci->akick[j].mask = s;
                        }
                        SAFE(read_string(&s, f));
                        if (ci->akick[j].mask)
                            ci->akick[j].reason = s;
                        SAFE(read_string(&s, f));
                        if (ci->akick[j].mask)
                            strbcpy(ci->akick[j].who, s);
                        SAFE(read_int32(&tmp32, f));
                        if (ci->akick[j].mask)
                            ci->akick[j].set = tmp32;
                    }
                }
            }

            SAFE(read_int32(&mlock_on, f));
            SAFE(read_int32(&mlock_off, f));
            ci->mlock.on = on = scalloc(64, 1);
            ci->mlock.off = off = scalloc(64, 1);
            for (j = 0; epona_cmodes_common[j].flag != 0; j++) {
                if (mlock_on & epona_cmodes_common[j].flag)
                    *on++ = epona_cmodes_common[j].mode;
                if (mlock_off & epona_cmodes_common[j].flag)
                    *off++ = epona_cmodes_common[j].mode;
            }
            for (j = 0; epona_cmode_index[servertype][j].flag != 0; j++) {
                if (mlock_on & epona_cmode_index[servertype][j].flag)
                    *on++ = epona_cmode_index[servertype][j].mode;
                if (mlock_off & epona_cmode_index[servertype][j].flag)
                    *off++ = epona_cmode_index[servertype][j].mode;
            }
            SAFE(read_int32(&ci->mlock.limit, f));
            SAFE(read_string(&ci->mlock.key, f));
            SAFE(read_string(&ci->mlock.flood, f));
            if (ver >= 17) {
                char *s;
                SAFE(read_string(&s, f));
                if (s) {
                    unsigned long rate1, rate2;
                    if (sscanf(s, "%lu:%lu", &rate1, &rate2) == 2
                     && rate1 > 0 && rate1 < 0x7FFFFFFFUL
                     && rate2 > 0 && rate2 < 0x7FFFFFFFUL
                    ) {
                        ci->mlock.joinrate1 = rate1;
                        ci->mlock.joinrate2 = rate2;
                    } else {
                        fprintf(stderr, "MLOCK +j setting for %s is"
                                " invalid, discarding", ci->name);
                    }
                    free(s);
                }
            }
            SAFE(read_string(&ci->mlock.link, f));

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

            /* BotServ-related */
            SAFE(read_string(&s, f));
            SAFE(read_int32(&tmp32, f));
            SAFE(read_int16(&tmp16, f));
            for (j = tmp16; j > 0; j--)
                SAFE(read_int16(&tmp16, f));
            SAFE(read_int16(&tmp16, f));
            SAFE(read_int16(&tmp16, f));
            SAFE(read_int16(&tmp16, f));
            SAFE(read_int16(&tmp16, f));
            SAFE(read_int16(&tmp16, f));
            SAFE(read_int16(&tmp16, f));
            for (j = tmp16; j > 0; j--) {
                SAFE(read_int16(&tmp16, f));
                if (tmp16) {
                    SAFE(read_string(&s, f));
                    SAFE(read_int16(&tmp16, f));
                }
            }

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

static void epona_load_oper(const char *dir)
{
    dbFILE *f;
    int32 ver;
    int16 i, n, tmp16;
    int32 tmp32;
    char *s;
    MaskData *md;

    f = open_db_ver(dir, "oper.db", 11, 13, &ver);
    if (ver == 12) {
        fprintf(stderr, "Unsupported version number (%d) on oper.db.\n"
                        "Are you using a beta version of Epona?\n", ver);
        exit(1);
    }

    /* stats */
    SAFE(read_int32(&maxusercnt, f));
    SAFE(read_int32(&tmp32, f));
    maxusertime = tmp32;
    /* akills */
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        char *user, *host;
        SAFE(read_string(&user, f));
        SAFE(read_string(&host, f));
        s = smalloc(strlen(user)+strlen(host)+2);
        sprintf(s, "%s@%s", user, host);
        md[i].mask = s;
        SAFE(read_string(&s, f));
        strbcpy(md[i].who, s);
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_int32(&tmp32, f));
        md[i].time = tmp32;
        SAFE(read_int32(&tmp32, f));
        md[i].expires = tmp32;
        if (md[i].mask)
            add_maskdata(MD_AKILL, &md[i]);
    }
    /* sglines */
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&s, f));
        strbcpy(md[i].who, s);
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_int32(&tmp32, f));
        md[i].time = tmp32;
        SAFE(read_int32(&tmp32, f));
        md[i].expires = tmp32;
        if (md[i].mask)
            add_maskdata(MD_SGLINE, &md[i]);
    }
    if (ver >= 13) {
        /* sqlines */
        SAFE(read_int16(&n, f));
        md = scalloc(sizeof(*md), n);
        for (i = 0; i < n; i++) {
            SAFE(read_string(&md[i].mask, f));
            SAFE(read_string(&s, f));
            strbcpy(md[i].who, s);
            SAFE(read_string(&md[i].reason, f));
            SAFE(read_int32(&tmp32, f));
            md[i].time = tmp32;
            SAFE(read_int32(&tmp32, f));
            md[i].expires = tmp32;
            if (md[i].mask)
                add_maskdata(MD_SQLINE, &md[i]);
        }
    }
    /* szlines */
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&s, f));
        strbcpy(md[i].who, s);
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_int32(&tmp32, f));
        md[i].time = tmp32;
        SAFE(read_int32(&tmp32, f));
        md[i].expires = tmp32;
        if (md[i].mask)
            add_maskdata(MD_SZLINE, &md[i]);
    }
    if (ver >= 13) {
        /* proxy (open-relay) host cache */
        for (i = 0; i < 1024; i++) {
            int8 c;
            if (read_int8(&c, f) < 0)  /* 1.7 doesn't save this data */
                break;
            while (c) {
                SAFE(read_string(&s, f));  /* host */
                SAFE(read_int16(&tmp16, f));  /* status */
                SAFE(read_int32(&tmp32, f));  /* used */
                SAFE(read_int8(&c, f));
            }
        }
    }

    close_db(f);
}

/*************************************************************************/

static void epona_load_exception(const char *dir)
{
    dbFILE *f;
    int32 ver;
    int16 i, n;
    int32 tmp32;
    MaskData *md;

    f = open_db_ver(dir, "exception.db", 9, 9, &ver);
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        char nickbuf[32];
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_int16(&md[i].limit, f));
        SAFE(read_buffer(nickbuf, f));
        strbcpy(md[i].who, nickbuf);
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_int32(&tmp32, f));
        md[i].time = tmp32;
        SAFE(read_int32(&tmp32, f));
        md[i].expires = tmp32;
        md[i].num = i;
        if (md[i].mask)
            add_maskdata(MD_EXCEPTION, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void epona_load_news(const char *dir)
{
    dbFILE *f;
    int32 ver;
    int16 i, n;
    int32 tmp32;
    NewsItem *news;

    f = open_db_ver(dir, "news.db", 9, 9, &ver);
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

static const char *check_epona(const char *dir)
{
    char buf[PATH_MAX+1];

    snprintf(buf, sizeof(buf), "%s/bot.db", dir);
    if (access(buf, R_OK) == 0) {
        FILE *f;
        snprintf(buf, sizeof(buf), "%s/chan.db", dir);
        f = fopen(buf, "rb");
        if (f) {
            int32 ver;
            ver  = fgetc(f)<<24;
            ver |= fgetc(f)<<16;
            ver |= fgetc(f)<< 8;
            ver |= fgetc(f);
            fclose(f);
            switch (ver) {
                case 14: return "Epona 1.3.x";
                case 15: return "Epona 1.4.0";
                case 16: return "Epona 1.4.1-1.4.14 / Anope";
                case 17: return "Epona 1.4.15+";
            }
        }
    }
    return NULL;
}

static void load_epona(const char *dir, int verbose, int ac, char **av)
{
    int i;

    for (i = 1; i < ac; i++) {
        if (strncmp(av[i],"-ircd=",6) == 0) {
            const char *ircd = av[i]+6;
            int j;
            for (j = 0; servertype_names[j].name != NULL; j++) {
                if (stricmp(servertype_names[j].name, ircd) == 0)
                    break;
            }
            if (servertype_names[j].name) {
                servertype = servertype_names[j].type;
            } else {
                fprintf(stderr, "Unrecognized IRC server type %s\n", ircd);
                fprintf(stderr, "Valid IRC server types are:\n");
                for (j = 0; servertype_names[j].name != NULL; j++)
                    fprintf(stderr, "    %s\n", servertype_names[j].name);
                exit(1);
            }
        } else if (strncmp(av[i],"-encryption=",12) == 0) {
            const char *encryption = av[i]+12;
            /* Allow Anope module names as well */
            if (strnicmp(encryption, "enc_", 4) == 0) {
                encryption += 4;
            }
            if (stricmp(encryption, "none") == 0) {
                enctype = ENC_NONE;
            } else if (stricmp(encryption, "md5") == 0
                    || stricmp(encryption, "old") == 0) {
                enctype = ENC_MD5;
            } else if (stricmp(encryption, "sha1") == 0) {
                enctype = ENC_SHA1;
            } else {
                fprintf(stderr, "Unsupported encryption type %s\n",
                        encryption);
                fprintf(stderr, "Supported encryption types are:"
                        " none, md5, old, sha1\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "Unrecognized option %s\n", av[i]);
            usage(av[0]);
        }
    }

    if (verbose)
        fprintf(stderr, "Loading nick.db...\n");
    epona_load_nick(dir);
    if (verbose)
        fprintf(stderr, "Loading chan.db...\n");
    epona_load_chan(dir);
    if (verbose)
        fprintf(stderr, "Loading oper.db...\n");
    epona_load_oper(dir);
    if (verbose)
        fprintf(stderr, "Loading exception.db...\n");
    epona_load_exception(dir);
    if (verbose)
        fprintf(stderr, "Loading news.db...\n");
    epona_load_news(dir);
}

/*************************************************************************/
/*************************************************************************/

DBTypeInfo dbtype_epona = {
    "epona",
    check_epona,
    load_epona
};


DBTypeInfo dbtype_anope = {
    "anope",
    check_epona,
    load_epona
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
