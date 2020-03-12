#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "xmalloc.h"

#define IT_NUM 10

int main()
{
    char ** s[IT_NUM];
    for (int i = 0; i < IT_NUM; ++i) {
        s[i] = xmalloc(1000);
        memset(s[i], 'a' + i, 30);
    }
    for (int i = 0; i < IT_NUM; ++i) {
        printf("%d %s\n", i, s[i]);
    }
    for (int i = 0; i < IT_NUM; ++i) {
        xfree(s[i]);
    }
    for (int i = 0; i < IT_NUM; ++i) {
        s[i] = xmalloc(1000);
        memset(s[i], 'a' + i, 30);
    }
    for (int i = 0; i < IT_NUM; ++i) {
        printf("%d %s\n", i, s[i]);
    }
    return 0;
}