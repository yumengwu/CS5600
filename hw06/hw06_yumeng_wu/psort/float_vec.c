// Author: Nat Tuck
// CS3650 starter code

#include <stdlib.h>
#include <stdio.h>

#include "float_vec.h"

/*
typedef struct floats {
    long size;
    long cap;
    float* data;
} floats;
*/

floats*
make_floats(long nn)
{
    floats* xs = malloc(sizeof(floats));
    xs->size = nn;
    xs->cap  = (nn > 1) ? nn : 2;
    xs->data = malloc(xs->cap * sizeof(float));
    return xs;
}

void
floats_push(floats* xs, float xx)
{
    if (xs->size >= xs->cap) {
        xs->cap *= 2;
        xs->data = realloc(xs->data, xs->cap * sizeof(float));
    }

    xs->data[xs->size] = xx;
    xs->size += 1;
}

void
free_floats(floats* xs)
{
    free(xs->data);
    free(xs);
}

void
floats_print(floats* xs)
{
    for (int ii = 0; ii < xs->size; ++ii) {
        printf("%.04f ", xs->data[ii]);
    }
    printf("\n");
}


