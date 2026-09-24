/* Configuration file handling: the parsed tree and directive binding.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * The file is read by the scanner and parser generated from conf_lexer.l
 * and conf_parser.y, which call back into this file to build the tree.
 * See conffile.h for the tree and the public interface.
 */

#include "conffile.h"
#include "services.h"

#include "conf_internal.h"

/*************************************************************************/

/* A whole configuration: the top-level block and the names of the files
 * it was read from (nodes point into `files'). */
typedef struct ConfTree_ {
    ConfNode root;
    char** files;
    int files_count;
} ConfTree;

/* Values of an entry under construction. */
struct ConfValues_ {
    char** values;
    int count;
};

static ConfTree* current_tree; /* Configuration in use */
static ConfTree* pending_tree; /* Loaded by conf_load(), not committed */
static char* main_filename;    /* As passed to conf_load() */

/* The tree lookups and configure() work on. */
#define active_tree() (pending_tree ? pending_tree : current_tree)

static void free_tree(ConfTree* tree);

static int read_directives(const char* blockname, const char* label,
                           ConfigDirective* directives);
static void do_all_directives(int action, ConfigDirective* directives);

/* Actions for do_all_directives(): */

#define ACTION_COPYNEW      0 /* Copy `new' parameters to config variables */
#define ACTION_RESTORESAVED 1 /* Restore saved values of config variables */

/*************************************************************************/
/****************** Tree construction (for the parser) *******************/
/*************************************************************************/

/* Plain malloc() and friends are used here (as the old line parser did):
 * the file is read before the signal handling smalloc() relies on is set
 * up. */

void conf_parse_error(ConfParseCtx* ctx, int line, const char* fmt, ...)
{
    char buf[4096];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    config_error(ctx->filename, line, "%s", buf);
    ctx->errors++;
}

/*************************************************************************/

char* conf_strndup(ConfParseCtx* ctx, const char* s, size_t len)
{
    char* copy = malloc(len + 1);

    if (!copy) {
        conf_parse_error(ctx, 0, "Out of memory");
        return NULL;
    }
    memcpy(copy, s, len);
    copy[len] = 0;
    return copy;
}

void conf_str_free(char* s)
{
    free(s);
}

/*************************************************************************/

ConfValues* conf_values_add(ConfParseCtx* ctx, ConfValues* values,
                            char* value)
{
    char** newarray;

    if (!values) {
        values = calloc(1, sizeof(*values));
        if (!values)
            goto oom;
    }
    newarray = realloc(values->values, sizeof(char*) * (values->count + 1));
    if (!newarray)
        goto oom;
    values->values = newarray;
    values->values[values->count++] = value;
    return values;

oom:
    conf_parse_error(ctx, 0, "Out of memory");
    conf_str_free(value);
    conf_values_free(values);
    return NULL;
}

void conf_values_free(ConfValues* values)
{
    int i;

    if (!values)
        return;
    for (i = 0; i < values->count; i++)
        free(values->values[i]);
    free(values->values);
    free(values);
}

/*************************************************************************/

ConfNode* conf_new_entry(ConfParseCtx* ctx, char* name, ConfValues* values,
                         int line)
{
    ConfNode* node = calloc(1, sizeof(*node));

    if (!node) {
        conf_parse_error(ctx, line, "Out of memory");
        conf_str_free(name);
        conf_values_free(values);
        return NULL;
    }
    node->type = CONF_ENTRY;
    node->name = name;
    node->file = ctx->filename;
    node->line = line;
    if (values) {
        node->values = values->values;
        node->values_count = values->count;
        free(values); /* the array now belongs to the node */
    }
    return node;
}

ConfNode* conf_new_block(ConfParseCtx* ctx, char* name, char* label,
                         ConfList children, int line)
{
    ConfNode *node = calloc(1, sizeof(*node)), *child;

    if (!node) {
        conf_parse_error(ctx, line, "Out of memory");
        conf_str_free(name);
        conf_str_free(label);
        conf_list_free(children);
        return NULL;
    }
    node->type = CONF_BLOCK;
    node->name = name;
    node->label = label;
    node->file = ctx->filename;
    node->line = line;
    node->children = children.head;
    for (child = node->children; child; child = child->next)
        child->parent = node;
    return node;
}

