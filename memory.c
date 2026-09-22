/* Memory management routines.
 * Leak checking based on code by Kelmar (2001/1/13).
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#define NO_MEMREDEF
#include "services.h"

/*************************************************************************/
/*************************************************************************/

/* smalloc, scalloc, srealloc, sstrdup:
 *      Versions of the memory allocation functions which will cause the
 *      program to terminate with an "Out of memory" error if the memory
 *      cannot be allocated.  (Hence, the return value from these functions
 *      is never NULL, except for smalloc()/scalloc() called with a size of
 *      zero.)
 */

#if MEMCHECKS
# define FILELINE , const char *file, int line
# define xmalloc(a)    MCmalloc(a,file,line)
# define xsmalloc(a)   smalloc(a,file,line)
# define xcalloc(a,b)  MCcalloc(a,b,file,line)
# define xrealloc(a,b) MCrealloc(a,b,file,line)
# define xfree(a)      MCfree(a,file,line)
#else
# define FILELINE /*nothing*/
# define xmalloc(a)    malloc(a)
# define xsmalloc(a)   smalloc(a)
# define xcalloc(a,b)  calloc(a,b)
# define xrealloc(a,b) realloc(a,b)
# define xfree(a)      free(a)
#endif

/*************************************************************************/

void *smalloc(long size FILELINE)
{
    void *buf;

    if (size == 0)
        return NULL;
    buf = xmalloc(size);
    if (buf == NULL)
        raise(SIGUSR1);
    return buf;
}

/*************************************************************************/

void *scalloc(long els, long elsize FILELINE)
{
    void *buf;

    if (elsize == 0 || els == 0)
        return NULL;
    buf = xcalloc(elsize, els);
    if (buf == NULL)
        raise(SIGUSR1);
    return buf;
}

/*************************************************************************/

void *srealloc(void *oldptr, long newsize FILELINE)
{
    void *buf;

    if (newsize == 0) {
        xfree(oldptr);
        return NULL;
    }
    buf = xrealloc(oldptr, newsize);
    if (buf == NULL)
        raise(SIGUSR1);
    return buf;
}

/*************************************************************************/

char *sstrdup(const char *s FILELINE)
{
    char *t = xsmalloc(strlen(s) + 1);
    strcpy(t, s);  /* safe, obviously */
    return t;
}

/*************************************************************************/
/************ Everything from here down is MEMCHECKS-related. ************/
/*************************************************************************/

#if MEMCHECKS

/*************************************************************************/

static long allocated = 0;      /* Amount of memory currently allocated */
static long runtime = 0;        /* `allocated' value at init_memory() time */

# if SHOWALLOCS
int showallocs = 1;             /* Actually log allocations? */
# endif

typedef struct _smemblock {
    long size;                  /* Size of this block */
    int32 sig;                  /* Signature word: 0x5AFEC0DE */
} MemBlock;
# define SIGNATURE      0x5AFEC0DE
# define FREE_SIGNATURE 0xDEADBEEF      /* Used for freed memory */
# define NEW_FILL       0xD017D017      /* New allocs are filled with this */
# define FREE_FILL      0xBEEF1E57      /* Freed memory is filled with this */

# define MEMBLOCK_TO_PTR(mb)  ((void *)((char *)(mb) + sizeof(MemBlock)))
# define PTR_TO_MEMBLOCK(ptr) ((MemBlock *)((char *)(ptr) - sizeof(MemBlock)))


/*************************************************************************/
/*************************************************************************/

/* Leak-checking initialization and exit code. */

static void show_leaks(void)
{
    if (runtime >= 0 && (allocated - runtime) > 0) {
        log("SAFEMEM: There were %ld bytes leaked on exit!",
            (allocated - runtime));
    } else {
        log("SAFEMEM: No memory leaks detected.");
    }
}

void init_memory(void)
{
    runtime = allocated;
    log("init_memory(): runtime = %ld", runtime);
    atexit(show_leaks);
}

/* Used to avoid memory-leak message from the parent process */
void uninit_memory(void)
{
    runtime = -1;
}

/*************************************************************************/

/* Helper to fill memory with a given 32-bit value. */

static inline void fill32(void *ptr, uint32 value, long size)
{
    register uint32 *ptr32 = ptr;
    register uint32 v = value;
    while (size >= 4) {
        *ptr32++ = v;
        size -= 4;
    }
    if (size > 0)
        memcpy(ptr32, &value, size);
}

