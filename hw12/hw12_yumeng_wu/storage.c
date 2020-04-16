
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <alloca.h>
#include <string.h>
#include <libgen.h>
#include <bsd/string.h>
#include <stdint.h>

#include "storage.h"
#include "bitmap.h"
#include "slist.h"
#include "util.h"
#include "pages.h"
#include "inode.h"
#include "directory.h"
#include "hardlink_map.h"

void
storage_init(const char* path, int create)
{
    //printf("storage_init(%s, %d);\n", path, create);
    pages_init(path, create);
    hardlink_map_init(pages_get_page(BLOCK_COUNT - 1), create);
    if (create) {
        directory_init();
        // bitmap_print(pages_get_page(0), 256);
        // printf("aa create file\n");
        // storage_mknod("/aa.txt", 0100644);
        // printf("bb create file\n");
        // storage_mknod("/bb.txt", 0100644);
        // bitmap_print(pages_get_page(0), 256);
        // printf("aa write\n");
        // char temp[4096 * 4 + 1];
        // memset(temp, 0, 4096 * 4 + 1);
        // memset(temp, 'a', 4096 * 4);
        // storage_write("/aa.txt", temp, strlen(temp), 0);
        // bitmap_print(pages_get_page(0), 256);
        // char buf[4096 * 11];
        // memset(buf, 0, 4096 * 11);
        // printf("aa read\n");
        // storage_read("/aa.txt", buf, 4096 * 4, 0);
        // printf("after read: %d\n", streq(temp, buf));
    }
}

int
storage_stat(const char* path, struct stat* st)
{
    printf("+ storage_stat(%s)\n", path);
    int inum = tree_lookup(path);
    if (inum < 0) {
        return inum;
    }

    inode* node = get_inode(inum);
    printf("+ storage_stat(%s); inode %d\n", path, inum);
    print_inode(node);

    if (st) {
        memset(st, 0, sizeof(struct stat));
        st->st_uid   = getuid();
        st->st_mode  = node->mode;
        st->st_size  = node->size;
        st->st_nlink = 1;
        st->st_atime = node->atime;
        st->st_mtime = node->mtime;
        st->st_blocks = node->size / 4096 + (node->size && node->size % 4096 == 0 ? 0 : 1);
    }
    return 0;
}

int
storage_read(const char* path, char* buf, size_t size, off_t offset)
{
    int inum = tree_lookup(path);
    if (inum < 0) {
        return inum;
    }
    inode* node = get_inode(inum);
    if (S_ISDIR(node->mode)) {
        return -EISDIR;
    }
    printf("+ storage_read(%s); inode %d\n", path, inum);
    print_inode(node);

    if (offset >= node->size) {
        return 0;
    }

    if (offset + size >= node->size) {
        size = node->size - offset;
    }

    // read data
    // uint8_t* data = pages_get_page(inum);
    // printf(" + reading from page: %d\n", inum);
    int block_num = node->size / 4096 + (node->size && node->size % 4096 == 0 ? 0 : 1);
    int block_start = offset / 4096 + 1;
    offset %= 4096;
    int remain_bytes = size;
    int idx = 0;

    // read direct pointers
    for (int i = block_start; i <= 3 && remain_bytes; ++i) {
        printf(" + reading from page: %d\n", node->blocks[i - 1]);
        uint8_t* page = pages_get_page(node->blocks[i - 1]);
        int read_bytes = 4096 - offset;
        if (read_bytes > remain_bytes) {
            read_bytes = remain_bytes;
        }
        memcpy(buf + idx, page + offset, read_bytes);
        idx += read_bytes;
        remain_bytes -= read_bytes;
        offset = 0;
        printf(" + reading %d bytes, remain %d bytes\n", read_bytes, remain_bytes);
    }

    // read indirect pointers
    if (remain_bytes) {
        block_start -= 4;
        printf("block start %d\n", block_start);
        printf("+ read indirect pointers at block: %d, remain %d bytes\n", node->blocks[3], remain_bytes);
        uint16_t* indir_table = pages_get_page(node->blocks[3]);
        while (block_start > 0) {
            ++indir_table;
            --block_start;
        }
        while (remain_bytes) {
            int read_bytes = 4096 - offset;
            if (read_bytes > remain_bytes) {
                read_bytes = remain_bytes;
            }
            printf(" + reading from page: %d\n", *indir_table);
            memcpy(buf + idx, pages_get_page(*indir_table) + offset, read_bytes);
            idx += read_bytes;
            remain_bytes -= read_bytes;
            ++indir_table;
            offset = 0;
            printf(" + reading %d bytes, remain %d bytes\n", read_bytes, remain_bytes);
        }
    }

    // memcpy(buf, data + offset, size);
    time_t utime = time(0);
    node->atime = utime;

    return idx;
}

