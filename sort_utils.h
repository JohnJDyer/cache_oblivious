// Interface to sorting arrays of keys ('keytype' values).

#ifndef INC_SORT_UTILS_H
#define INC_SORT_UTILS_H /*!< sort.hh already included */

int compare (const void* a, const void* b);

// 'keytype' is the primitive type for sorting keys.
typedef unsigned long keytype;

// Sorts an input array containing N keys, A[0:N-1] using quick sort. 
// The sorted output overwrites the input array.
void quickSort (long N, keytype* A);

// Returns a new uninitialized array of length N.
keytype* newKeys (long N);

// Returns a (deep) copy of A[0:N-1]
keytype* newCopy (long N, const keytype* A);

#endif
