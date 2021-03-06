#ifndef NUFS_STORAGE_H
#define NUFS_STORAGE_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "slist.h"

void   storage_init(const char* path, int create);
int    storage_stat(const char* path, struct stat* st);
int    storage_read(const char* path, char* buf, size_t size, off_t offset);
int    storage_write(const char* path, const char* buf, size_t size, off_t offset, int save);
int    storage_truncate(const char *path, off_t size, int save);
int    storage_mknod(const char* path, int mode, int save); 
int    storage_unlink(const char* path, int save);
int    storage_rmdir(const char* path);
int    storage_link(const char *from, const char *to, int save);
int    storage_rename(const char *from, const char *to, int save);
int    storage_chmod(const char *path, mode_t mode, int save);
int    storage_set_time(const char* path, const struct timespec ts[2], int save);
slist* storage_list(const char* path);
int    storage_symlink(const char * target, const char * link_path);
int    storage_readlink(const char * path_name, char * buf, size_t size);

#endif
