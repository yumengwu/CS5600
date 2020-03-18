
#include <stdio.h>

#include "gc.h"

#define NN 256

// This verifies that the allocator works, and that
// a forced GC run will clean up everything.

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

    gc_collect();
    gc_print_stats();

    long sum = 0;
    for (int ii = 0; ii < NN; ++ii) {
        for (int jj = 0; jj < ii; ++jj) {
            sum += data[ii][jj];
        }
    }

    printf("\n");
    printf("sum = %ld\n\n", sum);
    printf("do cleanup\n\n");

    data = 0;

    gc_collect();
    gc_print_stats();

    return 0;
}
