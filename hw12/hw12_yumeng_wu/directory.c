
#define _GNU_SOURCE
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

#include "bitmap.h"
#include "directory.h"
#include "pages.h"
#include "slist.h"
#include "util.h"
#include "inode.h"
#include "hardlink_map.h"

#define ENT_SIZE 16

void
directory_init()
{
    inode* rn = get_inode(0);
    char* block_bm = pages_get_page(0);
    char* inode_bm = block_bm + INODE_TABLE_OFFSET;

    // alloc root dir
    bitmap_put(block_bm, 2, 1);
    bitmap_put(inode_bm, 0, 1);

    if (rn->mode == 0) {
        rn->size = 0;
        rn->mode = 040755;
        rn->ref = -1;
        rn->blocks[0] = 2;
        uint8_t* page = pages_get_page(2);
        dirent* dirs = (dirent*) page;
        memset(dirs, 0, 4096);
        
        // .
        dirs[0].inum = 0;
        strcpy(dirs[0].name, ".");
        // ..
        dirs[1].inum = 0;
        strcpy(dirs[1].name, "..");
    }
}

char*
directory_get(int ii)
{
    char* base = pages_get_page(2);
    return base + ii*ENT_SIZE;
}

int
directory_lookup(inode* dd, slist* path_list)
{
    // if no such inode
    if (!dd) {
        return -ENOENT;
    }
    // if this is not a dir
    if (dd->mode & 0100000) {
        return -1;
    }
    uint8_t* page = pages_get_page(dd->blocks[0]);
    dirent* dirs = (dirent*) page;
    for (int ii = 0; ii < 4096 / sizeof(dirent); ++ii) {
        char* ent = dirs[ii].name;
        if (streq(ent, path_list->data)) {
            return path_list->next ? directory_lookup(get_inode(dirs[ii].inum), path_list->next) : dirs[ii].inum;
        }
    }
    return -ENOENT;
}

int
tree_lookup(const char* path)
{
    assert(path[0] == '/');

    if (streq(path, "/")) {
        return 0;
    }

    // split the path string and search start at root
    slist* path_list = s_split(path + 1, '/');
    inode* root_node = get_inode(0);

    int res = directory_lookup(root_node, path_list);

    s_free(path_list);

    return res;
}

int
directory_put(inode* dd, const char* name, int inum)
{
    // char* ent = pages_get_page(1) + inum*ENT_SIZE;
    // strlcpy(ent, name, ENT_SIZE);

    uint8_t* base = pages_get_page(dd->blocks[0]);
    dirent* dirs = (dirent*) base;
    int idx = -1;
    // find a empty dirent
    for (int i = 0; i < 4096 / sizeof(dirent); ++i) {
        if (strlen(dirs[i].name) == 0) {
            idx = i;
            break;
        }
    }
    if (idx < 0) {
        return -1;
    }

    strncpy(dirs[idx].name, name, DIR_NAME - 1);
    printf("+ dirent = '%s'\n", dirs[idx].name);
    dirs[idx].inum = inum;

    inode* node = get_inode(inum);
    printf("+ directory_put(..., %s, %d) -> 0\n", name, inum);
    print_inode(node);

    return 0;
}

int
directory_delete(int inum, const char* name)
{
    printf(" + directory_delete(%s)\n", name);
    if (!name || streq(name, ".") || streq(name, "..")) {
        return -1;
    }
    
    inode* dd = get_inode(inum);

    uint8_t* page = pages_get_page(dd->blocks[0]);
    dirent* dirs = (dirent*) page;
    int dir_per_page = 4096 / sizeof(dirent);
    int idx = -1;
    // find idx in dirent table
    for (int i = 0; i < dir_per_page; ++i) {
        if (streq(name, dirs[i].name)) {
            idx = i;
            break;
        }
    }
    if (idx < 0) {
        return -1;
    }

    // get inode to delete
    inode* node = get_inode(dirs[idx].inum);

    if (node->ref >= 0) {
        hardlink_map_remove_dir(node->ref, inum);
        if (hardlink_map_get_count(node->ref) > 0) {
            memset(&dirs[idx], 0, sizeof(dirent));
        }
        
        if (hardlink_map_entry_isempty(node->ref)) {
            node->ref = -1;
        }
    }
    if (node->ref >= 0) {
        return 0;
    }
    // if it is a file
    if (node->mode & 0100000) {
        printf("  delete file with inode: %d\n", dirs[idx].inum);
        free_inode(dirs[idx].inum);
    }
    else {
        printf("  delete dir %s with inode: %d\n", dirs[idx].name, dirs[idx].inum);
        uint8_t* node_page = pages_get_page(get_inode(dirs[idx].inum)->blocks[0]);
        dirent* node_dirs = (dirent*) node_dirs;
        for (int i = 0; i < dir_per_page; ++i) {
            if (!(streq(node_dirs[i].name, ".") || streq(node_dirs[i].name, "..") || strlen(node_dirs[i].name) == 0)) {
                directory_delete(dirs[idx].inum, node_dirs[i].name);
            }
        }
        free_inode(dirs[idx].inum);
    }
    memset(&dirs[idx], 0, sizeof(dirent));

    return 0;
}

slist*
directory_list(inode* dd)
{
    printf("+ directory_list()\n");
    slist* ys = 0;

    if (dd->mode & 010000) {
        return ys;
    }
    uint8_t* page = pages_get_page(dd->blocks[0]);
    dirent* dirs = (dirent*) page;

    for (int ii = 0; ii < 4096 / sizeof(dirent); ++ii) {
        if (strlen(dirs[ii].name)) {
            printf(" - %d: %s [%d]\n", ii, dirs[ii].name, ii);
            ys = s_cons(dirs[ii].name, ys);
        }
    }

    return ys;
}

void
print_directory(inode* dd)
{
    printf("Contents:\n");
    slist* items = directory_list(dd);
    for (slist* xs = items; xs != 0; xs = xs->next) {
        printf("- %s\n", xs->data);
    }
    printf("(end of contents)\n");
    s_free(items);
}