int
storage_write(const char* path, const char* buf, size_t size, off_t offset)
{
    int inum = tree_lookup(path);
    if (inum < 0) {
        return -ENOENT;
    }
    inode* node = get_inode(inum);

    if (S_ISDIR(node->mode)) {
        return -EISDIR;
    }

    int trv = storage_truncate(path, offset + size);
    if (trv < 0) {
        return trv;
    }

    printf("+ storage_write(%s); inode %d\n", path, inum);
    // uint8_t* data = pages_get_page(inum);
    // printf("+ writing to page: %d\n", inum);
    // memcpy(data + offset, buf, size);
    int block_start = offset / 4096 + 1;
    int remain_bytes = size;
    int idx = 0;
    offset %= 4096;

    // write direct pointers
    for (int i = block_start; i <= 3 && remain_bytes; ++i) {
        printf(" + writing page %d\n", node->blocks[i - 1]);
        uint8_t* page = pages_get_page(node->blocks[i - 1]);
        int write_bytes = 4096 - offset;
        if (write_bytes > remain_bytes) {
            write_bytes = remain_bytes;
        }
        memcpy(page + offset, buf + idx, write_bytes);
        idx += write_bytes;
        remain_bytes -= write_bytes;
        offset = 0;
        printf(" + writing %d bytes, remain %d bytes\n", write_bytes, remain_bytes);
    }

    // write indirect pointers
    if (remain_bytes) {
        printf("+ write indirect pointers at block: %d, remain %d bytes\n", node->blocks[3], remain_bytes);
        uint16_t* indir_table = pages_get_page(node->blocks[3]);
        block_start -= 4;
        while (block_start) {
            --block_start;
            ++indir_table;
        }
        while (remain_bytes) {
            int write_bytes = 4096 - offset;
            if (write_bytes > remain_bytes) {
                write_bytes = remain_bytes;
            }
            printf(" + writing from page: %d\n", *indir_table);
            memcpy(pages_get_page(*indir_table) + offset, buf + idx, write_bytes);
            idx += write_bytes;
            remain_bytes -= write_bytes;
            ++indir_table;
            offset = 0;
            printf(" + writing %d bytes, remain %d bytes\n", write_bytes, remain_bytes);
        }
    }
    time_t utime = time(0);
    node->mtime = utime;

    return idx;
}

int
storage_truncate(const char *path, off_t size)
{
    printf("truncate %s -> %ld bytes\n", path, size);
    int inum = tree_lookup(path);
    if (inum < 0) {
        return inum;
    }

    // inode* node = get_inode(inum);
    // node->size = size;
    return resize_inode(inum, size);
}

