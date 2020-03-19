#include <stdio.h>
#include "gc.h"

int main()
{
    GC_INIT();
    char * s = gc_malloc(20);
    printf("s: %ld\n", s);
    gc_collect();
    s = 0;
    gc_collect();
    return 0;
}