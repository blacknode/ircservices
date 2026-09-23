/* General prototypes and external variable declarations.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef EXTERN_H
#define EXTERN_H


#define E extern        /* This is used in other files as well */


/**** actions.c ****/

E int actions_init(int ac, char **av);
E void actions_cleanup(void);
E int bad_password(const char *service, User *u, const char *what);
E void clear_channel(Channel *chan, int what, const void *param);
E const char *set_clear_channel_sender(const char *newsender);
E void kill_user(const char *source, const char *user, const char *reason);
E void set_topic(const char *source, Channel *c, const char *topic,
                 const char *setter, time_t time);
E void set_cmode(const char *sender, Channel *channel, ...);


/**** channels.c ****/

E Channel *get_channel(const char *chan);
E Channel *first_channel(void);
E Channel *next_channel(void);

E int channel_init(int ac, char **av);
E void channel_cleanup(void);
E void get_channel_stats(long *nrec, long *memuse);

E Channel *chan_adduser(User *user, const char *chan, int32 modes);
E void chan_deluser(User *user, Channel *c);
E int chan_has_ban(const char *chan, const char *ban);

E void do_cmode(const char *source, int ac, char **av);
E void do_topic(const char *source, int ac, char **av);


/**** compat.c ****/

#if !HAVE_HSTRERROR
# undef hstrerror
E const char *hstrerror(int h_errnum);
#endif
#if !HAVE_SNPRINTF
# if BAD_SNPRINTF
#  define snprintf my_snprintf
# endif
# define vsnprintf my_vsnprintf
E int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
E int snprintf(char *buf, size_t size, const char *fmt, ...);
#endif
#if !HAVE_STRTOK
# undef strtok
E char *strtok(char *str, const char *delim);
#endif
#if !HAVE_STRICMP && !HAVE_STRCASECMP
# undef stricmp
# undef strnicmp
E int stricmp(const char *s1, const char *s2);
E int strnicmp(const char *s1, const char *s2, size_t len);
#endif
#if !HAVE_STRDUP && !MEMCHECKS
# undef strdup
E char *strdup(const char *s);
#endif
#if !HAVE_STRSPN
# undef strspn
# undef strcspn
E size_t strspn(const char *s, const char *accept);
E size_t strcspn(const char *s, const char *reject);
#endif
#if !HAVE_STRERROR
# undef strerror
E char *strerror(int errnum);
#endif
#if !HAVE_STRSIGNAL
# undef strsignal
E char *strsignal(int signum);
#endif


/**** init.c ****/

E char **LoadModules;
E int    LoadModules_count;
E char **LoadLanguageText;
E int    LoadLanguageText_count;

E char * RemoteServer;
E int32  RemotePort;
E char * RemotePassword;
E char * LocalHost;
E int32  LocalPort;

E char * ServerName;
E char * ServerDesc;
E char * ServiceUser;
E char * ServiceHost;

E char * LogFilename;
E char   PIDFilename[PATH_MAX+1];
E char * MOTDFilename;
E char * LockFilename;

E int16  DefTimeZone;

E int    NoBouncyModes;
E int    NoSplitRecovery;
E int    StrictPasswords;
E int    NoAdminPasswordCheck;
E int32  BadPassLimit;
E time_t BadPassTimeout;
E int32  BadPassWarning;
E int32  IgnoreDecay;
E double IgnoreThreshold;
E time_t UpdateTimeout;
E time_t WarningTimeout;
E int32  ReadTimeout;
E int32  TimeoutCheck;
E time_t PingFrequency;
E int32  MergeChannelModes;
E int32  TotalNetBufferSize;
E int32  NetBufferSize;
E int32  NetBufferLimitInactive;
E int32  NetBufferLimitIgnore;

E char * EncryptionType;
E char * GuestNickPrefix;
E char **RejectEmail;
E int    RejectEmail_count;
E int32  ListMax;
E int    LogMaxUsers;
E int    EnableGetpass;
E int    WallAdminPrivs;

E int introduce_user(const char *user);
E int init(int ac, char **av);
E int reconfigure(void);
E void cleanup(void);


/**** ignore.c ****/

E void ignore_init(User *u);
E void ignore_update(User *u, uint32 msec);


/**** main.c ****/

E const char *services_dir;
E int    debug;
E int    readonly;
E int    nofork;
E int    noexpire;
E int    noakill;
E int    forceload;
E int    encrypt_all;

E int    linked;
E int    quitting;
E int    delayed_quit;
E int    restart;
E char   quitmsg[BUFSIZE];
E char   inbuf[BUFSIZE];
E Socket *servsock;
E int    save_data;
E time_t start_time;
E int    openlog_failed, openlog_errno;
E int    cb_connect;
E int    cb_save_complete;

