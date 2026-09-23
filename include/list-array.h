/* Macros to handle insertion, deletion, iteration, and searching for
 * linked lists and arrays.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef LIST_ARRAY_H
#define LIST_ARRAY_H

/*************************************************************************/

/* Comments on the efficiency of lists vs. arrays:
 *
 * Lists are simple to insert into and remove from, but searching can be
 * inefficient, as the list must be walked through each time.  Arrays can
 * be searched quickly, particularly if the list is sorted (note that the
 * macros below do not support sorted arrays), but insertion and removal
 * are expensive, requiring data to be moved around in memory if the
 * element to be inserted or removed is not last in the array or if
 * reallocation of the array results in a different array pointer.  Lists
 * also have the advantage that the list can be easily modified (items
 * inserted or deleted) within a FOREACH loop, while arrays require extra
 * logic to modify the counter variable after an insert or delete.
 *
 * As far as searching goes, arrays do not show a significant improvement
 * until the data set size reaches around 15 elements, at which point the
 * reduced number of memory accesses (and reduced cache pressure,
 * particularly with large data sets) gives arrays a clear advantage.  The
 * following results were obtained from running the test program below on a
 * Celeron 1.7GHz system (compiled with gcc-4.1 -O3):
 *
 *      | Time/search (ns)
 * Size |   List  |  Array
 * -----+-------------------
 *    3 |     4.1 |    3.7
 *    5 |     7.3 |    6.7
 *    7 |     9.7 |    9.3
 *   10 |    14.5 |   12.3
 *   15 |    23.6 |   18.2
 *   20 |    53.5 |   21.7
 *   30 |    73.2 |   44.8
 *   50 |   109   |   56.7
 *  100 |   200   |  101
 *  300 |   564   |  267
 * 1000 |  4530   |  864
 * 3000 | 13740   | 4440
 *
 * Test program (outputs average time per trial):
 *
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(int ac, char **av)
{
    int size = atoi(av[1]), runs = atoi(av[2]), trials = ac>3?atoi(av[3]):1;
    struct p_s {struct p_s *next; int a,b; } *p, *p2;
    struct { int a,b; } *q;
    int i, j, t;
    struct rusage ru1, ru2, ru3, ru4;
    int list = 0, array = 0;

    p = p2 = malloc(sizeof(*p));
    q = malloc(sizeof(*q) * size);
    for (i = 0; i < size; i++) {
        p2->b = q[i].b = i;
        p2 = (p2->next = malloc(sizeof(*p2)));
    }
    for (t = 0; t < trials+1; t++) {
        for (p2 = p; p2->b != size-1; p2 = p2->next)
            ;
        getrusage(RUSAGE_SELF, &ru1);
        for (i = 0; i < runs; i++)
            for (p2 = p; p2->b != size-1; p2 = p2->next)
                ;
        getrusage(RUSAGE_SELF, &ru2);
        for (j = 0; q[j].b != size-1; j++)
            ;
        getrusage(RUSAGE_SELF, &ru3);
        for (i = 0; i < runs; i++)
            for (j = 0; q[j].b != size-1; j++)
                ;
        getrusage(RUSAGE_SELF, &ru4);
#define TIME(ru) ((ru).ru_utime.tv_sec*1000 + (ru).ru_utime.tv_usec/1000)
        if (t > 0) {  // skip first trial for cache loading
            list += TIME(ru2) - TIME(ru1);
            array += TIME(ru4) - TIME(ru3);
        }
    }
    printf("List : %d ms\nArray: %d ms\n",
           (list*2+trials)/(trials*2), (array*2+trials)/(trials*2));
    return 0;
}
 */

/*************************************************************************/
/*************************************************************************/

/* Remove anything defined by system headers. */

#undef LIST_INSERT
#undef LIST_INSERT_ORDERED
#undef LIST_REMOVE
#undef LIST_FOREACH
#undef LIST_FOREACH_SAFE
#undef LIST_SEARCH
#undef LIST_SEARCH_SCALAR
#undef LIST_SEARCH_ORDERED
#undef LIST_SEARCH_ORDERED_SCALAR
#undef ARRAY2_EXTEND
#undef ARRAY2_INSERT
#undef ARRAY2_REMOVE
#undef ARRAY2_FOREACH
#undef ARRAY2_SEARCH
#undef ARRAY2_SEARCH_SCALAR
#undef ARRAY2_SEARCH_PLAIN_SCALAR
#undef ARRAY_EXTEND
#undef ARRAY_INSERT
#undef ARRAY_REMOVE
#undef ARRAY_FOREACH
#undef ARRAY_SEARCH
#undef ARRAY_SEARCH_SCALAR
#undef ARRAY_SEARCH_PLAIN_SCALAR

/*************************************************************************/
/*************************************************************************/

/* List macros.  In these macros, `list' must be an lvalue (with no side
 * effects) of the same type as the nodes in the list. */

/*************************************************************************/

/* Insert `node' into the beginning of `list'.  Insertion is performed in
 * constant time. */

