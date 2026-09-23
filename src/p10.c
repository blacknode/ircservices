/* P10 server protocol (ircu 2.10+ / ircu2).
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * See p10.h for the overall design.  The file is laid out as:
 *
 *   1. numerics      base64 codec, the server/client numeric tables
 *   2. tokens        P10 token <-> command name
 *   3. inbound       p10_parse() and the P10-specific message handlers
 *   4. outbound      p10_send() and the rewrite rules per command
 *   5. protocol API  send_nick(), send_server(), wallops(), ... (send.h)
 *   6. callbacks     topic, G-lines, bookkeeping of users and servers
 *   7. setup         modes, p10_init(), p10_cleanup()
 */

#include "p10.h"
#include "language.h"
#include "messages.h"
#include "modules.h"
#include "services.h"

#include <arpa/inet.h>
#include <netinet/in.h>

/*************************************************************************/

int32 ServerNumeric = -1;

/* Capacity we announce for our own clients (a power of two). */
#define LOCAL_CAPACITY 4096
/* Expiry sent for AKILLs without one: ircu takes any value from a server. */
#define GLINE_FOREVER  (365 * 24 * 60 * 60)
/* Maximum parameters of an outbound message. */
#define MAXPARAMS      32

/*************************************************************************/
/***************************** 1. Numerics *******************************/
/*************************************************************************/

