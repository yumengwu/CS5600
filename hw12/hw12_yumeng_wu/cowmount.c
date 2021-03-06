#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <bsd/string.h>
#include <assert.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>

#include "storage.h"
#include "slist.h"
#include "util.h"

// implementation for: man 2 access
// Checks if a file exists.
int
nufs_access(const char *path, int mask)
{
    printf("=============nufs_access=============\n  path: %s\n", path);
    int rv = storage_stat(path, NULL);
    printf("access(%s, %04o) -> %d\n", path, mask, rv);
    return rv;
}

// implementation for: man 2 stat
// gets an object's attributes (type, permissions, size, etc)
int
nufs_getattr(const char *path, struct stat *st)
{
    printf("=============nufs_getattr=============\n");
    int rv = storage_stat(path, st);
    printf("getattr(%s) -> (%d) {mode: %04o, size: %ld}\n", path, rv, st->st_mode, st->st_size);
    return rv;
}

// implementation for: man 2 readdir
// lists the contents of a directory
int
nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi)
{
    printf("=============nufs_readdir=============\n");
    struct stat st;
    char item_path[256];
    int rv;

    printf("readdir(%s)\n", path);

    rv = storage_stat(path, &st);
    assert(rv == 0);

    filler(buf, ".", &st, 0);

    slist* items = storage_list(path);
    for (slist* xs = items; xs != 0; xs = xs->next) {
        printf("+ looking at path: '%s'\n", xs->data);
        // item_path[0] = '/';
        // strlcpy(item_path + 1, xs->data, 127);
        strcpy(item_path, path);
        int path_len = strlen(path);
        if (path[path_len - 1] != '/') {
            strcat(item_path, "/");
        }
        strcat(item_path, xs->data);
        rv = storage_stat(item_path, &st);
        assert(rv == 0);
        filler(buf, xs->data, &st, 0);
    }
    s_free(items);

    printf("readdir(%s) -> %d\n", path, rv);
    return 0;
}

// mknod makes a filesystem object like a file or directory
// called for: man 2 open, man 2 link
int
nufs_mknod(const char *path, mode_t mode, dev_t rdev)
{
    printf("=============nufs_mknod=============\n");
    int rv = storage_mknod(path, mode, 1);
    printf("mknod(%s, %04o) -> %d\n", path, mode, rv);
    return rv;
}

// most of the following callbacks implement
// another system call; see section 2 of the manual
int
nufs_mkdir(const char *path, mode_t mode)
{
    printf("=============nufs_mkdir=============\n  --  path: %s, mode: %o\n", path, mode);
    int rv = storage_mknod(path, mode | 0040000, 1);
    printf("mkdir(%s) -> %d\n", path, rv);
    return rv;
}

int
nufs_unlink(const char *path)
{
    printf("=============nufs_unlink=============\n");
    int rv = storage_unlink(path, 1);
    printf("unlink(%s) -> %d\n", path, rv);
    return rv;
}

int
nufs_link(const char *from, const char *to)
{
    printf("=============nufs_link=============\n");
    int rv = storage_link(from, to, 1);
    printf("link(%s => %s) -> %d\n", from, to, rv);
	return rv;
}

int
nufs_rmdir(const char *path)
{
    printf("=============nufs_rmdir=============\n");
    int rv = storage_rmdir(path);
    printf("rmdir(%s) -> %d\n", path, rv);
    return rv;
}

// implements: man 2 rename
// called to move a file within the same filesystem
int
nufs_rename(const char *from, const char *to)
{
    printf("=============nufs_rename=============\n");
    int rv = storage_rename(from, to, 1);
    printf("rename(%s => %s) -> %d\n", from, to, rv);
    return rv;
}

int
nufs_chmod(const char *path, mode_t mode)
{
    printf("=============nufs_chmod=============\n");
    int rv = storage_chmod(path, mode, 1);
    printf("chmod(%s, %04o) -> %d\n", path, mode, rv);
    return rv;
}

int
nufs_truncate(const char *path, off_t size)
{
    printf("=============nufs_truncate=============\n");
    int rv = storage_truncate(path, size, 1);
    printf("truncate(%s, %ld bytes) -> %d\n", path, size, rv);
    return rv;
}

// this is called on open, but doesn't need to do much
// since FUSE doesn't assume you maintain state for
// open files.
int
nufs_open(const char *path, struct fuse_file_info *fi)
{
    printf("=============nufs_open=============\n");
    int rv = nufs_access(path, 0);
    printf("open(%s) -> %d\n", path, rv);
    return rv;
}

// Actually read data
int
nufs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("=============nufs_read=============\n");
    int rv = storage_read(path, buf, size, offset);
    printf("read(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);
    return rv;
}

// Actually write data
int
nufs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("=============nufs_write=============\n   path: %s, size: %d, off: %d\n", path, size, offset);
    int rv = storage_write(path, buf, size, offset, 1);
    printf("write(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);
    return rv;
}

// Update the timestamps on a file or directory.
int
nufs_utimens(const char* path, const struct timespec ts[2])
{
    printf("=============nufs_utimens=============\n");
    int rv = storage_set_time(path, ts, 1);
    printf("utimens(%s, [%ld, %ld; %ld %ld]) -> %d\n",
           path, ts[0].tv_sec, ts[0].tv_nsec, ts[1].tv_sec, ts[1].tv_nsec, rv);
	return rv;
}

// Extended operations
int
nufs_ioctl(const char* path, int cmd, void* arg, struct fuse_file_info* fi,
           unsigned int flags, void* data)
{
    printf("=============nufs_ioctl=============\n");
    int rv = -1;
    printf("ioctl(%s, %d, ...) -> %d\n", path, cmd, rv);
    return rv;
}

int
nufs_symlink(const char * target, const char * link_path)
{
    printf("=============nufs_symlink=============\n  path (%s), link_path (%s)\n", target, link_path);
    int rv = storage_symlink(target, link_path);
    printf("symlink(%s => %s) -> %d\n", target, link_path, rv);
    return rv;
}

int
nufs_readlink(const char * path_name, char * buf, size_t size)
{
    printf("=============nufs_readlink=============\n  path (%s)\n", path_name);
    int rv = storage_readlink(path_name, buf, size);
    printf("readlink(%s) -> %d\n", path_name, rv);
    return rv;
}

void
nufs_init_ops(struct fuse_operations* ops)
{
    memset(ops, 0, sizeof(struct fuse_operations));
    ops->access   = nufs_access;
    ops->getattr  = nufs_getattr;
    ops->readdir  = nufs_readdir;
    ops->mknod    = nufs_mknod;
    ops->mkdir    = nufs_mkdir;
    ops->link     = nufs_link;
    ops->unlink   = nufs_unlink;
    ops->rmdir    = nufs_rmdir;
    ops->rename   = nufs_rename;
    ops->chmod    = nufs_chmod;
    ops->truncate = nufs_truncate;
    ops->open	  = nufs_open;
    ops->read     = nufs_read;
    ops->write    = nufs_write;
    ops->utimens  = nufs_utimens;
    ops->ioctl    = nufs_ioctl;
    ops->symlink  = nufs_symlink;
    ops->readlink = nufs_readlink;
};

struct fuse_operations nufs_ops;

int
main(int argc, char *argv[])
{
    assert(argc > 2 && argc < 6);
    storage_init(argv[--argc], 0);
    nufs_init_ops(&nufs_ops);
    return fuse_main(argc, argv, &nufs_ops, NULL);
}