#define LIST_INSERT(node_,list)         \
    do {                                \
        typeof(list) _node = (node_);   \
        _node->next = (list);           \
        _node->prev = NULL;             \
        if (list)                       \
            (list)->prev = _node;       \
        (list) = _node;                 \
    } while (0)

/*************************************************************************/

/* Append `node' to the end of `list'.  Insertion is performed in linear
 * time with the length of the list. */

#define LIST_APPEND(node_,list)                 \
    do {                                        \
        typeof(list) _node = (node_);           \
        typeof(_node) *_nextptr = &(list);      \
        typeof(_node) _prev = NULL;             \
        while (*_nextptr) {                     \
            _prev = *_nextptr;                  \
            _nextptr = &((*_nextptr)->next);    \
        }                                       \
        *_nextptr = _node;                      \
        _node->prev = _prev;                    \
        _node->next = NULL;                     \
    } while (0)

/*************************************************************************/

/* Insert `node' into `list' so that `list' maintains its order as
 * determined by the function `compare' called on the field `field' of each
 * node.  `field' must be a field of `node', and `compare' must be a
 * function that takes two `field' values and returns -1, 0, or 1
 * indicating whether the first argument is ordered before, equal to, or
 * after the second (strcmp(), for example).  If an equal node is found,
 * `node' is inserted after it.  Insertion is performed in linear time
 * with the length of the list. */

#define LIST_INSERT_ORDERED(node_,list,compare,field)   \
    do {                                                \
        typeof(list) _node = (node_);                   \
        typeof(_node) _ptr, _prev;                      \
        for (_ptr = (list), _prev = NULL; _ptr;         \
             _prev = _ptr, _ptr = _ptr->next            \
        ) {                                             \
            if (compare(_node->field, _ptr->field) < 0) \
                break;                                  \
        }                                               \
        _node->next = _ptr;                             \
        _node->prev = _prev;                            \
        if (_ptr)                                       \
            _ptr->prev = _node;                         \
        if (_prev)                                      \
            _prev->next = _node;                        \
        else                                            \
            (list) = _node;                             \
    } while (0)

/*************************************************************************/

/* Remove `node' from `list'.  Removal is performed in constant time. */

#define LIST_REMOVE(node_,list)                 \
    do {                                        \
        typeof(list) _node = (node_);           \
        if (_node->next)                        \
            _node->next->prev = _node->prev;    \
        if (_node->prev)                        \
            _node->prev->next = _node->next;    \
        else                                    \
            (list) = _node->next;               \
    } while (0)

/*************************************************************************/

/* Iterate over every element in `list', using `iter' as the iterator.  The
 * macro has the same properties as a for() loop.  `iter' must be an
 * lvalue. */

#define LIST_FOREACH(iter,list) \
    for ((iter) = (list); (iter); (iter) = (iter)->next)

/*************************************************************************/

/* Iterate over `list' using an extra variable (`temp') to hold the next
 * element, ensuring proper operation even when the current element is
 * deleted.  `iter' and `temp' must be lvalues. */

#define LIST_FOREACH_SAFE(iter,list,temp) \
    for ((iter) = (list); (iter) && (temp = (iter)->next, 1); (iter) = temp)

/*************************************************************************/

/* Search `list' for a node with `field' equal to `target' (as evaluated by
 * `compare') and place a pointer to the node found, or NULL if no node is
 * found, in `result'.  `field' must be a field of the nodes in `list';
 * `target' must be an expression of the type of `field' with no side
 * effects; `result' must be an lvalue; and `compare' must be a
 * strcmp()-like function (see LIST_INSERT_ORDERED).  The search is
 * performed in linear time, disregarding the comparison function. */

#define LIST_SEARCH(list,field,target,compare,result)   \
    do {                                                \
        LIST_FOREACH ((result), (list)) {               \
            if (compare((result)->field, (target)) == 0)\
                break;                                  \
        }                                               \
    } while (0)

/*************************************************************************/

/* Search `list' as LIST_SEARCH does, but for a scalar value.  The search
 * is performed in linear time. */

#define LIST_SEARCH_SCALAR(list,field,target,result)    \
    do {                                                \
        LIST_FOREACH ((result), (list)) {               \
            if ((result)->field == (target))            \
                break;                                  \
        }                                               \
    } while (0)

/*************************************************************************/

/* Search `list' as LIST_SEARCH does, but for a list known to be ordered.
 * The search is performed in linear time, disregarding the comparison
 * function. */

#define LIST_SEARCH_ORDERED(list,field,target,compare,result)   \
    do {                                                        \
        LIST_FOREACH ((result), (list)) {                       \
            int i = compare((result)->field, (target));         \
            if (i > 0)                                          \
                (result) = NULL;                                \
            if (i >= 0)                                         \
                break;                                          \
        }                                                       \
    } while (0)

/*************************************************************************/

/* Search `list' as LIST_SEARCH_ORDERED does, but for a scalar value.  The
 * search is performed in linear time. */

#define LIST_SEARCH_ORDERED_SCALAR(list,field,target,result)    \
    do {                                                        \
        LIST_FOREACH ((result), (list)) {                       \
            int i = (result)->field - (target);                 \
            if (i > 0)                                          \
                (result) = NULL;                                \
            if (i >= 0)                                         \
                break;                                          \
        }                                                       \
    } while (0)

