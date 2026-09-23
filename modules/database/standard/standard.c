/* Routines to load/save Services databases in standard format.
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
#include "databases.h"
#include "encrypt.h"

#include "fileutil.h"

#define SAFE(x) do { if ((x) < 0) goto fail; } while (0)

/*************************************************************************/

/* Database file format.
 * Note: all integer values are big-endian.
 *
 * File header:
 *    4 bytes -- version number (uint32)
 *    4 bytes -- size of header (uint32)
 *    4 bytes -- absolute offset to field list (uint32)
 *    4 bytes -- absolute offset to first record descriptor table (uint32)
 *
 * Field list:
 *    4 bytes -- size of field list (uint32)
 *    4 bytes -- number of fields (uint32)
 *    4 bytes -- size of a single record == sum of field sizes (uint32)
 *    N bytes -- field definitions
 *
 * Field definition:
 *    4 bytes -- field data size in bytes
 *    2 bytes -- field data type (DBTYPE_*) (uint16)
 *    2 bytes -- length of field name, including trailing \0 (uint16)
 *    N bytes -- field name
 *
 * Record descriptor table:
 *    4 bytes -- absolute offset to next table, or 0 if none (uint32)
 *    4 bytes -- size of this table in bytes
 *    N bytes -- record entries
 *
 * Entry in record descriptor table:
 *    4 bytes -- absolute offset to record data
 *    4 bytes -- total length of record, including any strings
 *
 * Record data:
 *    N bytes -- main record data
 *    P bytes -- first string
 *    Q bytes -- second string
 *    ...
 *
 * Field data:
 *    DBTYPE_INT8:     8-bit value
 *    DBTYPE_UINT8:    8-bit value
 *    DBTYPE_INT16:    16-bit value
 *    DBTYPE_UINT16:   16-bit value
 *    DBTYPE_INT32:    32-bit value
 *    DBTYPE_UINT32:   32-bit value
 *    DBTYPE_TIME:     64-bit value
 *    DBTYPE_STRING:   32-bit offset to string from start of record
 *    DBTYPE_BUFFER:   N-byte buffer contents
 *    DBTYPE_PASSWORD: 32-bit cipher string offset, (N-4) byte password buffer
 *
 * String data:
 *    2 bytes -- string length including trailing \0 (uint16), 0 if NULL
 *    N bytes -- string contents
 */

#define NEWDB_VERSION  ('I'<<24 | 'S'<<16 | 'D'<<8 | 1)  /* IRC Services DB */
#define RECTABLE_LEN   0x400  /* anything will do, really */
#define RECTABLE_SIZE  (RECTABLE_LEN*8)

/* Structure for keeping track of where to put data: */
typedef struct {
    const DBTable *table;
    int nfields;
    struct {
        DBField *field;
        int32 offset;   /* -1 if not found on load */
        int rawsize;    /* native size on system */
        int filesize;   /* size when written to file */
    } *fields;
} TableInfo;

/*************************************************************************/
/*********************** Internal helper routines ************************/
/*************************************************************************/

/* Helper routine: creates a TableInfo for a table. */

static TableInfo *create_tableinfo(const DBTable *table)
{
    TableInfo *ti;
    int i;

    ti = malloc(sizeof(*ti));
    if (!ti) {
        module_log("create_tableinfo(): Out of memory for table %s",
                   table->name);
        return NULL;
    }
    ti->table = table;
    for (i = 0; table->fields[i].name; i++)
        ;
    ti->nfields = i;
    ti->fields = malloc(sizeof(*ti->fields) * ti->nfields);
    for (i = 0; i < ti->nfields; i++) {
        uint32 fieldsize;
        int rawsize = 0;
        ti->fields[i].field = &table->fields[i];
        switch (ti->fields[i].field->type) {
          case DBTYPE_INT8:
          case DBTYPE_UINT8:    fieldsize = 1; break;
          case DBTYPE_INT16:
          case DBTYPE_UINT16:   fieldsize = 2; break;
          case DBTYPE_INT32:
          case DBTYPE_UINT32:   fieldsize = 4; break;
          case DBTYPE_TIME:     fieldsize = 8; rawsize = sizeof(time_t); break;
          case DBTYPE_STRING:   fieldsize = 4; rawsize = sizeof(char *); break;
          case DBTYPE_BUFFER:   fieldsize = ti->fields[i].field->length; break;
          case DBTYPE_PASSWORD: fieldsize = PASSMAX+4;
                                rawsize = sizeof(Password); break;
          default:
            module_log("create_tableinfo(): Invalid type (%d) for field %s"
                       " in table %s", ti->fields[i].field->type,
                       ti->fields[i].field->name, ti->table->name);
            return NULL;
        }
        if (!rawsize)
            rawsize = fieldsize;
        ti->fields[i].rawsize = rawsize;
        ti->fields[i].filesize = fieldsize;
        ti->fields[i].offset = -1;
    }
    return ti;
}

