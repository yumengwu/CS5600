// Author: Nat Tuck
// CS3650 starter code

#ifndef BARRIER_H
#define BARRIER_H

#include <semaphore.h>

typedef struct barrier {
    sem_t barrier;
    sem_t mutex;
    int   count;
    int   seen;
} barrier;

barrier* make_barrier(int nn);
void barrier_wait(barrier* bb);
void free_barrier(barrier* bb);


#endif

