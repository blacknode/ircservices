/* Miscellaneous MaskData-related functionality.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "language.h"
#include "commands.h"

#include "operserv.h"
#include "maskdata.h"

static void do_maskdata_add(const MaskDataCmdInfo *info, const User *u,
                            char *mask, const char *expiry);
static void do_maskdata_del(const MaskDataCmdInfo *info, const User *u,
                            const char *mask);
static void do_maskdata_clear(const MaskDataCmdInfo *info, const User *u,
                              const char *mask);
static void do_maskdata_list(const MaskDataCmdInfo *info, const User *u,
                             int is_view, const char *mask,
                             const char *skipstr);
static void maskdata_list_entry(const MaskDataCmdInfo *info, const User *u,
                                int is_view, const MaskData *md);
static void do_maskdata_check(const MaskDataCmdInfo *info, const User *u,
                              const char *mask);

/*************************************************************************/
/*************************** Database routines ***************************/
/*************************************************************************/

static MaskData *masklist[256];
static int32 masklist_count[256];
static int masklist_iterator[256];

static int cb_expire_md = -1;

/*************************************************************************/

/* Check for expiration of a MaskData; delete it and return 1 if expired,
 * return 0 if not expired.
 */

static int check_expire_maskdata(uint8 type, MaskData *md)
{
    if (md->expires && md->expires <= time(NULL)) {
        call_callback_2(cb_expire_md, type, md);
        del_maskdata(type, md);
        return 1;
    }
    return 0;
}

/*************************************************************************/

EXPORT_FUNC(add_maskdata)
MaskData *add_maskdata(uint8 type, MaskData *data)
{
    int num = masklist_count[type];
    if (num >= MAX_MASKDATA)
        fatal("add_maskdata(): too many items for type %u", type);
    ARRAY2_EXTEND(masklist[type], masklist_count[type]);
    memcpy(&masklist[type][num], data, sizeof(*data));
    masklist[type][num].next = (MaskData *)(long)num;  /* use as index */
    free(data);
    masklist[type][num].type = type;
    masklist[type][num].usecount = 1;
    return &masklist[type][num];
}

/*************************************************************************/

EXPORT_FUNC(del_maskdata)
void del_maskdata(uint8 type, MaskData *data)
{
    int num = (int)(long)(data->next);
    if (num < 0 || num >= masklist_count[type]) {
        module_log("del_maskdata(): invalid index %d for type %u at %p",
                   num, type, data);
        return;
    }
    free(data->mask);
    free(data->reason);
    ARRAY2_REMOVE(masklist[type], masklist_count[type], num);
    if (num < masklist_iterator[type])
        masklist_iterator[type]--;
    while (num < masklist_count[type]) {
        masklist[type][num].next = (MaskData *)(long)num;
        num++;
    }
}

/*************************************************************************/

EXPORT_FUNC(get_maskdata)
MaskData *get_maskdata(uint8 type, const char *mask)
{
    int i;
    MaskData *result;

    ARRAY2_SEARCH(masklist[type],masklist_count[type],mask,mask,stricmp,i);
    if (i >= masklist_count[type])
        return NULL;
    result = &masklist[type][i];
    if (!noexpire && !result->usecount && check_expire_maskdata(type,result))
        result = NULL;
    else
        result->usecount++;
    return result;
}

/*************************************************************************/

EXPORT_FUNC(get_matching_maskdata)
MaskData *get_matching_maskdata(uint8 type, const char *str)
{
    int i;

    ARRAY2_FOREACH (i, masklist[type], masklist_count[type]) {
        if (match_wild_nocase(masklist[type][i].mask, str)) {
            MaskData *result = &masklist[type][i];
            if (noexpire || result->usecount
             || !check_expire_maskdata(type,result)
            ) {
                result->usecount++;
                return result;
            } else {
                i--;
            }
        }
    }
    return NULL;
}

/*************************************************************************/

