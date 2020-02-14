// Author: Nat Tuck
// CS3650 starter code

#ifndef BARRIER_H
#define BARRIER_H

#include <pthread.h>

typedef struct barrier {
    float wrong_field;  // TODO: make a barrier
                        // TODO: use mutexes and condvars
} barrier;

barrier* make_barrier(int nn);
void barrier_wait(barrier* bb);
void free_barrier(barrier* bb);


#endif