int
storage_mknod(const char* path, int mode)
{
    char* tmp1 = alloca(strlen(path));
    char* tmp2 = alloca(strlen(path));
    strcpy(tmp1, path);
    strcpy(tmp2, path);

    const char* name = path + 1;

    slist* xs = s_split(name, '/');

    if (directory_lookup(get_inode(0), xs) != -ENOENT) {
        printf("mknod fail: already exist\n");
        return -EEXIST;
    }

    int    inum = alloc_inode();
    if (inum < 0) {
        return -ENOSPC;
    }
    inode* node = get_inode(inum);
    node->mode = mode;
    node->size = 0;

    printf("+ mknod create %s [%04o] - #%d\n", path, mode, inum);
    if (!xs->next) {
        if (mode & 0040000) {
            printf(" + mknode, init dir. cur: %d, parent: %d\n", inum, 0);
            uint8_t* page = pages_get_page(node->blocks[0]);
            dirent* dirs = (dirent*) page;
            strcpy(dirs[0].name, ".");
            dirs[0].inum = inum;
            strcpy(dirs[1].name, "..");
            dirs[1].inum = 0;
        }
        int res = directory_put(get_inode(0), xs->data, inum);
        s_free(xs);
        return res;
    }
    xs = s_rev_free(xs);
    slist* ys = xs->next;
    xs->next = 0;
    ys = s_rev_free(ys);

    int parent_inode = directory_lookup(get_inode(0), ys);
    if (parent_inode < 0) {
        s_free(ys);
        s_free(xs);
        return -ENOENT;
    }

    // if this is a dir, init . and ..
    if (mode & 0040000) {
        printf(" + mknode, init dir. cur: %d, parent: %d\n", inum, parent_inode);
        uint8_t* page = pages_get_page(node->blocks[0]);
        dirent* dirs = (dirent*) page;
        strcpy(dirs[0].name, ".");
        dirs[0].inum = inum;
        strcpy(dirs[1].name, "..");
        dirs[1].inum = parent_inode;
    }

    int res = directory_put(get_inode(parent_inode), xs->data, inum);
    s_free(ys);
    s_free(xs);

    return res;
}

slist*
storage_list(const char* path)
{
    int inum = tree_lookup(path);
    if (inum < 0) {
        return 0;
    }
    return directory_list(get_inode(inum));
}

int
storage_rmdir(const char* path)
{
    printf("+ storage_unlink (%s)\n", path);
    int inum = tree_lookup(path);
    if (inum < 0) {
        return -ENOENT;
    }

    inode* dd = get_inode(inum);
    if (!S_ISDIR(dd->mode)) {
        return -ENOTDIR;
    }
    return storage_unlink(path);
}

int
storage_unlink(const char* path)
{
    printf("+ storage_unlink (%s)\n", path);
    int inum = tree_lookup(path);
    if (inum < 0) {
        return -ENOENT;
    }

    inode* dd = get_inode(inum);
    slist* xs = s_split(path + 1, '/');
    xs = s_rev_free(xs);
    slist* ys = xs->next;

    int parent_inum = ys == 0 ? 0 : directory_lookup(get_inode(0), ys);
    // if (dd->ref >= 0) {
    //     hardlink_map_remove_dir(dd->ref, parent_inum);
    //     if (hardlink_map_get_count(dd->ref) > 0) {
    //         dirent* dirs = pages_get_page(get_inode(parent_inum)->blocks[0]);
    //         for (int i = 0; i < 4096 / sizeof(dirent); ++i) {
    //             if (streq(dirs[i].name, xs->data)) {
    //                 memset(&dirs[i], 0, sizeof(dirent));
    //                 break;
    //             }
    //         }
    //     }
        
    //     if (hardlink_map_entry_isempty(dd->ref)) {
    //         dd->ref = -1;
    //     }
    // }
    // if (dd->ref < 0) {
    //     printf(" + delete file (%s) from parent (%s) with inum %d\n", xs->data, ys ? ys->data : "", parent_inum);
    //     int res = directory_delete(get_inode(parent_inum), xs->data);
    //     s_free(xs);
    //     s_free(ys);
    //     return res;
    // }

    int res = directory_delete(parent_inum, xs->data);

    s_free(xs);
    s_free(ys);
    return res;
}

