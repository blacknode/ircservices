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

#include "services.h"

/*************************************************************************/

/* Constants for use with get_operserv_data(): */
#define OSDATA_MAXUSERCNT       1       /* int32 */
#define OSDATA_MAXUSERTIME      2       /* time_t */
#define OSDATA_SUPASS           3       /* Password * */

/*************************************************************************/

/* Events of operserv/main (see include/events.h).  "Stop" is what a
 * handler returning nonzero does.
 *
 * operserv.command:       (User *u, char *command)  a command, before it
 *                         runs; stop: it was handled
 * operserv.help:          (User *u, char *topic)  stop: help was given
 * operserv.help_commands: (User *u, int which)  append to the list of
 *                         commands: 0 = opers, 1 = admins, 2 = root,
 *                         3 = after the list
 * operserv.set:           (User *u, char *option, char *setting)  SET;
 *                         stop: the option was handled
 * operserv.stats:         (User *u, char *what)  STATS <what>; stop: handled
 * operserv.stats_all:     (User *u, const char *operserv_nick)  STATS ALL:
 *                         add your lines
 * operserv.expire_maskdata: (uint8 type, MaskData *md)  an entry expired
 */
#define OPERSERV_EVENT_COMMAND         "operserv.command"
#define OPERSERV_EVENT_HELP            "operserv.help"
#define OPERSERV_EVENT_HELP_COMMANDS   "operserv.help_commands"
#define OPERSERV_EVENT_SET             "operserv.set"
#define OPERSERV_EVENT_STATS           "operserv.stats"
#define OPERSERV_EVENT_STATS_ALL       "operserv.stats_all"
#define OPERSERV_EVENT_EXPIRE_MASKDATA "operserv.expire_maskdata"

/*************************************************************************/

/* Exports: */

E struct Service operserv_service;
E struct Service global_noticer_service;
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
