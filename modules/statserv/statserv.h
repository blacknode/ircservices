/* Statistical information.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef STATISTICS_H
#define STATISTICS_H

/*************************************************************************/

#if 0  // not implemented
typedef struct minmax_ MinMax;
struct minmax_ {
        int min, max;
        time_t mintime, maxtime;
};


typedef struct minmaxhistory_ MinMaxHistory;
struct minmaxhistory_ {
        MinMax hour, today, week, month, ever;
};
#endif


struct serverstats_ {
    ServerStats *next, *prev;   /* Use to navigate the entire server list */

    char *name;                 /* Server's name */

#if 0
    MinMaxHistory mm_users;
    MinMaxHistory mm_opers;
#endif

    time_t t_join;              /* Time server joined us. 0 == not here. */
    time_t t_quit;              /* Time server quit. */

    char *quit_message;         /* Server's last quit message */

#if 0
    int indirectsplits;         /* Times this server has split from view due to
                                 * a split between two downstream servers. */
    int directsplits;           /* Times this server has split from view due to
                                 * a split between it and its hub. */
#endif

    /* Online-only information: */

    int usercnt;                /* Current number of users on server */
    int opercnt;                /* Current number of opers on server */
};


/* Return whether the given server is currently online. */
#define SS_IS_ONLINE(ss)  ((ss)->t_join > (ss)->t_quit)

/*************************************************************************/

/* Exports: */

E ServerStats *add_serverstats(ServerStats *ss);
E void del_serverstats(ServerStats *ss);
E ServerStats *get_serverstats(const char *servername);
E ServerStats *put_serverstats(ServerStats *ss);
E ServerStats *first_serverstats(void);
E ServerStats *next_serverstats(void);

E ServerStats *new_serverstats(const char *servername);
E void free_serverstats(ServerStats *serverstats);

/*************************************************************************/

#endif  /* STATISTICS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