/*************************************************************************/

/* Helper routine: frees a TableInfo. */

static void free_tableinfo(TableInfo *ti)
{
    if (ti) {
        free(ti->fields);
        free(ti);
    }
}

/*************************************************************************/

/* Helper routine: returns the filename for a table. */

static const char *make_filename(const DBTable *table)
{
    static const char okchars[] =  /* characters allowed in filenames */
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    static char namebuf[1000];
    const char *s;
    char *d = namebuf;

    for (s = table->name; *s && d-namebuf < sizeof(namebuf)-5; s++, d++) {
        if (strchr(okchars, *s))
            *d = *s;
        else
            *d = '_';
    }
    strcpy(d, ".sdb");  /* safe: space left for this above */
    return namebuf;
}

/*************************************************************************/
/*********************** Database loading routines ***********************/
/*************************************************************************/

static int read_file_header(dbFILE *f, uint32 *fieldofs_ret,
                            uint32 *recofs_ret);
static int read_field_list(TableInfo *ti, dbFILE *f, uint32 *recsize_ret);
static int read_records(TableInfo *ti, dbFILE *f, uint32 recsize);

static int standard_load_table(DBTable *table)
{
    TableInfo *ti;
    const char *filename;
    dbFILE *f;
    uint32 fieldofs;  /* field list offset */
    uint32 recofs;    /* record descriptor table offset */
    uint32 recsize;   /* size of a record, less strings */

    /* Set up the TableInfo structure */
    ti = create_tableinfo(table);
    if (!ti)
        return 0;

    /* Open the file */
    filename = make_filename(table);
    f = open_db(filename, "r", NEWDB_VERSION);
    if (!f) {
        module_log_perror("Can't open %s for reading", filename);
        free_tableinfo(ti);
        return 0;
    }

    /* Check the file version and header size, and read header fields */
    if (!read_file_header(f, &fieldofs, &recofs))
        goto fail;

    /* Read in the field list and make sure it fits with the DBTable */
    if (fseek(f->fp, fieldofs, SEEK_SET) < 0) {
        module_log_perror("Can't seek in %s", filename);
        goto fail;
    }
    if (!read_field_list(ti, f, &recsize))
        goto fail;

    /* Read in the records */
    if (fseek(f->fp, recofs, SEEK_SET) < 0) {
        module_log_perror("Can't seek in %s", filename);
        goto fail;
    }
    if (!read_records(ti, f, recsize))
        goto fail;

    /* Call the postload routine, if any */
    if (table->postload && !(*table->postload)()) {
        module_log_perror("Table %s postload routine failed", table->name);
        goto fail;
    }

    /* Done! */
    close_db(f);
    free_tableinfo(ti);
    return 1;

  fail:
    close_db(f);
    free_tableinfo(ti);
    return 0;
}

/*************************************************************************/

/* Read the file header and verify the version and header size, then return
 * the field list offset and record descriptor table offset.
 */

