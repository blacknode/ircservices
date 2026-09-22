/* News module.
 * Based on code by Andrew Kempe (TheShadow).
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "conffile.h"
#include "commands.h"
#include "databases.h"
#include "language.h"

#include "operserv.h"
#include "news.h"

/*************************************************************************/

static Module *module_operserv;

static void do_logonnews(User *u);
static void do_opernews(User *u);

static Command cmds[] = {
    /* Anyone can use *NEWS LIST, but *NEWS {ADD,DEL} are reserved for
     * Services operators.  (The command routines check permissions.) */
    {"LOGONNEWS", do_logonnews, NULL,             NEWS_HELP_LOGON,     -1,-1},
    {"OPERNEWS",  do_opernews,  NULL,             NEWS_HELP_OPER,      -1,-1},
    {NULL}
};

/*************************************************************************/

/* List of messages for each news type.  This simplifies message sending. */

#define MSG_SYNTAX      0
#define MSG_LIST_HEADER 1
#define MSG_LIST_ENTRY  2
#define MSG_LIST_NONE   3
#define MSG_ADD_SYNTAX  4
#define MSG_ADD_FULL    5
#define MSG_ADDED       6
#define MSG_DEL_SYNTAX  7
#define MSG_DEL_NOT_FOUND 8
#define MSG_DELETED     9
#define MSG_DEL_NONE    10
#define MSG_DELETED_ALL 11
#define MSG_MAX         11

struct newsmsgs {
    int16 type;
    const char *name;
    int msgs[MSG_MAX+1];
};
static struct newsmsgs msgarray[] = {
    { NEWS_LOGON, "LOGON",
        { NEWS_LOGON_SYNTAX,
          NEWS_LOGON_LIST_HEADER,
          NEWS_LOGON_LIST_ENTRY,
          NEWS_LOGON_LIST_NONE,
          NEWS_LOGON_ADD_SYNTAX,
          NEWS_LOGON_ADD_FULL,
          NEWS_LOGON_ADDED,
          NEWS_LOGON_DEL_SYNTAX,
          NEWS_LOGON_DEL_NOT_FOUND,
          NEWS_LOGON_DELETED,
          NEWS_LOGON_DEL_NONE,
          NEWS_LOGON_DELETED_ALL
        }
    },
    { NEWS_OPER, "OPER",
        { NEWS_OPER_SYNTAX,
          NEWS_OPER_LIST_HEADER,
          NEWS_OPER_LIST_ENTRY,
          NEWS_OPER_LIST_NONE,
          NEWS_OPER_ADD_SYNTAX,
          NEWS_OPER_ADD_FULL,
          NEWS_OPER_ADDED,
          NEWS_OPER_DEL_SYNTAX,
          NEWS_OPER_DEL_NOT_FOUND,
          NEWS_OPER_DELETED,
          NEWS_OPER_DEL_NONE,
          NEWS_OPER_DELETED_ALL
        }
    }
};

static int *findmsgs(int16 type, char **typename) {
    int i;
    for (i = 0; i < lenof(msgarray); i++) {
        if (msgarray[i].type == type) {
            if (typename)
                *typename = (char *)msgarray[i].name;
            return msgarray[i].msgs;
        }
    }
    return NULL;
}

/*************************************************************************/

/* Main handler for NEWS commands. */
static void do_news(User *u, int16 type);

/* Lists all a certain type of news. */
static void do_news_list(User *u, int16 type, int *msgs);

/* Add news items. */
static void do_news_add(User *u, int16 type, int *msgs, const char *typename);
static int add_newsitem(User *u, const char *text, int16 type);

/* Delete news items. */
static void do_news_del(User *u, int16 type, int *msgs, const char *typename);
static int del_newsitem(int num, int16 type);

/*************************************************************************/
/**************************** Database stuff *****************************/
/*************************************************************************/

static NewsItem *newslist = NULL;
static int32 newslist_count = 0;
static int newslist_iterator;

/*************************************************************************/

static void *new_news(void)
{
    return scalloc(1, sizeof(NewsItem));
}

/*************************************************************************/

static void free_news(void *record)
{
    NewsItem *news = record;
    if (news) {
        free(news->text);
        free(news);
    }
}

/*************************************************************************/

NewsItem *add_news(NewsItem *newsitem)
{
    if (newslist_count >= MAX_NEWS)
        fatal("add_news(): too many news items!");
    ARRAY_EXTEND(newslist);
    memcpy(&newslist[newslist_count-1], newsitem, sizeof(NewsItem));
    newslist[newslist_count-1].next = 
        (NewsItem *)(long)(newslist_count-1); /*index*/
    free(newsitem);
    return &newslist[newslist_count-1];
}

/*************************************************************************/

void del_news(NewsItem *newsitem)
{
    int num = (int)(long)(newsitem->next);
    if (num < 0 || num >= newslist_count) {
        module_log("del_news(): invalid index %d in news item at %p",
                   num, newsitem);
        return;
    }
    free(newsitem->text);
    ARRAY_REMOVE(newslist, num);
    if (num < newslist_iterator)
        newslist_iterator--;
    while (num < newslist_count) {
        newslist[num].next = (NewsItem *)(long)num;
        num++;
    }
}

