/* Online server data.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef SERVERS_H
#define SERVERS_H

/*************************************************************************/

struct server_ {
    Server *next, *prev;        /* Use to navigate the entire server list */
    Server *hub;                /* Server's hub from our point of view */
    Server *child, *sibling;    /* Server's children from our P.O.V. */

    int fake;                   /* Is this a "fake" (root/juped) server? */
    char *name;                 /* Server's name */
    time_t t_join;              /* Time server joined us (0 == not here). */

    User *userlist;             /* List of users on server.  NOTE: this is
                                 * linked via snext/sprev, not next/prev. */

    ServerStats *stats;
};

/*************************************************************************/

#endif  /* SERVERS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