static int read_file_header(dbFILE *f, uint32 *fieldofs_ret,
                            uint32 *recofs_ret)
{
    uint32 version;   /* version number stored in file */
    uint32 hdrsize;   /* file header size */

    SAFE(read_uint32(&version, f));
    if (version != NEWDB_VERSION) {
        module_log("Bad version number on %s", f->filename);
        return 0;
    }
    SAFE(read_uint32(&hdrsize, f));
    if (hdrsize < 16) {
        module_log("Bad header size on %s", f->filename);
        return 0;
    }
    SAFE(read_uint32(fieldofs_ret, f));
    SAFE(read_uint32(recofs_ret, f));
    return 1;

  fail:
    module_log("Read error on %s", f->filename);
    return 0;
}

/*************************************************************************/

/* Read in the field list, matching it with the field list in the DBTable
 * structure and filling in field offsets.  The record size (as stored in
 * the file) is returned in *recsize_ret.
 */

static int read_field_list(TableInfo *ti, dbFILE *f, uint32 *recsize_ret)
{
    uint32 thispos;     /* current file position */
    uint32 maxpos;      /* end of table */
    uint32 nfields;
    int32 recofs = 0;
    int i, j;

    SAFE(read_uint32(&maxpos, f));      /* field list size */
    maxpos += ftell(f->fp)-4;
    SAFE(read_uint32(&nfields, f));     /* number of fields */
    SAFE(read_uint32(recsize_ret, f));  /* record size--filled in later */
    for (i = 0; i < nfields; i++) {
        uint32 length;
        uint16 type;
        uint16 strsize;
        char *fieldname;
        SAFE((int32)(thispos = ftell(f->fp)));
        SAFE(read_uint32(&length, f));
        SAFE(read_uint16(&type, f));
        SAFE(read_uint16(&strsize, f));
        if (thispos+8+strsize > maxpos) {
            module_log("load_table(): premature end of field list");
            return 0;
        }
        SAFE(fseek(f->fp, -2, SEEK_CUR));
        SAFE(read_string(&fieldname, f));
        if (!fieldname) {  /* impossible */
            module_log("load_table(): BUG: field name is NULL");
            return 0;
        }
        for (j = 0; j < ti->nfields; j++) {
            DBField *field = ti->fields[j].field;
            if (type == field->type
             && length == ti->fields[j].filesize
             && strcmp(field->name,fieldname) == 0
            ) {
                ti->fields[j].offset = recofs;
                break;
            }
        }
        free(fieldname);
        recofs += length;
    }
    return 1;

  fail:
    module_log_perror("Error reading from %s", f->filename);
    return 0;
}

/*************************************************************************/

/* Read in table records. */

