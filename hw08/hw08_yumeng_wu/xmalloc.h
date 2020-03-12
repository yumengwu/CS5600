#ifndef XMALLOC_H
#define XMALLOC_H

#include <stddef.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/sem.h>

void* xmalloc(size_t bytes);
void  xfree(void* ptr);
void* xrealloc(void* prev, size_t bytes);

#endif