void conf_node_free(ConfNode* node)
{
    ConfNode *child, *next;
    int i;

    if (!node)
        return;
    for (child = node->children; child; child = next) {
        next = child->next;
        conf_node_free(child);
    }
    for (i = 0; i < node->values_count; i++)
        free(node->values[i]);
    free(node->values);
    free(node->label);
    free(node->name);
    free(node);
}

/*************************************************************************/

void conf_list_init(ConfList* list)
{
    list->head = list->tail = NULL;
}

void conf_list_append(ConfList* list, ConfNode* node)
{
    node->next = NULL;
    if (list->tail)
        list->tail->next = node;
    else
        list->head = node;
    list->tail = node;
}

void conf_list_concat(ConfList* list, ConfList other)
{
    if (!other.head)
        return;
    if (list->tail)
        list->tail->next = other.head;
    else
        list->head = other.head;
    list->tail = other.tail;
}

void conf_list_free(ConfList list)
{
    ConfNode *node, *next;

    for (node = list.head; node; node = next) {
        next = node->next;
        conf_node_free(node);
    }
}

/*************************************************************************/

const char* conf_intern_filename(ConfTree* tree, const char* filename)
{
    char **newarray, *copy;
    int i;

    for (i = 0; i < tree->files_count; i++) {
        if (strcmp(tree->files[i], filename) == 0)
            return tree->files[i];
    }
    newarray = realloc(tree->files, sizeof(char*) * (tree->files_count + 1));
    copy = strdup(filename);
    if (!newarray || !copy) {
        free(copy);
        if (newarray)
            tree->files = newarray;
        config_error(filename, 0, "Out of memory");
        return NULL;
    }
    tree->files = newarray;
    tree->files[tree->files_count++] = copy;
    return copy;
}

/*************************************************************************/
/******************************* Loading *********************************/
/*************************************************************************/

int conf_load(const char* filename)
{
    ConfTree* tree;
    ConfList list;
    ConfNode* node;
    char* name;

    tree = calloc(1, sizeof(*tree));
    name = strdup(filename);
    if (tree)
        tree->root.name = strdup("");
    if (!tree || !name || !tree->root.name) {
        if (tree)
            free(tree->root.name);
        free(tree);
        free(name);
        config_error(filename, 0, "Out of memory");
        return 0;
    }
    tree->root.type = CONF_BLOCK;

    if (!conf_parse_file(tree, filename, 0, &list, NULL, 0)) {
        free_tree(tree);
        free(name);
        return 0;
    }
    tree->root.children = list.head;
    for (node = list.head; node; node = node->next)
        node->parent = &tree->root;
    if (list.head) {
        tree->root.file = list.head->file;
        tree->root.line = 0;
    }

    free_tree(pending_tree);
    pending_tree = tree;
    free(main_filename);
    main_filename = name;
    return 1;
}

void conf_commit(void)
{
    if (!pending_tree)
        return;
    free_tree(current_tree);
    current_tree = pending_tree;
    pending_tree = NULL;
}

void conf_discard(void)
{
    free_tree(pending_tree);
    pending_tree = NULL;
}

void conf_cleanup(void)
{
    conf_discard();
    free_tree(current_tree);
    current_tree = NULL;
    free(main_filename);
    main_filename = NULL;
}

const char* conf_filename(void)
{
    return main_filename ? main_filename : IRCSERVICES_CONF;
}

static void free_tree(ConfTree* tree)
{
    ConfNode *node, *next;
    int i;

    if (!tree)
        return;
    for (node = tree->root.children; node; node = next) {
        next = node->next;
        conf_node_free(node);
    }
    free(tree->root.name);
    for (i = 0; i < tree->files_count; i++)
        free(tree->files[i]);
    free(tree->files);
    free(tree);
}

/*************************************************************************/
/******************************* Lookups *********************************/
/*************************************************************************/

