
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "list.h"
#include "gc.h"

// TODO: More test cases.
//  - Simple alloc, no gc.
//  - List test (shared structure!)
//  - Array test
//  - Over 1MB fixed size (list)
//  - Over 1MB random size (array)

int
main(int argc, char* argv[])
{
    GC_INIT();

    list* xs = 0;
    for (long ii = 0; ii < 5; ++ii) {
        xs = cons(ii, xs);
    }

    xs = reverse(xs);

    assert(sum(xs) == 10);

    gc_collect();

    char* aa = gc_malloc(900);
    memset(aa, 0x99, 900);

    char* bb = gc_malloc(700);
    memset(bb, 0x77, 700);

    gc_collect();

    int** zs = gc_malloc(10*sizeof(int*));
    for (int ii = 0; ii < 10; ++ii) {
        int size = ii * 67 % 95;
        zs[ii] = gc_malloc(size);
        memset(zs[ii], ii, size);
    }

    for (int ii = 0; ii < 10; ++ii) {
        if (ii % 2 == 0) {
            int size = ii * 101 % 150;
            zs[ii] = gc_malloc(size);
            memset(zs[ii], ii, size);
        }
    }

    xs = 0;
    // Arbitrarily leave one thing.
    //aa = 0;
    bb = 0;
    zs = 0;

    gc_collect();
    gc_print_stats();

    return 0;
}
