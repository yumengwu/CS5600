// Author: Nat Tuck
// CS3650 starter code

#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>

void
seed_rng()
{
    struct timeval tv;
    gettimeofday(&tv, 0);

    long pid = getpid();
    long sec = tv.tv_sec;
    long usc = tv.tv_usec;

    srandom(pid ^ sec ^ usc);
}

void
check_rv(int rv)
{
    if (rv == -1) {
        perror("oops");
        fflush(stdout);
        fflush(stderr);
        abort();
    }
}