/*************************************************************************/
/*************************************************************************/

/* Substitutes for malloc() and friends.  memory.h redefines malloc(), etc.
 * to these functions if MEMCHECKS is defined. */

/*************************************************************************/

void *MCmalloc(long size, const char *file, int line)
{
    MemBlock *mb;
    void *data;

    if (size == 0)
        return NULL;
    mb = malloc(size + sizeof(MemBlock));
    if (mb) {
        mb->size = size;
        mb->sig = SIGNATURE;
        data = MEMBLOCK_TO_PTR(mb);
        allocated += size;
# if SHOWALLOCS
        if (showallocs) {
            log("smalloc(): Allocated %ld bytes at %p (%s:%d)",
                size, data, file, line);
        }
# endif
        fill32(data, NEW_FILL, size);
        return data;
    } else {
# if SHOWALLOCS
        if (showallocs) {
            log("mcalloc(): Unable to allocate %ld bytes (%s:%d)",
                size, file, line);
        }
# endif
        return NULL;
    }
}

/*************************************************************************/

void *MCcalloc(long els, long elsize, const char *file, int line)
{
    MemBlock *mb;
    void *data;

    if (elsize == 0 || els == 0)
        return NULL;
    mb = malloc(els*elsize + sizeof(MemBlock));
    if (mb) {
        mb->size = elsize * els;
        mb->sig = SIGNATURE;
        data = MEMBLOCK_TO_PTR(mb);
        memset(data, 0, els*elsize);
        allocated += mb->size;
# if SHOWALLOCS
        if (showallocs) {
            log("scalloc(): Allocated %ld bytes at %p (%s:%d)",
                els*elsize, data, file, line);
        }
# endif
        return data;
    } else {
# if SHOWALLOCS
        if (showallocs) {
            log("scalloc(): Unable to allocate %ld bytes (%s:%d)",
                els*elsize, file, line);
        }
# endif
        return NULL;
    }
}

/*************************************************************************/

void *MCrealloc(void *oldptr, long newsize, const char *file, int line)
{
    MemBlock *newb, *oldb;
    long oldsize;
    void *data;

    if (newsize == 0) {
        MCfree(oldptr, file, line);
        return NULL;
    }
    if (oldptr == NULL)
        return MCmalloc(newsize, file, line);
    oldb = PTR_TO_MEMBLOCK(oldptr);
    if (oldb->sig != SIGNATURE) {
        fatal("Attempt to realloc() an invalid pointer (%p) (%s:%d)",
              oldptr, file, line);
    }
    oldsize = oldb->size;
    newb = realloc(oldb, newsize + sizeof(MemBlock));
    if (newb) {
        newb->size = newsize;
        newb->sig = SIGNATURE;  /* should already be set... */
        data = MEMBLOCK_TO_PTR(newb);
        allocated += (newsize - oldsize);
# if SHOWALLOCS
        if (showallocs) {
            log("srealloc(): Adjusted %ld bytes (%p) to %ld bytes (%p)"
                " (%s:%d)", oldsize, oldptr, newsize, data, file, line);
        }
# endif
        return data;
    } else {
# if SHOWALLOCS
        if (showallocs) {
            log("srealloc(): Unable to adjust %ld bytes (%p) to %ld bytes"
                " (%s:%d)", oldsize, oldptr, newsize, file, line);
        }
# endif
        return NULL;
    }
}

/*************************************************************************/

void MCfree(void *ptr, const char *file, int line)
{
    MemBlock *mb;

    if (ptr == NULL)
        return;
    mb = PTR_TO_MEMBLOCK(ptr);
    if (mb->sig != SIGNATURE) {
        fatal("Attempt to free() an invalid pointer (%p) (%s:%d)",
              ptr, file, line);
    }
    allocated -= mb->size;
# if SHOWALLOCS
    if (showallocs) {
        log("sfree(): Released %ld bytes at %p (%s:%d)",
            mb->size, ptr, file, line);
    }
# endif
    mb->size = FREE_FILL;
    mb->sig = FREE_SIGNATURE;
    fill32(ptr, FREE_FILL, mb->size);
    free(mb);
}

/*************************************************************************/

char *MCstrdup(const char *s, const char *file, int line)
{
    char *t = MCmalloc(strlen(s) + 1, file, line);
    strcpy(t, s);  /* safe, obviously */
    return t;
}

/*************************************************************************/

#endif  /* MEMCHECKS */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
