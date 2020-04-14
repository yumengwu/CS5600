#ifndef INODE_H
#define INODE_H

#include <stdint.h>
#include "pages.h"

#define INODE_TABLE_OFFSET 32
#define INODE_OFFSET 64

typedef struct inode {
    // int mode; // permission & type; zero for unused
    // int size; // bytes
    // // inode #x always uses data page #x
    uint32_t mode;      // permission & type; zero for unused
    uint32_t count;     // reference count
    int size;           // file size, bytes
    uint16_t blocks[4];  // idx 0~2 for direct pointers, 3 is indirect pointer
    uint32_t time;      // last access time
    uint32_t ctime;     // create time
} inode;

void print_inode(inode* node);
inode* get_inode(int inum);
int alloc_inode();
void free_inode();
int inode_get_pnum(inode* node, int fpn);

#endif
