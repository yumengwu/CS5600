#include <string.h>
#include <stdlib.h>
#include <alloca.h>

#include "slist.h"

slist*
s_cons(const char* text, slist* rest)
{
    slist* xs = malloc(sizeof(slist));
    xs->data = strdup(text);
    xs->refs = 1;
    xs->next = rest;
    return xs;
}

void
s_free(slist* xs)
{
    if (xs == 0) {
        return;
    }

    xs->refs -= 1;

    if (xs->refs == 0) {
        s_free(xs->next);
        free(xs->data);
        free(xs);
    }
}

slist*
s_split(const char* text, char delim)
{
    if (*text == 0) {
        return 0;
    }

    int plen = 0;
    while (text[plen] != 0 && text[plen] != delim) {
        plen += 1;
    }

    int skip = 0;
    if (text[plen] == delim) {
        skip = 1;
    }

    slist* rest = s_split(text + plen + skip, delim);
    char*  part = alloca(plen + 2);
    memcpy(part, text, plen);
    part[plen] = 0;

    return s_cons(part, rest);
}

slist*
s_concat(slist* xs, slist* ys)
{
    if (xs == 0) {
        if (ys == 0) {
            return 0;
        }
        else {
            return s_concat(ys, 0);
        }
    }
    return s_cons(xs->data, s_concat(xs->next, ys));
}

slist*
s_cat_free(slist* xs, slist* ys)
{
    slist* zs = s_concat(xs, ys);
    s_free(xs);
    s_free(ys);
    return zs;
}

slist*
s_copy(slist* xs)
{
    return s_concat(xs, 0);
}

slist*
s_reverse(slist* xs)
{
    slist* ys = 0;
    for (; xs; xs = xs->next) {
        ys = s_cons(xs->data, ys);
    }
    return ys;
}

slist*
s_rev_free(slist* xs)
{
    slist* ys = s_reverse(xs);
    s_free(xs);
    return ys;
}
