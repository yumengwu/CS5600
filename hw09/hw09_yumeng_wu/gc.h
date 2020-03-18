#ifndef GC_H
#define GC_H

#include <stddef.h>

void  gc_init(void* base);
void* gc_malloc(size_t size);
void  gc_collect();
void  gc_print_stats();
void  gc_print_info(void* ptr);

#define GC_INIT() do { int _gc_base = 0; gc_init(&_gc_base); } while(0)

#endif