EXPORT_FUNC(put_maskdata)
MaskData *put_maskdata(MaskData *data)
{
    if (data) {
        if (data->usecount > 0)
            data->usecount--;
        else
            module_log_debug(1, "BUG: put_maskdata(%u,%s) with usecount==0",
                             data->type, data->mask);
    }
    return data;
}

/*************************************************************************/

EXPORT_FUNC(first_maskdata)
MaskData *first_maskdata(uint8 type)
{
    masklist_iterator[type] = 0;
    return next_maskdata(type);
}

EXPORT_FUNC(next_maskdata)
MaskData *next_maskdata(uint8 type)
{
    MaskData *result;

    do {
        if (masklist_iterator[type] >= masklist_count[type])
            return NULL;
        result = &masklist[type][masklist_iterator[type]++];
    } while (!noexpire && check_expire_maskdata(type, result));
    return result;
}

/*************************************************************************/

EXPORT_FUNC(maskdata_count)
int maskdata_count(uint8 type)
{
    return masklist_count[type];
}

/*************************************************************************/

EXPORT_FUNC(get_exception_by_num)
MaskData *get_exception_by_num(int num)
{
    int i;
    MaskData *result;

    ARRAY2_SEARCH_SCALAR(masklist[MD_EXCEPTION], masklist_count[MD_EXCEPTION],
                         num, num, i);
    if (i >= masklist_count[MD_EXCEPTION])
        return NULL;
    result = &masklist[MD_EXCEPTION][i];
    return !noexpire && check_expire_maskdata(MD_EXCEPTION, result)
        ? NULL : result;
}

/*************************************************************************/

/* Move an exception so it has the given number; we can assume that no
 * other exception already has that number.
 */

EXPORT_FUNC(move_exception)
MaskData *move_exception(MaskData *except, int newnum)
{
    int count;     /* shortcut for "masklist_count[MD_EXCEPTION]" */
    int index;     /* index of `except' */
    int newindex;  /* where `except' should be moved to */
    MaskData tmp;  /* to save the data while we move it around */

    count = masklist_count[MD_EXCEPTION];
    index = except - masklist[MD_EXCEPTION];
    if ((index == 0 || except[-1].num < newnum)
     && (index == count-1 || except[1].num >= newnum)
    ) {
        /* New number is already between previous and next entries; no need
         * to move it around, just renumber and exit */
        except->num = newnum;
        while (++index < count && except[1].num == except[0].num) {
            except[1].num++;
            except++;
        }
        return except;
    }

    /* Save the old exception data and remove it from the array */
    tmp = *except;
    if (index < count-1)
        memmove(except, except+1, sizeof(*except) * ((count-1)-index));

    /* Find where the exception should go */
    for (newindex = 0; newindex < count-1; newindex++) {
        if (masklist[MD_EXCEPTION][newindex].num >= newnum)
            break;
    }

    /* Sanity check--this case should have been caught above */
    if (index == newindex) {
        module_log("BUG: move_exception didn't catch index == newindex for"
                   " exception %d!", newnum);
    }

    /* Actually put it where it belongs */
    except = &masklist[MD_EXCEPTION][newindex];
    if (newindex < count-1)
        memmove(except+1, except, sizeof(*except) * ((count-1)-newindex));
    *except = tmp;
    except->num = newnum;

    /* Increment following exception number as needed */
    for (index = newindex+1; index < count; index++) {
        if (except[1].num == except[0].num)
            except[1].num++;
        else
            break;
        except++;
    }

    /* Update all index pointers (we only need to touch the ones that
     * moved, but do it for all of them for simplicity/safety) */
    for (index = 0; index < count; index++) {
        masklist[MD_EXCEPTION][index].next = (MaskData *)(long)index;
    }

    return &masklist[MD_EXCEPTION][newindex];
}

/*************************************************************************/

/* Free all memory used by database tables. */

static void clean_dbtables(void)
{
    int i, j;

    for (i = 0; i < 256; i++) {
        for (j = 0; j < masklist_count[i]; j++) {
            free(masklist[i][j].mask);
            free(masklist[i][j].reason);
        }
        free(masklist[i]);
        masklist[i] = NULL;
        masklist_count[i] = 0;
    }
}

