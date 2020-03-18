
#include <stdio.h>

#include "gc.h"

#define NN 256

// This test just verifies that memory allocation works.
// The garbage collector never needs to run.

int
main(int _ac, char* _av[])
{
    GC_INIT();

    int** data = gc_malloc(NN * sizeof(int*));
    for (int ii = 0; ii < NN; ++ii) {
        data[ii] = gc_malloc(ii * sizeof(int));

        for (int jj = 0; jj < ii; ++jj) {
            data[ii][jj] = jj;
        }
    }

    long sum = 0;

    for (int ii = 0; ii < NN; ++ii) {
        for (int jj = 0; jj < ii; ++jj) {
            sum += data[ii][jj];
        }
    }

    printf("sum = %ld\n", sum);

    return 0;
}
