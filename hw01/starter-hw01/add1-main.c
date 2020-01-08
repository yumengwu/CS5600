
#include <stdio.h>
#include <stdlib.h>

long add1(long);

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        puts("Usage: ./add1 N");
        return 1;
    }

    long xx = atoi(argv[1]);
    long yy = add1(xx);
    printf("result = %ld\n", yy);
    return 0;
}

