#ifndef BITMAP__H
#define BITMAP__H

int bitmap_get(char * bm, int idx);
int bitmap_put(char * bm, int idx, int val);
void bitmap_print(char * bm, int size);

#endif