/*************************************************************************/

NewsItem *get_news(int16 type, int32 num)
{
    int i;

    ARRAY_FOREACH (i, newslist) {
        if (newslist[i].type == type && newslist[i].num == num)
            break;
    }
    return i<newslist_count ? &newslist[i] : NULL;
}

/*************************************************************************/

NewsItem *put_news(NewsItem *news)
{
    return news;
}

/*************************************************************************/

NewsItem *first_news(void)
{
    newslist_iterator = 0;
    return next_news();
}

NewsItem *next_news(void)
{
    if (newslist_iterator >= newslist_count)
        return NULL;
    return &newslist[newslist_iterator++];
}

/*************************************************************************/

int news_count(void)
{
    return newslist_count;
}

/*************************************************************************/

/* Free all memory used by database tables. */

static void clean_dbtables(void)
{
    int i;

    ARRAY_FOREACH (i, newslist)
        free(newslist[i].text);
    free(newslist);
    newslist = NULL;
    newslist_count = 0;
}

/*************************************************************************/

/* News database table info */
static DBField news_dbfields[] = {
    { "type", DBTYPE_INT16,  offsetof(NewsItem,type) },
    { "num",  DBTYPE_INT32,  offsetof(NewsItem,num) },
    { "text", DBTYPE_STRING, offsetof(NewsItem,text) },
    { "who",  DBTYPE_BUFFER, offsetof(NewsItem,who), NICKMAX },
    { "time", DBTYPE_TIME,   offsetof(NewsItem,time) },
    { NULL }
};
static DBTable news_dbtable = {
    .name    = "news",
    .newrec  = new_news,
    .freerec = free_news,
    .insert  = (void *)add_news,
    .first   = (void *)first_news,
    .next    = (void *)next_news,
    .fields  = news_dbfields,
};

/*************************************************************************/
/***************************** News display ******************************/
/*************************************************************************/

static void display_news(User *u, int16 type, time_t min_time)
{
    NewsItem *news, *disp[NEWS_DISPCOUNT];
    int count = 0;      /* Number we're going to show--not more than 3 */
    int msg;

    if (type == NEWS_LOGON) {
        msg = NEWS_LOGON_TEXT;
    } else if (type == NEWS_OPER) {
        msg = NEWS_OPER_TEXT;
    } else {
        module_log("Invalid type (%d) to display_news()", type);
        return;
    }

    for (news = first_news(); news; news = next_news()) {
        if (count >= NEWS_DISPCOUNT)
            break;
        if (news->type == type && (!min_time || news->time > min_time)) {
            disp[count] = news;
            count++;
        }
    }
    while (--count >= 0) {
        char timebuf[BUFSIZE];

        strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                      STRFTIME_SHORT_DATE_FORMAT, disp[count]->time);
        notice_lang(s_GlobalNoticer, u, msg, timebuf, disp[count]->text);
    }
}

/*************************************************************************/
/***************************** News editing ******************************/
/*************************************************************************/

static void do_logonnews(User *u)
{
    do_news(u, NEWS_LOGON);
}

static void do_opernews(User *u)
{
    do_news(u, NEWS_OPER);
}

/*************************************************************************/

/* Main news command handling routine. */
static void do_news(User *u, int16 type)
{
    const char *cmd = strtok(NULL, " ");
    char *typename;
    int *msgs;

    msgs = findmsgs(type, &typename);
    if (!msgs) {
        module_log("Invalid type to do_news()");
        return;
    }

    if (!cmd)
        cmd = "";

    if (stricmp(cmd, "LIST") == 0) {
        do_news_list(u, type, msgs);

    } else if (stricmp(cmd, "ADD") == 0) {
        if (is_services_oper(u))
            do_news_add(u, type, msgs, typename);
        else
            notice_lang(s_OperServ, u, PERMISSION_DENIED);

    } else if (stricmp(cmd, "DEL") == 0) {
        if (is_services_oper(u))
            do_news_del(u, type, msgs, typename);
        else
            notice_lang(s_OperServ, u, PERMISSION_DENIED);

    } else {
        char buf[32];
        snprintf(buf, sizeof(buf), "%sNEWS", typename);
        syntax_error(s_OperServ, u, buf, msgs[MSG_SYNTAX]);
    }
}

/*************************************************************************/

/* Handle a {LOGON,OPER}NEWS LIST command. */

static void do_news_list(User *u, int16 type, int *msgs)
{
    NewsItem *news;
    int count = 0;
    char timebuf[64];

    for (news = first_news(); news; news = next_news()) {
        if (news->type == type) {
            if (count == 0)
                notice_lang(s_OperServ, u, msgs[MSG_LIST_HEADER]);
            strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                          STRFTIME_DATE_TIME_FORMAT, news->time);
            notice_lang(s_OperServ, u, msgs[MSG_LIST_ENTRY],
                        news->num, timebuf,
                        *news->who ? news->who : "<unknown>",
                        news->text);
            count++;
        }
    }
    if (count == 0)
        notice_lang(s_OperServ, u, msgs[MSG_LIST_NONE]);
}

