
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "gc.h"
#include "str.h"
#include "strint.h"

char*
fib(long xx)
{
    assert(xx >= 0);

    if (xx < 2) {
        return ntoa(xx);
    }

    char* aa = ntoa(0);
    char* bb = ntoa(1);
    char* cc;

    for (long ii = 0; ii < xx; ++ii) {
        cc = si_add(aa, bb);
        aa = bb;
        bb = cc;
    }

    return aa;
}

int
main(int argc, char* argv[])
{
    GC_INIT();

    long  xx = 1000;
    char* yy = fib(xx);

    printf("fib(%ld) = %s\n", xx, yy);
    printf("len(yy) = %ld\n", len(yy));

    gc_collect();
    gc_print_stats();

    return 0;
}