const ConfNode* conf_root(void)
{
    ConfTree* tree = active_tree();
    return tree ? &tree->root : NULL;
}

/*************************************************************************/

static int block_matches(const ConfNode* node, const char* name,
                         const char* label)
{
    return node->type == CONF_BLOCK && stricmp(node->name, name) == 0 &&
           (!label || (node->label && strcmp(node->label, label) == 0));
}

const ConfNode* conf_find_block(const ConfNode* parent, const char* name,
                                const char* label)
{
    const ConfNode* node;

    if (!parent || !name)
        return NULL;
    for (node = parent->children; node; node = node->next) {
        if (block_matches(node, name, label))
            return node;
    }
    return NULL;
}

const ConfNode* conf_next_block(const ConfNode* block, const char* name,
                                const char* label)
{
    const ConfNode* node;

    if (!block || !name)
        return NULL;
    for (node = block->next; node; node = node->next) {
        if (block_matches(node, name, label))
            return node;
    }
    return NULL;
}

const ConfNode* conf_module_block(const char* modulename)
{
    return modulename ? conf_find_block(conf_root(), "module", modulename)
                      : NULL;
}

/*************************************************************************/

static int entry_matches(const ConfNode* node, const char* key)
{
    return node->type == CONF_ENTRY && (!key || stricmp(node->name, key) == 0);
}

const ConfNode* conf_first_entry(const ConfNode* block, const char* key)
{
    const ConfNode* node;

    if (!block)
        return NULL;
    for (node = block->children; node; node = node->next) {
        if (entry_matches(node, key))
            return node;
    }
    return NULL;
}

const ConfNode* conf_next_entry(const ConfNode* entry, const char* key)
{
    const ConfNode* node;

    if (!entry)
        return NULL;
    for (node = entry->next; node; node = node->next) {
        if (entry_matches(node, key))
            return node;
    }
    return NULL;
}

const ConfNode* conf_find_entry(const ConfNode* block, const char* key)
{
    const ConfNode *node, *found = NULL;

    for (node = conf_first_entry(block, key); node;
         node = conf_next_entry(node, key))
        found = node;
    return found;
}

/*************************************************************************/

int conf_value_count(const ConfNode* entry)
{
    return entry && entry->type == CONF_ENTRY ? entry->values_count : 0;
}

const char* conf_value(const ConfNode* entry, int index)
{
    if (index < 0 || index >= conf_value_count(entry))
        return NULL;
    return entry->values[index];
}

/*************************************************************************/

int conf_parse_bool(const char* s)
{
    if (!s)
        return -1;
    if (stricmp(s, "yes") == 0 || stricmp(s, "on") == 0 ||
        stricmp(s, "true") == 0)
        return 1;
    if (stricmp(s, "no") == 0 || stricmp(s, "off") == 0 ||
        stricmp(s, "false") == 0)
        return 0;
    return -1;
}

/*************************************************************************/

/* Return the first value of entry `key' in `block', or NULL (with a
 * warning if the entry is there without a value). */

static const char* first_value(const ConfNode* block, const char* key,
                               const ConfNode** entry_ret)
{
    const ConfNode* entry = conf_find_entry(block, key);

    *entry_ret = entry;
    if (!entry)
        return NULL;
    if (!entry->values_count) {
        config_error(entry->file, entry->line,
                     "Warning: `%s' requires a value, using default", key);
        return NULL;
    }
    return entry->values[0];
}

const char* conf_get_string(const ConfNode* block, const char* key,
                            const char* def)
{
    const ConfNode* entry;
    const char* value = first_value(block, key, &entry);

    return value ? value : def;
}

int32 conf_get_int(const ConfNode* block, const char* key, int32 def)
{
    const ConfNode* entry;
    const char* value = first_value(block, key, &entry);
    char* end;
    long l;

    if (!value)
        return def;
    errno = 0;
    l = strtol(value, &end, 0);
    if (!*value || *end || errno == ERANGE
#if SIZEOF_LONG > 4
        || l < -0x80000000L || l > 0x7FFFFFFFL
#endif
    ) {
        config_error(entry->file, entry->line,
                     "Warning: `%s' expects an integer, using default", key);
        return def;
    }
    return (int32)l;
}

