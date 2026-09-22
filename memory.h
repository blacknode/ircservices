/* Memory management include file.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef MEMORY_H
#define MEMORY_H

/*************************************************************************/

#if MEMCHECKS
# define FILELINE , const char *file, int line
#else
# define FILELINE /*nothing*/
#endif
extern void *smalloc(long size FILELINE);
extern void *scalloc(long els, long elsize FILELINE);
extern void *srealloc(void *oldptr, long newsize FILELINE);
extern char *sstrdup(const char *s FILELINE);
#undef FILELINE

#if MEMCHECKS

# if SHOWALLOCS
extern int showallocs;
# endif
extern void init_memory(void);
extern void uninit_memory(void);
extern void *MCmalloc(long size, const char *file, int line);
extern void *MCcalloc(long elsize, long els, const char *file, int line);
extern void *MCrealloc(void *oldptr, long newsize, const char *file, int line);
extern void MCfree(void *ptr, const char *file, int line);
extern char *MCstrdup(const char *s, const char *file, int line);

#else  /* !MEMCHECKS */

# define init_memory() /*nothing*/
# define uninit_memory() /*nothing*/

#endif

/*************************************************************************/

#if MEMCHECKS && !defined(NO_MEMREDEF)

# undef malloc
# undef calloc
# undef realloc
# undef free
# undef strdup

# define malloc(size)        MCmalloc(size,__FILE__,__LINE__)
# define calloc(elsize,els)  MCcalloc(elsize,els,__FILE__,__LINE__)
# define realloc(ptr,size)   MCrealloc(ptr,size,__FILE__,__LINE__)
# define free(ptr)           MCfree(ptr,__FILE__,__LINE__)
# define strdup(str)         MCstrdup(str,__FILE__,__LINE__)

# define smalloc(size)       smalloc(size,__FILE__,__LINE__)
# define scalloc(elsize,els) scalloc(elsize,els,__FILE__,__LINE__)
# define srealloc(ptr,size)  srealloc(ptr,size,__FILE__,__LINE__)
# define sstrdup(str)        sstrdup(str,__FILE__,__LINE__)

#endif  /* MEMCHECKS && !NO_MEMREDEF */

/*************************************************************************/

#endif /* MEMORY_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
