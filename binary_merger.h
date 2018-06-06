#include "buffer.h"

// Basic binary merger structure
struct binary_merger
{
    struct binary_merger*   left;
    struct binary_merger*   right;

    struct buffer*          left_inp;
    struct buffer*          right_inp;

    struct buffer*          output;
};

// Initialize a binary merger
void binary_merger_init(
  struct binary_merger* bin,
  struct binary_merger* bin_left,
  struct binary_merger* bin_right,
  struct buffer*        linp,
  struct buffer*        rinp,
  struct buffer*        op)
{
  assert(bin);

  bin->left       = bin_left;
  bin->right      = bin_right;
  bin->left_inp   = linp;
  bin->right_inp  = rinp;
  bin->output     = op;

  return;
}

// Clean up code for a binary merger
void binary_merger_destroy(struct binary_merger* bin)
{

  if(bin){
    binary_merger_destroy(bin->left);      bin->left       = NULL; // RECURSIVE
    binary_merger_destroy(bin->right);     bin->right      = NULL; // RECURSIVE
    buffer_destroy       (bin->left_inp);  bin->left_inp   = NULL;
    buffer_destroy       (bin->right_inp); bin->right_inp  = NULL;
    free(bin);                             bin = NULL;
  }

  return;
}


void printbm(struct binary_merger* bin, int n){

  for(int i = 0; i < n; i ++){
    printf("\t");
  }
  printf("%p ", (void *) bin);

  if(bin){

    //printf("max: %u ", bin-> output    ->max_size); buffer_print(bin -> output);    printf(".");
    //printf("max: %u ", bin-> left_inp  ->max_size); buffer_print(bin -> left_inp);  printf(".");
    //printf("max: %u ", bin-> right_inp ->max_size); buffer_print(bin -> right_inp);

    printf("\n");
    printbm(bin->right, n+1);
    printf("\n");
    printbm(bin->left, n+1);

  } else {
    printf("bm_null\n");
  }

}

// Fills output buffer of a given Funnel
void fill(struct binary_merger* bin)
{

  if(bin){

    while( (bin -> output -> count) < (bin -> output -> max_size) ){

      if(bin->left_inp ->count ==0)
        fill(bin->left);

      if(bin->right_inp->count ==0)
        fill(bin->right);

      bool right_good = (bin->right_inp->count) > 0;
      bool left_good  = (bin->left_inp ->count) > 0;

      if(right_good && left_good){

        int c = compare(
          bin->left_inp ->first,
          bin->right_inp->first);

        if(c < 0){
          buffer_insert(bin->output, buffer_extract(bin->left_inp));
        } else {
          buffer_insert(bin->output, buffer_extract(bin->right_inp));
        }

      } else if(right_good){
        buffer_insert(bin->output, buffer_extract(bin->right_inp));
      } else if (left_good){
        buffer_insert(bin->output, buffer_extract(bin->left_inp));
      } else {
        break;
      }

    }
  }

  return;
}