static int read_records(TableInfo *ti, dbFILE *f, uint32 recsize)
{
    uint32 *rectable;   /* Table of record pointers and lengths */
    int recnum;         /* Record number within descriptor table */
    int reccount;       /* Record number in file */
    void *record;       /* Record pointer from first()/next() */
    void *recbuf;       /* Buffer for storing record data */
    int i;

    /* Allocate record buffer */
    recbuf = malloc(recsize);
    if (!recbuf)
        return 0;

    /* Variable init */
    rectable = NULL;
    reccount = 0;
    record = NULL;

    /* Termination check has to be performed in the middle of the loop,
     * since we may need to read the record descriptor table first */
    for (recnum = 0; ; recnum = (recnum+1) % (rectable[1]/8), reccount++) {

        if (!recnum) {  /* Start of a new record descriptor table */
            uint32 nextofs, tablesize;

            /* Seek to next table, if this isn't the first */
            if (rectable) {
                if (!rectable[0]) {
                    module_log("read_records(): %s: next table is 0!",
                               f->filename);
                    return 0;
                }
                SAFE(fseek(f->fp, rectable[0], SEEK_SET));
                free(rectable);
                rectable = NULL;
            }

            /* Read in and check the `next' pointer and table size */
            SAFE(read_uint32(&nextofs, f));
            SAFE(read_uint32(&tablesize, f));
            if (!tablesize) {
                module_log("read_records(): %s: rectable size is 0!",
                           f->filename);
                return 0;
            }

            /* Allocate the table memory */
            rectable = malloc(tablesize);
            if (!rectable) {
                module_log("read_records(): %s: no memory for rectable!",
                           f->filename);
                return 0;
            }

            /* Read in the table */
            rectable[0] = nextofs;
            rectable[1] = tablesize;
            for (i = 2; i < tablesize/4; i++)
                SAFE(read_uint32(&rectable[i], f));

            /* First record in the table is at index 1 */
            recnum++;
        }

        /* If this is the last record, bail out */
        if (!rectable[recnum*2])
            break;

        /* Make sure the record size is large enough */
        if (rectable[recnum*2+1] < recsize) {
            module_log("read_records(): %s: record %d is too small,"
                       " skipping", f->filename, reccount);
            continue;
        }

        /* Allocate a new record */
        record = ti->table->newrec();
        if (!record) {
            module_log("read_records(): %s: newrec() failed for record %d!",
                       f->filename, reccount);
            return 0;
        }

        /* Seek to the location of this record and read the main record data */
        SAFE(fseek(f->fp, rectable[recnum*2], SEEK_SET));
        if (fread(recbuf, recsize, 1, f->fp) != 1)
            goto fail;

        /* Read fields from record buffer and strings from file */
        for (i = 0; i < ti->nfields; i++) {
            const DBField *field = ti->fields[i].field;
            const void *src = (const int8 *)recbuf + ti->fields[i].offset;
            if (ti->fields[i].offset < 0)  /* field not present in file */
                continue;
            if (field->type == DBTYPE_STRING) {
                uint32 strofs;
                char *string;
                strofs = ((uint8 *)src)[0] << 24
                       | ((uint8 *)src)[1] << 16
                       | ((uint8 *)src)[2] <<  8
                       | ((uint8 *)src)[3];
                if (strofs >= rectable[recnum*2+1]) {
                    module_log("read_records(): %s: string for field `%s' of"
                               " record %d is out of range, skipping",
                               f->filename, field->name, reccount);
                    continue;
                }
                SAFE(fseek(f->fp, rectable[recnum*2] + strofs, SEEK_SET));
                SAFE(read_string(&string, f));
                put_dbfield(record, field, &string);
            } else if (field->type == DBTYPE_PASSWORD) {
                uint32 strofs;
                Password pass;
                char *cipher;
                strofs = ((uint8 *)src)[0] << 24
                       | ((uint8 *)src)[1] << 16
                       | ((uint8 *)src)[2] <<  8
                       | ((uint8 *)src)[3];
                if (strofs >= rectable[recnum*2+1]) {
                    module_log("read_records(): %s: string for field `%s' of"
                               " record %d is out of range, skipping",
                               f->filename, field->name, reccount);
                    continue;
                }
                SAFE(fseek(f->fp, rectable[recnum*2] + strofs, SEEK_SET));
                SAFE(read_string((char **)&cipher, f));
                init_password(&pass);
                set_password(&pass, src+4, cipher);
                free(cipher);
                put_dbfield(record, field, &pass);
            } else if (field->type == DBTYPE_BUFFER) {
                put_dbfield(record, field, src);
            } else {
                char fieldbuf[16];  /* Big enough for any int or time type */
                switch (field->type) {
                  case DBTYPE_INT8:
                  case DBTYPE_UINT8:
                    *((uint8 *)fieldbuf) = ((uint8 *)src)[0];
                    break;
                  case DBTYPE_INT16:
                  case DBTYPE_UINT16:
                    *((uint16 *)fieldbuf) = ((uint8 *)src)[0] << 8
                                          | ((uint8 *)src)[1];
                    break;
                  case DBTYPE_INT32:
                  case DBTYPE_UINT32:
                    *((uint32 *)fieldbuf) = ((uint8 *)src)[0] << 24
                                          | ((uint8 *)src)[1] << 16
                                          | ((uint8 *)src)[2] <<  8
                                          | ((uint8 *)src)[3];
                    break;
                  case DBTYPE_TIME:
                    *((time_t *)fieldbuf) = ((uint8 *)src)[4] << 24
                                          | ((uint8 *)src)[5] << 16
                                          | ((uint8 *)src)[6] <<  8
                                          | ((uint8 *)src)[7];
#if SIZEOF_TIME_T >= 8
                    *((time_t *)fieldbuf) |=(time_t)((uint8 *)src)[0] << 56
                                          | (time_t)((uint8 *)src)[1] << 48
                                          | (time_t)((uint8 *)src)[2] << 40
                                          | (time_t)((uint8 *)src)[3] << 32;
#endif
                    break;
                  case DBTYPE_STRING:
                  case DBTYPE_BUFFER:
                  case DBTYPE_PASSWORD:
                    /* Handled above--listed here to avoid warnings */
                    break;
                }
                put_dbfield(record, field, fieldbuf);
            }
        }  /* for each field */

        /* Add record to table */
        ti->table->insert(record);
        record = NULL;

    }  /* for each record */

    /* All done */
    free(recbuf);
    free(rectable);
    return 1;

  fail:
    module_log_perror("read_records(): Read error on %s", f->filename);
    if (record)
        ti->table->freerec(record);
    free(recbuf);
    free(rectable);
    return 0;
}

