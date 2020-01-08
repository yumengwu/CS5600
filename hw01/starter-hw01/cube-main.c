
#include <stdio.h>
#include <stdlib.h>

long cube(long);

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        puts("Usage: ./cube N");
        return 1;
    }

    long xx = atoi(argv[1]);
    long yy = cube(xx);
    printf("result = %ld\n", yy);
    return 0;
}

