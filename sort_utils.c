/**
 *  \file sort.cc
 *
 *  routines for managing keys. See 'sort.hh'.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "sort_utils.h"

/* ============================================================
 * The following code implements a sequentialSort().
 */
int compare (const void* a, const void* b)
{
    keytype ka = *(const keytype *)a;
    keytype kb = *(const keytype *)b;
    if (ka < kb)
        return -1;
    else if (ka == kb)
        return 0;
    else
        return 1;
}

void quickSort (long N, keytype* A)
{
  qsort (A, N, sizeof (keytype), compare);
}

/* ============================================================
 * Some helper routines for managing an array of keys.
 */

keytype *
newKeys (long N)
{
    keytype* A = (keytype *)malloc (N * sizeof (keytype));
    assert (A);
    return A;
}

/** Returns a new copy of A[0:N-1] */
keytype *
newCopy (long N, const keytype* A)
{
    keytype* A_copy = newKeys (N);
    memcpy (A_copy, A, N * sizeof (keytype));
    return A_copy;
}

/* ============================================================
 * Code for checking the sorted results
 */

void assertIsSorted (long N, const keytype* A)
{
    for (long i = 1; i < N; ++i) 
    {
        if (A[i-1] > A[i]) 
        {
            fprintf (stderr, "*** ERROR ***\n");
            fprintf (stderr, "  A[i=%lu] == %lu > A[%lu] == %lu\n", 
                i-1, A[i-1], i, A[i]);
            assert (A[i-1] <= A[i]);
        }
    } /* i */
    fprintf (stderr, "\t(Array is sorted.)\n");
}

void assertIsEqual (long N, const keytype* A, const keytype* B)
{
    for (long i = 0; i < N; ++i) 
    {
        if (A[i] != B[i]) 
        {
            fprintf (stderr, "*** ERROR ***\n");
            fprintf (stderr, "  A[i=%lu] == %lu, but B[%lu] == %lu\n", 
                i, A[i], i, B[i]);
            assert (A[i] == B[i]);
        }
    } /* i */
    fprintf (stderr, "\t(Arrays are equal.)\n");
}
