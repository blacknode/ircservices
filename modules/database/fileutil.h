/* Database file descriptor structure and file handling routine prototypes.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef DATABASE_FILEUTIL_H
#define DATABASE_FILEUTIL_H

/*************************************************************************/

typedef struct dbFILE_ dbFILE;

/* Callers may access fields in this structure directly, but MUST NOT
 * modify them. */
struct dbFILE_ {
    char mode;                  /* 'r' for reading, 'w' for writing */
    FILE *fp;                   /* The file pointer itself */
    char filename[PATH_MAX+1];  /* Name of the database file */
    char tempname[PATH_MAX+1];  /* Name of the temporary file (for writing) */
};

/*************************************************************************/

/* Prototypes and macros: */

E int32 get_file_version(dbFILE *f);
E int write_file_version(dbFILE *f, int32 filever);
E dbFILE *open_db(const char *filename, const char *mode, int32 version);
E void restore_db(dbFILE *f);   /* Restore to state before open_db() */
E int close_db(dbFILE *f);
#define read_db(f,buf,len)      (fread((buf),1,(len),(f)->fp))
#define write_db(f,buf,len)     (fwrite((buf),1,(len),(f)->fp))
#define getc_db(f)              (fgetc((f)->fp))

E int read_int8(int8 *ret, dbFILE *f);
E int read_uint8(uint8 *ret, dbFILE *f);
E int write_int8(int8 val, dbFILE *f);
E int read_int16(int16 *ret, dbFILE *f);
E int read_uint16(uint16 *ret, dbFILE *f);
E int write_int16(int16 val, dbFILE *f);
E int read_int32(int32 *ret, dbFILE *f);
E int read_uint32(uint32 *ret, dbFILE *f);
E int write_int32(int32 val, dbFILE *f);
E int read_time(time_t *ret, dbFILE *f);
E int write_time(time_t val, dbFILE *f);
E int read_ptr(void **ret, dbFILE *f);
E int write_ptr(const void *ptr, dbFILE *f);
E int read_string(char **ret, dbFILE *f);
E int write_string(const char *s, dbFILE *f);

#define read_buffer(buf,f) \
    (read_db((f),(buf),sizeof(buf)) == sizeof(buf) ? 0 : -1)
#define write_buffer(buf,f) \
    (write_db((f),(buf),sizeof(buf)) == sizeof(buf) ? 0 : -1)

/*************************************************************************/

#endif  /* DATABASE_FILEUTIL_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
