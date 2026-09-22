/* DevNull module.
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

/*************************************************************************/

static Module *module_nickserv;

char *s_DevNull;
char *desc_DevNull;

/*************************************************************************/
/**************************** Event callbacks ****************************/
/*************************************************************************/

/* Handler for introducing nicks. */

static int do_introduce(const char *nick)
{
    if (!nick || irc_stricmp(nick, s_DevNull) == 0) {
        send_pseudo_nick(s_DevNull, desc_DevNull, 0);
        if (nick)
            return 1;
    }
    return 0;
}

/*************************************************************************/

/* Handler for PRIVMSGs. */

static int do_privmsg(const char *source, const char *target, char *buf)
{
    return irc_stricmp(target, s_DevNull) == 0;
}

/*************************************************************************/

/* Handler for WHOIS's. */

static int do_whois(const char *source, char *who, char *extra)
{
    if (irc_stricmp(who, s_DevNull) == 0) {
        send_cmd(ServerName, "311 %s %s %s %s * :%s", source, who,
                 ServiceUser, ServiceHost, desc_DevNull);
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
 * registration/linking of the DevNull pseudoclient nickname.
 */

static int do_reglink_check(const User *u, const char *nick,
                            const char *pass, const char *email)
{
    return irc_stricmp(nick, s_DevNull) == 0;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "DevNullName",      { { CD_STRING, CF_DIRREQ, &s_DevNull },
                            { CD_STRING, 0, &desc_DevNull } } },
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
    static char old_s_DevNull[NICKMAX];
    static char *old_desc_DevNull = NULL;

    if (!after_configure) {
        /* Before reconfiguration: save old values. */
        strbcpy(old_s_DevNull, s_DevNull);
        old_desc_DevNull = strdup(desc_DevNull);
    } else {
        /* After reconfiguration: handle value changes. */
        if (strcmp(old_s_DevNull, s_DevNull) != 0)
            send_nickchange(old_s_DevNull, s_DevNull);
        if (!old_desc_DevNull || strcmp(old_desc_DevNull, desc_DevNull) != 0)
            send_namechange(s_DevNull, desc_DevNull);
        free(old_desc_DevNull);
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

    if (linked)
        do_introduce(NULL);

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (linked)
        send_cmd(s_DevNull, "QUIT :");

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
