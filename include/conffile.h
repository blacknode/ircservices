/* Configuration file: the parsed tree, and binding directive tables to it.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * The configuration file (ircservices.conf, plus whatever it includes) is
 * parsed once, before any module is loaded, into a tree of blocks and
 * entries:
 *
 *     serverinfo {                     a block named "serverinfo"
 *         name = "services.example.net";      an entry with one value
 *     };
 *     loadmodule "nickserv/main";      an entry at the top level
 *     module "nickserv/main" {         a block with a label
 *         NickServName = "NickServ", "Nickname Server";  two values
 *         NSEnableRegister;                   an entry with no value
 *     };
 *
 * There are two ways to get at it, and both work from the core and from
 * any module:
 *
 *   - Declare a table of ConfigDirective and let configure() copy the
 *     values into variables.  This is what a module's ModuleInfo.config
 *     table does: the loader binds it to the module's `module "<name>"'
 *     block(s) before the module's `init' runs, and again on REHASH.
 *
 *   - Read the tree directly with the conf_*() functions below, e.g.
 *
 *         const ConfNode *mod = conf_this_module();
 *         int32 max = conf_get_int(mod, "MaxEntries", 32);
 *
 * Block and entry names are case-insensitive; block labels (module
 * names) are case-sensitive.  Nodes belong to the tree and are freed when
 * the configuration is reloaded: do not keep pointers to them across a
 * REHASH (ModuleInfo.rehash).
 */

#ifndef CONFFILE_H
#define CONFFILE_H

#include "services.h"

/*************************************************************************/
/***************************** The tree **********************************/
/*************************************************************************/

typedef enum {
    CONF_BLOCK = 1, /* name [label] { children }; */
    CONF_ENTRY = 2, /* name [=] value, value...; */
} ConfNodeType;

typedef struct ConfNode_ ConfNode;

/* A node of the tree.  Read-only for everything outside conffile.c. */
struct ConfNode_ {
    ConfNode* next;    /* Next sibling */
    ConfNode* parent;  /* Enclosing block (the root for top-level nodes) */
    ConfNodeType type;
    char* name;        /* Block or entry name */
    const char* file;  /* Where the node was read, for error messages */
    int line;
    /* CONF_BLOCK: */
    char* label;       /* Optional label (NULL if none) */
    ConfNode* children;
    /* CONF_ENTRY: */
    char** values;     /* Values, in order; strings, quotes removed */
    int values_count;
};

/*************************************************************************/

/* Loading (core only). */

/* Parse `filename' (and everything it includes) into a new tree.  The new
 * tree becomes the one every lookup below (and configure()) sees, but the
 * previous tree is kept until conf_commit() or conf_discard().  Returns
 * nonzero on success; on failure all errors have been reported and the
 * previous tree is still in use. */
extern int conf_load(const char* filename);

/* Make the tree loaded by conf_load() permanent, freeing the previous one. */
extern void conf_commit(void);

/* Throw away the tree loaded by conf_load(), going back to the previous one. */
extern void conf_discard(void);

/* Free every tree (at exit). */
extern void conf_cleanup(void);

/* Name of the main configuration file, as passed to conf_load(). */
extern const char* conf_filename(void);

/*************************************************************************/

/* Lookups.  Every function accepts a NULL node (and then finds nothing),
 * so calls can be chained without checks: conf_find_block(NULL, ...) is
 * NULL, conf_get_int(NULL, ..., def) is `def'. */

/* The top level of the file (a block with no name).  NULL if no
 * configuration has been loaded. */
extern const ConfNode* conf_root(void);

/* First block called `name' directly inside `parent' (use conf_root() for
 * the top level) whose label is `label' (any label if `label' is NULL), or
 * NULL if there is none. */
extern const ConfNode* conf_find_block(const ConfNode* parent,
                                       const char* name, const char* label);

/* Next block after `block' matching `name' and `label' as above. */
extern const ConfNode* conf_next_block(const ConfNode* block,
                                       const char* name, const char* label);

/* First `module "<modulename>"' block at the top level, or NULL. */
extern const ConfNode* conf_module_block(const char* modulename);

/* The calling module's own block (use from inside a module). */
#define conf_this_module() conf_module_block(module_name(THIS_MODULE))

/* Last entry called `key' in `block' -- the one in effect when an entry is
 * repeated -- or NULL if there is none. */
extern const ConfNode* conf_find_entry(const ConfNode* block,
                                       const char* key);

/* Iterate over every entry called `key' in `block' (every entry if `key'
 * is NULL), in file order:
 *     for (e = conf_first_entry(b, k); e; e = conf_next_entry(e, k)) ...
 */
extern const ConfNode* conf_first_entry(const ConfNode* block,
                                        const char* key);
extern const ConfNode* conf_next_entry(const ConfNode* entry,
                                       const char* key);

/* Number of values of an entry, and value number `index' (from 0), or
 * NULL if there is no such value. */
extern int conf_value_count(const ConfNode* entry);
extern const char* conf_value(const ConfNode* entry, int index);

/* Typed access to the first value of entry `key' in `block'.  If the entry
 * is missing, or its value is not valid for the type (a warning is then
 * logged with the file and line), `def' is returned.  conf_get_bool()
 * returns 1 for an entry without value ("key;"). */
