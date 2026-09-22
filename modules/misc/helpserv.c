/* HelpServ functions.
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
#include "language.h"
#include <sys/stat.h>

/*************************************************************************/

static Module *module_nickserv;

static char *s_HelpServ;
static char *desc_HelpServ;
static char *HelpDir;

/*************************************************************************/
/*************************** HelpServ routine ****************************/
/*************************************************************************/

/* Main HelpServ routine. */

static void helpserv(const char *source, char *topic)
{
    FILE *f;
    struct stat st;
    char buf[PATH_MAX+1], *ptr;
    const char *s;
    char *old_topic;    /* an unclobbered (by strtok) copy */
    User *u;

    if (strnicmp(topic, "\1PING ", 6) == 0) {
        s = strtok(topic, " ");
        if (!(s = strtok_remaining()))
            s = "\1";
        notice(s_HelpServ, source, "\1PING %s", s);
        return;
    }

    old_topic = sstrdup(topic);
    u = get_user(source);

    /* As we copy path parts, (1) lowercase everything and (2) make sure
     * we don't let any special characters through -- this includes '.'
     * (which could get parent dir) or '/' (which couldn't _really_ do
     * anything nasty if we keep '.' out, but better to be on the safe
     * side).  Special characters turn into '_'.
     */
    strbcpy(buf, HelpDir);
    ptr = buf + strlen(buf);
    for (s = strtok(topic, " "); s && ptr-buf < sizeof(buf)-1;
                                                s = strtok(NULL, " ")) {
        *ptr++ = '/';
        while (*s && ptr-buf < sizeof(buf)-1) {
            if (*s == '.' || *s == '/')
                *ptr++ = '_';
            else
                *ptr++ = tolower(*s);
            s++;
        }
        *ptr = 0;
    }

    /* If we end up at a directory, go for an "index" file/dir if
     * possible.
     */
    while (ptr-buf < sizeof(buf)-6
                && stat(buf, &st) == 0 && S_ISDIR(st.st_mode)) {
        strcpy(ptr, "/index");
        ptr += strlen(ptr);
    }

    /* Send the file, if it exists.
     */
    if (!(f = fopen(buf, "r"))) {
        module_log_perror_debug(1, "Cannot open help file %s", buf);
        if (u) {
            notice_lang(s_HelpServ, u, NO_HELP_AVAILABLE, old_topic);
        } else {
            notice(s_HelpServ, source,
                        "Sorry, no help available for \2%s\2.", old_topic);
        }
    } else {
        while (fgets(buf, sizeof(buf), f)) {
            s = strtok(buf, "\n");
            /* Use this construction to prevent any %'s in the text from
             * doing weird stuff to the output.  Also replace blank lines by
             * spaces (see send.c/notice_list() for an explanation of why).
             */
            notice(s_HelpServ, source, "%s", s ? s : " ");
        }
        fclose(f);
    }
    free(old_topic);
}

/*************************************************************************/
/**************************** Event callbacks ****************************/
/*************************************************************************/

/* Handler for introducing nicks. */

static int do_introduce(const char *nick)
{
    if (!nick || irc_stricmp(nick, s_HelpServ) == 0) {
        send_pseudo_nick(s_HelpServ, desc_HelpServ, 0);
        if (nick)
            return 1;
    }
    return 0;
}

/*************************************************************************/

/* Handler for PRIVMSGs. */

static int do_privmsg(const char *source, const char *target, char *buf)
{
    if (irc_stricmp(target, s_HelpServ) == 0) {
        helpserv(source, buf);
        return 1;
    }
    return 0;
}

/*************************************************************************/

/* Handler for WHOIS's. */

static int do_whois(const char *source, char *who, char *extra)
{
    if (irc_stricmp(who, s_HelpServ) == 0) {
        send_cmd(ServerName, "311 %s %s %s %s * :%s", source, who,
                 ServiceUser, ServiceHost, desc_HelpServ);
        send_cmd(ServerName, "312 %s %s %s :%s", source, who,
                 ServerName, ServerDesc);
        send_cmd(ServerName, "318 %s %s End of /WHOIS response.", source, who);
        return 1;
    } else {
        return 0;
    }
}

/*************************************************************************/

/* Callback for NickServ REGISTER/LINK check; we disallow
 * registration/linking of the HelpServ pseudoclient nickname.
 */

static int do_reglink_check(const User *u, const char *nick,
                            const char *pass, const char *email)
{
    return irc_stricmp(nick, s_HelpServ) == 0;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "HelpDir",          { { CD_STRING, CF_DIRREQ, &HelpDir } } },
    { "HelpServName",     { { CD_STRING, CF_DIRREQ, &s_HelpServ },
                            { CD_STRING, 0, &desc_HelpServ } } },
    { NULL }
};

/*************************************************************************/

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "nickserv/main") == 0) {
        module_nickserv = mod;
        if (!add_callback(mod, "REGISTER/LINK check", do_reglink_check))
            module_log("Unable to register NickServ REGISTER/LINK check"
                       " callback");
    }

    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_nickserv) {
        remove_callback(module_nickserv, "REGISTER/LINK check",
                        do_reglink_check);
        module_nickserv = NULL;
    }
    return 0;
}

/*************************************************************************/

static int do_reconfigure(int after_configure)
{
    static char old_s_HelpServ[NICKMAX];
    static char *old_desc_HelpServ = NULL;

    if (!after_configure) {
        /* Before reconfiguration: save old values. */
        free(old_desc_HelpServ);
        strbcpy(old_s_HelpServ, s_HelpServ);
        old_desc_HelpServ = strdup(desc_HelpServ);
    } else {
        /* After reconfiguration: handle value changes. */
        if (strcmp(old_s_HelpServ, s_HelpServ) != 0)
            send_nickchange(old_s_HelpServ, s_HelpServ);
        if (!old_desc_HelpServ || strcmp(old_desc_HelpServ,desc_HelpServ) != 0)
            send_namechange(s_HelpServ, desc_HelpServ);
        free(old_desc_HelpServ);
        old_desc_HelpServ = NULL;
    }  /* if (!after_configure) */
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "reconfigure", do_reconfigure)
     || !add_callback(NULL, "introduce_user", do_introduce)
     || !add_callback(NULL, "m_privmsg", do_privmsg)
     || !add_callback(NULL, "m_whois", do_whois)
    ) {
        module_log("Unable to register callbacks");
        exit_module(0);
        return 0;
    }

    Module *tmpmod = find_module("nickserv/main");
    if (tmpmod)
        do_load_module(tmpmod, "nickserv/main");

    if (linked)
        do_introduce(NULL);

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (linked)
        send_cmd(s_HelpServ, "QUIT :");

    if (module_nickserv)
        do_unload_module(module_nickserv);

    remove_callback(NULL, "m_whois", do_whois);
    remove_callback(NULL, "m_privmsg", do_privmsg);
    remove_callback(NULL, "introduce_user", do_introduce);
    remove_callback(NULL, "reconfigure", do_reconfigure);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

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
