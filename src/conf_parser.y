/* Grammar of the configuration file.
 *
 * The grammar is generic: it knows blocks, entries and `include', not the
 * names of any directive.  Directives belong to the core and to modules,
 * which are loaded long after the file has been parsed, so they are
 * checked when a directive table is bound to the tree (see configure() in
 * conffile.c), not here.
 *
 *     config     := statement*
 *     statement  := block | entry | include
 *     block      := WORD [scalar] '{' statement* '}' [';']
 *     entry      := WORD ';'
 *                 | WORD ['='] scalar (',' scalar)* ';'
 *     include    := "include" scalar ';'
 *     scalar     := WORD | STRING
 *
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

%code requires {
#include "conf_internal.h"
}

%code provides {
int conf_yylex(CONF_YYSTYPE* lvalp, CONF_YYLTYPE* llocp, void* scanner);
int conf_yyparse(void* scanner, ConfParseCtx* ctx);
}

%code {
static void conf_yyerror(CONF_YYLTYPE* loc, void* scanner,
                         ConfParseCtx* ctx, const char* msg);

/* A list holding a single node (none if `node' is NULL, i.e. the node
 * could not be built; the error has been reported already). */
static ConfList single(struct ConfNode_* node)
{
    ConfList list;
    conf_list_init(&list);
    if (node)
        conf_list_append(&list, node);
    return list;
}
}

%define api.prefix {conf_yy}
%define api.pure full
%define parse.error verbose
%locations
%lex-param {void* scanner}
%parse-param {void* scanner} {ConfParseCtx* ctx}

%union {
    char* str;
    ConfValues* values;
    struct ConfNode_* node;
    ConfList list;
}

%token <str> WORD   "word"
%token <str> STRING "string"
%token INCLUDE      "include"

%type <str>    scalar
%type <values> values
%type <node>   block entry
%type <list>   statements statement

%destructor { conf_str_free($$); }    <str>
%destructor { conf_values_free($$); } <values>
%destructor { conf_node_free($$); }   <node>
%destructor { conf_list_free($$); }   <list>

%%

config
    : statements
        { ctx->result = $1; }
    ;

statements
    : %empty
        { conf_list_init(&$$); }
    | statements statement
        { $$ = $1; conf_list_concat(&$$, $2); }
    ;

statement
    : block
        { $$ = single($1); }
    | entry
        { $$ = single($1); }
    | INCLUDE scalar ';'
        {
            conf_list_init(&$$);
            if (!conf_parse_file(ctx->tree, $2, ctx->depth + 1, &$$,
                                 ctx->filename, @1.first_line))
                ctx->errors++;
            conf_str_free($2);
        }
    | error ';'
        { conf_list_init(&$$); yyerrok; }
    ;

block
    : WORD '{' statements '}' opt_semicolon
        { $$ = conf_new_block(ctx, $1, NULL, $3, @1.first_line); }
    | WORD scalar '{' statements '}' opt_semicolon
        { $$ = conf_new_block(ctx, $1, $2, $4, @1.first_line); }
    ;

opt_semicolon
    : %empty
    | ';'
    ;

entry
    : WORD ';'
        { $$ = conf_new_entry(ctx, $1, NULL, @1.first_line); }
    | WORD '=' values ';'
        { $$ = conf_new_entry(ctx, $1, $3, @1.first_line); }
    | WORD values ';'
        { $$ = conf_new_entry(ctx, $1, $2, @1.first_line); }
    ;

values
    : scalar
        { $$ = conf_values_add(ctx, NULL, $1); }
    | values ',' scalar
        { $$ = conf_values_add(ctx, $1, $3); }
    ;

scalar
    : WORD
    | STRING
    ;

%%

static void conf_yyerror(CONF_YYLTYPE* loc, void* scanner,
                         ConfParseCtx* ctx, const char* msg)
{
    (void)scanner;
    conf_parse_error(ctx, loc->first_line, "%s", msg);
}
