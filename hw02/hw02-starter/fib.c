
#include <stdio.h>
#include <stdlib.h>

int fib(int n)
{
    if (n <= 1)
    {
        return n;
    }
    else
    {
        return fib(n - 1) + fib(n - 2);
    }
}

int
main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./fib N, where N >= 0\n");
        exit(EXIT_FAILURE);
    }
    long xx = atol(argv[1]);
    if (xx < 0)
    {
        printf("Usage: ./fib N, where N >= 0\n");
        exit(EXIT_FAILURE);
    }
    // printf("fib(3) = %ld\n", xx);
    printf("fib(%ld) = %ld\n", xx, fib(xx));
    return 0;
}

