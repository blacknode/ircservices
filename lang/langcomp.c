/* Compiler for language definition files.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

/*
 * A language definition file contains all strings which Services sends to
 * users in a particular language.  A language file may contain comments
 * (lines beginning with "#"--note that inline comments are not allowed!)
 * and blank lines.  All other lines must adhere to the following format:
 *
 * Each string definition begins with the C name of a message (as defined
 * in the file "index"--see below).  This must be alone on a line, preceded
 * and followed by no blank space.  Following this line are zero or more
 * lines of text; each line of text must begin with exactly one tab
 * character, which is discarded.  Newlines are retained in the strings,
 * except the last newline in the text, which is discarded.  A message with
 * no text is replaced by a null pointer in the array (not an empty
 * string).
 *
 * All messages in the program are listed, one per line, in the "index"
 * file.  No comments or blank lines are permitted in that file.  The index
 * file can be generated from a language file with a command like:
 *      grep '^[A-Z]' en_us.l >index
 *
 * This program takes one parameter, the name of the language file.  It
 * generates a compiled language file whose name is created by removing any
 * extension on the source file on the input filename.
 *
 * You may also pass a "-w" option to print warnings for missing strings.
 *
 * This program isn't very flexible, because it doesn't need to be, but
 * anyone who wants to try making it more flexible is welcome to.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* CR/LF values--used instead of '\r' and '\n' to avoid platform-dependent
 * messiness */
#define CR ((char)13)
#define LF ((char)10)

int numstrings = 0;     /* Number of strings we should have */
char **stringnames;     /* Names of the strings (from index file) */
char **strings;         /* Strings we have loaded */

int linenum = 0;        /* Current line number in input file */

/*************************************************************************/

/* Read the index file and load numstrings and stringnames.  Return -1 on
 * error, 0 on success. */

static int read_index_file(void)
{
    FILE *f;
    char buf[256];
    int i;

    if (!(f = fopen("index", "r"))) {
        perror("fopen(index)");
        return -1;
    }
    while (fgets(buf, sizeof(buf), f))
        numstrings++;
    if (!(stringnames = calloc(sizeof(char *), numstrings))) {
        perror("calloc(stringnames)");
        return -1;
    }
    if (!(strings = calloc(sizeof(char *), numstrings))) {
        perror("calloc(strings)");
        return -1;
    }
    fseek(f, 0, SEEK_SET);
    i = 0;
    while (fgets(buf, sizeof(buf), f)) {
        if (buf[strlen(buf)-1] == LF)
            buf[strlen(buf)-1] = 0;
        if (buf[strlen(buf)-1] == CR)
            buf[strlen(buf)-1] = 0;
        if (!(stringnames[i++] = strdup(buf))) {
            perror("strdup()");
            return -1;
        }
    }
    fclose(f);
    return 0;
}

/*************************************************************************/

/* Return the index of a string name in stringnames, or -1 if not found. */

static int stringnum(const char *name)
{
    int i;

    for (i = 0; i < numstrings; i++) {
        if (strcmp(stringnames[i], name) == 0)
            return i;
    }
    return -1;
}

/*************************************************************************/

/* Read a non-comment, non-blank line from the input file.  Return NULL at
 * end of file. */

static char *readline(FILE *f)
{
    static char buf[1024];
    char *s;

    do {
        if (!(fgets(buf, sizeof(buf), f)))
            return NULL;
        linenum++;
    } while (*buf == '#' || *buf == CR || *buf == LF);
    s = buf + strlen(buf)-1;
    if (*s == LF)
        *s-- = 0;
    if (*s == CR)
        *s = 0;
    return buf;
}

/*************************************************************************/

/* Write a 32-bit value to a file in big-endian order.  Returns 0 on
 * success, -1 on error.
 */

static int fput32(long val, FILE *f)
{
    if (fputc(val>>24, f) == EOF ||
        fputc(val>>16, f) == EOF ||
        fputc(val>> 8, f) == EOF ||
        fputc(val    , f) == EOF
    ) {
        return -1;
    } else {
        return 0;
    }
}

/*************************************************************************/

