#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "xmalloc.h"

#define IT_NUM 6

int main()
{
    char ** s[IT_NUM];
    for (int i = 0; i < IT_NUM; ++i) {
        s[i] = xmalloc(1000);
        memset(s[i], 'a', 30);
    }
    for (int i = 0; i < IT_NUM; ++i) {
        printf("%d %s\n", i, s[i]);
    }
    return 0;
}