
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "gc.h"


long
len(char* xx)
{
    return strlen(xx);
}

char*
rev(char* xx)
{
    long nn = len(xx);
    char* yy = gc_malloc(nn + 1);
    for (long ii = 0; ii < nn; ++ii) {
        yy[ii] = xx[nn - ii - 1];
    }
    yy[nn] = 0;
    return yy;
}

long
aton(char* xx)
{
    return atol(xx);
}

char*
ntoa(long xx)
{
    long nn = snprintf(0, 0, "%ld", xx);
    nn += 1; // null byte
    char* yy = gc_malloc(nn);
    snprintf(yy, nn, "%ld", xx);
    return yy;
}

char*
dup(char* xx)
{
    long nn = 1 + len(xx);
    char* yy = gc_malloc(nn);
    memcpy(yy, xx, nn);
    return yy;
}
