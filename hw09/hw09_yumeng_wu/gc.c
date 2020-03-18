
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "gc.h"

#define CHUNK_SIZE (1024 * 1024)
#define ALLOC_UNIT 16
#define CELL_COUNT (CHUNK_SIZE / ALLOC_UNIT)

#define peq(aa, bb) (((intptr_t)aa) == ((intptr_t)bb))

typedef uint16_t u16;
typedef uint8_t u8;

typedef struct cell {
    u16 size; // Size is in multiples of ALLOC_UNIT
    u16 next;
    u16 conf; // Serves as a confirmation that some location in
              // memory really is a cell. Stores (size * 7) % (2^16)
    u8  used;
    u8  mark;
} cell;

// We track both a free list and a used list. The used list is nessiary for the
// sweep phase of mark and sweep.
//
// Both lists are built with the cell structure above. Since allocated memory is
// on a list, there's no distinction between a list cell and a header for
// allocated memory.
static u16 free_list = 0;
static u16 used_list = 0;

// the bottom of the 1MB garbage collected heap
static void* chunk_base = 0;

// our best guess at the top of the stack
static intptr_t stack_top = 0;

// Stats
static size_t bytes_allocated = 0;
static size_t bytes_freed = 0;
static size_t blocks_allocated = 0;
static size_t blocks_freed = 0;


cell*
o2p(u16 off)
{
    if (off == 0) {
        return 0;
    }
    intptr_t addr = (intptr_t)chunk_base;
    addr += off * ALLOC_UNIT;
    return (cell*) addr;
}

u16
p2o(cell* ptr)
{
    if (ptr == 0) {
        return 0;
    }
    intptr_t addr0 = (intptr_t)chunk_base;
    intptr_t addr1 = (intptr_t)ptr;
    intptr_t units = (addr1 - addr0) / ALLOC_UNIT;
    return (u16) units;
}

void
print_cell(cell* cc)
{
    if (cc) {
        printf(
            "cell %p +%d {size: %d, next: %d, conf: %d, used: %d, mark: %d}\n",
            cc, p2o(cc), cc->size, cc->next, cc->conf, cc->used, cc->mark
        );
    }
    else {
        printf("null cell\n");
    }
}

void
gc_print_info(void* addr)
{
    cell* cc = ((cell*)addr) - 1;
    print_cell(cc);
}

static
int
list_length(u16 off)
{
    if (off == 0) {
        return 0;
    }

    cell* item = o2p(off);
    return 1 + list_length(item->next);
}

static
long
list_total(u16 off)
{
    if (off == 0) {
        return 0;
    }

    cell* item = o2p(off);
    long rest = list_total(item->next);
    return rest + ALLOC_UNIT*item->size;
}


void
gc_print_stats()
{
    printf("== gc stats ==\n");
    printf("bytes allocated: %ld\n", bytes_allocated);
    printf("bytes freed: %ld\n", bytes_freed);
    printf("blocks allocated: %ld\n", blocks_allocated);
    printf("blocks freed: %ld\n", blocks_freed);
    printf("used_list length: %d\n", list_length(used_list));
    printf("free_list length: %d\n", list_length(free_list));
    printf("used space: %ld\n", list_total(used_list));
    printf("free space: %ld\n", list_total(free_list));
}

u16
insert_free(u16 coff, cell* item)
{
    assert(item != 0);

    // TODO: insert item into list in
    // sorted order and coalesce if needed

    return 0;
}

static
void
insert_used(cell* item)
{
    assert(item);
    u16 ioff = p2o(item);
    item->used = 1;
    item->next = used_list;
    used_list = ioff;
}

void
gc_init(void* main_frame)
{
    intptr_t addr = (intptr_t)main_frame;
    intptr_t pages = addr / 4096 + 1;
    stack_top = pages * 4096;

    chunk_base = aligned_alloc(CHUNK_SIZE, CHUNK_SIZE);
    assert(chunk_base != 0);
    memset(chunk_base, 0, CHUNK_SIZE);

    cell* base_cell = (cell*) o2p(1);
    base_cell->size = CELL_COUNT - 1;
    base_cell->next = 0;

    free_list = 1;
}

#if 0
static
void
check_list(u16 coff)
{
    assert(bytes_freed <= bytes_allocated);
    if (coff == 0) {
        return;
    }

    cell* cc = o2p(coff);
    assert(7*cc->size == cc->conf);
    check_list(cc->next);
}

static
void
print_list(u16 off)
{
    if (off == 0) {
        printf("list done");
        return;
    }

    cell* item = o2p(off);
    print_cell(item);
    print_list(item->next);
}
#endif

static
int
div_round_up(int num, int den)
{
    int quo = num / den;
    return (num % den == 0) ? quo : quo + 1;
}

static
void*
gc_malloc1(size_t bytes)
{
    //check_list(used_list);

    u16 units = (u16)div_round_up(bytes + sizeof(cell), ALLOC_UNIT);

    bytes_allocated += units * ALLOC_UNIT;
    blocks_allocated += 1;

    u16* pptr = &free_list;
    for (cell* cc = o2p(free_list); cc; pptr = &(cc->next), cc = o2p(*pptr)) {
        if (units <= cc->size) {
            // TODO: Split cells when appropriate.
            //
            // This currently just allocates the whole heap to the first
            // request.
            //
            // if (units < cc->size) {
            //   do something
            // }
            // else {
            //   this is doing the right thing for this case
            // }

            cell* dd = 0;
            *pptr = cc->next;
            dd = cc;

            dd->conf = 7*dd->size;
            insert_used(dd);

            void* addr = (void*)(dd + 1);
            memset(addr, 0x7F, bytes);

            assert(dd->size == units);
            assert(dd->conf == 7*dd->size);

            //check_list(used_list);
            return addr;
        }
    }

    return 0;
}

void*
gc_malloc(size_t bytes)
{
    void* addr;

    addr = gc_malloc1(bytes);
    if (addr) {
        return addr;
    }

    // First attempt failed. Run gc and try once more.
    gc_collect();

    addr = gc_malloc1(bytes);
    if (addr) {
        return addr;
    }

    gc_print_stats();
    fflush(stdout);

    fprintf(stderr, "oom @ malloc(%ld)\n", bytes);
    fflush(stderr);
    abort();
}

static
void
mark_range(intptr_t bot, intptr_t top)
{
    intptr_t chunk_bot = (intptr_t)chunk_base;
    intptr_t chunk_top = chunk_bot + CHUNK_SIZE;

    // TODO: Delete this next line.
    (void) (chunk_bot + chunk_top);

    // TODO: scan the region of memory (bot..top) for pointers
    // onto the garbage collected heap. Assume that anything pointing
    // to a location from (chunk_bot..chunk_top) is a pointer.
    //
    // If a pointer exists to an allocated block, set its mark flag
    // and recursively mark_range on the memory in that block.
}

static
void
mark()
{
    intptr_t stack_bot = 0;
    intptr_t bot = (intptr_t) &stack_bot;
    mark_range(bot, stack_top);
}

static
void
sweep()
{
    // TODO: For each item on the used list, check if it's been
    // marked. If not, free it - probalby by calling insert_free.
}

void
gc_collect()
{
    mark();
    sweep();
}
