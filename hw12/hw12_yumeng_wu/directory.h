#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <bsd/string.h>

#define DIR_NAME 48

#include "slist.h"
#include "pages.h"
#include "inode.h"

typedef struct dirent {
    char name[DIR_NAME];
    int  inum;
    char _reserved[12];
} dirent;

char* directory_get(int inum);
void directory_init();
int directory_lookup(inode* dd, slist* path_list);
int tree_lookup(const char* path);
int directory_put(inode* dd, const char* name, int inum);
int directory_delete(inode* dd, const char* name);
slist* directory_list(inode* dd);
void print_directory();

#endif

