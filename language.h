/* Include file for multi-language support.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef LANGUAGE_H
#define LANGUAGE_H

/*************************************************************************/

/* Languages.  Never insert anything in (or delete anything from) the
 * middle of this list, or everybody will start getting the wrong language!
 * If you want to change the order the languages are displayed in for
 * NickServ HELP SET LANGUAGE, do it in language.c.
 */
#define LANG_EN_US      0       /* United States English */
#define LANG_UNUSED1    1       /* Unused; was Japanese (JIS encoding) */
#define LANG_JA_EUC     2       /* Japanese (EUC encoding) */
#define LANG_JA_SJIS    3       /* Japanese (SJIS encoding) */
#define LANG_ES         4       /* Spanish */
#define LANG_PT         5       /* Portugese */
#define LANG_FR         6       /* French */
#define LANG_TR         7       /* Turkish */
#define LANG_IT         8       /* Italian */
#define LANG_DE         9       /* German */
#define LANG_NL         10      /* Dutch */
#define LANG_HU         11      /* Hungarian */
#define LANG_RU         12      /* Russian */

#define NUM_LANGS       13      /* Number of languages */
#define LANG_DEFAULT    -1      /* "Use the default" setting */

/* Sanity-check on default language value */
#if DEF_LANGUAGE < 0 || DEF_LANGUAGE >= NUM_LANGS
# error Invalid value for DEF_LANGUAGE: must be >= 0 and < NUM_LANGS
#endif

/*************************************************************************/

/* Flags for maketime() `flags' parameter. */

#define MT_DUALUNIT     0x0001  /* Allow two units (e.g. X hours Y mins) */
#define MT_SECONDS      0x0002  /* Allow seconds (default minutes only) */

/*************************************************************************/

/* External symbol declarations (see language.c for documentation). */

extern int langlist[NUM_LANGS+1];

extern int lang_init(void);
extern void lang_cleanup(void);
extern int load_ext_lang(const char *filename);
extern void reset_ext_lang(void);

extern int lookup_language(const char *name);
extern int have_language(int language);
extern int lookup_string(const char *name);
extern const char *getstring(const NickGroupInfo *ngi, int index);
extern const char *getstring_lang(int language, int index);
extern int setstring(int language, int index, const char *text);
extern int mapstring(int old, int new);
extern int addstring(const char *name);

extern int strftime_lang(char *buf, int size, const NickGroupInfo *ngi,
                         int format, time_t time);
extern char *maketime(const NickGroupInfo *ngi, time_t time, int flags);
extern void expires_in_lang(char *buf, int size, const NickGroupInfo *ngi,
                            time_t seconds);

extern void syntax_error(const char *service, const User *u,
                         const char *command, int msgnum);


/*************************************************************************/

/* Definitions of language string constants. */
#include "langstrs.h"


/*************************************************************************/

#endif  /* LANGUAGE_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
