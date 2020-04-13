#ifndef PAGES_H
#define PAGES_H

#include <stdio.h>

void pages_init(const char* path, int create);
void pages_free();
void* pages_get_page(int pnum);

#endif