int main(int ac, char **av)
{
    char *filename = NULL, *s;
    char langname[254], outfile[256];
    FILE *in, *out;
    int warn = 0;
    int retval = 0;
    int curstring = -2, i;
    char *line;
    int maxerr = 50;    /* Max errors before we bail out */
    long pos, totalsize;

    if (ac >= 2 && strcmp(av[1], "-w") == 0) {
        warn = 1;
        av[1] = av[2];
        ac--;
    }
    if (ac != 2) {
        fprintf(stderr, "Usage: %s [-w] <lang-file>\n", av[0]);
        return 1;
    }
    filename = av[1];
    s = strrchr(filename, '.');
    if (!s)
        s = filename + strlen(filename);
    if (s-filename > sizeof(langname)-3)
        s = filename + sizeof(langname)-1;
    strncpy(langname, filename, s-filename);
    langname[s-filename] = 0;
    sprintf(outfile, "%s", langname);

    if (read_index_file() < 0)
        return 1;
    if (!(in = fopen(filename, "r"))) {
        perror(filename);
        return 1;
    }
    if (!(out = fopen(outfile, "w"))) {
        perror(outfile);
        return 1;
    }

    while (maxerr > 0 && (line = readline(in)) != NULL) {
        if (*line == '\t') {
            if (curstring == -2) {
                fprintf(stderr, "%s:%d: Junk at beginning of file\n",
                        filename, linenum);
                retval = 1;
            } else if (curstring >= 0) {
                line++;
                i = strings[curstring] ? strlen(strings[curstring]) : 0;
                if (!(strings[curstring] =
                        realloc(strings[curstring], i+strlen(line)+2))) {
                    fprintf(stderr, "%s:%d: Out of memory!\n",
                            filename, linenum);
                    return 2;
                }
                sprintf(strings[curstring]+i, "%s\n", line);
            }

        } else {
            if ((curstring = stringnum(line)) < 0) {
                fprintf(stderr, "%s:%d: Unknown string name `%s'\n",
                        filename, linenum, line);
                retval = 1;
                maxerr--;
            } else if (strings[curstring]) {
                fprintf(stderr, "%s:%d: Duplicate occurrence of string `%s'\n",
                        filename, linenum, line);
                retval = 1;
                maxerr--;
            } else {
                if (!(strings[curstring] = malloc(1))) {
                    fprintf(stderr, "%s:%d: Out of memory!\n",
                            filename, linenum);
                    return 2;
                }
                *strings[curstring] = 0;
            }

        }
    }

    fclose(in);

    if (retval != 0) {
        if (maxerr == 0)
            fprintf(stderr, "%s:%d: Too many errors!\n", filename, linenum);
        fclose(out);
        unlink(outfile);
        return retval;
    }

    totalsize = 0;
    for (i = 0; i < numstrings; i++) {
        if (strings[i]) {
            if (*strings[i])
                strings[i][strlen(strings[i])-1] = 0;  /* kill last \n */
            if (*strings[i])
                totalsize += strlen(strings[i]) + 1;
        } else if (warn) {
            fprintf(stderr, "%s: String `%s' missing\n", filename,
                        stringnames[i]);
        }
    }

    if (fput32(numstrings, out) < 0 || fput32(totalsize, out) < 0) {
        perror("fwrite()");
        retval = 1;
    }
    for (i = 0; i < numstrings && retval == 0; i++) {
        if (strings[i] && *strings[i]) {
            if (fwrite(strings[i], strlen(strings[i])+1, 1, out) != 1) {
                perror("fwrite()");
                retval = 1;
            }
        }
    }
    pos = 0;
    for (i = 0; i < numstrings && retval == 0; i++) {
        if (strings[i] && *strings[i]) {
            if (fput32(pos, out) < 0) {
                perror("fwrite()");
                retval = 1;
            }
            pos += strlen(strings[i]) + 1;
        } else {
            if (fput32(-1, out) < 0) {
                perror("fwrite()");
                retval = 1;
            }
        }
    }

    if (fclose(out) == EOF && retval == 0) {
        perror("fclose()");
        retval = 1;
    }
    if (retval)
        unlink(outfile);
    return retval;
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