/*************************************************************************/
/**************** MaskData-related command helper routine ****************/
/*************************************************************************/

/* This routines is designed to be called from a command handler for a
 * command that deals with MaskData records (AKILL, EXCEPTION, etc.), to
 * help avoid code repetition in those command handlers.  The `info'
 * structure passed as the first parameter to each function contains
 * information about the command, such as command name and message numbers;
 * see maskdata.h for details.
 */

void do_maskdata_cmd(const MaskDataCmdInfo *info, const User *u)
{
    const char *cmd;
    char *mask, *expiry, *skipstr, *s;

    cmd = strtok(NULL, " ");
    if (!cmd)
        cmd = "";
    expiry = NULL;
    skipstr = NULL;
    s = strtok_remaining();
    if (stricmp(cmd,"ADD") == 0 && s && *s == '+') {
        expiry = strtok(s+1, " ");
        s = strtok_remaining();
    } else if ((stricmp(cmd,"LIST") == 0 || stricmp(cmd,"VIEW") == 0)
               && s && *s == '+') {
        skipstr = strtok(s+1, " ");
        s = strtok_remaining();
    }
    if (s && *s == '"') {
        mask = s+1;
        s = strchr(mask, '"');
        if (!s) {
            notice_lang(s_OperServ, u, MISSING_QUOTE);
            return;
        }
        strtok(s, " ");         /* prime strtok() for later */
        *s = 0;                 /* null-terminate quoted string */
    } else {
        mask = strtok(s, " ");  /* this will still work if s is NULL: mask
                                 * will end up NULL, which is what we want */
    }
    if (mask && info->mangle_mask)
        info->mangle_mask(mask);

    if (stricmp(cmd, "ADD") == 0) {
        do_maskdata_add(info, u, mask, expiry);
    } else if (stricmp(cmd, "DEL") == 0) {
        do_maskdata_del(info, u, mask);
    } else if (stricmp(cmd, "CLEAR") == 0) {
        do_maskdata_clear(info, u, mask);
    } else if (stricmp(cmd, "LIST") == 0 || stricmp(cmd, "VIEW") == 0) {
        do_maskdata_list(info, u, stricmp(cmd,"VIEW")==0, mask, skipstr);
    } else if (stricmp(cmd, "CHECK") == 0) {
        do_maskdata_check(info, u, mask);
    } else if (stricmp(cmd, "COUNT") == 0) {
        notice_lang(s_OperServ, u, info->msg_count,
                    maskdata_count(info->md_type), info->name);
    } else if (!info->do_unknown_cmd || !info->do_unknown_cmd(u, cmd, mask)) {
        syntax_error(s_OperServ, u, info->name, OPER_MASKDATA_SYNTAX);
    }
}

/*************************************************************************/

/* Handle the ADD subcommand. */

static void do_maskdata_add(const MaskDataCmdInfo *info, const User *u,
                            char *mask, const char *expiry)
{
    MaskData *md;
    time_t now = time(NULL);
    time_t expires;
    const char *reason;

    if (maskdata_count(info->md_type) >= MAX_MASKDATA) {
        notice_lang(s_OperServ, u, info->msg_add_too_many, info->name);
        return;
    }
    reason = strtok_remaining();
    if (!reason) {
        syntax_error(s_OperServ, u, info->name, OPER_MASKDATA_ADD_SYNTAX);
        return;
    }

    expires = expiry ? dotime(expiry) : *info->def_expiry_ptr;
    if (expires < 0) {
        notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
        return;
    } else {
        if (expires > 0)
            expires += now;     /* Make it into an absolute time */
    }

    /* Run command-specific checks. */
    if (info->check_add_mask
     && !info->check_add_mask(u, info->md_type, mask, &expires)
    ) {
        return;
    }

    /* Make sure mask does not already exist on list. */
    if (put_maskdata(get_maskdata(info->md_type, mask))) {
        notice_lang(s_OperServ, u, info->msg_add_exists, mask, info->name);
        return;
    }

    md = scalloc(1, sizeof(*md));
    md->mask = sstrdup(mask);
    md->reason = sstrdup(reason);
    md->time = time(NULL);
    md->expires = expires;
    strbcpy(md->who, u->nick);
    md = add_maskdata(info->md_type, md);
    if (info->do_add_mask)
        info->do_add_mask(u, info->md_type, md);
    notice_lang(s_OperServ, u, info->msg_added, mask, info->name);
    if (readonly)
        notice_lang(s_OperServ, u, READ_ONLY_MODE);
}

