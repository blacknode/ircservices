/* Scanner for the configuration file.
 *
 * Tokens:
 *     { } ; = ,        punctuation
 *     "..."            a string; \" \\ \n \t are the recognised escapes
 *     include          the include keyword (case-insensitive)
 *     anything else    a word: a run of characters other than blanks,
 *                      punctuation, quotes and `#' -- names, numbers,
 *                      times ("5m"), addresses, paths, module names
 * Comments run from `#' or `//' to the end of the line, or between
 * `/' `*' and `*' `/'.  Every character belongs to one of the above, so
 * there is no "unexpected character" error: the parser reports a word
 * where it does not belong.
 *
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

%{
#include "conf_parser.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define YYSTYPE CONF_YYSTYPE
#define YYLTYPE CONF_YYLTYPE

#define YY_USER_ACTION                                                        \
    yylloc->first_line = yylloc->last_line = yylineno;

static char* unescape(ConfParseCtx* ctx, const char* text, size_t len);
%}

%option prefix="conf_yy"
%option reentrant bison-bridge bison-locations
%option extra-type="ConfParseCtx*"
%option yylineno 8bit nodefault warn
%option noyywrap nounput noinput never-interactive

%x COMMENT

WORDCHAR    [^ \t\r\n{};=,"#]

%%

[ \t\r\n]+                  /* whitespace */
"#".*                       /* comment */
"//".*                      /* comment */
"/*"                        BEGIN(COMMENT);
<COMMENT>"*/"               BEGIN(INITIAL);
<COMMENT>[^*\n]+            /* comment */
<COMMENT>"*"                /* comment */
<COMMENT>\n                 /* comment */
<COMMENT><<EOF>>            {
                                conf_parse_error(yyextra, yylineno,
                                                 "Unterminated comment");
                                BEGIN(INITIAL);
                                yyterminate();
                            }

[{};=,]                     return yytext[0];

(?i:include)                return INCLUDE;

\"([^"\\\n]|\\.)*\"         {
                                yylval->str = unescape(yyextra, yytext + 1,
                                                       yyleng - 2);
                                if (!yylval->str)
                                    return CONF_YYerror;
                                return STRING;
                            }
\"([^"\\\n]|\\.)*\\?        {
                                conf_parse_error(yyextra, yylineno,
                                                 "Unterminated string");
                                return CONF_YYerror;
                            }

{WORDCHAR}+                 {
                                yylval->str = conf_strndup(yyextra, yytext,
                                                           yyleng);
                                if (!yylval->str)
                                    return CONF_YYerror;
                                return WORD;
                            }

%%

/*************************************************************************/

/* Return a copy of the body of a quoted string with its escapes resolved,
 * or NULL (error reported) if out of memory. */

static char* unescape(ConfParseCtx* ctx, const char* text, size_t len)
{
    char* out = conf_strndup(ctx, text, len);
    char* d = out;
    size_t i;

    if (!out)
        return NULL;
    for (i = 0; i < len; i++) {
        if (text[i] == '\\' && i + 1 < len) {
            switch (text[++i]) {
                case 'n': *d++ = '\n'; break;
                case 't': *d++ = '\t'; break;
                default: *d++ = text[i]; break; /* \" \\ and the rest */
            }
        }
        else {
            *d++ = text[i];
        }
    }
    *d = 0;
    return out;
}

/*************************************************************************/

int conf_parse_file(struct ConfTree_* tree, const char* filename, int depth,
                    ConfList* out, const char* from_file, int from_line)
{
    ConfParseCtx ctx;
    yyscan_t scanner;
    FILE* f;
    int result;

    conf_list_init(out);
    memset(&ctx, 0, sizeof(ctx));
    ctx.tree = tree;
    ctx.depth = depth;
    ctx.filename = from_file ? from_file : filename;

    if (depth > CONF_MAX_INCLUDE_DEPTH) {
        conf_parse_error(&ctx, from_line,
                         "include: nesting deeper than %d levels (include"
                         " loop?)",
                         CONF_MAX_INCLUDE_DEPTH);
        return 0;
    }
    f = fopen(filename, "r");
    if (!f) {
        if (from_file)
            conf_parse_error(&ctx, from_line, "include: unable to open %s: %s",
                             filename, strerror(errno));
        else
            conf_parse_error(&ctx, 0, "Unable to open: %s", strerror(errno));
        return 0;
    }
    ctx.filename = conf_intern_filename(tree, filename);
    if (!ctx.filename) {
        fclose(f);
        return 0;
    }
    if (conf_yylex_init_extra(&ctx, &scanner) != 0) {
        conf_parse_error(&ctx, 0, "Unable to create scanner: %s",
                         strerror(errno));
        fclose(f);
        return 0;
    }
    conf_yyset_in(f, scanner);
    result = conf_yyparse(scanner, &ctx);
    conf_yylex_destroy(scanner);
    fclose(f);

    if (result != 0 || ctx.errors) {
        conf_list_free(ctx.result);
        return 0;
    }
    *out = ctx.result;
    return 1;
}
