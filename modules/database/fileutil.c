/* Database file handling routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#ifndef CONVERT_DB
# include "modules.h"
#endif
#include <fcntl.h>

#include "fileutil.h"

/*************************************************************************/
/*************************************************************************/

/* Return the version number of the file.  Return -1 if there is no version
 * number or the number doesn't make sense (i.e. less than 1).
 */

int32 get_file_version(dbFILE *f)
{
    FILE *fp = f->fp;
    int version = fgetc(fp)<<24 | fgetc(fp)<<16 | fgetc(fp)<<8 | fgetc(fp);
    if (ferror(fp)) {
#ifndef CONVERT_DB
        module_log_perror("Error reading version number on %s", f->filename);
#endif
        return -1;
    } else if (feof(fp)) {
#ifndef CONVERT_DB
        module_log("Error reading version number on %s: End of file detected",
                   f->filename);
#endif
        return -1;
    } else if (version < 1) {
#ifndef CONVERT_DB
        module_log("Invalid version number (%d) on %s", version, f->filename);
#endif
        return -1;
    }
    return version;
}

/*************************************************************************/

/* Write the version number to the file.  Return 0 on success, -1 on
 * failure.
 */

int write_file_version(dbFILE *f, int32 filever)
{
    FILE *fp = f->fp;
    if (
        fputc(filever>>24 & 0xFF, fp) < 0 ||
        fputc(filever>>16 & 0xFF, fp) < 0 ||
        fputc(filever>> 8 & 0xFF, fp) < 0 ||
        fputc(filever     & 0xFF, fp) < 0
    ) {
#ifndef CONVERT_DB
        module_log_perror("Error writing version number on %s", f->filename);
#endif
        return -1;
    }
    return 0;
}

/*************************************************************************/
/*************************************************************************/

/* Helper functions for open_db(). */

static dbFILE *open_db_read(const char *filename)
{
    dbFILE *f;
    FILE *fp;

    f = smalloc(sizeof(*f));
    *f->tempname = 0;
    strbcpy(f->filename, filename);
    f->mode = 'r';
    fp = fopen(f->filename, "rb");
    if (!fp) {
        int errno_save = errno;
#ifndef CONVERT_DB
        if (errno != ENOENT)
            module_log_perror("Can't read database file %s", f->filename);
#endif
        free(f);
        errno = errno_save;
        return NULL;
    }
    f->fp = fp;
    return f;
}

/************************************/

static dbFILE *open_db_write(const char *filename, int32 filever)
{
    dbFILE *f;
    int fd;

    f = smalloc(sizeof(*f));
    *f->tempname = 0;
    strbcpy(f->filename, filename);
    filename = f->filename;
    f->mode = 'w';

    snprintf(f->tempname, sizeof(f->tempname), "%s.new", filename);
    if (!*f->tempname || strcmp(f->tempname, filename) == 0) {
#ifndef CONVERT_DB
        module_log("Opening database file %s for write: Filename too long",
                   filename);
#endif
        free(f);
        errno = ENAMETOOLONG;
        return NULL;
    }
    remove(f->tempname);
    /* Use open() to avoid people sneaking a new file in under us */
    fd = open(f->tempname, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd >= 0)
        f->fp = fdopen(fd, "wb");
    if (!f->fp || write_file_version(f, filever) < 0) {
        int errno_save = errno;
#ifndef CONVERT_DB
        static int walloped = 0;
        if (!walloped) {
            walloped++;
            wallops(NULL, "Can't create temporary database file %s",
                    f->tempname);
        }
        errno = errno_save;
        module_log_perror("Can't create temporary database file %s",
                          f->tempname);
#endif
        if (f->fp)
            fclose(f->fp);
        remove(f->tempname);
        errno = errno_save;
        return NULL;
    }
    return f;
}

/*************************************************************************/

/* Open a database file for reading (*mode == 'r') or writing (*mode == 'w').
 * Return the stream pointer, or NULL on error.  When opening for write, the
 * file actually opened is a temporary file, which will be renamed to the
 * original file on close.
 *
 * `version' is only used when opening a file for writing, and indicates the
 * version number to write to the file.
 */

dbFILE *open_db(const char *filename, const char *mode, int32 version)
{
    if (*mode == 'r') {
        return open_db_read(filename);
    } else if (*mode == 'w') {
        return open_db_write(filename, version);
    } else {
        errno = EINVAL;
        return NULL;
    }
}

/*************************************************************************/

/* Close a database file.  If the file was opened for write, moves the new
 * file over the old one, and logs/wallops an error message if the rename()
 * fails.
 */

int close_db(dbFILE *f)
{
    int res;
    if (!f->fp) {
        errno = EINVAL;
        return -1;
    }
    res = fclose(f->fp);
    f->fp = NULL;
    if (res != 0)
        return -1;
    if (f->mode=='w' && *f->tempname && strcmp(f->tempname,f->filename)!=0) {
        if (rename(f->tempname, f->filename) < 0) {
#ifndef CONVERT_DB
            int errno_save = errno;
            wallops(NULL, "Unable to move new data to database file %s;"
                    " new data NOT saved.", f->filename);
            errno = errno_save;
            module_log_perror("Unable to move new data to database file %s;"
                              " new data NOT saved.", f->filename);
#endif
            remove(f->tempname);
        }
    }
    free(f);
    return 0;
}

