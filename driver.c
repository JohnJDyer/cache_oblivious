/**
 *  \file driver.cc
 *  \brief Captures times of quicksort, funnelsort
 *
 *  This program
 *
 *  - creates an input array of keys to sort, where the caller gives
 *  the array size as a command-line input;
 *
 *  - sorts it using qsort, noting the execution time;
 *
 *  - sorts it using funnelsort, also noting the execution time;
 *
 *  - checks that the sorts produce the same result;
 *
 *  - outputs the execution times and effective sorting rate (i.e.,
 *  keys per second).
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include "timer.h"

#include "sort_utils.h"
#include "funnel.h"

#define USAGE                                 \
"usage:\n"                                    \
"  driver [options]\n"                        \
"options:\n"                                  \
"  -n [list_length]  Length of the list (Default: 2^20)\n"  \
"  -f          Selects Funnelsort, the default\n"           \
"  -q          Selects C's built-in qsort\n"                \
"  -c          Compare qsort and funnelsort\n"              \
"  -h          Show this help message\n" 

static void assertIsSorted (long N, const keytype* A){
  for (long i = 1; i < N; ++i) {
    if (A[i-1] > A[i]) {
      fprintf (stderr, "*** ERROR ***\n");
      fprintf (stderr, "  A[i=%lu] == %lu > A[%lu] == %lu\n", 
        i-1, A[i-1], i, A[i]);
      assert (A[i-1] <= A[i]);
    }
  }
  fprintf (stderr, "\t(Array is sorted.)\n");
}

static void assertIsEqual (long N, const keytype* A, const keytype* B){
  for (long i = 0; i < N; ++i) {
    if (A[i] != B[i]) {
      fprintf (stderr, "*** ERROR ***\n");
      fprintf (stderr, "  A[i=%lu] == %lu, but B[%lu] == %lu\n", i, A[i], i, B[i]);
      assert (A[i] == B[i]);
    }
  }
  fprintf (stderr, "\t(Arrays are equal.)\n");
}


int main (int argc, char* argv[]){
  long N = 1L << 20;
  int t  = 2;
  int option_char;

  while ((option_char = getopt(argc, argv, "n:fqch")) != -1) {
    switch (option_char) {
    case 'n': 
      N = atoi(optarg);
      break;
    case 'f':
      t = 2;
      break;
    case 'q':
      t = 1;
      break;
    case 'c':
      t = 3;
      break;    
    case 'h':
      fprintf(stderr, "%s\n", USAGE);
      exit(EXIT_SUCCESS);           
    default:
      fprintf(stderr, "%s\n", USAGE);
      exit(EXIT_FAILURE);
    }
  }
  
  stopwatch_init ();
  struct stopwatch_t* timer = stopwatch_create (); assert (timer);
  long double t_qs,t_es,t_fs,t_ms;

  srand48(time(NULL));
  /* Create an input array of length N, initialized to random values */
  keytype* A_in = newKeys (N);
  for (long i = 0; i < N; ++i)
    A_in[i] = lrand48 ();
  
  printf ("\nN == %lu wordsize = %lu\n\n", N,sizeof(keytype));

  switch(t)
  {
    case 1:
      stopwatch_start (timer);
      quickSort (N, A_in);
      t_qs = stopwatch_stop (timer);
      printf ("Quick sort: %Lg seconds ==> %Lg million keys per second\n",
        t_qs, 1e-6 * N / t_qs);
      assertIsSorted (N, A_in);
      break;

    case 2:
      stopwatch_start (timer);
      funnelSort (N, A_in);
      t_fs = stopwatch_stop (timer);
      printf ("Funnel sort: %Lg seconds ==> %Lg million keys per second\n",
        t_fs, 1e-6 * N / t_fs);
      assertIsSorted (N, A_in);
      break;

    case 3:
    {   /* Sort using quicksort */ 
      keytype* A_qs = newCopy (N, A_in);
      stopwatch_start (timer);
      quickSort (N, A_qs);
      t_qs = stopwatch_stop (timer);
      printf ("Quick sort: %Lg seconds ==> %Lg million keys per second\n",
        t_qs, 1e-6 * N / t_qs);       
      assertIsSorted(N, A_qs);

      /* Sort using funnelsort */
      keytype* A_fs = newCopy (N, A_in);
      stopwatch_start (timer);
      funnelSort (N, A_fs);
      t_fs = stopwatch_stop (timer);
      printf ("Funnel sort: %Lg seconds ==> %Lg million keys per second\n",
        t_fs, 1e-6 * N / t_fs);

      assertIsSorted(N, A_fs);
      assertIsEqual(N, A_fs, A_qs);      
      
      /* Cleanup */
      printf ("\n");
      free (A_qs);
      free (A_fs);
      break;
    }
  }

  // Common clean up
  free (A_in);
  stopwatch_destroy (timer);
  return 0;
}