int
storage_link(const char* from, const char* to)
{
    printf("+ storage_link (from: %s, to: %s)\n", from, to);
    int from_inum = tree_lookup(from);
    if (from_inum < 0) {
        return -ENOENT;
    }
    if (tree_lookup(to) != -ENOENT) {
        return -EEXIST;
    }

    int entry_idx = hardlink_map_find_empty();
    printf(" + hardlink map entry: %d\n", entry_idx);
    if (entry_idx < 0) {
        return -ENOSPC;
    }

    inode* from_node = get_inode(from_inum);
    from_node->ref = entry_idx;

    // add from parent node
    slist* xs = s_split(from + 1, '/');
    xs = s_rev_free(xs);
    slist* ys = xs->next;
    xs->next = 0;
    ys = s_rev_free(ys);

    if (ys == 0) {
        hardlink_map_add_dir(entry_idx, 0);
    }
    else {
        hardlink_map_add_dir(entry_idx, directory_lookup(get_inode(0), ys));
    }
    s_free(xs);
    s_free(ys);

    // add to dir node
    xs = s_split(to + 1, '/');
    xs = s_rev_free(xs);
    ys = xs->next;
    xs->next = 0;
    ys = s_rev_free(ys);

    int topn = 0;

    if (ys == 0) {
        hardlink_map_add_dir(entry_idx, 0);
    }
    else {
        topn = directory_lookup(get_inode(0), ys);
        hardlink_map_add_dir(entry_idx, topn);
    }

    directory_put(get_inode(topn), xs->data, from_inum);

    s_free(xs);
    s_free(ys);
    return 0;
}

int
storage_rename(const char* from, const char* to)
{
    // int inum = directory_lookup(NULL, from + 1);
    // if (inum < 0) {
    //     printf("mknod fail");
    //     return inum;
    // }

    // char* ent = directory_get(inum);
    // strlcpy(ent, to + 1, 16);

    printf("+ storage_rename (from: %s, to: %s)\n", from, to);
    struct stat st;
    int res;
    res = storage_stat(from, &st);
    if (res < 0) {
        return res;
    }
    char buf[st.st_size + 1];
    res = storage_read(from, buf, st.st_size, 0);
    if (res < 0) {
        return res;
    }
    res = storage_unlink(from);
    if (res < 0) {
        return res;
    }
    res = storage_mknod(to, 0100644);
    if (res < 0) {
        return res;
    }
    res = storage_write(to, buf, st.st_size, 0);

    return 0;
}

int
storage_chmod(const char *path, mode_t mode)
{
    printf("+ storage_chmod(%s, %04o)\n", path, mode);
    int inum = tree_lookup(path);
    if (inum < 0) {
        return -ENOENT;
    }
    inode* node = get_inode(inum);
    node->mode &= 0xffffffff ^ 0777;
    node->mode |= mode;
    time_t utime = time(0);
    node->mtime = utime;
    return 0;
}

int
storage_set_time(const char* path, const struct timespec ts[2])
{
    // Maybe we need space in a pnode for timestamps.
    printf("+ storage_set_time(%s)\n", path);
    int inum = tree_lookup(path);
    if (inum < 0) {
        return -ENOENT;
    }

    inode* node = get_inode(inum);
    node->atime = ts[0].tv_sec;
    node->mtime = ts[1].tv_sec;
    return 0;
}

int
storage_symlink(const char * target, const char * link_path)
{
    printf("+ storage_symlink(%s => %s)\n", target, link_path);
    // check target
    int target_inum = tree_lookup(target);
    if (target_inum < 0) {
        return -ENOENT;
    }
    inode* target_node = get_inode(target_inum);
    if (!S_ISDIR(target_node->mode)) {
        return -ENOTDIR;
    }

    // check link
    if (tree_lookup(link_path) != ENOENT) {
        return -EEXIST;
    }

    int res = storage_mknod(link_path, target_node->mode & 0777 | 0120000);
    if (res < 0) {
        return res;
    }
    res = storage_write(link_path, target, strlen(target), 0);
    return res == strlen(target_node) ? 0 : res;
}

int
storage_readlink(const char * path_name, char * buf, size_t size)
{
    printf("+ storage_readlink(%s)\n", path_name);
    int inum = tree_lookup(path_name);
    if (inum < 0) {
        return -ENOENT;
    }
    return 0;
}
