/* Include file for OperServ.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef OPERSERV_H
#define OPERSERV_H

/*************************************************************************/

/* Constants for use with get_operserv_data(): */
#define OSDATA_MAXUSERCNT       1       /* int32 */
#define OSDATA_MAXUSERTIME      2       /* time_t */
#define OSDATA_SUPASS           3       /* Password * */

/*************************************************************************/

/* Exports: */

E char *s_OperServ;
E char *s_GlobalNoticer;
E char *ServicesRoot;

E int get_operserv_data(int what, void *ret);
E int put_operserv_data(int what, void *ptr);
E int is_services_root(const User *u);
E int is_services_admin(const User *u);
E int is_services_oper(const User *u);
E int nick_is_services_admin(const NickInfo *ni);

/*************************************************************************/

#endif  /* OPERSERV_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