time_t conf_get_time(const ConfNode* block, const char* key, time_t def)
{
    const ConfNode* entry;
    const char* value = first_value(block, key, &entry);
    int t;

    if (!value)
        return def;
    t = dotime(value);
    if (t < 0) {
        config_error(entry->file, entry->line,
                     "Warning: `%s' expects a time value, using default",
                     key);
        return def;
    }
    return (time_t)t;
}

int conf_get_bool(const ConfNode* block, const char* key, int def)
{
    const ConfNode* entry = conf_find_entry(block, key);
    int b;

    if (!entry)
        return def;
    if (!entry->values_count)
        return 1;
    b = conf_parse_bool(entry->values[0]);
    if (b < 0) {
        config_error(entry->file, entry->line,
                     "Warning: `%s' expects yes or no, using default", key);
        return def;
    }
    return b;
}

/*************************************************************************/
/*************************** Directive binding ***************************/
/*************************************************************************/

int configure_block(const char* blockname, const char* label,
                    ConfigDirective* directives, int action)
{
    /* If no directives were given, return success */
    if (!directives)
        return 1;

    if (action & CONFIGURE_READ) {
        if (!read_directives(blockname, label, directives))
            return 0;
    }

    if (action & CONFIGURE_SET)
        do_all_directives(ACTION_COPYNEW, directives);

    return 1;
}

/*************************************************************************/

int configure(const char* modulename, ConfigDirective* directives, int action)
{
    return configure_block(modulename ? "module" : NULL, modulename,
                           directives, action);
}

/*************************************************************************/

/* Deconfigure given directive array (free any allocated storage and
 * restore original values).  A no-op if `directives' is NULL.
 */

void deconfigure(ConfigDirective* directives)
{
    if (directives)
        do_all_directives(ACTION_RESTORESAVED, directives);
}

/*************************************************************************/

/* Print a warning or error message to the log (and the console, if open). */

void config_error(const char* filename, int linenum, const char* message, ...)
{
    char buf[4096];
    va_list args;

    va_start(args, message);
    vsnprintf(buf, sizeof(buf), message, args);
    va_end(args);
    if (linenum)
        log("%s:%d: %s", filename, linenum, buf);
    else
        log("%s: %s", filename, buf);
    if (!nofork && isatty(2)) {
        if (linenum)
            fprintf(stderr, "%s:%d: %s\n", filename, linenum, buf);
        else
            fprintf(stderr, "%s: %s\n", filename, buf);
    }
}

/*************************************************************************/
/*************************************************************************/

/* Describe the block(s) a directive table is bound to, for messages. */

static const char* describe_block(char* buf, size_t size,
                                  const char* blockname, const char* label)
{
    if (!blockname)
        snprintf(buf, size, "the top level");
    else if (label)
        snprintf(buf, size, "block `%s \"%s\"'", blockname, label);
    else
        snprintf(buf, size, "block `%s'", blockname);
    return buf;
}

/*************************************************************************/

/* Store the values of `entry' as the new values of directive `d'.  Returns
 * 1 on success; otherwise reports the error and returns 0.
 */