/*************************************************************************/
/*************************************************************************/

/* Array macros.  In these macros, `array' and `size' must be lvalues with
 * no side effects; `array' is a pointer to the elements of the array, and
 * `size' is the current size (length) of the array. */

/*************************************************************************/

/* Extend a variable-length array by one entry.  Execution time is no
 * greater than linear with the length of the array. */

#define ARRAY2_EXTEND(array,size)                               \
    do {                                                        \
        (size)++;                                               \
        (array) = srealloc((array), sizeof(*(array)) * (size)); \
    } while (0)

/*************************************************************************/

/* Insert a slot at position `index' in a variable-length array.
 * Execution time is linear with the length of the array. */

#define ARRAY2_INSERT(array,size,index_)                        \
    do {                                                        \
        unsigned int _index = (index_);                         \
        (size)++;                                               \
        (array) = srealloc((array), sizeof(*(array)) * (size)); \
        if (_index < (size)-1)                                  \
            memmove(&(array)[_index+1], &(array)[_index],       \
                    sizeof(*(array)) * (((size)-1)-_index));    \
    } while (0)

/*************************************************************************/

/* Delete entry number `index' from a variable-length array.  Execution
 * time is linear with the length of the array. */

#define ARRAY2_REMOVE(array,size,index_)                        \
    do {                                                        \
        unsigned int _index = (index_);                         \
        (size)--;                                               \
        if (_index < (size))                                    \
            memmove(&(array)[_index], &(array)[_index]+1,       \
                    sizeof(*(array)) * ((size)-_index));        \
        (array) = srealloc((array), sizeof(*(array)) * (size)); \
    } while (0)

/*************************************************************************/

/* Iterate over every element in a variable-length array. */

#define ARRAY2_FOREACH(iter,array,size) \
    for ((iter) = 0; (iter) < (size); (iter)++)

/*************************************************************************/

/* Search a variable-length array for a value.  Operates like LIST_SEARCH.
 * `result' must be an integer lvalue.  If nothing is found, `result' will
 * be set equal to `size'.  The search is performed in linear time,
 * disregarding the comparison function. */

#define ARRAY2_SEARCH(array,size,field,target,compare,result)   \
    do {                                                        \
        ARRAY2_FOREACH ((result), (array), (size)) {            \
            if (compare((array)[(result)].field, (target)) == 0)\
                break;                                          \
        }                                                       \
    } while (0)

/*************************************************************************/

/* Search a variable-length array for a value, when the array elements do
 * not have fields.  The search is performed in linear time, disregarding
 * the comparison function. */

#define ARRAY2_SEARCH_PLAIN(array,size,target,compare,result)   \
    do {                                                        \
        ARRAY2_FOREACH ((result), (array), (size)) {            \
            if (compare((array)[(result)], (target)) == 0)      \
                break;                                          \
        }                                                       \
    } while (0)

/*************************************************************************/

/* Search a variable-length array for a scalar value.  The search is
 * performed in linear time. */

#define ARRAY2_SEARCH_SCALAR(array,size,field,target,result)    \
    do {                                                        \
        ARRAY2_FOREACH ((result), (array), (size)) {            \
            if ((array)[(result)].field == (target))            \
                break;                                          \
        }                                                       \
    } while (0)

/*************************************************************************/

/* Search a variable-length array for a scalar value, when the array
 * elements do not have fields.  The search is performed in linear time. */

#define ARRAY2_SEARCH_PLAIN_SCALAR(array,size,target,result)    \
    do {                                                        \
        ARRAY2_FOREACH ((result), (array), (size)) {            \
            if ((array)[(result)] == (target))                  \
                break;                                          \
        }                                                       \
    } while (0)

/*************************************************************************/
/*************************************************************************/

/* Perform the ARRAY2_* actions on an array `array' whose size is stored in
 * `array_count'. */

#define ARRAY_EXTEND(array)       ARRAY2_EXTEND(array,array##_count)
#define ARRAY_INSERT(array,index) ARRAY2_INSERT(array,array##_count,index)
#define ARRAY_REMOVE(array,index) ARRAY2_REMOVE(array,array##_count,index)
#define ARRAY_FOREACH(iter,array) ARRAY2_FOREACH(iter,array,array##_count)
#define ARRAY_SEARCH(array,field,target,compare,result) \
    ARRAY2_SEARCH(array,array##_count,field,target,compare,result)
#define ARRAY_SEARCH_PLAIN(array,target,compare,result) \
    ARRAY2_SEARCH_PLAIN(array,array##_count,target,compare,result)
#define ARRAY_SEARCH_SCALAR(array,field,target,result) \
    ARRAY2_SEARCH_SCALAR(array,array##_count,field,target,result)
#define ARRAY_SEARCH_PLAIN_SCALAR(array,target,result) \
    ARRAY2_SEARCH_PLAIN_SCALAR(array,array##_count,target,result)

/*************************************************************************/
/*************************************************************************/

#endif  /* LIST_ARRAY_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
