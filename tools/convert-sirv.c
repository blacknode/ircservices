/* Conversion routines for SirvNET databases (version 2.9.0 and earlier),
 * Auspice databases (versions 2.5 and later), and Bolivia databases
 * (version 1.2.0).
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "convert-db.h"

#define TYPE_SIRV       0
#define TYPE_AUSPICE    1
#define TYPE_BOLIVIA    2

#if NICKMAX < 32
# error NICKMAX too small (must be >=32)
#elif CHANMAX < 64
# error CHANMAX too small (must be >=64)
#elif PASSMAX < 32
# error PASSMAX too small (must be >=32)
#endif

/*************************************************************************/

static void sirv_load_nick(const char *dir, int type)
{
    char *s;
    dbFILE *f;
    long i, j;
    int c;
    NickInfo *ni;
    NickGroupInfo *ngi;
    struct oldni_ {
        struct oldni_ *next, *prev;
        char nick[32];
        char pass[32];
        char *usermask;
        char *realname;
        time_t reg;
        time_t seen;
        long naccess;
        char **access;
        long flags;
        time_t idstamp;
        unsigned short memomax;
        unsigned short channelcount;
        char *url;
        char *email;
        char *forward;
        char *hold;
        char *mark;
        char *forbid;   /* Bolivia calls it "banish" instead */
    } oldni;
    struct {
        int news;
        char *regemail;
        long icq;
        long auth;      /* 2.9 only */
        long resv[2];
    } oldni_sirv;
    struct {
        int news;
        char *uin;
        char *age;
        char *info;
        char *sex;
        char *mlock;
        char *last_quit;
        long eflags;
        long ajoincount;
        char **autojoin;
        long comline;
        char **comment;
        long noteline;
        char **note;
    } oldni_auspice;

    if (type == TYPE_SIRV) {
        f = open_db_ver(dir, "nick.db", 5, 8, NULL);
    } else if (type == TYPE_AUSPICE) {
        f = open_db_ver(dir, "nick.db", 6, 6, NULL);
    } else if (type == TYPE_BOLIVIA) {
        f = open_db_ver(dir, "nick.db", 5, 5, NULL);
    } else {
        fprintf(stderr, "BUG: invalid type %d to sirv_load_nick()\n", type);
        exit(1);
    }
    for (i = 33; i < 256; i++) {
        while ((c = getc_db(f)) == 1) {
            SAFE(read_variable(oldni, f));
            if (type == TYPE_AUSPICE) {
                SAFE(read_variable(oldni_auspice, f));
            } else if (type == TYPE_BOLIVIA) {
                /* Bolivia should have a long resv[6]; at the end, but a
                 * missing comment terminator in services.h cuts it out */
                /*SAFE(read_variable(oldni_bolivia, f));*/
            } else {
                SAFE(read_variable(oldni_sirv, f));
            }
            if (oldni.url)
                SAFE(read_string(&oldni.url, f));
            if (oldni.email)
                SAFE(read_string(&oldni.email, f));
            if (type == TYPE_AUSPICE) {
                if (oldni_auspice.uin)
                    SAFE(read_string(&oldni_auspice.uin, f));
                if (oldni_auspice.age)
                    SAFE(read_string(&oldni_auspice.age, f));
            }
            if (oldni.forward)
                SAFE(read_string(&oldni.forward, f));
            if (oldni.hold)
                SAFE(read_string(&oldni.hold, f));
            if (oldni.mark)
                SAFE(read_string(&oldni.mark, f));
            if (oldni.forbid)
                SAFE(read_string(&oldni.forbid, f));
            if (type == TYPE_AUSPICE) {
                if (oldni_auspice.info)
                    SAFE(read_string(&oldni_auspice.info, f));
                if (oldni_auspice.sex)
                    SAFE(read_string(&oldni_auspice.sex, f));
                if (oldni_auspice.mlock)
                    SAFE(read_string(&oldni_auspice.mlock, f));
                if (oldni_auspice.last_quit)
                    SAFE(read_string(&oldni_auspice.last_quit, f));
            } else if (type == TYPE_SIRV) {
                if (oldni_sirv.regemail)
                    SAFE(read_string(&oldni_sirv.regemail, f));
            }
            if (type != TYPE_AUSPICE || oldni.usermask)
                SAFE(read_string(&oldni.usermask, f));
            if (type != TYPE_AUSPICE || oldni.realname)
                SAFE(read_string(&oldni.realname, f));
            ni = makenick(oldni.nick, &ngi);
            ni->last_usermask = oldni.usermask;
            ni->last_realname = oldni.realname;
            if (type == TYPE_AUSPICE)
                ni->last_quit = oldni_auspice.last_quit;
            ni->time_registered = oldni.reg;
            ni->last_seen = oldni.seen;
            init_password(&ngi->pass);
            memcpy(ngi->pass.password, oldni.pass, sizeof(oldni.pass));
            ngi->url = oldni.url;
            ngi->email = oldni.email;
            ngi->access_count = oldni.naccess;
            ngi->memos.memomax = oldni.memomax;
            ngi->memos.memos = NULL;
            if (oldni.flags & 0x00000001)
                ngi->flags |= NF_KILLPROTECT;
            if (oldni.flags & 0x00000002)
                ngi->flags |= NF_SECURE;
            if (oldni.flags & 0x00000004) {
                ni->status |= NS_VERBOTEN;
                del_nickgroupinfo(ngi);
                ni->nickgroup = 0;
                /* Just in case--to differentiate from linked nicks */
                ni->last_usermask = NULL;
            }
            if (oldni.flags & 0x00000008)
                ngi->pass.cipher = sstrdup("md5");
            if (oldni.flags & 0x00000010)
                ngi->flags |= NF_MEMO_SIGNON;
            if (oldni.flags & 0x00000020)
                ngi->flags |= NF_MEMO_RECEIVE;
            if (oldni.flags & 0x00000040)
                ngi->flags |= NF_PRIVATE;
            if (oldni.flags & 0x00000080)
                ngi->flags |= NF_HIDE_EMAIL;
            if (oldni.flags & 0x00000200)
                ni->status |= NS_NOEXPIRE;
            if (oldni.flags & 0x00000800)
                ngi->flags |= NF_NOOP;
            if (oldni.flags & 0x00001000)
                ngi->memos.memomax = 0;
            if (oldni.flags & 0x00002000)
                ngi->flags |= NF_HIDE_MASK;
            if (type == TYPE_BOLIVIA && (oldni.flags & 0x00020000)) {
                strbcpy(ngi->suspend_who, "<unknown>");
                ngi->suspend_reason =
                    (char *)"Unknown (imported from Bolivia IRC Services)";
                ngi->suspend_time = time(NULL);
                ngi->suspend_expires = 0;
                ngi->flags |= NF_SUSPENDED;
            }
            if (type == TYPE_AUSPICE && (oldni.flags & 0x00800000)) {
                /* Linked ("slave") nick; parent is in last_usermask */
                ni->nickgroup = 0;
            }
            ngi->access = scalloc(ngi->access_count, sizeof(char *));
            for (j = 0; j < ngi->access_count; j++)
                SAFE(read_string(&ngi->access[j], f));
            if (type == TYPE_AUSPICE) {
                if (oldni_auspice.ajoincount) {
                    ngi->ajoin_count = oldni_auspice.ajoincount;
                    ngi->ajoin =
                        smalloc(sizeof(char *) * oldni_auspice.ajoincount);
                    for (j = 0; j < oldni_auspice.ajoincount; j++)
                        SAFE(read_string(&ngi->ajoin[j], f));
                }
                for (j = 0; j < oldni_auspice.comline; j++)
                    SAFE(read_string(&s, f));
                if (oldni_auspice.noteline) {
                    ngi->memos.memos_count = oldni_auspice.noteline;
                    ngi->memos.memos =
                        scalloc(sizeof(Memo), oldni_auspice.noteline);
                    for (j = 0; j < oldni_auspice.noteline; j++) {
                        ngi->memos.memos[j].number = j+1;
                        ngi->memos.memos[j].flags = 0;
                        ngi->memos.memos[j].time = time(NULL);
                        strbcpy(ngi->memos.memos[j].sender, oldni.nick);
                        SAFE(read_string(&ngi->memos.memos[j].text, f));
                    }
                }
            }
        } /* while ((c = getc_db(f)) == 1) */
        if (c != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    } /* for (i = 33..256) */
    close_db(f);

    /* Resolve links */
    for (ni = first_nickinfo(); ni; ni = next_nickinfo()) {
        if (!ni->nickgroup && ni->last_usermask) {
            NickInfo *ni2;
            /* Find parent nick */
            ni2 = get_nickinfo(ni->last_usermask);
            /* Set nickgroup, or delete nick if an error occurred */
            if (ni2 == ni) {
                fprintf(stderr,
                        "Warning: dropping nick %s with circular link\n",
                        ni->nick);
                del_nickinfo(ni);
            } else if (!ni2) {
                fprintf(stderr, "Warning: dropping nick %s linked to"
                        " nonexistent nick %s\n", ni->nick, ni->last_usermask);
                del_nickinfo(ni);
            } else if (ni2->status & NS_VERBOTEN) {
                /* Auspice allows links to forbidden nicks; we don't
                 * (because forbidden nicks don't have nickgroups).
                 * Just make the nick forbidden. */
                ni->status = NS_VERBOTEN;
                ni->last_usermask = NULL;
            } else if (ni2->nickgroup == 0) {
                fprintf(stderr, "BUG: link target %s of nick %s has no"
                        " no nickgroup, dropping both nicks\n",
                        ni2->nick, ni->nick);
                del_nickinfo(ni);
                del_nickinfo(ni2);
            } else {
                ni->last_usermask = ni2->last_usermask;
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

struct flagmode {
    short flag;
    char mode;
};
static struct flagmode sirv_cmodes[] = {
    { 0x0001, 'i' },
    { 0x0002, 'm' },
    { 0x0004, 'n' },
    { 0x0008, 'p' },
    { 0x0010, 's' },
    { 0x0020, 't' },
    { 0x0040, 'k' },
    { 0x0080, 'l' },
    { 0x0100, 0   }, /* CMODE_r, never set in mlock */
    { 0x0200, 'J' },
    { 0x0400, 'R' },
    { 0x0800, 'c' },
    { 0x1000, 'O' },
    { 0, 0 }
};
static struct flagmode bolivia_cmodes[] = {
    { 0x0001, 'i' },
    { 0x0002, 'm' },
    { 0x0004, 'n' },
    { 0x0008, 'p' },
    { 0x0010, 's' },
    { 0x0020, 't' },
    { 0x0040, 'k' },
    { 0x0080, 'l' },
    { 0x0100, 0   }, /* CMODE_r, never set in mlock */
    { 0x0200, 'R' },
    { 0x0400, 'c' },
    { 0x0800, 'M' },
    { 0, 0 }
};

static void sirv_load_chan(const char *dir, int type)
{
    char *s;
    dbFILE *f;
    int32 ver;
    long i, j;
    int c;
    int16 tmp16;
    int tmpint;
    ChannelInfo *ci;
    NickInfo *ni;
    char **adders;  /* for remembering which access entries had an adder */
    int has_adders; /* is this a database with access entry adder info? */
    struct access_ {
        int16 level;
        short in_use;
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
        time_t reg;
        time_t used;
        long naccess;
        struct access_ *access;
        long nakick;
        struct akick_ *akick;
    } oldci;
    /* Sirv/Bolivia use shorts for mlock_{on,off}; Auspice uses char[64] */
    short mlock_on_sirv, mlock_off_sirv;
    char mlock_on_auspice[64], mlock_off_auspice[64];
    struct {
        long mlock_limit;
        char *mlock_key;
        char *topic;
        char topic_setter[32];
        time_t topic_time;
        long flags;
        short *levels;
        char *url;
        char *email;
        char *welcome;
        char *hold;
        char *mark;
        char *freeze;
        char *forbid;
        int topic_allow;
    } oldci2;
    struct {
        long auth;      /* 2.9 only */
        long resv[4];
    } oldci_sirv;
    struct {
        char *successor;
        char *mlock_link;
        char *mlock_flood;
        char *bot;
        long botflag;
        long newsline;
        char **news;
        long badwline;
        char **badwords;
        long resv[3];
    } oldci_auspice;
    struct {
        char *successor;
        char *mlock_link;
        char *mlock_flood;
        long newsline;
        char **news;
        long badwline;
        char **badwords;
        char *markreason;
        char *freezereason;
        char *holdreason;
        char *lastgetpass;
        char *bot;
        long botflag;
        int ttb;
        int capsmin, capspercent;
        int floodlines, floodsecs;
        int repeattimes;
    } oldci_auspice8;
    struct {
        long resv[5];
    } oldci_bolivia;

#if CLEAN_COMPILE
    /* To avoid "possibly uninitialized" warnings: */
    adders = NULL;
#endif

    if (type == TYPE_SIRV) {
        f = open_db_ver(dir, "chan.db", 5, 8, &ver);
    } else if (type == TYPE_AUSPICE) {
        f = open_db_ver(dir, "chan.db", 7, 8, &ver);
    } else if (type == TYPE_BOLIVIA) {
        f = open_db_ver(dir, "chan.db", 5, 5, &ver);
    } else {
        fprintf(stderr, "BUG: invalid type %d to sirv_load_chan()\n", type);
        exit(1);
    }
    has_adders = (type == TYPE_AUSPICE || ver >= 8);
    for (i = 33; i < 256; i++) {
        while ((c = getc_db(f)) == 1) {
            SAFE(read_variable(oldci, f));
            if (type == TYPE_AUSPICE) {
                SAFE(read_buffer(mlock_on_auspice, f));
                SAFE(read_buffer(mlock_off_auspice, f));
                /* An apparent bug in (at least) Auspice 2.7 causes CRs to
                 * sometimes appear in mode locks. */
                s = mlock_on_auspice;
                while (*s) {
                    if (*s == 13)
                        memmove(s, s+1, strlen(s+1)+1);
                    else
                        s++;
                }
                s = mlock_off_auspice;
                while (*s) {
                    if (*s == 13)
                        memmove(s, s+1, strlen(s+1)+1);
                    else
                        s++;
                }
            } else {
                SAFE(read_variable(mlock_on_sirv, f));
                SAFE(read_variable(mlock_off_sirv, f));
            }
            SAFE(read_variable(oldci2, f));
            if (type == TYPE_AUSPICE) {
                if (ver == 8) {
                    SAFE(read_variable(oldci_auspice8, f));
                    oldci_auspice.successor   = oldci_auspice8.successor;
                    oldci_auspice.mlock_link  = oldci_auspice8.mlock_link;
                    oldci_auspice.mlock_flood = oldci_auspice8.mlock_flood;
                    oldci_auspice.bot         = oldci_auspice8.bot;
                    oldci_auspice.botflag     = oldci_auspice8.botflag;
                    oldci_auspice.newsline    = oldci_auspice8.newsline;
                    oldci_auspice.news        = oldci_auspice8.news;
                    oldci_auspice.badwline    = oldci_auspice8.badwline;
                    oldci_auspice.badwords    = oldci_auspice8.badwords;
                } else {
                    SAFE(read_variable(oldci_auspice, f));
                }
            } else if (type == TYPE_BOLIVIA) {
                SAFE(read_variable(oldci_bolivia, f));
            } else {
                SAFE(read_variable(oldci_sirv, f));
            }
            SAFE(read_string(&oldci.desc, f));
            if (oldci2.url)
                SAFE(read_string(&oldci2.url, f));
            if (oldci2.email)
                SAFE(read_string(&oldci2.email, f));
            if (oldci2.mlock_key)
                SAFE(read_string(&oldci2.mlock_key, f));
            if (oldci2.topic)
                SAFE(read_string(&oldci2.topic, f));
            if (oldci2.welcome)
                SAFE(read_string(&oldci2.welcome, f));
            if (oldci2.hold)
                SAFE(read_string(&oldci2.hold, f));
            if (oldci2.mark)
                SAFE(read_string(&oldci2.mark, f));
            if (oldci2.freeze)
                SAFE(read_string(&oldci2.freeze, f));
            if (oldci2.forbid)
                SAFE(read_string(&oldci2.forbid, f));
            if (type == TYPE_AUSPICE) {
                if (oldci_auspice.successor)
                    SAFE(read_string(&oldci_auspice.successor, f));
                if (oldci_auspice.mlock_link)
                    SAFE(read_string(&oldci_auspice.mlock_link, f));
                if (oldci_auspice.mlock_flood)
                    SAFE(read_string(&oldci_auspice.mlock_flood, f));
                if (oldci_auspice.bot)
                    SAFE(read_string(&oldci_auspice.bot, f));
                if (ver >= 8 && oldci_auspice8.markreason)
                    SAFE(read_string(&oldci_auspice8.markreason, f));
                if (ver >= 8 && oldci_auspice8.freezereason)
                    SAFE(read_string(&oldci_auspice8.freezereason, f));
                if (ver >= 8 && oldci_auspice8.holdreason)
                    SAFE(read_string(&oldci_auspice8.holdreason, f));
                if (ver >= 8 && oldci_auspice8.lastgetpass)
                    SAFE(read_string(&oldci_auspice8.lastgetpass, f));
            }
            ci = makechan(oldci.name);
            if (*oldci.founder) {
                ni = get_nickinfo(oldci.founder);
                if (!ni) {
                    fprintf(stderr,
                            "Warning: Founder %s for channel %s not found\n",
                            oldci.founder, oldci.name);
                } else if (!ni->nickgroup) {
                    fprintf(stderr, "Warning: Founder %s for channel %s is a"
                            " forbidden nick\n", oldci.founder, oldci.name);
                } else {
                    ci->founder = ni->nickgroup;
                }
            }
            if (type == TYPE_AUSPICE && oldci_auspice.successor) {
                ni = get_nickinfo(oldci_auspice.successor);
                if (!ni) {
                    fprintf(stderr,
                            "Warning: Successor %s for channel %s not found,"
                            " clearing\n", oldci_auspice.successor,
                            oldci.name);
                } else if (!ni->nickgroup) {
                    fprintf(stderr, "Warning: Successor %s for channel %s is a"
                            " forbidden nick, clearing\n",
                            oldci_auspice.successor, oldci.name);
                } else if (ni->nickgroup == ci->founder) {
                    fprintf(stderr, "Warning: Successor %s for channel %s is"
                            " the same as the founder, clearing\n",
                            oldci_auspice.successor, oldci.name);
                } else {
                    ci->successor = ni->nickgroup;
                }
            }
            init_password(&ci->founderpass);
            strbcpy(ci->founderpass.password, oldci.pass);
            ci->desc = oldci.desc;
            ci->url = oldci2.url;
            ci->email = oldci2.email;
            ci->time_registered = oldci.reg;
            ci->last_used = oldci.used;
            ci->access_count = oldci.naccess;
            ci->akick_count = oldci.nakick;
            ci->mlock.limit = oldci2.mlock_limit;
            ci->mlock.key = oldci2.mlock_key;
            if (type == TYPE_AUSPICE) {
                ci->mlock.link = oldci_auspice.mlock_link;
                ci->mlock.flood = oldci_auspice.mlock_flood;
            }
            ci->last_topic = oldci2.topic;
            strbcpy(ci->last_topic_setter, oldci2.topic_setter);
            ci->last_topic_time = oldci2.topic_time;
            if (oldci2.flags & 0x00000001)
                ci->flags |= CF_KEEPTOPIC;
            if (oldci2.flags & 0x00000002)
                ci->flags |= CF_SECUREOPS;
            if (oldci2.flags & 0x00000004) {
                if (type == TYPE_BOLIVIA)
                    ci->flags |= CF_OPNOTICE;
                else
                    ci->flags |= CF_PRIVATE;
            }
            if (oldci2.flags & 0x00000008)
                ci->flags |= CF_TOPICLOCK;
            if (oldci2.flags & 0x00000010)
                ci->flags |= CF_RESTRICTED;
            if (oldci2.flags & 0x00000020)
                ci->flags |= CF_LEAVEOPS;
            if (oldci2.flags & 0x00000040)
                ci->flags |= CF_SECURE;
            if (oldci2.flags & 0x00000080)
                ci->flags |= CF_VERBOTEN;
            if (oldci2.flags & 0x00000100)
                ci->founderpass.cipher = sstrdup("md5");
            if (oldci2.flags & 0x00000200)
                ci->flags |= CF_NOEXPIRE;
            ci->flags |= CF_MEMO_RESTRICTED;
            if (oldci2.flags & 0x00040000) {  /* CI_MEMO_AV */
                ci->levels[CA_MEMO] = ACCLEV_VOP;
            } else if (oldci2.flags & 0x00080000) {  /* CI_MEMO_AOP */
                ci->levels[CA_MEMO] = ACCLEV_AOP;
            } else if (oldci2.flags & 0x00100000) {  /* CI_MEMO_SOP */
                ci->levels[CA_MEMO] = ACCLEV_SOP;
            } else if (oldci2.flags & 0x00200000) {  /* CI_MEMO_CF */
                ci->levels[CA_MEMO] = ACCLEV_FOUNDER;
            } else if (oldci2.flags & 0x00400000) {  /* CI_MEMO_FR */
                ci->levels[CA_MEMO] = ACCLEV_FOUNDER;
            } else if (oldci2.flags & 0x00800000) {  /* CI_MEMO_NONE */
                ci->levels[CA_MEMO] = ACCLEV_INVALID;
            } else {
                ci->levels[CA_MEMO] = ACCLEV_VOP;
            }
            if (!(oldci2.flags & 0x01000000))
                ci->flags |= CF_SECURE;
            if (type == TYPE_SIRV && (oldci2.flags & 0x04000000))
                ci->flags |= CF_ENFORCE;
            if (type != TYPE_BOLIVIA && (oldci2.flags & 0x08000000)) {
                ci->flags |= CF_SUSPENDED;
                strbcpy(ci->suspend_who, "<unknown>");
                if (type == TYPE_AUSPICE) {
                    ci->suspend_reason =
                        (char *)"Unknown (imported from Auspice Services)";
                } else {
                    ci->suspend_reason =
                        (char *)"Unknown (imported from SirvNET IRC Services)";
                }
                ci->suspend_time = time(NULL);
                ci->suspend_expires = 0;
            }

            ci->mlock.on = scalloc(64, 1);
            ci->mlock.off = scalloc(64, 1);
            if (type == TYPE_AUSPICE) {
                strscpy(ci->mlock.on, mlock_on_auspice, 64);
                strscpy(ci->mlock.off, mlock_off_auspice, 64);
            } else {
                char *on = ci->mlock.on;
                char *off = ci->mlock.off;
                struct flagmode *cmodes;
                if (type == TYPE_BOLIVIA)
                    cmodes = bolivia_cmodes;
                else
                    cmodes = sirv_cmodes;
                for (j = 0; cmodes[j].flag != 0; j++) {
                    if (mlock_on_sirv & cmodes[j].flag)
                        *on++ = cmodes[j].mode;
                    if (mlock_off_sirv & cmodes[j].flag)
                        *off++ = cmodes[j].mode;
                }
                *on = 0;
                *off = 0;
            }

            ci->access = scalloc(sizeof(ChanAccess), oldci.naccess);
            if (has_adders)
                adders = scalloc(sizeof(char *), oldci.naccess);
            for (j = 0; j < oldci.naccess; j++) {
                SAFE(read_variable(access, f));
                if (has_adders)
                    SAFE(read_variable(adders[j], f));
                ci->access[j].nickgroup = access.in_use;
                ci->access[j].level = access.level;
                if (type == TYPE_AUSPICE && ver >= 8) {
                    SAFE(read_variable(tmpint, f));  /* accflag */
                    if (tmpint)  /* discard TIMEOP entries */
                        ci->access[j].nickgroup = 0;
                }
                ci->access[j].level = convert_acclev(ci->access[j].level);
            }
            for (j = 0; j < oldci.naccess; j++) {
                SAFE(read_string(&s, f));
                if (has_adders && adders[j])
                    SAFE(read_string(&adders[j], f));
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

            SAFE(read_int16(&tmp16, f));  /* levels */
            for (j = tmp16; j > 0; j--)
                SAFE(read_int16(&tmp16, f));

            if (type == TYPE_AUSPICE) {
                for (j = 0; j < oldci_auspice.newsline; j++)
                    SAFE(read_string(&s, f));
                for (j = 0; j < oldci_auspice.badwline; j++)
                    SAFE(read_string(&s, f));
            }

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

static void sirv_load_memo(const char *dir, int type)
{
    char *s;
    dbFILE *f;
    struct memo_ {
        char sender[32];
        long number;
        time_t time;
        char *text;
        char *chan;
        short flags;
        short pad;
        long resv[3];
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
    long i, j, chancount;
    int c;

    if (type == TYPE_SIRV) {
        f = open_db_ver(dir, "memo.db", 5, 8, NULL);
    } else if (type == TYPE_AUSPICE) {
        f = open_db_ver(dir, "memo.db", 6, 6, NULL);
    } else if (type == TYPE_BOLIVIA) {
        f = open_db_ver(dir, "memo.db", 5, 5, NULL);
    } else {
        fprintf(stderr, "BUG: invalid type %d to sirv_load_memo()\n", type);
        exit(1);
    }
    for (i = 33; i < 256; i++) {
        while ((c = getc_db(f)) == 1) {
            SAFE(read_variable(memolist, f));
            ngi = get_nickgroupinfo_by_nick(memolist.nick);
            if (ngi) {
                ngi->memos.memos_count = memolist.n_memos;
                m = scalloc(sizeof(*m), ngi->memos.memos_count);
                ngi->memos.memos = m;
            }
            chancount = 0;
            for (j = 0; j < memolist.n_memos; j++) {
                SAFE(read_variable(memo, f));
                if (ngi) {
                    m[j].number = memo.number;
                    if (memo.flags & 0x0001)
                        m[j].flags |= MF_UNREAD;
                    if (type == TYPE_AUSPICE && !(memo.flags & 0x0008))
                        m[j].flags |= MF_EXPIREOK;
                    m[j].time = memo.time;
                    strbcpy(m[j].sender, memo.sender);
                    if (memo.chan)
                        m[j].flags |= 0x8000;
                } else if (memo.chan) {
                    chancount++;
                }
            }
            for (j = 0; j < memolist.n_memos; j++) {
                SAFE(read_string(&s, f));
                if (ngi) {
                    m[j].text = s;
                    if (m[j].flags & 0x8000) {
                        m[j].flags &= ~0x8000;
                        SAFE(read_string(&s, f));
                    }
                }
            }
            for (j = 0; j < chancount; j++)
                SAFE(read_string(&s, f));
        }
        if (c != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    }
    close_db(f);
}

/*************************************************************************/

static void sirv_load_os_sop(const char *dir)
{
    char *s;
    dbFILE *f;
    int16 n, i;

    f = open_db_ver(dir, "os_sop.db", 5, 8, NULL);
    SAFE(read_int16(&n, f));
    for (i = 0; i < n; i++) {
        SAFE(read_string(&s, f));
        set_os_priv(s, NP_SERVOPER);
    }
    close_db(f);
}

/*************************************************************************/

static void sirv_load_os_sa(const char *dir)
{
    char *s;
    dbFILE *f;
    int16 n, i;

    f = open_db_ver(dir, "os_sa.db", 5, 8, NULL);
    SAFE(read_int16(&n, f));
    for (i = 0; i < n; i++) {
        SAFE(read_string(&s, f));
        set_os_priv(s, NP_SERVADMIN);
    }
    close_db(f);
}

/*************************************************************************/

static void auspice_load_admin(const char *dir)
{
    char *s;
    dbFILE *f;
    long i, j;
    int c;
    struct admin_ {
        struct admin_ *next, *prev;
        char *nick;
        char *host;     /* fake host to set */
        char *who;      /* added by who */
        char *server;
        char **mark;
        long markline;
        long adflags;
        long flags;
        time_t added;
    } admin;

    f = open_db_ver(dir, "admin.db", 1, 1, NULL);
    for (i = 33; i < 256; i++) {
        while ((c = getc_db(f)) == 1) {
            SAFE(read_variable(admin, f));
            if (admin.nick)
                SAFE(read_string(&admin.nick, f));
            if (admin.host)
                SAFE(read_string(&admin.host, f));
            if (admin.who)
                SAFE(read_string(&admin.who, f));
            if (admin.server)
                SAFE(read_string(&admin.server, f));
            for (j = 0; j < admin.markline; j++)
                SAFE(read_string(&s, f));
            if (admin.adflags & 4)
                set_os_priv(admin.nick, NP_SERVADMIN);
            else if (admin.adflags & 2)
                set_os_priv(admin.nick, NP_SERVOPER);
        }
        if (c != 0) {
            fprintf(stderr, "%s is corrupt, aborting.\n", f->filename);
            exit(1);
        }
    }
    close_db(f);
}

/*************************************************************************/

static void sirv_load_akill(const char *dir, int type)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;
    struct akill_ {
        char *mask;
        char *reason;
        char who[32];
        time_t time;
        time_t expires;
        long resv[4];
    } akill;

    if (type == TYPE_SIRV) {
        f = open_db_ver(dir, "akill.db", 5, 8, NULL);
    } else if (type == TYPE_AUSPICE) {
        f = open_db_ver(dir, "akill.db", 6, 6, NULL);
    } else if (type == TYPE_BOLIVIA) {
        f = open_db_ver(dir, "akill.db", 5, 5, NULL);
    } else {
        fprintf(stderr, "BUG: invalid type %d to sirv_load_akill()\n", type);
        exit(1);
    }
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        SAFE(read_variable(akill, f));
        strbcpy(md[i].who, akill.who);
        md[i].time = akill.time;
        md[i].expires = akill.expires;
    }
    for (i = 0; i < n; i++) {
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&md[i].reason, f));
        add_maskdata(MD_AKILL, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void sirv_load_trigger(const char *dir, int type)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;
    struct trigger_ {
        char *mask;
        long tvalue;
        char who[32];
        long resv[4];
    } trigger;

    if (type == TYPE_SIRV) {
        f = open_db_ver(dir, "trigger.db", 5, 8, NULL);
    } else if (type == TYPE_AUSPICE) {
        f = open_db_ver(dir, "trigger.db", 6, 6, NULL);
    } else if (type == TYPE_BOLIVIA) {
        f = open_db_ver(dir, "trigger.db", 5, 5, NULL);
    } else {
        fprintf(stderr, "BUG: invalid type %d to sirv_load_trigger()\n", type);
        exit(1);
    }
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        SAFE(read_variable(trigger, f));
        if (trigger.tvalue > MAX_MASKDATA_LIMIT)
            trigger.tvalue = MAX_MASKDATA_LIMIT;
        md[i].limit = trigger.tvalue;
        strbcpy(md[i].who, trigger.who);
        md[i].reason = (char *)"(unknown)";
        md[i].time = time(NULL);
        md[i].expires = 0;
    }
    for (i = 0; i < n; i++) {
        SAFE(read_string(&md[i].mask, f));
        add_maskdata(MD_EXCEPTION, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void bolivia_load_gline(const char *dir)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;
    char *s;

    f = open_db_ver(dir, "gline.db", 5, 5, NULL);
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_string(&s, f));
        if (!s)
            s = (char *)"<unknown>";
        strbcpy(md[i].who, s);
        add_maskdata(MD_SGLINE, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void bolivia_load_qline(const char *dir)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;

    f = open_db_ver(dir, "qline.db", 5, 5, NULL);
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        SAFE(read_string(&md[i].mask, f));
        md[i].reason = (char *)"Unknown (imported from Bolivia IRC Services)";
        strbcpy(md[i].who, "<unknown>");
        add_maskdata(MD_SQLINE, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/

static void bolivia_load_zline(const char *dir)
{
    dbFILE *f;
    int16 i, n;
    MaskData *md;
    char *s;

    f = open_db_ver(dir, "zline.db", 5, 5, NULL);
    SAFE(read_int16(&n, f));
    md = scalloc(sizeof(*md), n);
    for (i = 0; i < n; i++) {
        SAFE(read_string(&md[i].mask, f));
        SAFE(read_string(&md[i].reason, f));
        SAFE(read_string(&s, f));
        if (!s)
            s = (char *)"<unknown>";
        strbcpy(md[i].who, s);
        add_maskdata(MD_SZLINE, &md[i]);
    }
    close_db(f);
}

/*************************************************************************/
/*************************************************************************/

static const char *check_auspice(const char *dir)
{
    FILE *f;
    char buf[PATH_MAX+1];

    snprintf(buf, sizeof(buf), "%s/admin.db", dir);
    if (access(buf, R_OK) == 0) {
        snprintf(buf, sizeof(buf), "%s/chan.db", dir);
        f = fopen(buf, "rb");
        if (f) {
            int ver;
            ver  = fgetc(f)<<24;
            ver |= fgetc(f)<<16;
            ver |= fgetc(f)<<8;
            ver |= fgetc(f);
            fclose(f);
            if (ver == 7)
                return "Auspice 2.5.x";
            else if (ver == 8)
                return "Auspice 2.6/2.7";
        }
    }
    return NULL;
}

static void load_auspice(const char *dir, int verbose, int ac, char **av)
{
    if (ac > 1) {
        fprintf(stderr, "Unrecognized option %s\n", av[1]);
        usage(av[0]);
    }
    if (verbose)
        fprintf(stderr, "Loading nick.db...\n");
    sirv_load_nick(dir, TYPE_AUSPICE);
    if (verbose)
        fprintf(stderr, "Loading chan.db...\n");
    sirv_load_chan(dir, TYPE_AUSPICE);
    if (verbose)
        fprintf(stderr, "Loading memo.db...\n");
    sirv_load_memo(dir, TYPE_AUSPICE);
    if (verbose)
        fprintf(stderr, "Loading admin.db...\n");
    auspice_load_admin(dir);
    if (verbose)
        fprintf(stderr, "Loading akill.db...\n");
    sirv_load_akill(dir, TYPE_AUSPICE);
    if (verbose)
        fprintf(stderr, "Loading trigger.db...\n");
    sirv_load_trigger(dir, TYPE_AUSPICE);
}

/*************************************************************************/
/*************************************************************************/

static const char *check_bolivia(const char *dir)
{
    char buf[PATH_MAX+1];

    snprintf(buf, sizeof(buf), "%s/jupenick.db", dir);
    if (access(buf, R_OK) == 0)
        return "Bolivia";
    return NULL;
}

static void load_bolivia(const char *dir, int verbose, int ac, char **av)
{
    if (ac > 1) {
        fprintf(stderr, "Unrecognized option %s\n", av[1]);
        usage(av[0]);
    }
    if (verbose)
        fprintf(stderr, "Loading nick.db...\n");
    sirv_load_nick(dir, TYPE_BOLIVIA);
    if (verbose)
        fprintf(stderr, "Loading chan.db...\n");
    sirv_load_chan(dir, TYPE_BOLIVIA);
    if (verbose)
        fprintf(stderr, "Loading memo.db...\n");
    sirv_load_memo(dir, TYPE_BOLIVIA);
    if (verbose)
        fprintf(stderr, "Loading akill.db...\n");
    sirv_load_akill(dir, TYPE_BOLIVIA);
    if (verbose)
        fprintf(stderr, "Loading trigger.db...\n");
    sirv_load_trigger(dir, TYPE_BOLIVIA);
    if (verbose)
        fprintf(stderr, "Loading gline.db...\n");
    bolivia_load_gline(dir);
    if (verbose)
        fprintf(stderr, "Loading qline.db...\n");
    bolivia_load_qline(dir);
    if (verbose)
        fprintf(stderr, "Loading zline.db...\n");
    bolivia_load_zline(dir);
}

/*************************************************************************/
/*************************************************************************/

static const char *check_sirv(const char *dir)
{
    FILE *f;
    char buf[PATH_MAX+1];

    if (check_auspice(dir) || check_bolivia(dir))
        return NULL;
    snprintf(buf, sizeof(buf), "%s/trigger.db", dir);
    f = fopen(buf, "rb");
    if (f) {
        int ver;
        ver  = fgetc(f)<<24;
        ver |= fgetc(f)<<16;
        ver |= fgetc(f)<<8;
        ver |= fgetc(f);
        fclose(f);
        if (ver == 5)
            return "SirvNET 1.x";
        else if (ver == 6)
            return "SirvNET 2.0.0-2.2.0";
        else if (ver == 7)
            return "SirvNET 2.2.1-2.8.0";
        else if (ver == 8)
            return "SirvNET 2.9.0";
    }
    return NULL;
}

static void load_sirv(const char *dir, int verbose, int ac, char **av)
{
    if (ac > 1) {
        fprintf(stderr, "Unrecognized option %s\n", av[1]);
        usage(av[0]);
    }
    if (verbose)
        fprintf(stderr, "Loading nick.db...\n");
    sirv_load_nick(dir, TYPE_SIRV);
    if (verbose)
        fprintf(stderr, "Loading chan.db...\n");
    sirv_load_chan(dir, TYPE_SIRV);
    if (verbose)
        fprintf(stderr, "Loading memo.db...\n");
    sirv_load_memo(dir, TYPE_SIRV);
    if (verbose)
        fprintf(stderr, "Loading os_sa.db...\n");
    sirv_load_os_sa(dir);
    if (verbose)
        fprintf(stderr, "Loading os_sop.db...\n");
    sirv_load_os_sop(dir);
    if (verbose)
        fprintf(stderr, "Loading akill.db...\n");
    sirv_load_akill(dir, TYPE_SIRV);
    if (verbose)
        fprintf(stderr, "Loading trigger.db...\n");
    sirv_load_trigger(dir, TYPE_SIRV);
}

/*************************************************************************/
/*************************************************************************/

DBTypeInfo dbtype_sirv = {
    "sirv",
    check_sirv,
    load_sirv
};

DBTypeInfo dbtype_auspice = {
    "auspice",
    check_auspice,
    load_auspice
};

DBTypeInfo dbtype_bolivia = {
    "bolivia",
    check_bolivia,
    load_bolivia
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