/*************************************************************************/

/* Handle a {LOGON,OPER}NEWS ADD command. */

static void do_news_add(User *u, int16 type, int *msgs, const char *typename)
{
    char *text = strtok_remaining();

    if (!text) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%sNEWS", typename);
        syntax_error(s_OperServ, u, buf, msgs[MSG_ADD_SYNTAX]);
    } else {
        int n = add_newsitem(u, text, type);
        if (n < 0)
            notice_lang(s_OperServ, u, msgs[MSG_ADD_FULL]);
        else
            notice_lang(s_OperServ, u, msgs[MSG_ADDED], n);
        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    }
}


/* Actually add a news item.  Return the number assigned to the item, or -1
 * if the news list is full (MAX_NEWS items).
 */

static int add_newsitem(User *u, const char *text, int16 type)
{
    NewsItem *news;
    int num;

    if (news_count() >= MAX_NEWS)
        return -1;

    num = 0;
    for (news = first_news(); news; news = next_news()) {
        if (news->type == type && num < news->num)
            num = news->num;
    }
    if (num+1 < num) {
        module_log("BUG: add_newsitem(): news number overflow (MAX_NEWS"
                   " too small?)");
        return -1;
    }
    news = scalloc(1, sizeof(*news));
    news->type = type;
    news->num = num+1;
    news->text = sstrdup(text);
    news->time = time(NULL);
    strbcpy(news->who, u->nick);
    add_news(news);
    return num+1;
}

/*************************************************************************/

/* Handle a {LOGON,OPER}NEWS DEL command. */

static void do_news_del(User *u, int16 type, int *msgs, const char *typename)
{
    char *text = strtok(NULL, " ");

    if (!text) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%sNEWS", typename);
        syntax_error(s_OperServ, u, buf, msgs[MSG_DEL_SYNTAX]);
    } else {
        if (stricmp(text, "ALL") != 0) {
            int num = atoi(text);
            if (num > 0 && del_newsitem(num, type))
                notice_lang(s_OperServ, u, msgs[MSG_DELETED], num);
            else
                notice_lang(s_OperServ, u, msgs[MSG_DEL_NOT_FOUND], num);
        } else {
            if (del_newsitem(0, type))
                notice_lang(s_OperServ, u, msgs[MSG_DELETED_ALL]);
            else
                notice_lang(s_OperServ, u, msgs[MSG_DEL_NONE]);
        }
        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    }
}


/* Actually delete a news item.  If `num' is 0, delete all news items of
 * the given type.  Returns the number of items deleted.
 */

static int del_newsitem(int num, int16 type)
{
    NewsItem *news;
    int count = 0;

    for (news = first_news(); news; news = next_news()) {
        if (news->type == type && (num == 0 || news->num == num)) {
            del_news(news);
            count++;
        }
    }
    return count;
}

/*************************************************************************/
/*************************** Callback routines ***************************/
/*************************************************************************/

/* Callback for users logging on. */

static int new_user_callback(User *u, int ac, char **av, int reconnect)
{
    display_news(u, NEWS_LOGON, reconnect ? u->signon : 0);
    return 0;
}

/*************************************************************************/

/* Callback to watch for mode +o to send oper news. */

static int user_mode_callback(User *u, int modechar, int add)
{
    if (modechar == 'o' && add)
        display_news(u, NEWS_OPER, 0);
    return 0;
}

/*************************************************************************/

/* OperServ STATS ALL callback. */

static int do_stats_all(User *user, const char *s_OperServ)
{
    int32 count, mem;
    NewsItem *news;

    count = mem = 0;
    for (news = first_news(); news; news = next_news()) {
        count++;
        mem += sizeof(*news);
        if (news->text)
            mem += strlen(news->text)+1;
    }
    notice_lang(s_OperServ, user, OPER_STATS_ALL_NEWS_MEM,
                count, (mem+512) / 1024);

    return 0;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { NULL }
};

/*************************************************************************/

int init_module()
{
    module_operserv = find_module("operserv/main");
    if (!module_operserv) {
        module_log("Main OperServ module not loaded");
        return 0;
    }
    use_module(module_operserv);

    if (!register_commands(module_operserv, cmds)) {
        module_log("Unable to register commands");
        exit_module(0);
        return 0;
    }

    if (!add_callback(NULL, "user create", new_user_callback)
     || !add_callback(NULL, "user MODE", user_mode_callback)
     || !add_callback(module_operserv, "STATS ALL", do_stats_all)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    if (!register_dbtable(&news_dbtable)) {
        module_log("Unable to register database table");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    unregister_dbtable(&news_dbtable);
    clean_dbtables();

    remove_callback(NULL, "user create", new_user_callback);
    remove_callback(NULL, "user MODE", user_mode_callback);

    if (module_operserv) {
        remove_callback(module_operserv, "STATS ALL", do_stats_all);
        unregister_commands(module_operserv, cmds);
        unuse_module(module_operserv);
        module_operserv = NULL;
    }

    return 1;
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
