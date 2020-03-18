
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "gc.h"

// Tests allocating two linked lists and
// freeing one of them.

int
main(int argc, char* argv[])
{
    GC_INIT();

    list* xs = 0;
    for (long ii = 0; ii < 5; ++ii) {
        xs = cons(ii, xs);
    }

    printf("forward: ");
    print_list(xs);

    xs = reverse(xs);
    printf("reverse: ");
    print_list(xs);

    printf("\n");

    gc_collect();
    gc_print_stats();

    return 0;
}
