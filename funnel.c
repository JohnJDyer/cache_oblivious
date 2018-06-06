#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "binary_merger.h"

//when len(input) < 2^B use a different sorting technique
#define FUNNEL_BASE 5 // d     used in functions is equivalent to the FUNNEL_BASE: ONE OF 3,4,5
#define ALPHA       2 // alpha is another tuning parameter controlling the number of elements outputted by a Funnel

int buffer_size(int K){
  if(K==2)
    return 8;
  int sqrt_K = ceil(sqrt(K));
  int ret    = buffer_size(sqrt_K);
  return ret * ret;
}

int funnel_size(int K){
  if(K ==2)
    return 0;
  int sqrt_K = ceil(sqrt(K));
  return sqrt_K * buffer_size(sqrt_K) + (1 + sqrt_K) * funnel_size(sqrt_K);
}

int get_k(int N){
  int K = 4;
  while(K * K * K < N){
    K = K * K;
  }
  return K;
}

// Recursive Funnel Initializer
struct binary_merger* funnel_init_rec (
  int                    K,
  struct buffer*         out,
  struct buffer**        in,
  struct binary_merger** sources,
  keytype*               mem)
{

  struct binary_merger* res;

  if(K ==2){
    res = malloc(sizeof(struct binary_merger ));
    if(sources){
      binary_merger_init(res, *sources, *(sources+1), *in, *(in +1), out);
    } else {
      binary_merger_init(res, NULL, NULL, *in, *(in +1), out);
    }
    return res;
  }

  int sqrt_K = ceil(sqrt(K));

  //recursively create funnel
  keytype* allocator = mem;
  int next_funnel_size = funnel_size(sqrt_K);
  allocator += next_funnel_size;

  // Create middle buffers.
  struct buffer **middle_buffs = malloc(sizeof(struct buffer *) * sqrt_K);

  for(int i = 0; i < sqrt_K; i++){
    middle_buffs[i] = malloc(sizeof(struct buffer ));
    buffer_init_mem(middle_buffs[i], allocator, buffer_size(sqrt_K));
    allocator += buffer_size(sqrt_K);
  }

  // Create bottom mergers.
  struct binary_merger **bottom_mergers = malloc(sizeof(struct binary_merger *) * sqrt_K); //for the recursive calls
  for(int i = 0; i < sqrt_K; i++){
    bottom_mergers[i] = funnel_init_rec(
      sqrt_K,
      middle_buffs      [i],
      in                + (i * sqrt_K),
      sources ? sources + (i * sqrt_K): NULL,
      allocator);

      allocator += next_funnel_size;
  }

  // Create and return top merger
  res = funnel_init_rec(
    sqrt_K,
    out,
    middle_buffs,
    bottom_mergers,
    mem);

  free(bottom_mergers); bottom_mergers = NULL;
  free(middle_buffs);   middle_buffs   = NULL;

  return res;
}

// Initialize a K-Funnel in a given memory
struct binary_merger* funnel_init(
  int             K,
  struct buffer*  out,
  struct buffer** in,
  keytype*        mem)
{
  return funnel_init_rec(K, out, in, NULL, mem);
}

void ofunnelsort(
  long      N,
  keytype*  A,
  keytype*  O,
  keytype*  fspace)
{

  int K  = get_k(N);
  int M  = N / K;            // ~ N ^ (2/3) == K ^ 2
  int LB = N - (M * (K-1));  // ~ N ^ (2/3) == K ^ 2

  keytype* _A;

  //recursive sort =========================================================
  if(M < (1 << FUNNEL_BASE)){

    for(int i=0; i < K-1; i++){
      qsort(A + M * i    , M , sizeof(keytype), compare);
    } qsort(A + M * (K-1), LB, sizeof(keytype), compare);

  } else {

    keytype* _O_swap = malloc(sizeof(keytype) * M);
    keytype* _O      = _O_swap;

    _A = A;

    for(int i = 0; i < K-1; i++){
      ofunnelsort(M , _A, _O, fspace);
      _O  = _A;
      _A +=  M;
    }

    memcpy(_O, _O_swap, M * sizeof(keytype)); //load
    _O += M;
    free(_O_swap); _O_swap = NULL;
    _O_swap = malloc(sizeof(keytype) * LB);
    memcpy(_O_swap, _A, LB * sizeof(keytype));
    ofunnelsort(LB, _O_swap,_O, fspace);

    free(_O_swap); _O_swap = NULL;
  }

  // Create input buffers
  struct buffer** in_buffers = malloc(sizeof(struct buffer*) * K); // we should have K + 1 buffers

  _A = A;
  for(int i = 0; i < K -1; i++){
    in_buffers[i] = malloc(sizeof(struct buffer));
    buffer_init(in_buffers[i], _A, _A + M);
    _A += M;
  }

  in_buffers[K-1] = malloc(sizeof(struct buffer));
  buffer_init(in_buffers[K-1], _A, _A + LB);

  //Create output buffers
  struct buffer out_buffer;
  buffer_init_mem(&out_buffer, O, N);

  // Create the top funnel and merge
  struct binary_merger* bm = funnel_init(K, &out_buffer, in_buffers, fspace);
  fill(bm);
  binary_merger_destroy(bm);

  free(in_buffers);

  return;
}

void funnelSort(
  long      N,
  keytype*  A)
{

  keytype* O      = malloc(sizeof(keytype) * N);
  keytype* fspace = malloc(sizeof(keytype) * funnel_size(get_k(N)));
  ofunnelsort(N, A, O, fspace);
  memcpy(A, O, sizeof(keytype) * N);

  free(O);
  free(fspace);

  return;
}
