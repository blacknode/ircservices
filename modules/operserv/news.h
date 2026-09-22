/* News data structures and constants.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef NEWS_H
#define NEWS_H

/*************************************************************************/

typedef struct newsitem_ NewsItem;
struct newsitem_ {
    NewsItem *next, *prev;

    int16 type;
    int32 num;          /* Numbering is separate for login and oper news */
    char *text;
    char who[NICKMAX];
    time_t time;
};

#define MAX_NEWS        32767

/* Maximum number of news items to display */
#define NEWS_DISPCOUNT  3

/*************************************************************************/

/* Constants for news types. */

#define NEWS_INVALID    -1      /* Used as placeholder */
#define NEWS_LOGON      0
#define NEWS_OPER       1

/*************************************************************************/

/* Database functions: */

E NewsItem *add_news(NewsItem *news);
E void del_news(NewsItem *news);
E NewsItem *get_news(int16 type, int32 num);
E NewsItem *put_news(NewsItem *news);
E NewsItem *first_news(void);
E NewsItem *next_news(void);
E int news_count(void);

/*************************************************************************/

#endif  /* NEWS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
