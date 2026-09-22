/* Header for common mask data structure.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef MASKDATA_H
#define MASKDATA_H

/*************************************************************************/

/* This structure and the corresponding functions are used by the autokill,
 * exception, and S-line modules.
 */

typedef struct maskdata_ MaskData;
struct maskdata_ {
    MaskData *next, *prev;
    int usecount;

    uint8 type;
    int num;            /* Index number */
    char *mask;
    int16 limit;        /* For exceptions only */
    char *reason;
    char who[NICKMAX];
    time_t time;
    time_t expires;     /* Or 0 for no expiry */
    time_t lastused;    /* Last time used, 0 if never used */
};

/* Types of mask data: */
#define MD_AKILL        0
#define MD_EXCLUDE      1
#define MD_EXCEPTION    2
#define MD_SGLINE       'G'     /* DO NOT CHANGE: some code relies on */
#define MD_SQLINE       'Q'     /* these values being the same as the */
#define MD_SZLINE       'Z'     /* corresponding S-line letter        */

/* Maximum number of mask data items for a single type: */
#define MAX_MASKDATA    32767

/* Maximum value for `limit' field of an entry: */
#define MAX_MASKDATA_LIMIT  32767

/*************************************************************************/

/* Database functions: */

E MaskData *add_maskdata(uint8 type, MaskData *data);
E void del_maskdata(uint8 type, MaskData *data);
E MaskData *get_maskdata(uint8 type, const char *mask);
E MaskData *get_matching_maskdata(uint8 type, const char *str);
E MaskData *put_maskdata(MaskData *data);
E MaskData *first_maskdata(uint8 type);
E MaskData *next_maskdata(uint8 type);
E int maskdata_count(uint8 type);
E MaskData *get_exception_by_num(int num);
E MaskData *move_exception(MaskData *except, int newnum);

/*************************************************************************/
/*************************************************************************/

/* OperServ internal stuff. */

/*************************************************************************/

/* Data structure for MaskData-related commands: */

typedef struct {
    const char *name;           /* Command name */
    uint8 md_type;              /* MaskData type */
    time_t *def_expiry_ptr;     /* Pointer to default expiry time */

    /* Various messages.  sprintf() parameters are given in order in
     * parentheses. */

    /* Syntax message (%s: command name) */
    /* Message for adding to a full list (%s: command name) */
    int msg_add_too_many;
    /* Message for adding a duplicate record (%s: mask, %s: command name) */
    int msg_add_exists;
    /* Message for successful add (%s: mask, %s: command name) */
    int msg_added;
    /* Message for no record found on delete (%s: mask, %s: command name) */
    int msg_del_not_found;
    /* Message for successful delete (%s: mask, %s: command name) */
    int msg_deleted;
    /* Message for successful clear (%s: command name) */
    int msg_cleared;
    /* Message for list header (%s: command name) */
    int msg_list_header;
    /* Message for LIST on empty list (%s: command name) */
    int msg_list_empty;
    /* Message for LIST with no matching records (%s: command name) */
    int msg_list_no_match;
    /* Message for no match on CHECK (%s: mask, %s: command name) */
    int msg_check_no_match;
    /* Message for CHECK response header (%s: mask, %s: command name) */
    int msg_check_header;
    /* Message for CHECK response trailer (%d: count of matches) */
    int msg_check_count;
    /* Message for record count (%d: count, %s: command name) */
    int msg_count;

    /* Helper functions called by do_maskdata_cmd(). */

    /* Make any alterations to the mask necessary for addition or deletion;
     * if not NULL, called before any other checks (other than for NULL)
     * are performed. */
    void (*mangle_mask)(char *mask);
    /* Check whether the mask is appropriate; return 1 if OK, else 0.
     * The mask and expiry time may be modified.  If NULL, any mask and
     * expiry time are accepted. */
    int (*check_add_mask)(const User *u, uint8 type, char *mask,
                          time_t *expiry_ptr);
    /* Operations to perform on mask addition.  If NULL, nothing is done. */
    void (*do_add_mask)(const User *u, uint8 type, MaskData *md);
    /* Operations to perform on mask deletion.  If NULL, nothing is done. */
    void (*do_del_mask)(const User *u, uint8 type, MaskData *md);
    /* Handler for unknown commands; should return 1 if the command was
     * handled, 0 otherwise. */
    int (*do_unknown_cmd)(const User *u, const char *cmd, char *mask);
} MaskDataCmdInfo;

/*************************************************************************/

/* MaskData command related functions: */

E void do_maskdata_cmd(const MaskDataCmdInfo *info, const User *u);
E char *make_reason(const char *format, const MaskData *data);

/* Other functions: */

E void *new_maskdata(void);
E void free_maskdata(void *record);
E int init_maskdata(void);
E void exit_maskdata(void);

/*************************************************************************/

#endif  /* MASKDATA_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
