// Author: Nat Tuck
// CS3650 starter code

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include "barrier.h"

barrier*
make_barrier(int nn)
{
    barrier* bb = malloc(sizeof(barrier));
    assert(bb != 0);

    // TODO: More setup?

    return bb;
}

void
barrier_wait(barrier* bb)
{
    // TODO: something?
}

void
free_barrier(barrier* bb)
{
    // TODO: More cleanup?
    free(bb);
}

