#ifndef VERSION_H
#define VERSION_H

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <string.h>
#include "storage.h"
#include "pages.h"
#include "util.h"
#include "directory.h"

#define VERSION_BLOCK_NUM 254
#define VERSION_OP_BYTES 16
#define VERSION_PATH_BYTES 64
#define VERSION_LIMIT 16

typedef struct cow_history_t {
    int num;
    char op[VERSION_OP_BYTES];
    char file1[VERSION_PATH_BYTES];
    char file2[VERSION_PATH_BYTES];
    char data1[VERSION_PATH_BYTES];
} cow_history_t;

int cow_history_init(int create);
slist* cow_history_getall();
int cow_history_add(cow_history_t * history);
void cow_history_rollback(int ver);

#endif