static int bind_entry(const ConfNode* entry, ConfigDirective* d)
{
    const char* filename = entry->file;
    int linenum = entry->line;
    int ac = entry->values_count;
    char** av = entry->values;
    int i, optind = 0;
    long longval;
    unsigned long ulongval;
    char* s;
    int retval = 1;

    d->was_seen = 1;
    for (i = 0; i < CONFIG_MAXPARAMS && d->params[i].type != CD_NONE; i++) {
        if (d->params[i].type == CD_SET) {
            int value = optind < ac ? conf_parse_bool(av[optind]) : -1;
            if (value >= 0) {
                optind++;
            }
            else if (optind < ac && (i + 1 >= CONFIG_MAXPARAMS ||
                                     d->params[i + 1].type == CD_NONE)) {
                /* A plain flag given something that is not yes/no */
                config_error(filename, linenum,
                             "%s: Expected yes or no (or no value)",
                             d->name);
                return 0;
            }
            else {
                value = 1;
            }
            if (!(d->params[i].flags & CF_SAVED)) {
                d->params[i].prev.intval = *(int*)d->params[i].ptr;
                d->params[i].flags |= CF_SAVED;
            }
            d->params[i].new.intval = value;
            d->params[i].flags |= CF_WASSET;
            if (!value) {
                /* Switched off: whatever follows does not apply */
                optind = ac;
                break;
            }
            continue;
        }
        if (d->params[i].type == CD_DEPRECATED) {
            config_error(filename, linenum, "Deprecated directive `%s' used",
                         d->name);
            d->params[i].flags |= CF_WASSET;
            continue;
        }
        if (optind >= ac) {
            if (!(d->params[i].flags & CF_OPTIONAL)) {
                config_error(filename, linenum,
                             "Not enough parameters for `%s'", d->name);
                retval = 0;
            }
            break;
        }
        switch (d->params[i].type) {
            case CD_INT:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.intval = *(int32*)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                longval = strtol(av[optind++], &s, 0);
                if (*s || s == av[optind - 1]) {
                    config_error(filename, linenum,
                                 "%s: Expected an integer for parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
#if SIZEOF_LONG > 4
                if (longval < -0x80000000L || longval > 0x7FFFFFFFL) {
                    config_error(filename, linenum,
                                 "%s: Value out of range for parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
#endif
                d->params[i].new.intval = (int32)longval;
                break;
            case CD_POSINT:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.intval = *(int32*)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                ulongval = strtoul(av[optind++], &s, 0);
                if (*s || s == av[optind - 1] || ulongval <= 0) {
                    config_error(filename, linenum,
                                 "%s: Expected a positive integer for"
                                 " parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
#if SIZEOF_LONG > 4
                if (ulongval > 0xFFFFFFFFL) {
                    config_error(filename, linenum,
                                 "%s: Value out of range for parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
#endif
                d->params[i].new.intval = (int32)ulongval;
                break;
            case CD_PORT:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.intval = *(int32*)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                longval = strtol(av[optind++], &s, 0);
                if (*s || s == av[optind - 1]) {
                    config_error(filename, linenum,
                                 "%s: Expected a port number for parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
                if (longval < 1 || longval > 65535) {
                    config_error(filename, linenum,
                                 "Port numbers must be in the range 1..65535");
                    retval = 0;
                    break;
                }
                d->params[i].new.intval = (int32)longval;
                break;
            case CD_STRING:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.ptrval = *(char**)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                /* A repeated directive replaces the value read before */
                if (d->params[i].flags & CF_ALLOCED_NEW)
                    free(d->params[i].new.ptrval);
                d->params[i].flags &= ~CF_ALLOCED_NEW;
                d->params[i].new.ptrval = strdup(av[optind++]);
                if (!d->params[i].new.ptrval) {
                    config_error(filename, linenum, "%s: Out of memory",
                                 d->name);
                    return 0;
                }
                d->params[i].flags |= CF_ALLOCED_NEW;
                break;
            case CD_TIME:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.timeval = *(time_t*)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                d->params[i].new.timeval = dotime(av[optind++]);
                if (d->params[i].new.timeval < 0) {
                    config_error(filename, linenum,
                                 "%s: Expected a time value for parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
                break;
            case CD_TIMEMSEC:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.intval = *(int32*)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                longval = strtol(av[optind++], &s, 10);
                if (longval < 0) {
                    config_error(filename, linenum,
                                 "%s: Expected a positive value for"
                                 " parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
                else if (longval > 1000000) {
                    config_error(filename, linenum,
                                 "%s: Value too large (maximum 1000000)",
                                 d->name);
                    retval = 0;
                    break;
                }
                longval *= 1000;
                if (*s == '.') {
                    int decimal = 0;
                    int count = 0;
                    s++;
                    while (count < 3 && isdigit(*s)) {
                        decimal = decimal * 10 + (*s++ - '0');
                        count++;
                    }
                    while (count++ < 3)
                        decimal *= 10;
                    longval += decimal;
                    while (isdigit(*s))
                        s++;
                }
                if (*s) {
                    config_error(filename, linenum,
                                 "%s: Expected a decimal number for"
                                 " parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
                d->params[i].new.intval = (int32)longval;
                break;
            case CD_FUNC: {
                int (*func)(const char*, int, char*) =
                    (int (*)(const char*, int, char*))(d->params[i].ptr);
                /* Handlers may modify their parameter, so give them a
                 * copy; a CF_MULTI handler gets every remaining value */
                do {
                    char* param = strdup(av[optind++]);
                    if (!param) {
                        config_error(filename, linenum, "%s: Out of memory",
                                     d->name);
                        return 0;
                    }
                    if (!func(filename, linenum, param))
                        retval = 0;
                    free(param);
                } while ((d->params[i].flags & CF_MULTI) && optind < ac);
                break;
            }
            default:
                config_error(filename, linenum,
                             "%s: Unknown type %d for param %d", d->name,
                             d->params[i].type, i + 1);
                return 0; /* don't bother continuing--something's bizarre */
        } /* switch (d->params[i].type) */
        d->params[i].flags |= CF_WASSET;
    } /* for all parameters */

    if (optind < ac) {
        config_error(filename, linenum,
                     "Warning: too many parameters for `%s' (extra ignored)",
                     d->name);
    }

    return retval;
}

/*************************************************************************/

/* Bind the entries of one block to a directive table. */

static int bind_block(const ConfNode* block, ConfigDirective* directives,
                      const char* where)
{
    const ConfNode* entry;
    int n, retval = 1;

    for (entry = conf_first_entry(block, NULL); entry;
         entry = conf_next_entry(entry, NULL)) {
        for (n = 0; directives[n].name; n++) {
            if (stricmp(entry->name, directives[n].name) == 0)
                break;
        }
        if (!directives[n].name) {
            /* don't cause abort */
            config_error(entry->file, entry->line,
                         "Unknown directive `%s' in %s", entry->name, where);
            continue;
        }
        if (!bind_entry(entry, &directives[n]))
            retval = 0;
    }
    return retval;
}

/*************************************************************************/

/* Read in configuration options, and return nonzero for success, zero for
 * failure.  Performs the actions needed by configure(...,CONFIGURE_READ).
 */

static int read_directives(const char* blockname, const char* label,
                           ConfigDirective* directives)
{
    char where[256];
    const ConfNode* block;
    int retval = 1, i, n;

    if (!conf_root()) {
        log("conffile: BUG: configure() called before conf_load()");
        return 0;
    }
    describe_block(where, sizeof(where), blockname, label);

    /* Clear `was_set' flag and `new' value for all directives */
    for (n = 0; directives[n].name != NULL; n++) {
        directives[n].was_seen = 0;
        for (i = 0; i < CONFIG_MAXPARAMS; i++) {
            if (directives[n].params[i].flags & CF_ALLOCED_NEW)
                free(directives[n].params[i].new.ptrval);
            directives[n].params[i].flags &= ~(CF_WASSET | CF_ALLOCED_NEW);
            memset(&directives[n].params[i].new, 0,
                   sizeof(directives[n].params[i].new));
            if (directives[n].params[i].type == CD_FUNC) {
                int (*func)(const char*, int, char*) = (int (*)(
                    const char*, int, char*))(directives[n].params[i].ptr);
                func(NULL, CDFUNC_INIT, NULL);
            }
        }
    }

    /* Actually do the work */
    if (!blockname) {
        retval = bind_block(conf_root(), directives, where);
    }
    else {
        for (block = conf_find_block(conf_root(), blockname, label); block;
             block = conf_next_block(block, blockname, label)) {
            if (!bind_block(block, directives, where))
                retval = 0;
        }
    }

    /* Make sure all required directives were seen */
    for (n = 0; directives[n].name != NULL; n++) {
        if (!directives[n].was_seen &&
            (directives[n].params[0].flags & CF_DIRREQ)) {
            config_error(conf_filename(), 0,
                         "Required directive `%s' missing from %s",
                         directives[n].name, where);
            retval = 0;
        }
    }

    return retval;
}

/*************************************************************************/

/* Perform an action for all directives in an array; the action is given
 * by ACTION_*, defined above.
 */

static void do_all_directives(int action, ConfigDirective* directives)
{
    int n, i;

    for (n = 0; directives[n].name; n++) {
        ConfigDirective* d = &directives[n];
        for (i = 0; i < CONFIG_MAXPARAMS && d->params[i].type != CD_NONE;
             i++) {
            CDValue val;

            /* Select the appropriate value to copy */
            if (action == ACTION_COPYNEW)
                val = d->params[i].new;
            else
                val = d->params[i].prev;

            /* When restoring, a value read but never set (a REHASH that
             * failed elsewhere) is dropped too */
            if (action == ACTION_RESTORESAVED &&
                (d->params[i].flags & CF_ALLOCED_NEW)) {
                free(d->params[i].new.ptrval);
                d->params[i].new.ptrval = NULL;
                d->params[i].flags &= ~CF_ALLOCED_NEW;
            }

            /* In any case, we'll be rewriting the config variable, so free
             * the previous value if it was one we allocated */
            if (d->params[i].flags & CF_ALLOCED) {
                free(*(void**)d->params[i].ptr);
                d->params[i].flags &= ~CF_ALLOCED;
                /* If nothing replaces it below (a string directive that is
                 * gone from the file after a REHASH), the variable must not
                 * be left pointing at freed memory: it goes back to what
                 * it was before it was first configured. */
                *(void**)d->params[i].ptr =
                    (d->params[i].flags & CF_SAVED) ? d->params[i].prev.ptrval
                                                    : NULL;
            }

            /* Don't do anything if we're copying new values and this
             * directive/parameter wasn't seen, or if we're restoring saved
             * values and this parameter hasn't had its value saved (except
             * for function parameters) */
            if (action == ACTION_COPYNEW &&
                (!d->was_seen || !(d->params[i].flags & CF_WASSET)))
                continue;
            if (action == ACTION_RESTORESAVED &&
                d->params[i].type != CD_FUNC &&
                !(d->params[i].flags & CF_SAVED))
                continue;

            /* Copy new value to configuration variable */
            switch (d->params[i].type) {
                case CD_SET:
                    if (action == ACTION_COPYNEW)
                        *(int*)d->params[i].ptr = (int)val.intval;
                    break;
                case CD_TIME:
                    *(time_t*)d->params[i].ptr = val.timeval;
                    break;
                case CD_STRING:
                    *(char**)d->params[i].ptr = val.ptrval;
                    break;
                case CD_INT:
                case CD_POSINT:
                case CD_PORT:
                case CD_TIMEMSEC:
                    *(int32*)d->params[i].ptr = val.intval;
                    break;
                case CD_FUNC: {
                    int (*func)(const char*, int, char*) =
                        (int (*)(const char*, int, char*))(d->params[i].ptr);
                    if (action == ACTION_COPYNEW)
                        func(NULL, CDFUNC_SET, NULL);
                    else
                        func(NULL, CDFUNC_DECONFIG, NULL);
                    break;
                } /* case CD_FUNC */
                case CD_DEPRECATED:
                    /* Nothing to do */
                    break;
                default:
                    log("conffile: do_all_directives BUG: don't know how to "
                        " copy type %d (%s/%d)",
                        d->params[i].type, d->name, i);
                    break;
            } /* switch */

            /* Fix up flags */
            if (action == ACTION_COPYNEW) {
                if (d->params[i].flags & CF_ALLOCED_NEW) {
                    d->params[i].flags |= CF_ALLOCED;
                    /* The value is still allocated, but it's now stored in
                     * the configuration variable, so we don't want to free
                     * it when clearing `new' */
                    d->params[i].flags &= ~CF_ALLOCED_NEW;
                }
            }
            else {
                d->params[i].flags &= ~CF_SAVED;
            }
        } /* for each parameter */
    } /* for each directive */
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
