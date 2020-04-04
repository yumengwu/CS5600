#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <stdlib.h>

static int
streq(const char* aa, const char* bb)
{
    return strcmp(aa, bb) == 0;
}

static int
min(int x, int y)
{
    return (x < y) ? x : y;
}

static int
max(int x, int y)
{
    return (x > y) ? x : y;
}

static int
clamp(int x, int v0, int v1)
{
    return max(v0, min(x, v1));
}

static int
bytes_to_pages(int bytes)
{
    int quo = bytes / 4096;
    int rem = bytes % 4096;
    if (rem == 0) {
        return quo;
    }
    else {
        return quo + 1;
    }
}

static void
join_to_path(char* buf, const char* item)
{
    int nn = strlen(buf);
    if (buf[nn - 1] != '/') {
        strcat(buf, "/");
    }
    strcat(buf, item);
}

static char*
path_join(const char* aa, const char* bb)
{
    size_t nn = 4 + strlen(aa) + strlen(bb);
    char* cc = calloc(1, nn);
    strcat(cc, aa);
    join_to_path(cc, bb);
    return cc;
}

static void
assert_ok_real(long rv, const char* file, const int line)
{
    if (rv == -1) {
        fprintf(stderr, "assert_ok failed at %s:%d, ", file, line);
        perror("msg");
        fflush(stderr);
        abort();
    }
}

#define assert_ok(rv) assert_ok_real(rv, __FILE__, __LINE__)

#endif
