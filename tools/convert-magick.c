/* Conversion routines for Magick 1.4b2 and Wrecked 1.2 databases.
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

static void m14_load_nick(const char *dir, int32 version)
{
    dbFILE *f;
    long i, j;
    int c;
    NickInfo *ni;
    NickGroupInfo *ngi;
    struct oldni_ {
        struct oldni_ *next, *prev;
        char nick[32];
        char pass[32];
        char *email;
        char *url;
        char *usermask;
        char *realname;
        time_t reg;
        time_t seen;
        long naccess;
        char **access;
        long nignore;
        char **ignore;
        long flags;
        long resv[4];
    } oldni;

    f = open_db_ver(dir, "nick.db", version, version, NULL);
    for (i = 33; i < 256; i++) {
        while ((c = getc_db(f)) == 1) {
            SAFE(read_variable(oldni, f));
            if (version == 6) {
                time_t last_signon;
                long uin;
                SAFE(read_variable(last_signon, f));
                SAFE(read_variable(uin, f));
            }
            if (oldni.email)
                SAFE(read_string(&oldni.email, f));
            if (oldni.url)
                SAFE(read_string(&oldni.url, f));
            SAFE(read_string(&oldni.usermask, f));
            SAFE(read_string(&oldni.realname, f));
            ni = makenick(oldni.nick, &ngi);
            ni->last_usermask = oldni.usermask;
            ni->last_realname = oldni.realname;
            ni->last_quit = NULL;
            ni->time_registered = oldni.reg;
            ni->last_seen = oldni.seen;
            init_password(&ngi->pass);
            strbcpy(ngi->pass.password, oldni.pass);
            ngi->url = oldni.url;
            ngi->email = oldni.email;
            ngi->access_count = oldni.naccess;
            ngi->ignore_count = oldni.nignore;
            if (version == 5)
                oldni.flags &= 0x000000FF;
            if (oldni.flags & 0x00000001)
                ngi->flags |= NF_KILLPROTECT;
            if (oldni.flags & 0x00000002)
                ngi->flags |= NF_SECURE;
            if (oldni.flags & 0x00000004) {
                ni->status |= NS_VERBOTEN;
                ni->nickgroup = 0;
            }
            if (oldni.flags & 0x00004008)
                ni->status |= NS_NOEXPIRE;
            if (oldni.flags & 0x00000010)
                ngi->flags |= NF_PRIVATE;
            if (oldni.flags & 0x00000020) {
                strbcpy(ngi->suspend_who, "<unknown>");
                ngi->suspend_reason = (char *)(
                    version==6 ? "Unknown (imported from Wrecked IRC Services)"
                               : "Unknown (imported from Magick IRC Services)"
                );
                ngi->suspend_time = time(NULL);
                ngi->suspend_expires = 0;
                ngi->flags |= NF_SUSPENDED;
            }
            if (oldni.flags & 0x00000080) {
                ni->status |= 0x8000;  /* Flag: this is a linked nick */
                ni->nickgroup = 0;
            }
            if (oldni.flags & 0x00000100)
                ngi->memos.memomax = 0;
            if (oldni.flags & 0x00000200)
                ngi->flags |= NF_HIDE_EMAIL;
            ngi->access = smalloc(ngi->access_count * sizeof(char *));
            for (j = 0; j < ngi->access_count; j++)
                SAFE(read_string(&ngi->access[j], f));
            ngi->ignore = smalloc(ngi->ignore_count * sizeof(char *));
            for (j = 0; j < ngi->ignore_count; j++)
                SAFE(read_string(&ngi->ignore[j], f));
        } /* while more entries */
        if (c != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    } /* for 33..256 */
    close_db(f);

    /* Resolve links */
    for (ni = first_nickinfo(); ni; ni = next_nickinfo()) {
        NickInfo *ni2;
        const char *last_nick;
        if (ni->status & 0x8000) {
            ni->status &= ~0x8000;
            ni2 = ni;
            /* Find root nick (this will actually stop at the first nick
             * in the path to the root that isn't marked as linked, but
             * that's okay because such a nick will already have its
             * nickgroup ID set correctly) */
            do {
                last_nick = ni2->last_usermask;
                ni2 = get_nickinfo(last_nick);
            } while (ni2 && (ni2->status & 0x8000));
            /* Set nickgroup and last usermask field, or delete nick if an
             * error occurred */
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
                ni->last_usermask = ni2->last_usermask;
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

static int16 magick_convert_level(int16 lev) {
    if (lev >= 20)
        return lev-10;
    else if (lev >= 5)
        return lev/2;
    else if (lev >= 1)
        return (lev+1)/2;
    else
        return lev;
};

static int16 wrecked_convert_level(int16 lev) {
    if (lev >= 25)
        return lev-15;
    else if (lev >= 15)
        return (lev-5)/10;
    else if (lev >= 10)
        return 4;
    else if (lev >= 5)
        return 3;
    else if (lev >= 1)
        return (lev+1) / 2;
    else
        return lev;
};


static void m14_load_chan(const char *dir, int32 version)
{
    char *s;
    dbFILE *f;
    long i, j;
    int c;
    int16 tmp16;
    int16 (*my_convert_level)(int16);
    ChannelInfo *ci;
    NickInfo *ni;
    struct access_ {
        short level;
        short is_nick;
        char *name;
    } access;
    struct akick_ {
        short is_nick;
        short pad;
        char *name;
        char *reason;
    } akick;
    struct oldci_ {
        struct oldci_ *next, *prev;
        char name[64];
        char founder[32];
        char pass[32];
        char *desc;
        char *url;
        time_t reg;
        time_t used;
        long naccess;
        struct access_ *access;
        long nakick;
        struct akick_ *akick;
        char mlock_on[64], mlock_off[64];
        long mlock_limit;
        char *mlock_key;
        char *topic;
        char topic_setter[32];
        time_t topic_time;
        long flags;
        short *levels;
        long resv[3];
    } oldci;

    if (version == 6)
        my_convert_level = wrecked_convert_level;
    else
        my_convert_level = magick_convert_level;

    f = open_db_ver(dir, "chan.db", version, version, NULL);
    for (i = 33; i < 256; i++) {
        while ((c = getc_db(f)) == 1) {
            SAFE(read_variable(oldci, f));
            SAFE(read_string(&oldci.desc, f));
            if (oldci.url)
                SAFE(read_string(&oldci.url, f));
            if (oldci.mlock_key)
                SAFE(read_string(&oldci.mlock_key, f));
            if (oldci.topic)
                SAFE(read_string(&oldci.topic, f));
            ci = makechan(oldci.name);
            if (*oldci.founder) {
                ni = get_nickinfo(oldci.founder);
                if (!ni) {
                    fprintf(stderr,
                            "Warning: Founder %s for channel %s not found\n",
                            oldci.founder, oldci.name);
                } else if (ni->status & NS_VERBOTEN) {
                    fprintf(stderr, "Warning: Founder %s for channel %s is a"
                            " forbidden nick\n", oldci.founder, oldci.name);
                } else if (!ni->nickgroup) {
                    fprintf(stderr, "Warning: Founder %s for channel %s has"
                            " an invalid nickname record (bug?)\n",
                            oldci.founder, oldci.name);
                } else {
                    ci->founder = ni->nickgroup;
                }
            }
            init_password(&ci->founderpass);
            strbcpy(ci->founderpass.password, oldci.pass);
            ci->desc = oldci.desc;
            ci->url = oldci.url;
            ci->time_registered = oldci.reg;
            ci->last_used = oldci.used;
            ci->access_count = oldci.naccess;
            ci->akick_count = oldci.nakick;
            ci->mlock.on = scalloc(64, 1);
            strscpy(ci->mlock.on, oldci.mlock_on, 64);
            ci->mlock.off = scalloc(64, 1);
            strscpy(ci->mlock.off, oldci.mlock_off, 64);
            ci->mlock.limit = oldci.mlock_limit;
            ci->mlock.key = oldci.mlock_key;
            ci->last_topic = oldci.topic;
            strbcpy(ci->last_topic_setter, oldci.topic_setter);
            ci->last_topic_time = oldci.topic_time;
            if (version == 5)
                oldci.flags &= 0x000003FF;
            if (oldci.flags & 0x00000001)
                ci->flags |= CF_KEEPTOPIC;
            if (oldci.flags & 0x00000002)
                ci->flags |= CF_SECUREOPS;
            if (oldci.flags & 0x00000004)
                ci->flags |= CF_PRIVATE;
            if (oldci.flags & 0x00000008)
                ci->flags |= CF_TOPICLOCK;
            if (oldci.flags & 0x00000010)
                ci->flags |= CF_RESTRICTED;
            if (oldci.flags & 0x00000020)
                ci->flags |= CF_LEAVEOPS;
            if (oldci.flags & 0x00000040)
                ci->flags |= CF_SECURE;
            if (oldci.flags & 0x00000080)
                ci->flags |= CF_VERBOTEN;
            if (oldci.flags & 0x00000100) {
                ci->flags |= CF_SUSPENDED;
                strbcpy(ci->suspend_who, "<unknown>");
                ci->suspend_reason = (char *)(
                    version==6 ? "Unknown (imported from Wrecked IRC Services)"
                               : "Unknown (imported from Magick IRC Services)"
                );
                ci->suspend_time = time(NULL);
                ci->suspend_expires = 0;
            }
            if (oldci.flags & 0x00000400)
                ci->flags |= CF_NOEXPIRE;

            ci->access = scalloc(sizeof(ChanAccess), ci->access_count);
            for (j = 0; j < oldci.naccess; j++) {
                SAFE(read_variable(access, f));
                ci->access[j].nickgroup = (access.is_nick == 1);  /* in_use */
                ci->access[j].level =
                    convert_acclev(my_convert_level(access.level));
            }
            for (j = 0; j < oldci.naccess; j++) {
                SAFE(read_string(&s, f));
                if (!s) {
                    ci->access[j].nickgroup = 0;
                    continue;
                }
                if (ci->access[j].nickgroup) {
                    ni = get_nickinfo(s);
                    ci->access[j].nickgroup = ni ? ni->nickgroup : 0;
                }
            }

            ci->akick = scalloc(sizeof(AutoKick), oldci.nakick);
            for (j = 0; j < oldci.nakick; j++) {
                SAFE(read_variable(akick, f));
                /* `lastused' field temporarily used to hold `is_nick' */
                ci->akick[j].lastused = akick.is_nick;
                ci->akick[j].reason = akick.reason;
                strbcpy(ci->akick[j].who, "<unknown>");
                ci->akick[j].set = time(NULL);
            }
            for (j = 0; j < oldci.nakick; j++) {
                SAFE(read_string(&ci->akick[j].mask, f));
                if (ci->akick[j].lastused) {  /* was `is_nick' */
                    char *s = smalloc(strlen(ci->akick[j].mask)+5);
                    sprintf(s, "%s!*@*", ci->akick[j].mask);
                    free(ci->akick[j].mask);
                    ci->akick[j].mask = s;
                }
                ci->akick[j].lastused = 0;
                if (ci->akick[j].reason) {
                    SAFE(read_string(&ci->akick[j].reason, f));
                    if (!ci->akick[j].mask && ci->akick[j].reason)
                        ci->akick[j].reason = NULL;
                }
            }

            SAFE(read_int16(&tmp16, f));
            for (j = tmp16; j > 0; j--)
                SAFE(read_int16(&tmp16, f));

            /* Remove from list if founder does not exist and channel is
             * not forbidden */
            if (!(ci->flags & CF_VERBOTEN) && !ci->founder) {
                fprintf(stderr, "Ignoring channel %s (missing founder)\n",
                        ci->name);
                del_channelinfo(ci);
            }

        } /* while more entries */

        if (c != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    } /* for 33..256 */

    close_db(f);
}

/*************************************************************************/

static void m14_load_memo(const char *dir, int32 version)
{
    char *s;
    dbFILE *f;
    struct memo_ {
        char sender[32];
        long number;
        time_t time;
        char *text;
        long resv[4];
    } memo;
    struct memolist_ {
        struct memolist_ *next, *prev;
        char nick[32];
        long n_memos;
        Memo *memos;
        long resv[4];
    } memolist;
    NickGroupInfo *ngi;
    Memo *m = NULL;
    long i, j, flags = 0;
    int c;

    f = open_db_ver(dir, "memo.db", version, version, NULL);
    for (i = 33; i < 256; i++) {
        while ((c = getc_db(f)) == 1) {
            SAFE(read_variable(memolist, f));
            ngi = get_nickgroupinfo_by_nick(memolist.nick);
            if (ngi) {
                ngi->memos.memos_count = memolist.n_memos;
                m = scalloc(sizeof(*m), ngi->memos.memos_count);
                ngi->memos.memos = m;
            }
            for (j = 0; j < memolist.n_memos; j++) {
                SAFE(read_variable(memo, f));
                if (version == 6) {
                    SAFE(read_variable(flags, f));
                    flags = memo.resv[0] & 1;
                }
                if (ngi) {
                    m[j].number = memo.number;
                    if (flags & 1)
                        m[j].flags |= MF_UNREAD;
                    m[j].time = memo.time;
                    strbcpy(m[j].sender, memo.sender);
                }
            }
            for (j = 0; j < memolist.n_memos; j++) {
                SAFE(read_string(&s, f));
                if (ngi)
                    m[j].text = s;
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

static void m14_load_sop(const char *dir, int32 version)
{
    char nick[32];
    dbFILE *f;
    int16 n, i;

    f = open_db_ver(dir, "sop.db", version, version, NULL);
    SAFE(read_int16(&n, f));
    for (i = 0; i < n; i++) {
        SAFE(read_buffer(nick, f));
        set_os_priv(nick, NP_SERVADMIN);
    }
    close_db(f);
}

/*************************************************************************/

static void m14_load_akill(const char *dir, int32 version)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;
    struct akill_ {
        char *mask;
        char *reason;
        char who[32];
        time_t time;
    } akill;

    f = open_db_ver(dir, "akill.db", version, version, NULL);
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        SAFE(read_variable(akill, f));
        strbcpy(md[i].who, akill.who);
        md[i].time = akill.time;
        md[i].expires = 0;
    }
    for (i = 0; i < n; i++) {
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&md[i].reason, f));
        add_maskdata(MD_AKILL, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void m14_load_clone(const char *dir, int32 version)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;
    struct allow_ {
        char *host;
        int amount;
        char *reason;
        char who[32];
        time_t time;
    } allow;

    f = open_db_ver(dir, "clone.db", version, version, NULL);
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        SAFE(read_variable(allow, f));
        strbcpy(md[i].who, allow.who);
        md[i].limit = allow.amount;
        md[i].time = allow.time;
        md[i].expires = 0;
        md[i].num = i+1;
    }
    for (i = 0; i < n; i++) {
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&md[i].reason, f));
        add_maskdata(MD_EXCEPTION, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void m14_load_message(const char *dir, int32 version)
{
    dbFILE *f;
    int16 i, n;
    NewsItem *news;
    struct message_ {
        char *text;
        int type;
        char who[32];
        time_t time;
    } msg;

    f = open_db_ver(dir, "message.db", version, version, NULL);
    SAFE(read_int16(&n, f));
    news = scalloc(sizeof(*news), n);
    for (i = 0; i < n; i++) {
        SAFE(read_variable(msg, f));
        news[i].type = msg.type;
        strbcpy(news[i].who, msg.who);
        news[i].time = msg.time;
    }
    for (i = 0; i < n; i++) {
        SAFE(read_string(&news[i].text, f));
        add_news(&news[i]);
    }
    close_db(f);
}

/*************************************************************************/
/*************************************************************************/

static const char *check_magick_14b2(const char *dir)
{
    FILE *f;
    char buf[PATH_MAX+1];

    snprintf(buf, sizeof(buf), "%s/message.db", dir);
    f = fopen(buf, "rb");
    if (f) {
        int ver;
        ver  = fgetc(f)<<24;
        ver |= fgetc(f)<<16;
        ver |= fgetc(f)<<8;
        ver |= fgetc(f);
        fclose(f);
        if (ver == 5)
            return "Magick 1.4b2";
    }
    return NULL;
}

static void load_magick_14b2(const char *dir, int verbose, int ac,
                             char **av)
{
    if (ac > 1) {
        fprintf(stderr, "Unrecognized option %s\n", av[1]);
        usage(av[0]);
    }
    if (verbose)
        fprintf(stderr, "Loading nick.db...\n");
    m14_load_nick(dir, 5);
    if (verbose)
        fprintf(stderr, "Loading chan.db...\n");
    m14_load_chan(dir, 5);
    if (verbose)
        fprintf(stderr, "Loading memo.db...\n");
    m14_load_memo(dir, 5);
    if (verbose)
        fprintf(stderr, "Loading sop.db...\n");
    m14_load_sop(dir, 5);
    if (verbose)
        fprintf(stderr, "Loading akill.db...\n");
    m14_load_akill(dir, 5);
    if (verbose)
        fprintf(stderr, "Loading clone.db...\n");
    m14_load_clone(dir, 5);
    if (verbose)
        fprintf(stderr, "Loading message.db...\n");
    m14_load_message(dir, 5);
}

/*************************************************************************/
/*************************************************************************/

static const char *check_wrecked_1_2(const char *dir)
{
    FILE *f;
    char buf[PATH_MAX+1];

    snprintf(buf, sizeof(buf), "%s/message.db", dir);
    f = fopen(buf, "rb");
    if (f) {
        int ver;
        ver  = fgetc(f)<<24;
        ver |= fgetc(f)<<16;
        ver |= fgetc(f)<<8;
        ver |= fgetc(f);
        fclose(f);
        if (ver == 6)
            return "Wrecked 1.2.0";
    }
    return NULL;
}

static void load_wrecked_1_2(const char *dir, int verbose, int ac,
                             char **av)
{
    if (ac > 1) {
        fprintf(stderr, "Unrecognized option %s\n", av[1]);
        usage(av[0]);
    }
    if (verbose)
        fprintf(stderr, "Loading nick.db...\n");
    m14_load_nick(dir, 6);
    if (verbose)
        fprintf(stderr, "Loading chan.db...\n");
    m14_load_chan(dir, 6);
    if (verbose)
        fprintf(stderr, "Loading memo.db...\n");
    m14_load_memo(dir, 6);
    if (verbose)
        fprintf(stderr, "Loading sop.db...\n");
    m14_load_sop(dir, 6);
    if (verbose)
        fprintf(stderr, "Loading akill.db...\n");
    m14_load_akill(dir, 6);
    if (verbose)
        fprintf(stderr, "Loading clone.db...\n");
    m14_load_clone(dir, 6);
    if (verbose)
        fprintf(stderr, "Loading message.db...\n");
    m14_load_message(dir, 6);
}

/*************************************************************************/
/*************************************************************************/

DBTypeInfo dbtype_magick_14b2 = {
    "magick-1.4",
    check_magick_14b2,
    load_magick_14b2
};

DBTypeInfo dbtype_wrecked_1_2 = {
    "wrecked-1.2",
    check_wrecked_1_2,
    load_wrecked_1_2
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
