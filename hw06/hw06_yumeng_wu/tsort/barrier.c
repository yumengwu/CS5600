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
    pthread_cond_init(&bb->cond, NULL);
    pthread_mutex_init(&bb->mutex, NULL);
    pthread_mutex_init(&bb->add_mutex, NULL);
    bb->count = nn;
    bb->seen = 0;

    return bb;
}

void
barrier_wait(barrier* bb)
{
    // TODO: something?
    pthread_mutex_lock(&bb->add_mutex);
    bb->seen++;
    int seen = bb->seen;
    pthread_mutex_unlock(&bb->add_mutex);
    if (seen < bb->count)
    {
        pthread_mutex_lock(&bb->mutex);
        pthread_cond_wait(&bb->cond, &bb->mutex);
        pthread_mutex_unlock(&bb->mutex);
    }
    else
    {
        pthread_cond_broadcast(&bb->cond);
    }

}

void
free_barrier(barrier* bb)
{
    // TODO: More cleanup?
    pthread_cond_destroy(&bb->cond);
    pthread_mutex_destroy(&bb->mutex);
    pthread_mutex_destroy(&bb->add_mutex);
    free(bb);
}