/*************************************************************************/
/*********************** Database saving routines ************************/
/*************************************************************************/

static int write_file_header(TableInfo *ti, dbFILE *f);
static int write_field_list(TableInfo *ti, dbFILE *f, uint32 *recsize_ret);
static int write_records(TableInfo *ti, dbFILE *f, uint32 recsize);

static int standard_save_table(DBTable *table)
{
    TableInfo *ti;
    const char *filename;
    dbFILE *f;
    uint32 recsize;  /* size of a record, less strings */

    /* Set up the TableInfo structure */
    ti = create_tableinfo(table);
    if (!ti)
        return 0;

    /* Open the file */
    filename = make_filename(table);
    f = open_db(filename, "w", NEWDB_VERSION);
    if (!f) {
        module_log_perror("Can't open %s for writing", filename);
        free_tableinfo(ti);
        return 0;
    }

    /* Write the file header */
    SAFE(write_file_header(ti, f));

    /* Write the field list */
    SAFE(write_field_list(ti, f, &recsize));

    /* Write the actual records */
    SAFE(write_records(ti, f, recsize));

    /* Done! */
    close_db(f);
    free_tableinfo(ti);
    return 1;

  fail:
    module_log_perror("Can't write to %s", filename);
    restore_db(f);
    free_tableinfo(ti);
    return 0;
}

/*************************************************************************/

/* Write the file header. */

static int write_file_header(TableInfo *ti, dbFILE *f)
{
    /* Note that the version number is already written */
    SAFE(write_int32(16, f));  /* header size */
    SAFE(write_int32(0, f));   /* field list offset--filled in later */
    SAFE(write_int32(0, f));   /* record desc. table offset--filled in later */
    return 0;
  fail:
    return -1;
}

/*************************************************************************/

/* Write the field list, filling in ti->fields[].offset as we go, and send
 * back the size of a record; also fills in field list offset in file
 * header.
 */

static int write_field_list(TableInfo *ti, dbFILE *f, uint32 *recsize_ret)
{
    uint32 listpos;  /* file position of start of table */
    uint32 endpos;   /* file position of end of table */
    uint32 recsize = 0;
    int i, nfields = 0;

    SAFE((int32)(listpos = ftell(f->fp)));
    SAFE(fseek(f->fp, 8, SEEK_SET));
    SAFE(write_int32(listpos, f));
    SAFE(fseek(f->fp, listpos, SEEK_SET));
    SAFE(write_int32(0, f));  /* field list size--filled in later*/
    SAFE(write_int32(0, f));  /* number of fields--filled in later */
    SAFE(write_int32(0, f));  /* record size--filled in later */
    for (i = 0; i < ti->nfields; i++) {
        if (ti->fields[i].field->load_only)
            continue;
        SAFE(write_int32(ti->fields[i].filesize, f));
        SAFE(write_int16(ti->fields[i].field->type, f));
        SAFE(write_string(ti->fields[i].field->name, f));
        ti->fields[i].offset = recsize;
        nfields++;
        recsize += ti->fields[i].filesize;
    }
    SAFE((int32)(endpos = ftell(f->fp)));
    SAFE(fseek(f->fp, listpos, SEEK_SET));
    SAFE(write_int32(endpos-listpos, f));
    SAFE(write_int32(nfields, f));
    SAFE(write_int32(recsize, f));
    SAFE(fseek(f->fp, endpos, SEEK_SET));
    *recsize_ret = recsize;
    return 0;
  fail:
    return -1;
}