/*************************************************************************/

/* Handle the DEL subcommand. */

static void do_maskdata_del(const MaskDataCmdInfo *info, const User *u,
                            const char *mask)
{
    MaskData *md;

    if (!mask) {
        syntax_error(s_OperServ, u, info->name, OPER_MASKDATA_DEL_SYNTAX);
        return;
    }
    md = get_maskdata(info->md_type, mask);
    if (md) {
        if (info->do_del_mask)
            info->do_del_mask(u, info->md_type, md);
        del_maskdata(info->md_type, md);
        notice_lang(s_OperServ, u, info->msg_deleted, mask, info->name);
        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    } else {
        notice_lang(s_OperServ, u, info->msg_del_not_found, mask, info->name);
    }
}

/*************************************************************************/

/* Handle the CLEAR subcommand. */

static void do_maskdata_clear(const MaskDataCmdInfo *info, const User *u,
                              const char *mask)
{
    MaskData *md;

    if (!mask || stricmp(mask,"ALL") != 0) {
        syntax_error(s_OperServ, u, info->name, OPER_MASKDATA_CLEAR_SYNTAX);
        return;
    }
    for (md = first_maskdata(info->md_type); md;
         md = next_maskdata(info->md_type)
    ) {
        if (info->do_del_mask)
            info->do_del_mask(u, info->md_type, md);
        del_maskdata(info->md_type, md);
    }
    notice_lang(s_OperServ, u, info->msg_cleared, info->name);
    if (readonly)
        notice_lang(s_OperServ, u, READ_ONLY_MODE);
}

/*************************************************************************/

/* Handle the LIST and VIEW subcommands. */

static void do_maskdata_list(const MaskDataCmdInfo *info, const User *u,
                             int is_view, const char *mask,
                             const char *skipstr)
{
    const MaskData *md;
    int count = 0, skip = 0, noexpire = 0;
    char *s;

    if (skipstr) {
        skip = (int)atolsafe(skipstr, 0, INT_MAX);
        if (skip < 0) {
            syntax_error(s_OperServ, u, info->name, OPER_MASKDATA_LIST_SYNTAX);
            return;
        }
    }
    if ((s = strtok(NULL," ")) != NULL && stricmp(s,"NOEXPIRE") == 0)
        noexpire = 1;

    if (maskdata_count(info->md_type) == 0) {
        notice_lang(s_OperServ, u, info->msg_list_empty, info->name);
        return;
    }

    for (md = first_maskdata(info->md_type); md;
         md = next_maskdata(info->md_type)
    ) {
        if ((!mask || (match_wild_nocase(mask, md->mask)
                       && (!noexpire || !md->expires)))
        ) {
            if (!count)
                notice_lang(s_OperServ, u, info->msg_list_header, info->name);
            count++;
            if (count > skip && count <= skip+ListMax) {
                maskdata_list_entry(info, u, is_view, md);
            }
        }
    }
    if (count) {
        int shown = count - skip;
        if (shown < 0)
            shown = 0;
        else if (shown > ListMax)
            shown = ListMax;
        notice_lang(s_OperServ, u, LIST_RESULTS, shown, count);
    } else {
        notice_lang(s_OperServ, u, info->msg_list_no_match, info->name);
    }
}

/*************************************************************************/

/* Display a single MaskData entry.  Called from do_maskdata_list(). */

