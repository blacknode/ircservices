/* Common routines and constants for message sending.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef SEND_H
#define SEND_H

/*************************************************************************/

/* Protocol information (set by protocol module): */

extern const char *protocol_name;
extern const char *protocol_version;
extern uint32 protocol_features;
extern int protocol_nickmax;
extern const char *pseudoclient_modes;
extern const char *enforcer_modes;
extern int pseudoclient_oper;


/* Constants for protocol_features: */

/* Has a "halfop" (+h) channel user mode (Unreal, etc.) */
#define PF_HALFOP       0x00000001
/* Has "protect" (+a) and "owner" (+q) channel user modes (Unreal, etc.) */
#define PF_CHANPROT     0x00000002
/* Has channel ban exceptions (+e) */
#define PF_BANEXCEPT    0x00000004
/* Has SZLINE command or equivalent (ban IP address/mask from all servers) */
#define PF_SZLINE       0x00000008
/* Suppresses QUIT messages from clients on server disconnects */
#define PF_NOQUIT       0x00000010
/* Supports some method to make a client join a channel */
#define PF_SVSJOIN      0x00000020
/* Supports some method of forcibly changing a client's nickname */
#define PF_CHANGENICK   0x00000040
/* Supports autokill exclusions */
#define PF_AKILL_EXCL   0x00000080
/* Timestamp in MODE message comes right after channel name */
#define PF_MODETS_FIRST 0x00000100
/* Has channel invite masks (+I) */
#define PF_INVITEMASK   0x00000200

/* Invalid flag, used to check whether protocol_features was set */
#define PF_UNSET        0x80000000


/* Routines to be implemented by protocol modules: */

E_FUNCPTR(void, send_nick, (const char *nick, const char *user,
                            const char *host, const char *server,
                            const char *name, const char *modes));
E_FUNCPTR(void, send_nickchange, (const char *nick, const char *newnick));
E_FUNCPTR(void, send_namechange, (const char *nick, const char *newname));
E_FUNCPTR(void, send_server, (void));
E_FUNCPTR(void, send_server_remote, (const char *server, const char *desc));
E_FUNCPTR(void, wallops, (const char *source, const char *fmt, ...)
                         FORMAT(printf,2,3));
E_FUNCPTR(void, notice_all, (const char *source, const char *fmt, ...)
                            FORMAT(printf,2,3));
E_FUNCPTR(void, send_channel_cmd, (const char *source, const char *fmt, ...)
                                  FORMAT(printf,2,3));
E_FUNCPTR(void, send_nickchange_remote,
                (const char *nick, const char *newnick));

/*************************************************************************/

/* Flags for send_pseudo_nick(): */

/* Pseudoclient requires oper privileges (note that depending on the
 * protocol, the pseudoclient may not actually get +o) */
#define PSEUDO_OPER     0x01
/* Pseudoclient should be invisible (+i) */
#define PSEUDO_INVIS    0x02

/*************************************************************************/

/* External declarations. */

/* Last time at which data was sent.  Used to check whether a PING needs
 * to be sent. */
extern time_t last_send;

/* Initialization/cleanup */
extern int send_init(int ac, char **av);
extern void send_cleanup(void);

/* Basic message-sending routine ([v]printf-like) */
extern void send_cmd(const char *source, const char *fmt, ...)
        FORMAT(printf,2,3);
extern void vsend_cmd(const char *source, const char *fmt, va_list args)
        FORMAT(printf,2,0);

/* Shortcuts for sending miscellaneous messages */
extern void send_error(const char *fmt, ...) FORMAT(printf,1,2);
extern void send_cmode_cmd(const char *source, const char *channel,
                           const char *fmt, ...);
extern void send_pseudo_nick(const char *nick, const char *realname,
                             int flags);

/* Routines for PRIVMSG/NOTICE sending */
extern void notice(const char *source, const char *dest, const char *fmt, ...)
        FORMAT(printf,3,4);
extern void notice_list(const char *source, const char *dest,
                        const char **text);
extern void notice_lang(const char *source, const User *dest, int message,
                        ...);
extern void notice_help(const char *source, const User *dest, int message,
                        ...);
extern void privmsg(const char *source, const char *dest, const char *fmt, ...)
        FORMAT(printf,3,4);

/*************************************************************************/

#endif  /* SEND_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