E void connect_callback(Socket *s, void *param_unused);
E void disconnect_callback(Socket *s, void *param);
E void readfirstline_callback(Socket *s, void *param_unused);
E void readline_callback(Socket *s, void *param_unused);

E int lock_data(void);
E int is_data_locked(void);
E int unlock_data(void);
E void save_data_now(void);


/**** messages.c ****/

E int allow_ignore;
/* rest is in messages.h */


/**** misc.c ****/

E unsigned char irc_lowertable[256];
E unsigned char irc_tolower(char c);
#define irc_tolower(c) (irc_lowertable[(uint8)(c)])  /* for speed */
E int irc_stricmp(const char *s1, const char *s2);
E int irc_strnicmp(const char *s1, const char *s2, int max);
E char *strscpy(char *d, const char *s, size_t len);
E char *strmove(char *d, const char *s);
E char *stristr(const char *s1, const char *s2);
E char *strupper(char *s);
E char *strlower(char *s);
E char *strnrepl(char *s, int32 size, const char *old, const char *new);
E char *strtok_remaining(void);
/* strbcpy(): strscpy() for a char array destination; uses sizeof(d) as
 * the size to copy */
#define strbcpy(d,s) strscpy((d), (s), sizeof(d))

E char *merge_args(int argc, char **argv);

E int match_wild(const char *pattern, const char *str);
E int match_wild_nocase(const char *pattern, const char *str);

E unsigned char valid_nick_table[0x10000];
E unsigned char valid_chan_table[0x10000];
E int valid_nick(const char *str);
E int valid_chan(const char *str);
E int valid_domain(const char *str);
E int valid_email(const char *str);
E int valid_url(const char *str);
E int rejected_email(const char *email);

E uint32 time_msec(void);
E time_t strtotime(const char *str, char **endptr);
E int dotime(const char *s);

E uint8 *pack_ip(const char *ipaddr);
E char *unpack_ip(const uint8 *ip);
E uint8 *pack_ip6(const char *ipaddr);
E char *unpack_ip6(const uint8 *ip);

E int encode_base64(const void *in, int insize, char *out, int outsize);
E int decode_base64(const char *in, void *out, int outsize);

typedef int (*range_callback_t)(int num, va_list args);
E int process_numlist(const char *numstr, int *count_ret,
                      range_callback_t callback, ...);

E long atolsafe(const char *s, long min, long max);


/**** process.c ****/

E int split_buf(char *buf, char ***argv, int colon_special);
E int process_init(int ac, char **av);
E void process_cleanup(void);
E void process(void);


/**** servers.c ****/

E Server *get_server(const char *servername);
E Server *first_server(void);
E Server *next_server(void);

E int server_init(int ac, char **av);
E void server_cleanup(void);
E void get_server_stats(long *nservers, long *memuse);
E void do_server(const char *source, int ac, char **av);
E void do_squit(const char *source, int ac, char **av);


/**** signals.c ****/

E void init_signals(void);
E void do_sigsetjmp(void *bufptr);  /* actually a sigjmp_buf * */
E void enable_signals(void);
E void disable_signals(void);


/**** users.c ****/

E int32 usercnt, opcnt;

E User *get_user(const char *nick);
E User *first_user(void);
E User *next_user(void);

E void quit_user(User *user, const char *quitmsg, int is_kill);
E int user_init(int ac, char **av);
E void user_cleanup(void);
E void get_user_stats(long *nusers, long *memuse);

E int do_nick(const char *source, int ac, char **av);
E void do_join(const char *source, int ac, char **av);
E void do_part(const char *source, int ac, char **av);
E void do_kick(const char *source, int ac, char **av);
E void do_umode(const char *source, int ac, char **av);
E void do_quit(const char *source, int ac, char **av);
E void do_kill(const char *source, int ac, char **av);

E Channel *join_channel(User *user, const char *channel, int32 modes);
E int part_channel(User *user, const char *channel, int callback,
                   const char *param, const char *source);
E void part_all_channels(User *user);

E int is_oper(const User *user);
E Channel *is_on_chan(const User *user, const char *chan);
E int is_chanop(const User *user, const char *chan);
E int is_voiced(const User *user, const char *chan);

E int match_usermask(const char *mask, const User *user);
E void split_usermask(const char *mask, char **nick, char **user, char **host);
E char *create_mask(const User *user, int use_fakehost);

E char *make_guest_nick(void);
E int is_guest_nick(const char *nick);



#endif  /* EXTERN_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
