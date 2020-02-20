// Author: Nat Tuck
// CS3650 starter code

#ifndef BARRIER_H
#define BARRIER_H

#include <pthread.h>

typedef struct barrier {
    pthread_mutex_t mutex;
    pthread_mutex_t add_mutex;
    pthread_cond_t cond;
    int   count;
    int   seen;
} barrier;

barrier* make_barrier(int nn);
void barrier_wait(barrier* bb);
void free_barrier(barrier* bb);


#endif

