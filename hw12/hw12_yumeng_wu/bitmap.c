#include <stdio.h>
#include "bitmap.h"

#define BITMAP_LIMIT 256

int bitmap_get(char * bm, int idx)
{
    if (!bm || idx >= BITMAP_LIMIT) {
        return -1;
    }
    return (bm[idx / 8] & (1 << (idx % 8))) >> (idx % 8);
}

int bitmap_put(char * bm, int idx, int val)
{
    if (!bm || idx >= BITMAP_LIMIT || (val != 0 && val != 1)) {
        return -1;
    }
    if (val) {
        bm[idx / 8] |= 1 << (idx % 8);
    }
    else {
        bm[idx / 8] &= 0xff ^ (1 << idx % 8);
    }
    return 0;
}

void bitmap_print(char * bm, int size)
{
    printf("bitmap print\n");
    for (int i = 0; i < size; ++i) {
        if (i != 0 && i % 8 == 0) printf("\n");
        printf("%d ", bitmap_get(bm, i));
    }
    printf("\n");
}