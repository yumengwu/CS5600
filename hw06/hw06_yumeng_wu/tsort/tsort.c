#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <assert.h>
#include <pthread.h>

#include "float_vec.h"
#include "barrier.h"
#include "utils.h"

long size;
float * data;

long sizes_bytes;
long * sizes;

floats * samps;

barrier* bb;

void swap_float(float * a, float * b)  
{  
    float t = *a;  
    *a = *b;  
    *b = t;  
}  

int qsort_partition(float * data, int left, int right)
{
    float pivot = data[right];
    int i = left - 1;
    for (int j = left; j <= right - 1; j++)
    {
        if (data[j] < pivot)
        {
            i++;
            swap_float(&data[i], &data[j]);
        }
    }
    swap_float(&data[i + 1], &data[right]);
    return i + 1;
}

void qsort_f(float * data, int left, int right)
{
    if (left < right)
    {
        int partition = qsort_partition(data, left, right);
        qsort_f(data, left, partition - 1);
        qsort_f(data, partition + 1, right);
    }
}

void
qsort_floats(floats* xs)
{
    // TODO: man 3 qsort ?
    qsort_f(xs->data, 0, xs->size - 1);
}

floats*
sample(int P)
{
    // TODO: Randomly sample 3*(P-1) items from the input data.
    int rss = 61 * (P - 1);
    floats * result = make_floats(rss);
    for (int i = 0; i < rss; ++i)
    {
        result->data[i] = data[rand() % size];
    }
    return result;
}

void * sort_work(void * args)
{
    int pnum = *((int *)(args));
    floats* xs = make_floats(0);

    for (int i = 0; i < size; ++i)
    {
        if (samps->data[pnum] <= data[i] && data[i] < samps->data[pnum + 1])
        {
            floats_push(xs, data[i]);
        }
    }
    sizes[pnum] = xs->size;

    printf("%d: start %.04f, count %ld\n", pnum, samps->data[pnum], xs->size);

    qsort_floats(xs);

    barrier_wait(bb);

    long sum = 0, start = 0, end = 0;
    for (int i = 0; i < pnum; ++i)
    {
        sum += sizes[i];
    }
    start = sum;
    end = sum + sizes[pnum];
    for (int i = start; i < end; ++i)
    {
        data[i] = xs->data[i - start];
    }
    free_floats(xs);

    pthread_exit(NULL);
}

void run_sort_workers(int P)
{
    pthread_t threads[P];
    int args[P];
    for (int i = 0; i < P; ++i)
    {
        args[i] = i;
        pthread_create(&threads[i], NULL, sort_work, (void *) (&args[i]));
    }
    for (int i = 0; i < P; ++i)
    {
        pthread_join(threads[i], NULL);
    }
}

void sample_sort(int P)
{
    floats * randomSample = sample(P);
    qsort_floats(randomSample);
    samps = make_floats(P + 1);
    samps->data[0] = 0;
    samps->data[samps->size - 1] = __FLT_MAX__;
    for (int i = 0; i < P - 1; ++i)
    {
        samps->data[i + 1] = randomSample->data[i * 3 + 1];
    }
    run_sort_workers(P);
    free_floats(randomSample);
    free_floats(samps);
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

    read(fd, (void *)(&size), sizeof(long));
    data = malloc(size * sizeof(float));
    read(fd, data, size * sizeof(float));
    sizes_bytes = P * sizeof(long);
    sizes = malloc(sizes_bytes);

    bb = make_barrier(P);

    sample_sort(P);

    lseek(fd, 0, SEEK_SET);
    write(fd, (void *)(&size), sizeof(long));
    write(fd, data, size * sizeof(float));
    close(fd);

    free_barrier(bb);

    free(data);
    free(sizes);

    return 0;
}

