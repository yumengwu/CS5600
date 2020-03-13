
#include "xmalloc.h"

#define INIT_PW 20

#define ALLOC_SIZE 12288 // 3 * 4096

#define IS_POW_OF_2(x) !(x & (x - 1))

size_t align_pow_2(size_t s)
{
    if (IS_POW_OF_2(s))
    {
        return s;
    }
    else
    {
        s |= s >> 1;
        s |= s >> 2;
        s |= s >> 4;
        s |= s >> 8;
        s |= s >> 16;
        return s + 1;
    }
}

struct block
{
    size_t size;
    struct block *next;
    struct block *prev;
};

static struct block *base = NULL;

void *
xmalloc(size_t bytes)
{
    // TODO: write an optimized malloc
    return malloc(bytes);
    void *mem;
    struct block *ptr, *nptr;
    size_t size;
    if (bytes >= ALLOC_SIZE)
    {
        size = bytes + sizeof(struct block);
    }
    else
    {
        size = ALLOC_SIZE;
    }
    ptr = base;
    while (ptr)
    {
        if (ptr->size >= bytes + sizeof(struct block))
        {
            mem = (void *)((size_t)ptr + sizeof(struct block));
            if (!ptr->prev)
            {
                if (ptr->next)
                {
                    base = ptr->next;
                }
                else
                {
                    base = NULL;
                }
            }
            else
            {
                ptr->prev->next = ptr->next;
            }
            if (ptr->next)
            {
                ptr->next->prev = ptr->prev;
            }
            if (ptr->size == bytes)
            {
                return mem;
            }
            void *__mem = (void *)((size_t)ptr + sizeof(struct block));
            nptr = (struct block *)((size_t)__mem + bytes);
            nptr->size = ptr->size - (bytes + sizeof(struct block));
            ptr->size = bytes;
            ptr->prev = NULL;
            ptr->next = NULL;
            if (!base || (size_t)base > (size_t)ptr)
            {
                if (base)
                {
                    base->prev = ptr;
                }
                ptr->next = base;
                base = ptr;
            }
            else
            {
                struct block *curr = base;
                while (curr->next && (size_t)curr->next < (size_t)ptr)
                {
                    curr = curr->next;
                }
                ptr->next = curr->next;
                curr->next = ptr;
            }
            return mem;
        }
        else
        {
            ptr = ptr->next;
        }
    }
    ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (!ptr)
    {
        return NULL;
    }
    ptr->next = NULL;
    ptr->prev = NULL;
    ptr->size = size - sizeof(struct block);
    if (size > bytes + sizeof(struct block))
    {
        void *__mem = (void *)((size_t)ptr + sizeof(struct block));
        nptr = (struct block *)((size_t)__mem + bytes);
        nptr->size = ptr->size - (bytes + sizeof(struct block));
        ptr->size = bytes;
        ptr->prev = NULL;
        ptr->next = NULL;
        if (!base || (size_t)base > (size_t)ptr)
        {
            if (base)
            {
                base->prev = ptr;
            }
            ptr->next = base;
            base = ptr;
        }
        else
        {
            struct block *curr = base;
            while (curr->next && (size_t)curr->next < (size_t)ptr)
            {
                curr = curr->next;
            }
            ptr->next = curr->next;
            curr->next = ptr;
        }
    }
    return (void *)((size_t)ptr + sizeof(struct block));
}

void merge()
{
    struct block *curr = base;
    size_t header_curr, header_next;
    while (curr->next)
    {
        header_curr = (size_t)curr;
        header_next = (size_t)curr->next;
        if (header_curr + curr->size + sizeof(struct block) == header_next)
        {
            curr->size += curr->next->size + sizeof(struct block);
            curr->next = curr->next->next;
            if (curr->next)
            {
                curr->next->prev = curr;
            }
            else
            {
                break;
            }
        }
        curr = curr->next;
    }
}

void add_free_list(struct block *b)
{
    b->prev = NULL;
    b->next = NULL;
    if (!base || (size_t)base > (size_t)b)
    {
        if (base)
        {
            base->prev = b;
        }
        b->next = base;
        base = b;
    }
    else
    {
        struct block *curr = base;
        while (curr->next && (size_t)curr->next < (size_t)b)
        {
            curr = curr->next;
        }
        b->next = curr->next;
        curr->next = b;
    }
}

void xfree(void *ptr)
{
    // TODO: write an optimized free
    free(ptr);
    return;
    add_free_list((void *)((size_t)ptr - sizeof(struct block)));

    merge();
}

void *xrealloc(void *prev, size_t bytes)
{
    // TODO: write an optimized realloc
    return realloc(prev, bytes);
    struct block * ptr = (void *)((size_t)prev - sizeof(struct block));
    if (ptr->size > bytes)
    {
        return prev;
    }
    xfree(prev);
    return xmalloc(bytes);
}