/*************************************************************************/

/* Restore the database file to its condition before open_db().  This is
 * identical to close_db() for files open for reading; however, for files
 * open for writing, we discard the new temporary file instead of renaming
 * it over the old file.  The value of errno is preserved.
 */

void restore_db(dbFILE *f)
{
    int errno_orig = errno;
    if (f->fp)
        fclose(f->fp);
    if (f->mode == 'w' && *f->tempname)
        remove(f->tempname);
    free(f);
    errno = errno_orig;
}

/*************************************************************************/
/*************************************************************************/

/* Read and write 2- and 4-byte quantities, pointers, and strings.  All
 * multibyte values are stored in big-endian order (most significant byte
 * first).  A pointer is stored as a byte, either 0 if NULL or 1 if not,
 * and read pointers are returned as either (void *)0 or (void *)1.  A
 * string is stored with a 2-byte unsigned length (including the trailing
 * \0) first; a length of 0 indicates that the string pointer is NULL.
 * Written strings are truncated silently at 65534 bytes, and are always
 * null-terminated.
 *
 * All routines return -1 on error, 0 otherwise.
 */

/*************************************************************************/

int read_int8(int8 *ret, dbFILE *f)
{
    int c = fgetc(f->fp);
    if (c == EOF)
        return -1;
    *ret = c;
    return 0;
}

/* Alternative version of read_int8() to avoid GCC's pointer signedness
 * warnings when reading into an unsigned variable: */
int read_uint8(uint8 *ret, dbFILE *f) {
    return read_int8((int8 *)ret, f);
}

int write_int8(int8 val, dbFILE *f)
{
    if (fputc(val, f->fp) == EOF)
        return -1;
    return 0;
}

/*************************************************************************/

/* These are inline to help out {read,write}_string. */

inline int read_int16(int16 *ret, dbFILE *f)
{
    int c1, c2;

    c1 = fgetc(f->fp);
    c2 = fgetc(f->fp);
    if (c2 == EOF)
        return -1;
    *ret = c1<<8 | c2;
    return 0;
}

inline int read_uint16(uint16 *ret, dbFILE *f) {
    return read_int16((int16 *)ret, f);
}

inline int write_int16(int16 val, dbFILE *f)
{
    fputc((val>>8), f->fp);
    if (fputc(val, f->fp) == EOF)
        return -1;
    return 0;
}

/*************************************************************************/

int read_int32(int32 *ret, dbFILE *f)
{
    int c1, c2, c3, c4;

    c1 = fgetc(f->fp);
    c2 = fgetc(f->fp);
    c3 = fgetc(f->fp);
    c4 = fgetc(f->fp);
    if (c4 == EOF)
        return -1;
    *ret = c1<<24 | c2<<16 | c3<<8 | c4;
    return 0;
}

int read_uint32(uint32 *ret, dbFILE *f) {
    return read_int32((int32 *)ret, f);
}

int write_int32(int32 val, dbFILE *f)
{
    fputc((val>>24), f->fp);
    fputc((val>>16), f->fp);
    fputc((val>> 8), f->fp);
    if (fputc((val & 0xFF), f->fp) == EOF)
        return -1;
    return 0;
}

/*************************************************************************/

int read_time(time_t *ret, dbFILE *f)
{
    int32 high, low;
    if (read_int32(&high, f) < 0 || read_int32(&low, f) < 0)
        return -1;
#if SIZEOF_TIME_T > 4
    *ret = (time_t)high << 32 | (time_t)low;
#else
    *ret = low;
#endif
    return 0;
}

int write_time(time_t val, dbFILE *f)
{
#if SIZEOF_TIME_T > 4
    if (write_int32(val>>32, f) < 0
     || write_int32(val & (time_t)0xFFFFFFFF, f) < 0)
#else
    if (write_int32(0, f) < 0 || write_int32(val, f) < 0)
#endif
        return -1;
    return 0;
}

/*************************************************************************/

int read_ptr(void **ret, dbFILE *f)
{
    int c;

    c = fgetc(f->fp);
    if (c == EOF)
        return -1;
    *ret = (c ? (void *)1 : (void *)0);
    return 0;
}

int write_ptr(const void *ptr, dbFILE *f)
{
    if (fputc(ptr ? 1 : 0, f->fp) == EOF)
        return -1;
    return 0;
}

/*************************************************************************/

int read_string(char **ret, dbFILE *f)
{
    char *s;
    uint16 len;

    if (read_uint16(&len, f) < 0)
        return -1;
    if (len == 0) {
        *ret = NULL;
        return 0;
    }
    s = smalloc(len);
    if (len != fread(s, 1, len, f->fp)) {
        free(s);
        return -1;
    }
    *ret = s;
    return 0;
}

int write_string(const char *s, dbFILE *f)
{
    uint32 len;

    if (!s)
        return write_int16(0, f);
    len = strlen(s);
    if (len > 65534)
        len = 65534;
    if (write_int16((uint16)(len+1), f) < 0)
        return -1;
    if (fwrite(s, 1, len, f->fp) != len)
        return -1;
    if (fputc(0, f->fp) == EOF)
        return -1;
    return 0;
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
