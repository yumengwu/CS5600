#ifndef LIST_H
#define LIST_H

typedef struct list {
    long head;
    struct list* tail;
} list;

list* cons(long hd, list* tl);
list* reverse(list* xs);
list* take(long kk, list* xs);
list* drop(long kk, list* xs);
long  sum(list* xs);
void  print_list(list* xs);

#endif
