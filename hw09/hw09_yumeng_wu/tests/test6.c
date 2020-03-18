
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "gc.h"

int
main(int argc, char* argv[])
{
    GC_INIT();

    char* text = 0;

    for (long ii = 0; ii < 8192; ++ii) {
        text = gc_malloc(1025);
        memset(text, 'x', 1024);
        text[1024] = 0;
    }

    printf("len: %ld\n", strlen(text));

    gc_print_stats();

    return 0;
}