static const char b64_digits[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789[]";
static int8 b64_values[256];

static void b64_init(void)
{
    int i;

    memset(b64_values, -1, sizeof(b64_values));
    for (i = 0; i < 64; i++)
        b64_values[(uint8)b64_digits[i]] = i;
}

/* Decode `len' base64 digits; return -1 if any is invalid. */
static int32 b64_decode(const char* s, int len)
{
    int32 value = 0;

    while (len-- > 0) {
        int digit = b64_values[(uint8)*s++];
        if (digit < 0)
            return -1;
        value = (value << 6) | digit;
    }
    return value;
}

/* Encode `value' as exactly `len' digits, NUL-terminated. */
static void b64_encode(uint32 value, char* out, int len)
{
    out[len] = 0;
    while (len-- > 0) {
        out[len] = b64_digits[value & 63];
        value >>= 6;
    }
}

/* Decode a base64 IP address from a NICK line into printable form. */
static const char* decode_ip(const char* b64)
{
    static char buf[INET6_ADDRSTRLEN];
    uint8 addr[16];
    int len = strlen(b64);

    if (len == 6) { /* IPv4 */
        int32 v = b64_decode(b64, 6);
        if (v < 0)
            return NULL;
        addr[0] = v >> 24;
        addr[1] = v >> 16;
        addr[2] = v >> 8;
        addr[3] = v;
        return inet_ntop(AF_INET, addr, buf, sizeof(buf));
    }

    /* IPv6: eight groups of three digits, "_" standing for a run of
     * zero groups (ircu's iptobase64()). */
    {
        uint16 groups[8] = {0};
        int ngroups = 0, zero_at = -1, i;
        const char* s = b64;

        while (*s && ngroups < 8) {
            if (*s == '_') {
                zero_at = ngroups;
                s++;
                continue;
            }
            if (strlen(s) < 3 || b64_decode(s, 3) < 0)
                return NULL;
            groups[ngroups++] = (uint16)b64_decode(s, 3);
            s += 3;
        }
        if (zero_at >= 0) { /* Expand the zero run in place */
            int shift = 8 - ngroups;
            for (i = 7; i >= zero_at + shift; i--)
                groups[i] = groups[i - shift];
            for (i = zero_at; i < zero_at + shift; i++)
                groups[i] = 0;
        }
        else if (ngroups != 8) {
            return NULL;
        }
        for (i = 0; i < 8; i++) {
            addr[2 * i] = groups[i] >> 8;
            addr[2 * i + 1] = groups[i] & 0xFF;
        }
        return inet_ntop(AF_INET6, addr, buf, sizeof(buf));
    }
}

/*************************************************************************/

/* A server known by numeric, and the users behind it indexed by the
 * client part of their numeric (masked by the server's capacity). */
typedef struct {
    Server* server;
    uint32 mask;
    User** clients;
} NumServer;

static NumServer* numservers[4096];

static char me_numeric[3];     /* Our server numeric */
static char uplink_numeric[3]; /* Our uplink's numeric */
static int burst_done;         /* Our END_OF_BURST was sent */

/* Our own clients (pseudo-clients and enforcers), by client index. */
static char local_nicks[LOCAL_CAPACITY][NICKMAX];
static int local_next;

/* The user a NICK line is introducing, while it is being introduced:
 * callbacks run inside do_nick() (AKILL checks, NickServ) may already
 * address that user before it has a User structure. */
static const char *pending_nick, *pending_numeric;

/* The raw numeric prefix of the line being processed. */
static const char* current_prefix = "";

/*************************************************************************/

/* Split a user numeric into server and client parts (YYXXX or old YXX). */
static int split_user_numeric(const char* num, int32* srv, int32* cli)
{
    int len = strlen(num);

    if (len == 5) {
        *srv = b64_decode(num, 2);
        *cli = b64_decode(num + 2, 3);
    }
    else if (len == 3) {
        *srv = b64_decode(num, 1);
        *cli = b64_decode(num + 1, 2);
    }
    else {
        return 0;
    }
    return *srv >= 0 && *cli >= 0;
}

static int32 server_index(const char* num)
{
    int len = strlen(num);
    return (len == 1 || len == 2) ? b64_decode(num, len) : -1;
}

static NumServer* find_numserver(const char* num)
{
    int32 i = server_index(num);
    return i >= 0 ? numservers[i] : NULL;
}

/* Record a server from its SERVER line numeric/capacity field ("ABAAB"). */
static void register_server(Server* server, const char* yxx)
{
    int len = strlen(yxx);
    int numlen = (len == 5) ? 2 : 1;
    int32 idx = b64_decode(yxx, numlen);
    int32 cap = b64_decode(yxx + numlen, len - numlen);
    NumServer* ns;
    uint32 mask;

    if (idx < 0 || cap < 0 || (len != 5 && len != 3)) {
        log("p10: bad numeric/capacity `%s' for server %s", yxx, server->name);
        return;
    }
    /* Round the capacity up to a power of two minus one, as ircu does. */
    for (mask = 1; mask < (uint32)cap; mask = mask << 1 | 1)
        ;
    if (numservers[idx]) {
        log("p10: numeric %s reused by %s", yxx, server->name);
        free(numservers[idx]->clients);
        free(numservers[idx]);
    }
    ns = scalloc(sizeof(*ns), 1);
    ns->server = server;
    ns->mask = mask;
    ns->clients = scalloc(sizeof(User*), mask + 1);
    numservers[idx] = ns;
    strscpy(server->numeric, yxx, numlen + 1);
}

static void unregister_server(Server* server)
{
    int32 idx = server_index(server->numeric);

    if (idx < 0 || !numservers[idx] || numservers[idx]->server != server)
        return;
    free(numservers[idx]->clients);
    free(numservers[idx]);
    numservers[idx] = NULL;
    *server->numeric = 0;
}

User* p10_find_user(const char* num)
{
    int32 srv, cli;
    NumServer* ns;

    if (!num || !split_user_numeric(num, &srv, &cli) ||
        !(ns = numservers[srv]))
        return NULL;
    return ns->clients[cli & ns->mask];
}

static void register_user(User* u, const char* num)
{
    int32 srv, cli;
    NumServer* ns;

    if (!split_user_numeric(num, &srv, &cli) || !(ns = numservers[srv])) {
        log("p10: user %s has unknown numeric %s", u->nick, num);
        return;
    }
    ns->clients[cli & ns->mask] = u;
    strbcpy(u->numeric, num);
}

static void unregister_user(User* u)
{
    int32 srv, cli;
    NumServer* ns;

    if (!*u->numeric || !split_user_numeric(u->numeric, &srv, &cli))
        return;
    if ((ns = numservers[srv]) && ns->clients[cli & ns->mask] == u)
        ns->clients[cli & ns->mask] = NULL;
    *u->numeric = 0;
}

/*************************************************************************/

/* Our own clients. */

static int local_find(const char* nick)
{
    int i;

    for (i = 0; i < LOCAL_CAPACITY; i++) {
        if (*local_nicks[i] && irc_stricmp(local_nicks[i], nick) == 0)
            return i;
    }
    return -1;
}

static int local_alloc(const char* nick)
{
    int i, slot;

    if ((slot = local_find(nick)) >= 0)
        return slot;
    for (i = 0; i < LOCAL_CAPACITY; i++) {
        slot = (local_next + i) % LOCAL_CAPACITY;
        if (!*local_nicks[slot]) {
            local_next = slot + 1;
            strbcpy(local_nicks[slot], nick);
            return slot;
        }
    }
    fatal("p10: more than %d clients of our own", LOCAL_CAPACITY);
    return -1; /* not reached */
}

static void local_free(const char* nick)
{
    int slot = local_find(nick);
    if (slot >= 0)
        *local_nicks[slot] = 0;
}

static const char* local_numeric(int slot)
{
    static char buf[6];

    memcpy(buf, me_numeric, 2);
    b64_encode(slot, buf + 2, 3);
    return buf;
}

/* The nick of our client with the given numeric, or NULL. */
static const char* local_nick_by_numeric(const char* num)
{
    int32 srv, cli;

    if (!split_user_numeric(num, &srv, &cli) || srv != ServerNumeric ||
        cli >= LOCAL_CAPACITY || !*local_nicks[cli])
        return NULL;
    return local_nicks[cli];
}

/*************************************************************************/

const char* p10_server_numeric(void)
{
    return me_numeric;
}

const char* p10_user_numeric(const char* nick)
{
    User* u;
    int slot;

    if (!nick || !*nick)
        return NULL;
    if (pending_nick && irc_stricmp(nick, pending_nick) == 0)
        return pending_numeric;
    if ((slot = local_find(nick)) >= 0)
        return local_numeric(slot);
    u = get_user(nick);
    return (u && *u->numeric) ? u->numeric : NULL;
}

/* Resolve a numeric parameter to a nick; leave anything else alone. */
static char* numeric_to_nick(char* param)
{
    User* u = p10_find_user(param);
    const char* nick;

    if (u)
        return u->nick;
    if ((nick = local_nick_by_numeric(param)) != NULL)
        return (char*)nick;
    return param;
}

/*************************************************************************/
/****************************** 2. Tokens ********************************/
/*************************************************************************/

typedef struct {
    const char* token;
    const char* name;
} Token;

/* ircu's include/msg.h, token and command name of every server message
 * Services may send or receive. */
static const Token tokens[] = {{"AC", "ACCOUNT"},   {"AD", "ADMIN"},
                               {"A", "AWAY"},       {"B", "BURST"},
                               {"CF", "CONFIG"},    {"CM", "CLEARMODE"},
                               {"CN", "CNOTICE"},   {"CO", "CONNECT"},
                               {"CP", "CPRIVMSG"},  {"C", "CREATE"},
                               {"DE", "DESTRUCT"},  {"DS", "DESYNCH"},
                               {"EA", "EOB_ACK"},   {"EB", "END_OF_BURST"},
                               {"F", "INFO"},       {"GL", "GLINE"},
                               {"G", "PING"},       {"H", "WHO"},
                               {"I", "INVITE"},     {"J", "JOIN"},
                               {"JU", "JUPE"},      {"K", "KICK"},
                               {"D", "KILL"},       {"LG", "LANGUAGE"},
                               {"LI", "LINKS"},     {"LL", "ASLL"},
                               {"LU", "LUSERS"},    {"L", "PART"},
                               {"MD", "MODULE"},    {"MO", "MOTD"},
                               {"M", "MODE"},       {"N", "NICK"},
                               {"E", "NAMES"},      {"O", "NOTICE"},
                               {"OM", "OPMODE"},    {"P", "PRIVMSG"},
                               {"PA", "PASS"},      {"Q", "QUIT"},
                               {"R", "STATS"},      {"RD", "REDACT"},
                               {"RI", "RPING"},     {"RO", "RPONG"},
                               {"S", "SERVER"},     {"SE", "SETTIME"},
                               {"SL", "SLINE"},     {"SQ", "SQUIT"},
                               {"T", "TOPIC"},      {"TI", "TIME"},
                               {"TM", "TAGMSG"},    {"TR", "TRACE"},
                               {"U", "SILENCE"},    {"UP", "UPING"},
                               {"V", "VERSION"},    {"W", "WHOIS"},
                               {"WA", "WALLOPS"},   {"WC", "WALLCHOPS"},
                               {"WU", "WALLUSERS"}, {"WV", "WALLVOICES"},
                               {"X", "WHOWAS"},     {"XQ", "XQUERY"},
                               {"XR", "XREPLY"},    {"Y", "ERROR"},
                               {"Z", "PONG"},       {NULL}};

static const char* token_to_name(const char* token)
{
    const Token* t;

    for (t = tokens; t->token; t++) {
        if (strcmp(t->token, token) == 0)
            return t->name;
    }
    return token; /* Numerics and full names pass through */
}

static const char* name_to_token(const char* name)
{
    const Token* t;

    for (t = tokens; t->token; t++) {
        if (stricmp(t->name, name) == 0)
            return t->token;
    }
    return NULL;
}

/*************************************************************************/
/***************************** 3. Inbound ********************************/
/*************************************************************************/

/* Messages that arrive without a numeric prefix (before the link is up,
 * or sent raw by ircu). */
static int is_unprefixed(const char* word)
{
    static const char* const names[] = {"PASS", "SERVER", "ERROR",
                                        "PING", "NOTICE", NULL};
    int i;

    for (i = 0; names[i]; i++) {
        if (strcmp(word, names[i]) == 0)
            return 1;
    }
    return 0;
}

/* Resolve the numeric prefix of a line to a nick or server name. */
static const char* resolve_source(const char* prefix)
{
    NumServer* ns;
    User* u;
    const char* nick;

    if (strlen(prefix) <= 2) {
        if ((ns = find_numserver(prefix)) != NULL && ns->server)
            return ns->server->name;
    }
    else if ((u = p10_find_user(prefix)) != NULL) {
        return u->nick;
    }
    else if ((nick = local_nick_by_numeric(prefix)) != NULL) {
        return nick;
    }
    log_debug(1, "p10: message from unknown numeric %s", prefix);
    return prefix;
}

/* Rewrite the numeric arguments of channel user modes (+o/+v) into nicks,
 * and drop the channel timestamp that server MODEs may carry.  av[0] is
 * the channel, av[1] the mode string.  Returns the new argument count. */
static int translate_chanmode_args(int ac, char** av)
{
    const char* s;
    int add = 1, arg = 2;

    if (ac < 2)
        return ac;
    for (s = av[1]; *s && arg <= ac; s++) {
        int params;

        if (*s == '+' || *s == '-') {
            add = (*s == '+');
            continue;
        }
        if (mode_char_to_flag(*s, MODE_CHANUSER)) {
            if (arg < ac)
                av[arg] = numeric_to_nick(av[arg]);
            arg++;
            continue;
        }
        params = mode_char_to_params(*s, MODE_CHANNEL);
        if (params > 0)
            arg += (params >> (add ? 8 : 0)) & 0xFF;
    }
    if (arg == ac - 1 && isdigit((uint8)*av[ac - 1]))
        ac--; /* trailing channel timestamp */
    return ac;
}

/* Normalize the parameters of generic messages whose meaning does not
 * change in P10 but whose user arguments are numerics, so the RFC 1459
 * handlers in messages.c can process them unchanged. */
static int translate_params(const char* cmd, int ac, char** av)
{
    if (!strcmp(cmd, "PRIVMSG") || !strcmp(cmd, "NOTICE")) {
        if (ac >= 1)
            av[0] = numeric_to_nick(av[0]);
    }
    else if (!strcmp(cmd, "KICK")) {
        if (ac >= 2)
            av[1] = numeric_to_nick(av[1]);
    }
    else if (!strcmp(cmd, "WHOIS")) {
        /* W <target server> :<nick> */
        if (ac >= 2) {
            av[0] = av[1];
            ac = 1;
        }
    }
    else if (!strcmp(cmd, "SQUIT")) {
        /* SQ <server> <timestamp> :<reason> */
        if (ac >= 3) {
            av[1] = av[2];
            ac = 2;
        }
        else if (ac == 2 && isdigit((uint8)*av[1])) {
            av[1] = "";
        }
        if (ac >= 1) {
            NumServer* ns = find_numserver(av[0]);
            if (!get_server(av[0]) && ns && ns->server)
                av[0] = ns->server->name;
        }
    }
    else if (!strcmp(cmd, "TOPIC")) {
        /* T <chan> [<creation ts> [<topic ts> [<setter>]]] :<topic>
         *   -> <chan> <setter> <topic ts> <topic> (see do_topic()) */
        static char tsbuf[32];
        char *chan = av[0], *topic = av[ac - 1];
        char* setter =
            (ac >= 5) ? av[ac - 2] : (char*)resolve_source(current_prefix);
        char* ts = (ac >= 4) ? av[2] : NULL;

        if (ac < 2)
            return ac;
        if (!ts) {
            snprintf(tsbuf, sizeof(tsbuf), "%ld", (long)time(NULL));
            ts = tsbuf;
        }
        av[0] = chan;
        av[1] = setter;
        av[2] = ts;
        av[3] = topic;
        ac = 4;
    }
    else if (!strcmp(cmd, "MODE")) {
        if (ac >= 2 && (*av[0] == '#' || *av[0] == '&'))
            ac = translate_chanmode_args(ac, av);
    }
    return ac;
}

/* Cut the next space-delimited word off `*s'. */
static char* next_word(char** s)
{
    char* word = *s;

    *s += strcspn(*s, " ");
    if (**s)
        *(*s)++ = 0;
    while (**s == ' ')
        (*s)++;
    return word;
}

int p10_parse(char* line, const char** source_ret, const char** cmd_ret,
              int* ac_ret, char*** av_ret)
{
    char *s = line, *word;
    int named_prefix;

    if (*s == '@') { /* IRCv3 message tags */
        s += strcspn(s, " ");
    }
    while (*s == ' ')
        s++;
    named_prefix = (*s == ':'); /* ":name CMD ..." */
    if (named_prefix)
        s++;
    word = next_word(&s);
    if (!*word)
        return 0;

    if (!named_prefix && is_unprefixed(word)) {
        current_prefix = "";
        *source_ret = "";
        *cmd_ret = word;
    }
    else {
        char* cmd = next_word(&s);
        if (!*cmd)
            return 0;
        current_prefix = named_prefix ? "" : word;
        *source_ret = named_prefix ? word : resolve_source(word);
        *cmd_ret = token_to_name(cmd);
    }
    *ac_ret = split_buf(s, av_ret, 1);
    *ac_ret = translate_params(*cmd_ret, *ac_ret, *av_ret);
    return 1;
}

/*************************************************************************/

/* SERVER: our uplink (no source) or a server behind it.
 *   av[0]=name av[1]=hops av[2]=start ts av[3]=link ts av[4]=protocol
 *   av[5]=numeric+capacity av[6]=+flags av[ac-1]=description */
static void m_server(char* source, int ac, char** av)
{
    Server* server;

    if (ac < 7) {
        log("p10: SERVER with too few parameters (%d)", ac);
        return;
    }
    do_server(source, ac, av);
    server = get_server(av[0]);
    if (!server)
        return;
    register_server(server, av[5]);
    if (!*source) {
        strbcpy(uplink_numeric, server->numeric);
        log("p10: linked to %s (numeric %s)", av[0], uplink_numeric);
    }
}

/* NICK: a new user (from a server) or a nick change (from a user).
 *   new: av[0]=nick av[1]=hops av[2]=ts av[3]=user av[4]=host
 *        [av[5]=+modes [mode args...]] av[ac-3]=ip av[ac-2]=numeric
 *        av[ac-1]=real name
 *   change: av[0]=new nick av[1]=ts */
static void m_nick(char* source, int ac, char** av)
{
    char modes[64] = "", stamp[32] = "0", *account = NULL;
    char* nav[10];
    const char *ip, *numnick;
    int modeargs, i;
    User* u;

    if (strlen(current_prefix) > 2) { /* Nick change */
        if (ac >= 1)
            do_nick(source, ac, av);
        return;
    }
    if (ac < 8) {
        log("p10: NICK with too few parameters (%d)", ac);
        return;
    }

    ip = decode_ip(av[ac - 3]);
    numnick = av[ac - 2];
    if (ac > 8 && *av[5] == '+') {
        /* Modes, followed by the parameters of those that take one:
         * +r <account>[:<id>[:<flags>]] and +z <fingerprint>. */
        const char* m;
        strbcpy(modes, av[5]);
        modeargs = 6;
        for (m = av[5]; *m; m++) {
            if ((*m == 'r' || *m == 'z') && modeargs < ac - 3) {
                if (*m == 'r')
                    account = av[modeargs];
                modeargs++;
            }
        }
    }
    if (account) {
        char* id = strchr(account, ':');
        if (id) {
            *id++ = 0;
            snprintf(stamp, sizeof(stamp), "%lu", strtoul(id, NULL, 10));
        }
    }

    nav[0] = av[0];      /* nick */
    nav[1] = av[1];      /* hops */
    nav[2] = av[2];      /* signon time */
    nav[3] = av[3];      /* user */
    nav[4] = av[4];      /* host */
    nav[5] = source;     /* server */
    nav[6] = av[ac - 1]; /* real name */
    nav[7] = stamp;      /* Services stamp (account id) */
    nav[8] = (char*)ip;  /* IP address */
    nav[9] = modes;      /* user modes */

    pending_nick = av[0];
    pending_numeric = numnick;
    i = do_nick("", lenof(nav), nav);
    pending_nick = pending_numeric = NULL;

    if (i && (u = get_user(av[0])) != NULL) {
        register_user(u, numnick);
        if (account)
            u->account = sstrdup(account);
    }
}

/* Parse the member list of a BURST ("ABAAA,ABAAB:o,ABAAC:v") and join
 * everyone.  A ":<modes>" suffix applies to that member and every
 * following one until the next suffix; a digit means op with an oplevel. */
static Channel* burst_members(const char* chan, char* list)
{
    int32 modes = 0;
    Channel* c = NULL;
    char *member, *next;

    for (member = list; member && *member; member = next) {
        char* suffix;
        User* u;
        Channel* joined;

        next = strchr(member, ',');
        if (next)
            *next++ = 0;
        suffix = strchr(member, ':');
        if (suffix) {
            *suffix++ = 0;
            modes = 0;
            for (; *suffix; suffix++) {
                if (*suffix == 'v')
                    modes |= CUMODE_v;
                else if (*suffix == 'o' || isdigit((uint8)*suffix))
                    modes |= CUMODE_o;
            }
        }
        u = p10_find_user(member);
        if (!u) {
            if (!local_nick_by_numeric(member))
                log_debug(1, "p10: BURST %s: unknown member %s", chan, member);
            continue;
        }
        joined = join_channel(u, chan, modes);
        if (joined)
            c = joined;
    }
    return c;
}

/* BURST: the state of a channel.
 *   av[0]=channel av[1]=creation ts [av[2]=+modes [mode args...]]
 *   [member list] [%bans (last parameter)] */
static void m_burst(char* source, int ac, char** av)
{
    char *mav[16], *bans = NULL;
    int mac = 0, i = 2;
    time_t ts;
    Channel* c = NULL;

    if (ac < 2)
        return;
    ts = strtotime(av[1], NULL);
    if (ac > 2 && *av[ac - 1] == '%') {
        bans = av[ac - 1] + 1;
        ac--;
    }

    /* Channel modes and their parameters. */
    mav[mac++] = av[0];
    if (i < ac && *av[i] == '+') {
        const char* m;
        mav[mac++] = av[i++];
        for (m = mav[1]; *m && i < ac; m++) {
            int params = mode_char_to_params(*m, MODE_CHANNEL);
            if (params > 0 && (params >> 8) && mac < lenof(mav))
                mav[mac++] = av[i++];
        }
    }

    /* Members. */
    for (; i < ac; i++) {
        Channel* joined = burst_members(av[0], av[i]);
        if (joined)
            c = joined;
    }

    c = c ? c : get_channel(av[0]);
    if (!c)
        return;
    if (!c->ci && (!c->creation_time || ts < c->creation_time))
        c->creation_time = ts;
    if (mac > 1)
        do_cmode(source, mac, mav);
    if (bans) {
        char *ban, *save = NULL;
        for (ban = strtok_r(bans, " ", &save); ban;
             ban = strtok_r(NULL, " ", &save)) {
            char* bav[3] = {av[0], "+b", ban};
            do_cmode(source, 3, bav);
        }
    }
}

/* CREATE: a user creates channel(s) and gets ops.
 *   av[0]=channel[,channel...] av[1]=timestamp */
static void m_create(char* source, int ac, char** av)
{
    User* u = get_user(source);
    char *chan, *save = NULL;
    time_t ts;

    if (!u || ac < 1)
        return;
    ts = ac > 1 ? strtotime(av[1], NULL) : time(NULL);
    for (chan = strtok_r(av[0], ",", &save); chan;
         chan = strtok_r(NULL, ",", &save)) {
        Channel* c = join_channel(u, chan, CUMODE_o);
        if (c && !c->ci)
            c->creation_time = ts;
    }
}

/* KILL: av[0]=target numeric, av[1]="<path> (<reason>)".  One of our
 * own clients being killed is simply brought back. */
static void m_kill(char* source, int ac, char** av)
{
    const char* nick;
    char* kav[2];

    if (ac < 2)
        return;
    if ((nick = local_nick_by_numeric(av[0])) != NULL) {
        char savenick[NICKMAX];
        strbcpy(savenick, nick);
        local_free(savenick);
        if (!readonly)
            introduce_user(savenick);
        return;
    }
    kav[0] = numeric_to_nick(av[0]);
    kav[1] = av[1];
    do_kill(source, 2, kav);
}

/* PART: av[0]=channel[,channel...] [av[1]=reason].  After a KICK, ircu
 * has the kicked user's server confirm it with a PART (the zombie
 * handshake of P10); by then the user is already off the channel here,
 * so channels the user is not on are skipped quietly. */
static void m_part(char* source, int ac, char** av)
{
    char chans[BUFSIZE], *chan, *save = NULL, *pav[2];
    User* u = get_user(source);

    if (!u || ac < 1)
        return;
    *chans = 0;
    for (chan = strtok_r(av[0], ",", &save); chan;
         chan = strtok_r(NULL, ",", &save)) {
        if (!is_on_chan(u, chan)) {
            log_debug(1, "p10: %s parts %s, already gone (kick ack)", u->nick,
                      chan);
            continue;
        }
        if (*chans)
            strscpy(chans + strlen(chans), ",", sizeof(chans) - strlen(chans));
        strscpy(chans + strlen(chans), chan, sizeof(chans) - strlen(chans));
    }
    if (!*chans)
        return;
    pav[0] = chans;
    pav[1] = ac > 1 ? av[1] : "";
    do_part(source, 2, pav);
}

/* OPMODE: like a server MODE, from an operator. */
static void m_opmode(char* source, int ac, char** av)
{
    if (ac < 2)
        return;
    if (*av[0] == '#' || *av[0] == '&') {
        ac = translate_chanmode_args(ac, av);
        do_cmode(source, ac, av);
    }
    else {
        av[0] = numeric_to_nick(av[0]);
        do_umode(source, 2, av);
    }
}

/* MODE: channel modes arrive with nicks already translated; user modes
 * may carry parameters (+r <account>) that do_umode() does not expect. */
static void m_mode(char* source, int ac, char** av)
{
    if (ac < 2)
        return;
    if (*av[0] == '#' || *av[0] == '&') {
        do_cmode(source, ac, av);
    }
    else {
        if (!get_user(av[0]))
            av[0] = numeric_to_nick(av[0]);
        do_umode(source, 2, av);
    }
}

/* CLEARMODE: av[0]=channel, av[1]=mode letters to clear. */
static void m_clearmode(char* source, int ac, char** av)
{
    Channel* c;
    const char* m;
    struct c_userlist* cu;
    char modebuf[3] = "-?";
    char* mav[3];
    int i;

    if (ac < 2 || !(c = get_channel(av[0])))
        return;
    mav[0] = c->name;
    mav[1] = modebuf;
    for (m = av[1]; *m; m++) {
        modebuf[1] = *m;
        if (mode_char_to_flag(*m, MODE_CHANUSER)) {
            int32 flag = mode_char_to_flag(*m, MODE_CHANUSER);
            LIST_FOREACH(cu, c->users)
            {
                if (cu->mode & flag) {
                    mav[2] = cu->user->nick;
                    do_cmode(source, 3, mav);
                }
            }
        }
        else if (*m == 'b') {
            for (i = c->bans_count - 1; i >= 0; i--) {
                mav[2] = c->bans[i];
                do_cmode(source, 3, mav);
            }
        }
        else if (*m == 'k') {
            if (c->key) {
                mav[2] = c->key;
                do_cmode(source, 3, mav);
            }
        }
        else if (mode_char_to_flag(*m, MODE_CHANNEL)) {
            do_cmode(source, 2, mav);
        }
    }
}

/* ACCOUNT: av[0]=user numeric av[1]=account [av[2]=id [av[3]=flags]] */
static void m_account(char* source, int ac, char** av)
{
    User* u;

    if (ac < 2 || !(u = p10_find_user(av[0])))
        return;
    free(u->account);
    u->account = sstrdup(av[1]);
}

/* PING: answer for the uplink, whatever form it takes. */
static void m_ping(char* source, int ac, char** av)
{
    if (ac < 1)
        return;
    if (*current_prefix)
        send_cmd(NULL, "PONG %s :%s", me_numeric, av[0]);
    else
        send_cmd(NULL, "PONG :%s", av[0]);
}

/* END_OF_BURST: once our uplink has sent its burst, send ours (we have no
 * state to burst beyond our clients, already introduced) and acknowledge. */
static void m_end_of_burst(char* source, int ac, char** av)
{
    if (strcmp(current_prefix, uplink_numeric) != 0)
        return;
    if (!burst_done) {
        send_cmd(NULL, "END_OF_BURST");
        burst_done = 1;
    }
    send_cmd(NULL, "EOB_ACK");
    log("p10: burst from %s complete", source);
}

/* MODULE: ircu2 synchronises the module set of every server.  Services
 * run no ircu modules and never announce a set (a service is exempt), but
 * a network-wide load/unload waits for every server to answer PREPARE.
 *   av[0]=PREPARE av[1]=* av[2]=transaction id ... */
static void m_module(char* source, int ac, char** av)
{
    if (ac >= 3 && stricmp(av[0], "PREPARE") == 0)
        send_cmd(NULL, "MODULE READY %s %s", current_prefix, av[2]);
}

static void m_error(char* source, int ac, char** av)
{
    log("p10: uplink sent ERROR: %s", ac > 0 ? av[0] : "(no reason)");
}

/*************************************************************************/

/* P10 messages and P10 variants of RFC 1459 ones.  Registered after the
 * base messages (messages.c), so these entries take precedence. */
static Message p10_messages[] = {{"ACCOUNT", m_account},
                                 {"BURST", m_burst},
                                 {"CLEARMODE", m_clearmode},
                                 {"CREATE", m_create},
                                 {"END_OF_BURST", m_end_of_burst},
                                 {"ERROR", m_error},
                                 {"KILL", m_kill},
                                 {"MODE", m_mode},
                                 {"MODULE", m_module},
                                 {"NICK", m_nick},
                                 {"OPMODE", m_opmode},
                                 {"PART", m_part},
                                 {"PING", m_ping},
                                 {"SERVER", m_server},

                                 /* Nothing for Services to do. */
                                 {"ADMIN", NULL},
                                 {"ASLL", NULL},
                                 {"AWAY", NULL},
                                 {"CNOTICE", NULL},
                                 {"CONFIG", NULL},
                                 {"CPRIVMSG", NULL},
                                 {"DESTRUCT", NULL},
                                 {"DESYNCH", NULL},
                                 {"EOB_ACK", NULL},
                                 {"GLINE", NULL},
                                 {"INVITE", NULL},
                                 {"JUPE", NULL},
                                 {"LANGUAGE", NULL},
                                 {"LINKS", NULL},
                                 {"LUSERS", NULL},
                                 {"NAMES", NULL},
                                 {"REDACT", NULL},
                                 {"RPING", NULL},
                                 {"RPONG", NULL},
                                 {"SETTIME", NULL},
                                 {"SILENCE", NULL},
                                 {"SLINE", NULL},
                                 {"TAGMSG", NULL},
                                 {"TRACE", NULL},
                                 {"UPING", NULL},
                                 {"WALLCHOPS", NULL},
                                 {"WALLUSERS", NULL},
                                 {"WALLVOICES", NULL},
                                 {"WHO", NULL},
                                 {"WHOWAS", NULL},
                                 {"XQUERY", NULL},
                                 {"XREPLY", NULL},
                                 {NULL}};

/*************************************************************************/
/***************************** 4. Outbound *******************************/
/*************************************************************************/

/* An outbound message being rewritten. */
typedef struct {
    const char* prefix; /* Numeric of the source */
    const char* token;  /* P10 token */
    int ac;
    char* av[MAXPARAMS];
    int trailing;               /* Last parameter was a ":" parameter */
    char prefixbuf[6];          /* Storage for the source numeric */
    char numbufs[MAXPARAMS][6]; /* Storage for rewritten numerics */
} OutMsg;

/* Replace parameter `i' (a nick) by the user's numeric.  Return zero if
 * the nick is unknown. */
static int param_to_numeric(OutMsg* m, int i)
{
    const char* num;

    if (i >= m->ac)
        return 0;
    if (!(num = p10_user_numeric(m->av[i])))
        return 0;
    strbcpy(m->numbufs[i], num);
    m->av[i] = m->numbufs[i];
    return 1;
}

static int is_channel(const char* s)
{
    return *s == '#' || *s == '&' || *s == '+' || *s == '!';
}

/* PRIVMSG/NOTICE: a user target is addressed by numeric. */
static int out_message(OutMsg* m)
{
    const char* target = m->ac ? m->av[0] : "";

    if (is_channel(target) || *target == '$' || strchr(target, '@'))
        return 1;
    return param_to_numeric(m, 0);
}

/* KICK/MODE for a channel come from our server: our clients are not
 * channel operators, and ircu bounces those from a non-op user. */
static int out_kick(OutMsg* m)
{
    m->prefix = me_numeric;
    return param_to_numeric(m, 1);
}

static int out_mode(OutMsg* m)
{
    const char* s;
    int add = 1, arg = 2;

    m->prefix = me_numeric;
    if (m->ac < 2 || !is_channel(m->av[0]))
        return 1; /* User mode: the target is a nick */
    for (s = m->av[1]; *s; s++) {
        int params;
        if (*s == '+' || *s == '-') {
            add = (*s == '+');
            continue;
        }
        if (mode_char_to_flag(*s, MODE_CHANUSER)) {
            param_to_numeric(m, arg++);
            continue;
        }
        params = mode_char_to_params(*s, MODE_CHANNEL);
        if (params > 0)
            arg += (params >> (add ? 8 : 0)) & 0xFF;
    }
    return 1;
}

/* KILL <nick> :<reason>  ->  D <numeric> :<server>!<killer> (<reason>) */
static int out_kill(OutMsg* m)
{
    static char path[BUFSIZE];
    const char* reason = m->ac > 1 ? m->av[1] : "";

    if (!param_to_numeric(m, 0))
        return 0;
    if (strchr(reason, '!') && strchr(reason, '!') < strchr(reason, ' '))
        return 1; /* Already a kill path */
    snprintf(path, sizeof(path), "%s!%s", ServerName, reason);
    m->av[1] = path;
    m->ac = 2;
    m->trailing = 1;
    return 1;
}

/* JOIN of one of our clients: CREATE if the channel does not exist. */
static int out_join(OutMsg* m)
{
    static char tsbuf[32];
    Channel* c;

    if (m->ac < 1)
        return 0;
    c = get_channel(m->av[0]);
    if (!c) {
        m->token = "C";
        snprintf(tsbuf, sizeof(tsbuf), "%ld", (long)time(NULL));
    }
    else {
        snprintf(tsbuf, sizeof(tsbuf), "%ld", (long)c->creation_time);
    }
    m->av[1] = tsbuf;
    m->ac = 2;
    m->trailing = 0;
    return 1;
}

/* SQUIT <server> :<reason>  ->  SQ <server> 0 :<reason> */
static int out_squit(OutMsg* m)
{
    if (m->ac < 1)
        return 0;
    m->av[2] = m->ac > 1 ? m->av[1] : "";
    m->av[1] = "0";
    m->ac = 3;
    m->trailing = 1;
    return 1;
}

/* NICK of one of our clients changing nick: N <newnick> <ts> */
static int out_nick(OutMsg* m)
{
    static char tsbuf[32];

    if (m->ac < 1)
        return 0;
    snprintf(tsbuf, sizeof(tsbuf), "%ld", (long)time(NULL));
    m->av[1] = tsbuf;
    m->ac = 2;
    m->trailing = 0;
    return 1;
}

/* Numeric replies: the target is a numeric, the source our server. */
static int out_numeric(OutMsg* m)
{
    m->prefix = me_numeric;
    return param_to_numeric(m, 0);
}

/* SVSMODE <nick> :+r  ->  AC <numeric> <account> <services stamp>
 *
 * ircu has no SVSMODE: the one thing NickServ needs from it, marking a
 * user identified, is logging the user in to an account.  The account is
 * the nick identified for, and its id is the user's Services stamp, so a
 * restart of Services recognizes the user from the NICK burst. */
static int out_svsmode(OutMsg* m)
{
    static char idbuf[16];
    User* u;

    if (m->ac < 2 || *m->av[1] != '+' || !strchr(m->av[1], 'r'))
        return 0; /* ircu cannot take an account away */
    if (!(u = get_user(m->av[0])) || u->account)
        return 0;
    if (strlen(u->nick) > P10_ACCOUNTLEN) {
        log_debug(1, "p10: nick %s too long for an account name", u->nick);
        return 0;
    }
    u->account = sstrdup(u->nick);
    snprintf(idbuf, sizeof(idbuf), "%lu", (unsigned long)u->servicestamp);
    param_to_numeric(m, 0);
    m->prefix = me_numeric;
    m->token = "AC";
    m->av[1] = u->account;
    m->av[2] = idbuf;
    m->ac = 3;
    m->trailing = 0;
    return 1;
}

typedef struct {
    const char* name;
    int (*rewrite)(OutMsg* m); /* NULL: parameters are sent as they are */
} OutRule;

static const OutRule out_rules[] = {{"PRIVMSG", out_message},
                                    {"NOTICE", out_message},
                                    {"KICK", out_kick},
                                    {"MODE", out_mode},
                                    {"OPMODE", out_mode},
                                    {"KILL", out_kill},
                                    {"JOIN", out_join},
                                    {"SQUIT", out_squit},
                                    {"NICK", out_nick},
                                    {"SVSMODE", out_svsmode},
                                    {NULL}};

/* Split `line' into parameters the way IRC does; remember whether the
 * last one was a ":" parameter. */
static int split_params(char* line, char** av, int max, int* trailing)
{
    int ac = 0;

    *trailing = 0;
    while (*line && ac < max) {
        if (*line == ':') {
            av[ac++] = line + 1;
            *trailing = 1;
            break;
        }
        av[ac++] = line;
        line += strcspn(line, " ");
        if (*line)
            *line++ = 0;
        while (*line == ' ')
            line++;
    }
    return ac;
}

/* Write a raw line to the uplink. */
static void p10_write(const char* line)
{
    if (!servsock)
        return;
    sockprintf(servsock, "%s\r\n", line);
    log_debug(1, "[DEBUG] Sent: %s", line);
    last_send = time(NULL);
}

void p10_send(const char* source, const char* line)
{
    char work[BUFSIZE], out[BUFSIZE * 2];
    char *cmd, *rest;
    const OutRule* rule;
    OutMsg m;
    int i, len;

    strbcpy(work, line);
    cmd = work;
    rest = work + strcspn(work, " ");
    if (*rest)
        *rest++ = 0;

    /* The handshake goes out as it is. */
    if (!strcmp(cmd, "PASS") || !strcmp(cmd, "SERVER") || !*me_numeric) {
        p10_write(line);
        return;
    }

    memset(&m, 0, sizeof(m));
    m.ac = split_params(rest, m.av, MAXPARAMS, &m.trailing);
    if (!source || !*source || irc_stricmp(source, ServerName) == 0 ||
        strchr(source, '.')) {
        m.prefix = me_numeric;
    }
    else if ((m.prefix = p10_user_numeric(source)) != NULL) {
        strbcpy(m.prefixbuf, m.prefix); /* p10_user_numeric() is static */
        m.prefix = m.prefixbuf;
    }
    else {
        log_debug(1, "p10: sending as unknown source %s", source);
        m.prefix = me_numeric;
    }

    if (strlen(cmd) == 3 && isdigit((uint8)cmd[0])) {
        m.token = cmd;
        if (!out_numeric(&m))
            return;
    }
    else {
        m.token = name_to_token(cmd);
        for (rule = out_rules; rule->name; rule++) {
            if (stricmp(rule->name, cmd) == 0)
                break;
        }
        if (!m.token && !rule->name) {
            log("p10: no P10 equivalent for `%s', not sent", cmd);
            return;
        }
        if (rule->name && rule->rewrite && !rule->rewrite(&m)) {
            log_debug(2, "p10: dropped `%s' (%s)", line,
                      source ? source : ServerName);
            return;
        }
    }

    len = snprintf(out, sizeof(out), "%s %s", m.prefix, m.token);
    for (i = 0; i < m.ac && len < (int)sizeof(out); i++) {
        int last = (i == m.ac - 1);
        int colon = last && (m.trailing || !*m.av[i] || strchr(m.av[i], ' ') ||
                             *m.av[i] == ':');
        len += snprintf(out + len, sizeof(out) - len, " %s%s",
                        colon ? ":" : "", m.av[i]);
    }
    p10_write(out);

    /* A client of ours that quits frees its numeric. */
    if (!stricmp(cmd, "QUIT") && source)
        local_free(source);
    else if (!stricmp(cmd, "NICK") && source && m.ac > 0) {
        int slot = local_find(source);
        if (slot >= 0)
            strbcpy(local_nicks[slot], m.av[0]);
    }
}

/*************************************************************************/
/*************************** 5. Protocol API *****************************/
/*************************************************************************/

void send_nick(const char* nick, const char* user, const char* host,
               const char* server, const char* name, const char* modes)
{
    char line[BUFSIZE];
    const char* num = local_numeric(local_alloc(nick));

    snprintf(line, sizeof(line), "%s N %s 1 %ld %s %s %s%s%sAAAAAA %s :%s",
             me_numeric, nick, (long)time(NULL), user, host,
             (modes && *modes) ? "+" : "", modes ? modes : "",
             (modes && *modes) ? " " : "", num, name);
    p10_write(line);
}

void send_nickchange(const char* nick, const char* newnick)
{
    send_cmd(nick, "NICK %s", newnick);
}

void send_namechange(const char* nick, const char* newname)
{
    /* P10 cannot change a real name. */
}

void send_nickchange_remote(const char* nick, const char* newnick)
{
    /* ircu2 has no SVSNICK: PF_CHANGENICK is not set, so nothing calls
     * this. */
    log_debug(1, "p10: cannot change nick of %s to %s", nick, newnick);
}

void send_server(void)
{
    char line[BUFSIZE];
    char capacity[4];

    burst_done = 0;
    *uplink_numeric = 0;
    b64_encode(LOCAL_CAPACITY - 1, capacity, 3);
    snprintf(line, sizeof(line), "PASS :%s", RemotePassword);
    p10_write(line);
    /* +s: a services server (exempt from ircu2's module-set check);
     * +6: we understand IPv6 addresses in NICK. */
    snprintf(line, sizeof(line), "SERVER %s 1 %ld %ld J10 %s%s +s6 :%s",
             ServerName, (long)start_time, (long)time(NULL), me_numeric,
             capacity, ServerDesc);
    p10_write(line);
}

/* Jupe a server with ircu's JUPE: it is refused network-wide for a week
 * (ircu's maximum) and does not need a fake server of ours. */
void send_server_remote(const char* server, const char* reason)
{
    send_cmd(NULL, "JUPE * +%s %d %ld :%s", server, 7 * 24 * 60 * 60,
             (long)time(NULL), reason);
}

void wallops(const char* source, const char* fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    send_cmd(source ? source : ServerName, "WALLOPS :%s", buf);
}

void notice_all(const char* source, const char* fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    send_cmd(source, "NOTICE $* :%s", buf);
}

void send_channel_cmd(const char* source, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vsend_cmd(source, fmt, args);
    va_end(args);
}

/*************************************************************************/
/***************************** 6. Callbacks ******************************/
/*************************************************************************/

/* set_topic() calls this twice; the second call (setter == NULL) comes
 * after the Channel has been updated.  ircu ignores a topic older than
 * the one it has, so the topic goes out with the current time. */
static int do_set_topic(const char* source, Channel* c, const char* topic,
                        const char* setter, time_t t)
{
    if (setter)
        return 0;
    c->topic_time = t;
    send_cmd(source, "TOPIC %s 0 %ld %s :%s", c->name, (long)time(NULL),
             c->topic_setter, c->topic ? c->topic : "");
    return 1;
}

static int do_user_delete(User* u, const char* reason, int is_kill)
{
    unregister_user(u);
    return 0;
}

static int do_server_delete(Server* server, const char* reason)
{
    unregister_server(server);
    return 0;
}

/*************************************************************************/

/* G-lines: AKILLs and SZLINEs.  <lastmod> must grow with every change. */

static long gline_lastmod(void)
{
    static long last;
    long now = (long)time(NULL);

    last = (now > last) ? now : last + 1;
    return last;
}

static int do_send_akill(const char* username, const char* host,
                         time_t expires, const char* who, const char* reason)
{
    time_t now = time(NULL);
    long duration =
        (expires && expires > now) ? (long)(expires - now) : GLINE_FOREVER;

    send_cmd(NULL, "GLINE * +%s@%s %ld %ld :%s", username, host, duration,
             gline_lastmod(), reason);
    return 1;
}

static int do_cancel_akill(const char* username, const char* host)
{
    send_cmd(NULL, "GLINE * -%s@%s %ld", username, host, gline_lastmod());
    return 1;
}

static int do_send_szline(const char* mask, time_t expires, const char* who,
                          const char* reason)
{
    return do_send_akill("*", mask, expires, who, reason);
}

static int do_cancel_szline(const char* mask)
{
    return do_cancel_akill("*", mask);
}

static int do_load_module(Module* mod, const char* modname)
{
    if (strcmp(modname, "operserv/akill") == 0) {
        if (!add_callback(mod, "send_akill", do_send_akill) ||
            !add_callback(mod, "cancel_akill", do_cancel_akill))
            log("p10: unable to add AKILL callbacks");
    }
    else if (strcmp(modname, "operserv/sline") == 0) {
        if (!add_callback(mod, "send_szline", do_send_szline) ||
            !add_callback(mod, "cancel_szline", do_cancel_szline))
            log("p10: unable to add SZLINE callbacks");
    }
    return 0;
}

/*************************************************************************/
/******************************* 7. Setup ********************************/
/*************************************************************************/

/* ircu2 user and channel modes, beyond the RFC 1459 ones predefined in
 * modes.c.  The flag values are only Services' own bookkeeping. */
static const struct {
    uint8 mode;
    ModeData data;
} p10_usermodes[] =
    {
        {'d', {0x00000008}},                         /* deaf */
        {'s', {0x00000010}},                         /* server notices */
        {'k', {0x00000020}},                         /* channel service */
        {'g', {0x00000040}},                         /* debug */
        {'r', {0x00000080, 0, 0, 0, MI_REGISTERED}}, /* has an account */
        {'R', {0x00000100}},                         /* only from accounts */
        {'x', {0x00000200}},                         /* hidden host */
        {'z', {0x00000400}},                         /* TLS */
        {'I', {0x00000800}},                         /* hide idle time */
        {'c', {0x00001000}},                         /* common channels */
        {'B', {0x00002000}},                         /* bot */
        {'S', {0x00004000}},                         /* service bot */
        {'f', {0x00008000}},                         /* frozen */
        {'O', {0x00010000}},                         /* local operator */
},
  p10_chanmodes[] = {
      {'r', {0x00000100, 0, 0, 0, MI_REGNICKS_ONLY}}, /* accounts only */
      {'R', {0x00000200, 0, 0, 0, MI_REGISTERED}},    /* registered */
      {'D', {0x00000400}},                            /* delayed joins */
      {'C', {0x00000800}},                            /* no CTCP */
      {'c', {0x00001000}},                            /* no colours */
      {'M', {0x00002000}},       /* moderate unregistered */
      {'u', {0x00004000}},       /* no part/quit messages */
      {'Z', {0x00008000}},       /* TLS only */
      {'A', {0x00010000, 1, 1}}, /* admin password */
      {'U', {0x00020000, 1, 1}}, /* user password */
};

static void init_modes(void)
{
    int i;

    for (i = 0; i < lenof(p10_usermodes); i++)
        usermodes[p10_usermodes[i].mode] = p10_usermodes[i].data;
    for (i = 0; i < lenof(p10_chanmodes); i++)
        chanmodes[p10_chanmodes[i].mode] = p10_chanmodes[i].data;
    mode_setup();
}

int p10_init(void)
{
    if (ServerNumeric < 0 || ServerNumeric > 4095) {
        log("p10: ServerNumeric must be set to a value from 0 to 4095");
        return 0;
    }
    b64_init();
    b64_encode(ServerNumeric, me_numeric, 2);

    protocol_name = "P10";
    protocol_version = "ircu2";
    protocol_features = PF_NOQUIT | PF_SZLINE;
    protocol_nickmax = P10_NICKLEN;
    /* +k: may INVITE and cannot be kicked or deopped; +S: a service of the
     * network, the one thing a frozen (+f) user may talk to. */
    pseudoclient_modes = "kS";
    enforcer_modes = "i";
    pseudoclient_oper = 1;

    if (!register_messages(p10_messages)) {
        log("p10: unable to register messages");
        return 0;
    }
    if (!add_callback(NULL, "load module", do_load_module) ||
        !add_callback(NULL, "set topic", do_set_topic) ||
        !add_callback(NULL, "user delete", do_user_delete) ||
        !add_callback(NULL, "server delete", do_server_delete)) {
        log("p10: unable to add callbacks");
        return 0;
    }
    init_modes();
    mapstring(OPER_BOUNCY_MODES, OPER_BOUNCY_MODES_U_LINE);
    return 1;
}

void p10_cleanup(void)
{
    int i;

    remove_callback(NULL, "server delete", do_server_delete);
    remove_callback(NULL, "user delete", do_user_delete);
    remove_callback(NULL, "set topic", do_set_topic);
    remove_callback(NULL, "load module", do_load_module);
    unregister_messages(p10_messages);
    for (i = 0; i < lenof(numservers); i++) {
        if (numservers[i]) {
            free(numservers[i]->clients);
            free(numservers[i]);
            numservers[i] = NULL;
        }
    }
}
