#include "sort_utils.h"

/*
 * Circular Buffer functions
 */

struct buffer{
  keytype* start;
  keytype* first;
  keytype* last;

  long max_size;
  long count;
};

// Create an empty buffer at start location of an array
void buffer_init_mem(struct buffer* buf, keytype* mem, long total_size){
  assert(buf);

  buf->first      = mem;
  buf->last       = mem;
  buf->start      = mem;
  buf->count      = 0;
  buf->max_size   = total_size;

  return;
}

// Init Buffer
// Given pointers to start, end of an array and a buffer pointer
void buffer_init(struct buffer* buf, keytype* start, keytype* end){
  assert(buf);

  // Here the last position is one ahead of the last data
  buf->first    = start;
  buf->last     = end - 1;
  buf->start    = start;
  buf->count    = end - start;
  buf->max_size = end - start;

  return;
}

// Destroy a buffer struct
void buffer_destroy(struct buffer* buf){
  if(buf) {
    buf->first = NULL;
    buf->start = NULL;
    buf->last  = NULL;
    free(buf); buf = NULL;
  }

  return;
}

// Increment the end element pointer in a circular queue
void increment(struct buffer* buf, keytype** p){
  if((*p) == buf->start + buf->max_size - 1) {
    (*p) = buf->start;
  } else {
    ++(*p);
  }

  return;
}

// Insert element to the back of a buffer
void buffer_insert(struct buffer* buf, keytype elem){
  assert(buf);

  //Check if buffer is not full
  if(buf->count < buf->max_size) {
    *(buf->last) = elem;
    ++(buf->count);
    increment(buf,&buf->last);
  }

  return;
}

// Extract top element of a buffer
keytype buffer_extract(struct buffer* buf){
  assert(buf);
  assert(buf->count != 0);

  keytype ret = *(buf->first);
  --(buf->count);

  increment(buf,&buf->first);

  // Reset once empty
  if(buf->count == 0) {
    buf->first = buf->start;
    buf->last  = buf->start;
  }

  return ret;
}

// Look at top element of a funnel
keytype buffer_peep(struct buffer* buf){
  assert(buf);
  assert(buf->count != 0);
  return *(buf->first);
}

// Prints contents of a buffer
void buffer_print(struct buffer* buf){
  assert(buf);
  printf("count: %lu ",buf->count);

  keytype* iter;
  if(buf->count != 0){
    int i = 0;
    iter = buf->first;
    while(i < buf->count){
      printf("%lu ",*(iter));
      increment(buf,&iter);
      i++;
    }
  }

  return;
}

