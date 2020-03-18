
#include <stdio.h>

#include "gc.h"
#include "list.h"

list*
cons(long hd, list* tl)
{
    list* xs = gc_malloc(sizeof(list));
    xs->head = hd;
    xs->tail = tl;
    return xs;
}

static
list*
reverse1(list* xs, list* ys)
{
    if (xs) {
        return reverse1(xs->tail, cons(xs->head, ys));
    }
    else {
        return ys;
    }
}

list*
reverse(list* xs)
{
    return reverse1(xs, 0);
}

list*
take(long kk, list* xs)
{
    if (xs == 0 || kk == 0) {
        return 0;
    }
    else {
        return cons(xs->head, take(kk - 1, xs->tail));
    }
}

list*
drop(long kk, list* xs)
{
    if (xs == 0 || kk == 0) {
        return xs;
    }
    else {
        return drop(kk - 1, xs->tail);
    }
}

void
print_list(list* xs)
{
    for (; xs; xs = xs->tail) {
        printf("%ld ", xs->head);
    }
    printf("\n");
}

long
sum(list* xs)
{
    if (xs == 0) {
        return 0;
    }
    else {
        return xs->head + sum(xs->tail);
    }
}