/*************************************************************************/

/* Write out table records. */

static int write_records(TableInfo *ti, dbFILE *f, uint32 recsize)
{
    uint32 listpos;       /* Offset of current record descriptor table */
    int recnum;           /* Record number within table */
    const void *record;   /* Record pointer from first()/next() */
    void *recbuf = NULL;  /* Buffer for storing record data */
    int i;

    /* Allocate record buffer */
    recbuf = malloc(recsize);
    if (!recbuf)
        return -1;

    /* Variable init */
    listpos = 12;  /* First record descriptor table pointer is recorded here */
    recnum = 0;

    /* Note the lack of a termination condition in this loop--we need to
     * write a 0 to terminate the record list, which may involve creating a
     * new descriptor table, so we let the first part of the loop run and
     * break out in the middle when we hit the end of the table. */
    for (record = ti->table->first(); ; record = ti->table->next()) {
        uint32 thispos;  /* Offset this record is written at */
        uint32 nextpos;  /* Temp variable to hold next record's offset */

        if (!recnum) {  /* Start of a new rectable */
            uint32 oldpos = listpos;
            static char dummy_rectable[RECTABLE_SIZE];

            /* Seek to a multiple of the table size, just for the hell of it */
            SAFE((int32)(listpos = ftell(f->fp)));
            listpos = (listpos + (RECTABLE_SIZE-1))
                    / RECTABLE_SIZE * RECTABLE_SIZE;
            SAFE(fseek(f->fp, listpos, SEEK_SET));

            /* Write out an empty descriptor table */
            SAFE(write_buffer(dummy_rectable, f));
            SAFE(fseek(f->fp, listpos, SEEK_SET));

            /* Write the `next' pointer (currently 0) and size */
            SAFE(write_int32(0, f));
            SAFE(write_int32(RECTABLE_SIZE, f));

            /* Write the new table offset into the old `next' pointer */
            SAFE(fseek(f->fp, oldpos, SEEK_SET));
            SAFE(write_int32(listpos, f));

            /* Seek to where the next record should be written */
            SAFE(fseek(f->fp, listpos + RECTABLE_SIZE, SEEK_SET));

            /* The first record is written at index 1 (don't smash the
             * next pointer) */
            recnum = 1;
        }

        /* If this is the last record, record a 0 to terminate the record
         * list and bail out */
        if (!record) {
            SAFE(fseek(f->fp, listpos + recnum*8, SEEK_SET));
            SAFE(write_int32(0, f));
            SAFE(write_int32(0, f));
            break;
        }

        /* Save the location of this record */
        SAFE((int32)(thispos = ftell(f->fp)));

        /* Seek to end of record, for writing strings */
        SAFE(fseek(f->fp, thispos + recsize, SEEK_SET));

        /* Write fields to record buffer and strings to file */
        for (i = 0; i < ti->nfields; i++) {
            const DBField *field = ti->fields[i].field;
            uint8 *dest = (uint8 *)recbuf + ti->fields[i].offset;
            if (field->load_only)
                continue;
            if (field->type == DBTYPE_STRING) {
                const char *string;
                uint32 strofs;
                get_dbfield(record, field, (void *)&string);
                SAFE((int32)(strofs = ftell(f->fp)));
                strofs -= thispos;
                dest[0] = strofs >> 24;
                dest[1] = strofs >> 16;
                dest[2] = strofs >>  8;
                dest[3] = strofs;
                SAFE(write_string(string, f));
            } else if (field->type == DBTYPE_PASSWORD) {
                Password pass;
                uint32 strofs;
                get_dbfield(record, field, (void *)&pass);
                SAFE((int32)(strofs = ftell(f->fp)));
                strofs -= thispos;
                dest[0] = strofs >> 24;
                dest[1] = strofs >> 16;
                dest[2] = strofs >>  8;
                dest[3] = strofs;
                SAFE(write_string(pass.cipher, f));
                memcpy(dest+4, pass.password, PASSMAX);
            } else if (field->type == DBTYPE_BUFFER) {
                get_dbfield(record, field, dest);
            } else {
                uint8 fieldbuf[16];  /* Big enough for any int or time type */
                get_dbfield(record, field, fieldbuf);
                switch (field->type) {
                  case DBTYPE_INT8:
                  case DBTYPE_UINT8:
                    dest[0] = *((uint8 *)fieldbuf);
                    break;
                  case DBTYPE_INT16:
                  case DBTYPE_UINT16:
                    dest[0] = *((uint16 *)fieldbuf) >> 8;
                    dest[1] = *((uint16 *)fieldbuf);
                    break;
                  case DBTYPE_INT32:
                  case DBTYPE_UINT32:
                    dest[0] = *((uint32 *)fieldbuf) >> 24;
                    dest[1] = *((uint32 *)fieldbuf) >> 16;
                    dest[2] = *((uint32 *)fieldbuf) >>  8;
                    dest[3] = *((uint32 *)fieldbuf);
                    break;
                  case DBTYPE_TIME:
#if SIZEOF_TIME_T >= 8
                    dest[0] = *((time_t *)fieldbuf) >> 56;
                    dest[1] = *((time_t *)fieldbuf) >> 48;
                    dest[2] = *((time_t *)fieldbuf) >> 40;
                    dest[3] = *((time_t *)fieldbuf) >> 32;
#else
                    /* Copy the sign bit into the four high bytes */
                    dest[0] = (int32) *((time_t *)fieldbuf) >> 31;
                    dest[1] = (int32) *((time_t *)fieldbuf) >> 31;
                    dest[2] = (int32) *((time_t *)fieldbuf) >> 31;
                    dest[3] = (int32) *((time_t *)fieldbuf) >> 31;
#endif
                    dest[4] = *((time_t *)fieldbuf) >> 24;
                    dest[5] = *((time_t *)fieldbuf) >> 16;
                    dest[6] = *((time_t *)fieldbuf) >>  8;
                    dest[7] = *((time_t *)fieldbuf);
                    break;
                  case DBTYPE_STRING:
                  case DBTYPE_BUFFER:
                  case DBTYPE_PASSWORD:
                    /* Handled above--listed here to avoid warnings */
                    break;
                }
            }
        }  /* for each field */

        /* Save the location of the next record */
        SAFE((int32)(nextpos = ftell(f->fp)));

        /* Write record pointer and length to table */
        SAFE(fseek(f->fp, listpos + recnum*8, SEEK_SET));
        SAFE(write_int32(thispos, f));
        SAFE(write_int32(nextpos-thispos, f));

        /* Write record buffer to file */
        SAFE(fseek(f->fp, thispos, SEEK_SET));
        if (fwrite(recbuf, recsize, 1, f->fp) != 1)
            goto fail;

        /* Seek to next record's starting position */
        SAFE(fseek(f->fp, nextpos, SEEK_SET));

        /* Move to the next record index */
        recnum = (recnum+1) % RECTABLE_LEN;
    }

    /* All done */
    free(recbuf);
    return 0;

  fail:
    free(recbuf);
    return -1;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

#ifndef INCLUDE_IN_VERSION4

ConfigDirective module_config[] = {
    { NULL }
};


static DBModule dbmodule_standard = {
    .load_table = standard_load_table,
    .save_table = standard_save_table,
};

/*************************************************************************/

int init_module(void)
{
    if (!register_dbmodule(&dbmodule_standard)) {
        module_log("Unable to register module with database core");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown)
{
    unregister_dbmodule(&dbmodule_standard);
    return 1;
}

/*************************************************************************/

#endif  /* !INCLUDE_IN_VERSION4 */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
