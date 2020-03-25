// Author: Nat Tuck
// 3650 starter code
// don't change this file

#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

int
main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage:\n");
        printf("\t%s count file.dat\n", argv[0]);
        exit(1);
    }

    seed_rng();

    const long  count = atoi(argv[1]);
    const char* fname = argv[2];
    int rv;

    FILE* data = fopen(fname, "w");
    assert(data != NULL);
    rv = fwrite(&count, sizeof(long), 1, data);
    assert(rv > 0);
    for (long ii = 0; ii < count; ++ii) {
        int   inum = random();
        float fnum = (inum % 100000) / 1000.0f;
        rv = fwrite(&fnum, sizeof(float), 1, data);
        assert(rv > 0);
    }
    rv = fclose(data);
    assert(rv == 0);
    return 0;
}