static void maskdata_list_entry(const MaskDataCmdInfo *info, const User *u,
                                int is_view, const MaskData *md)
{
    if (is_view) {
        char timebuf[BUFSIZE], usedbuf[BUFSIZE];
        char expirebuf[BUFSIZE];
        const char *who;

        strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                      STRFTIME_SHORT_DATE_FORMAT, md->time);
        strftime_lang(usedbuf, sizeof(usedbuf), u->ngi,
                      STRFTIME_SHORT_DATE_FORMAT, md->lastused);
        expires_in_lang(expirebuf, sizeof(expirebuf), u->ngi, md->expires);
        who = *md->who ? md->who : "<unknown>";
        if (md->lastused) {
            notice_lang(s_OperServ, u, OPER_MASKDATA_VIEW_FORMAT, md->mask,
                        who, timebuf, usedbuf, expirebuf, md->reason);
        } else {
            notice_lang(s_OperServ, u, OPER_MASKDATA_VIEW_UNUSED_FORMAT,
                        md->mask, who, timebuf, expirebuf, md->reason);
        }
    } else { /* !is_view */
        notice_lang(s_OperServ, u, OPER_MASKDATA_LIST_FORMAT, md->mask,
                    md->reason);
    }
}

/*************************************************************************/

/* Handle the CHECK subcommand. */

static void do_maskdata_check(const MaskDataCmdInfo *info, const User *u,
                              const char *mask)
{
    const MaskData *md;
    int count = 0;
    int did_header = 0;

    if (!mask) {
        syntax_error(s_OperServ, u, info->name, OPER_MASKDATA_CHECK_SYNTAX);
        return;
    }
    for (md = first_maskdata(info->md_type); md;
         md = next_maskdata(info->md_type)
    ) {
        if (count < ListMax && match_wild_nocase(md->mask, mask)) {
            count++;
            if (!did_header) {
                notice_lang(s_OperServ, u, info->msg_check_header,
                            mask, info->name);
                did_header = 1;
            }
            notice(s_OperServ, u->nick, "    %s", md->mask);
        }
    }
    if (did_header)
        notice_lang(s_OperServ, u, info->msg_check_count, count);
    else
        notice_lang(s_OperServ, u, info->msg_check_no_match, mask, info->name);
}

/*************************************************************************/
/************************ Miscellaneous functions ************************/
/*************************************************************************/

/* Allocate or free a MaskData structure.  Used for database new/free
 * functions.
 */

void *new_maskdata(void)
{
    return scalloc(1, sizeof(MaskData));
}


void free_maskdata(void *record)
{
    MaskData *md = record;
    if (md) {
        free(md->mask);
        free(md->reason);
        free(md);
    }
}


/*************************************************************************/

/* Create and return the reason string for the given MaskData and format
 * string.  The string will be truncated at BUFSIZE-1 bytes.  The string is
 * returned in a static buffer, and will be overwritten by subsequent calls.
 */

char *make_reason(const char *format, const MaskData *data)
{
    static char reason[BUFSIZE];
    char *s;
    int data_reason_len = -1;

    s = reason;
    while (*format && s-reason < sizeof(reason)-1) {
        if (*format == '%' && format[1] == 's') {
            int left = (sizeof(reason)-1) - (s-reason);
            if (data_reason_len < 0)
                data_reason_len = strlen(data->reason);
            if (left > data_reason_len)
                left = data_reason_len;
            memcpy(s, data->reason, left);
            s += left;
            format += 2;
        } else {
            *s++ = *format++;
        }
    }
    *s = 0;
    return reason;
}

/*************************************************************************/

/* MaskData initialization: set up expiration callback. */

int init_maskdata(void)
{
    cb_expire_md = register_callback("expire maskdata");
    if (cb_expire_md < 0) {
        module_log("Unable to register MaskData expiration callback");
        return 0;
    }
    return 1;
}


/* MaskData cleanup: remove expiration callback. */

void exit_maskdata(void)
{
    clean_dbtables();
    unregister_callback(cb_expire_md);
    cb_expire_md = -1;
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
