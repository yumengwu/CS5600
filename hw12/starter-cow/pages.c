
#define _GNU_SOURCE
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include "pages.h"
#include "util.h"

const int PAGE_COUNT = 256;
const int NUFS_SIZE  = 4096 * 256; // 1MB

static int   pages_fd   = -1;
static void* pages_base =  0;

void
pages_init(const char* path, int create)
{
    if (create) {
        pages_fd = open(path, O_CREAT | O_EXCL | O_RDWR, 0644);
        assert(pages_fd != -1);

        int rv = ftruncate(pages_fd, NUFS_SIZE);
        assert(rv == 0);
    }
    else {
        pages_fd = open(path, O_RDWR);
        assert(pages_fd != -1);
    }

    pages_base = mmap(0, NUFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, pages_fd, 0);
    assert(pages_base != MAP_FAILED);

    // mark inode 0 as taken
    char* pbase = (char*) pages_base;
    pbase[0] = 1;
}

void
pages_free()
{
    int rv = munmap(pages_base, NUFS_SIZE);
    assert(rv == 0);
}

void*
pages_get_page(int pnum)
{
    return pages_base + 4096 * pnum;
}

