#ifndef INODE_H
#define INODE_H

#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include "pages.h"

#define INODE_TABLE_OFFSET 32
#define INODE_OFFSET 64

typedef struct inode {
    // int mode; // permission & type; zero for unused
    // int size; // bytes
    // // inode #x always uses data page #x
    uint16_t mode;      // permission & type; zero for unused
    short ref;          // reference map
    int size;           // file size, bytes
    uint8_t blocks[4];  // idx 0~2 for direct pointers, 3 is indirect pointer
    time_t atime;       // last access time
    time_t mtime;       // last modification time
} inode;

void print_inode(inode* node);
inode* get_inode(int inum);
int alloc_inode();
int resize_inode(int inum, int size);
void free_inode();
int inode_get_pnum(inode* node, int fpn);

#endif
