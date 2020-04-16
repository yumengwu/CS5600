#include "hardlink_map.h"

static hardlink_entry* hmbase;

void
hardlink_map_init(uint8_t* ptr, int create)
{
    printf("hardlink_map_init, create: %d\n", create);
    printf("size of hardlink_entry: %d\n", sizeof(hardlink_entry));
    hmbase = ptr;
    int num = 4096 / sizeof(hardlink_entry);
    if (create) {
        memset(hmbase, -1, 4096);
        for (int i = 0; i < num; ++i) {
            hmbase[i].cnt = 0;
        }
    }
}

int
hardlink_map_entry_isempty(int idx)
{
    return idx < 0 || idx > 4096 / sizeof(hardlink_entry) || hmbase[idx].cnt == 0 ? 1 : 0;
}

int
hardlink_map_get_count(int idx)
{
    return idx < 0 || idx > 4096 / sizeof(hardlink_entry) ? -1 : hmbase[idx].cnt;
}

int
hardlink_map_find_empty()
{
    int num = 4096 / sizeof(hardlink_entry);
    for (int i = 0; i < num; ++i) {
        if (hmbase[i].cnt == 0) {
            return i;
        }
    }
    return -1;
}

int
hardlink_map_add_dir(int idx, int inum)
{
    if (idx < 0 || idx >= 4096 / sizeof(hardlink_entry)) {
        return -1;
    }
    ++hmbase[idx].cnt;
    for (int i = 0; i < HARDLINK_ENTRY_SIZE; ++i) {
        if (hmbase[idx].data[i] < 0) {
            hmbase[idx].data[i] = inum;
            return 0;
        }
    }
    return -1;
}

int
hardlink_map_remove_dir(int idx, int inum)
{
    if (idx < 0 || idx >= 4096 / sizeof(hardlink_entry)) {
        return -1;
    }
    for (int i = 0; i < HARDLINK_ENTRY_SIZE; ++i) {
        if (hmbase[idx].data[i] == inum) {
            hmbase[idx].data[i] = -1;
            break;
        }
    }
    --hmbase[idx].cnt;
    return 0;
}
