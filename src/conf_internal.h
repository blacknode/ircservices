/* Private interface between the configuration lexer (conf_lexer.l), the
 * parser (conf_parser.y) and the tree they build (conffile.c).
 *
 * This header is included by the generated scanner and parser, so it
 * deliberately pulls in nothing from Services: the generated code only
 * handles opaque node pointers and hands every allocation to the helpers
 * below, which live in conffile.c.  That keeps all memory owned by the
 * tree going through one allocator (see MEMCHECKS in memory.h).
 *
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef CONF_INTERNAL_H
#define CONF_INTERNAL_H

#include <stddef.h>

/*************************************************************************/

/* Maximum nesting of `include' statements. */
#define CONF_MAX_INCLUDE_DEPTH 16

struct ConfNode_;
struct ConfTree_;

/* A list of sibling nodes under construction. */
typedef struct {
    struct ConfNode_ *head, *tail;
} ConfList;

/* The values of an entry under construction (opaque). */
typedef struct ConfValues_ ConfValues;

/* State of one parse (one file; `include' starts a nested parse). */
typedef struct {
    struct ConfTree_* tree; /* Tree the nodes will belong to */
    const char* filename;   /* This file's name, interned in `tree' */
    int depth;              /* Include nesting level (0 = main file) */
    int errors;             /* Errors seen so far in this file */
    ConfList result;        /* Top-level statements of this file */
} ConfParseCtx;

/*************************************************************************/

/* Tree construction (conffile.c).  Every function taking ownership of a
 * string, value list or node list frees it on failure; on out-of-memory
 * the error is reported and counted in `ctx', and NULL is returned. */

extern char* conf_strndup(ConfParseCtx* ctx, const char* s, size_t len);
extern void conf_str_free(char* s);

extern ConfValues* conf_values_add(ConfParseCtx* ctx, ConfValues* values,
                                   char* value);
extern void conf_values_free(ConfValues* values);

extern struct ConfNode_* conf_new_entry(ConfParseCtx* ctx, char* name,
                                        ConfValues* values, int line);
extern struct ConfNode_* conf_new_block(ConfParseCtx* ctx, char* name,
                                        char* label, ConfList children,
                                        int line);
extern void conf_node_free(struct ConfNode_* node);

extern void conf_list_init(ConfList* list);
extern void conf_list_append(ConfList* list, struct ConfNode_* node);
extern void conf_list_concat(ConfList* list, ConfList other);
extern void conf_list_free(ConfList list);

extern const char* conf_intern_filename(struct ConfTree_* tree,
                                        const char* filename);

/* Report an error in the file being parsed and count it. */
extern void conf_parse_error(ConfParseCtx* ctx, int line, const char* fmt,
                             ...) __attribute__((format(printf, 3, 4)));

/*************************************************************************/

/* Parsing (conf_lexer.l).  Parse `filename' into `*out', a list of
 * top-level nodes belonging to `tree'.  `depth' is the include nesting
 * level; `from_file'/`from_line' locate the `include' statement that
 * asked for the file (NULL/0 for the main file) for error messages.
 * Returns nonzero on success; on failure every error has been reported
 * and `*out' is left empty. */
extern int conf_parse_file(struct ConfTree_* tree, const char* filename,
                           int depth, ConfList* out, const char* from_file,
                           int from_line);

/*************************************************************************/

#endif /* CONF_INTERNAL_H */