extern const char* conf_get_string(const ConfNode* block, const char* key,
                                   const char* def);
extern int32 conf_get_int(const ConfNode* block, const char* key, int32 def);
extern time_t conf_get_time(const ConfNode* block, const char* key,
                            time_t def);
extern int conf_get_bool(const ConfNode* block, const char* key, int def);

/* 1 for yes/on/true, 0 for no/off/false (any case), -1 for anything else. */
extern int conf_parse_bool(const char* s);

/*************************************************************************/
/************************** Directive tables *****************************/
/*************************************************************************/

/* A table of ConfigDirective describes the entries a block may contain and
 * where their values go.  Note that all numeric parameter types except
 * CD_TIME and CD_SET take an int32 value pointer (including CD_TIMEMSEC).
 * Each value of an entry fills the next parameter of its directive:
 *
 *     { "NSRegDelay", { { CD_TIME, 0, &NSRegDelay } } },
 *
 * binds `NSRegDelay = 30s;'.
 */

/* Information about a configuration parameter's value: */
typedef union {
    void* ptrval;
    int32 intval;
    time_t timeval;
} CDValue;

/* Information about a configuration directive: */
typedef struct {
    const char* name;
    struct {
        int type;  /* Parameter type (CD_* below) */
        int flags; /* Parameter flags (CF_* below) */
        void* ptr; /* Pointer to where to store the value */
        /* The following data is internal-use-only: */
        CDValue prev; /* Previous value (to restore when deconfigured) */
        CDValue new;  /* New value (to set if conf-file successfully read) */
    } params[CONFIG_MAXPARAMS];
    /* Also internal use only: */
    int was_seen; /* Non-zero if directive was seen this time around */
} ConfigDirective;

#define CD_NONE   0
#define CD_INT    1
#define CD_POSINT 2 /* Positive integer only */
#define CD_PORT   3 /* 1..65535 only */
#define CD_STRING 4
#define CD_TIME   5 /* Type of `ptr' is `time_t *' */
#define CD_TIMEMSEC                                                           \
    6 /* Variable is in milliseconds, parameter                               \
       *    in seconds (decimal allowed) */
#define CD_FUNC                                                               \
    7 /* `ptr' is a function to call; see                                     \
       *    init.c for examples */
#define CD_SET                                                                \
    -1 /* Flag: set the given `int' variable to 1                             \
        *    when the entry is present.  Consumes a                           \
        *    value only if it is yes/no/on/off/                               \
        *    true/false; a false value sets 0 and                             \
        *    ignores the remaining parameters */
#define CD_DEPRECATED                                                         \
    -2 /* Set for deprecated directives; causes                               \
        *    a warning to be printed */

/* Flags: */
#define CF_OPTIONAL    0x01 /* Parameter is optional (defaults to 0) */
#define CF_DIRREQ      0x02 /* Directive is required (set on first param)*/
#define CF_MULTI       0x04 /* CD_FUNC only, last parameter: called once for
                             * each remaining value ("key = a, b, c;") */
/* Internal-use-only flags: */
#define CF_SAVED       0x80 /* Original value saved in `prev' */
#define CF_WASSET      0x40 /* Parameter set this time around */
#define CF_ALLOCED     0x20 /* Current value is alloc'd by parser */
#define CF_ALLOCED_NEW 0x10 /* Value of `new' is alloc'd by parser */

/* Values for `action' parameter to configure() (can be or'ed together): */
#define CONFIGURE_READ 1 /* Read settings from the configuration tree */
#define CONFIGURE_SET  2 /* Set configuration variables to new values */

/* Values passed in `linenum' parameter to configuration directive handlers
 * (CD_FUNC parameters) when `filename' is NULL: */
#define CDFUNC_INIT     0 /* Prepare for reading data */
#define CDFUNC_SET      1 /* Copy new data to real config variables */
#define CDFUNC_DECONFIG 2 /* Delete any data in config variables */

/*************************************************************************/

/* Bind a directive table to the entries of every block called `blockname'
 * whose label is `label' (any label if NULL) at the top level of the
 * current tree; a NULL `blockname' binds the top-level entries themselves.
 *
 * `action' is a bitmask of CONFIGURE_* values:
 *     - CONFIGURE_READ: read new values from the tree
 *     - CONFIGURE_SET: copy new values to configuration variables
 * If both are given, new values are copied only if all values were read
 * successfully.  CONFIGURE_SET alone never fails.  Returns nonzero on
 * success, 0 on error (errors are logged, and printed to the terminal if
 * applicable).  Returns successfully without doing anything if
 * `directives' is NULL.  Unknown entries only produce a warning. */
extern int configure_block(const char* blockname, const char* label,
                           ConfigDirective* directives, int action);

/* Bind a module's directive table to its `module "<modulename>"' blocks
 * (the top-level entries if `modulename' is NULL). */
extern int configure(const char* modulename, ConfigDirective* directives,
                     int action);

/* Deconfigure given directive array (free any allocated storage and
 * restore original values).  A no-op if `directives' is NULL. */
extern void deconfigure(ConfigDirective* directives);

/* Print a warning or error message to the log (and the console, if open). */
extern void config_error(const char* filename, int linenum,
                         const char* message, ...);

/*************************************************************************/

#endif /* CONFFILE_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
