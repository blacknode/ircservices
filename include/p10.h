/* P10 server protocol (ircu 2.10+, Undernet / ircu2).
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Services speak P10 natively; there is no protocol module to load.  The
 * rest of Services still thinks in RFC 1459 terms -- nicknames, server
 * names and full command names -- and this layer translates at the edge:
 *
 *   inbound   "AB N nick 1 ts ...", "ABAAC P ADAAA :hi"
 *                 -> source/target numerics resolved to nicks and server
 *                    names, tokens expanded to command names, and P10-only
 *                    messages (BURST, CREATE, ACCOUNT, ...) applied to the
 *                    user/channel/server state;
 *   outbound  send_cmd("NickServ", "NOTICE %s :%s", nick, text)
 *                 -> "AzAAB O ABAAC :text".
 *
 * Numerics.  Every server has a two-character numeric and every client a
 * five-character one (server numeric + three characters), in the P10
 * base64 alphabet (A-Z a-z 0-9 [ ]).  Services' own numeric comes from
 * the ServerNumeric directive in ircservices.conf and must be unique on
 * the network.
 *
 * The uplink has to treat Services as a services server: a Connect{}
 * block for the link and a UWorld{} block naming ServerName, so that
 * ACCOUNT, OPMODE and server-sourced MODE/KICK are accepted.
 */

#ifndef P10_H
#define P10_H

#include "services.h"

/*************************************************************************/

/* ServerNumeric directive: our server numeric, 0..4095. */
extern int32 ServerNumeric;

/* Highest nickname length ircu accepts (NICKLEN). */
#define P10_NICKLEN    15
/* Longest account name ircu accepts (ACCOUNTLEN). */
#define P10_ACCOUNTLEN 12

/*************************************************************************/

/* Set up the protocol layer; called once from init().  Returns nonzero on
 * success. */
extern int p10_init(void);
extern void p10_cleanup(void);

/* Split one line from the uplink into its source (resolved to a nick or
 * server name, "" when the line has none), command name (tokens expanded)
 * and parameters.  `line' is modified.  Returns zero for a line that
 * should be ignored. */
extern int p10_parse(char* line, const char** source_ret, const char** cmd_ret,
                     int* ac_ret, char*** av_ret);

/* Translate an RFC 1459-style command sent as `source' (a nick, a server
 * name or NULL for ourselves) to P10 and write it to the uplink. */
extern void p10_send(const char* source, const char* line);

/* Numeric <-> object lookups.  Return NULL when unknown. */
extern User* p10_find_user(const char* numeric);
extern const char* p10_user_numeric(const char* nick);
extern const char* p10_server_numeric(void);

/*************************************************************************/

#endif /* P10_H */
