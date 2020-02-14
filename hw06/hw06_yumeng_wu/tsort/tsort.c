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

#include "float_vec.h"
#include "barrier.h"
#include "utils.h"

int
main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage:\n");
        printf("\t%s P data.dat\n", argv[0]);
        return 1;
    }

    printf("It'll probably be easiest to copy your working\n");
    printf("psort.c file over tsort.c and start from that.\n");

    return 0;
}

