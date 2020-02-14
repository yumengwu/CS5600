#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include "float_vec.h"
#include "barrier.h"
#include "utils.h"

void
qsort_floats(floats* xs)
{
    // TODO: man 3 qsort ?
}

floats*
sample(float* data, long size, int P)
{
    // TODO: Randomly sample 3*(P-1) items from the input data.
    return (floats*) -1;
}

void
sort_worker(int pnum, float* data, long size, int P, floats* samps, long* sizes, barrier* bb)
{
    floats* xs = malloc(sizeof(floats));

    // TODO: Copy the data for our partition into a locally allocated array.
    printf("%d: start %.04f, count %ld\n", pnum, samps->data[pnum], xs->size);

    // TODO: Sort the local array.

    // TODO: Using the shared sizes array, determine where the local
    // output goes in the global data array.

    // TODO: Copy the local array to the appropriate place in the global array.

    // TODO: Make sure this function doesn't have any data races.
}

void
run_sort_workers(float* data, long size, int P, floats* samps, long* sizes, barrier* bb)
{
    // TODO: Spawn P processes running sort_worker
    //
    // TODO: Once all P processes have been started, wait for them all to finish.
}

void
sample_sort(float* data, long size, int P, long* sizes, barrier* bb)
{
    // TODO: Sequentially sample the input data.
    //
    // TODO: Sort the input data using the sampled array to allocate work
    // between parallel processes.
}

int
main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage:\n");
        printf("\t%s P data.dat\n", argv[0]);
        return 1;
    }

    const int P = atoi(argv[1]);
    const char* fname = argv[2];

    seed_rng();

    int fd = open(fname, O_RDWR);
    check_rv(fd);

    void* file = 0; // TODO: Use mmap for I/O

    long count = 0; // TODO: this is in the file
    float* data = 0; // TODO: this is in the file

    long sizes_bytes = P * sizeof(long);
    long* sizes = malloc(sizes_bytes); // TODO: This should be shared memory.

    barrier* bb = make_barrier(P);

    sample_sort(data, count, P, sizes, bb);

    free_barrier(bb);

    // TODO: Clean up resources.
    (void) file;

    return 0;
}

