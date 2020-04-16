#ifndef HARDLINK_MAP_H
#define HARDLINK_MAP_H

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define HARDLINK_ENTRY_SIZE 31

typedef struct hardlink_entry {
    int cnt;
    int data[HARDLINK_ENTRY_SIZE];
} hardlink_entry;

// inti a map
void hardlink_map_init(uint8_t* ptr, int create);

int hardlink_map_entry_isempty(int idx);

// find a entry point
int hardlink_map_find_empty();

int hardlink_map_get_count(int idx);

// when add a hardlink
int hardlink_map_add_dir(int idx, int inum);

// when remove a hardlink
int hardlink_map_remove_dir(int idx, int inum);

#endif