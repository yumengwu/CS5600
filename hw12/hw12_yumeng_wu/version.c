#include "version.h"

static cow_history_t* chbase;

int
cow_history_init(int create)
{
    printf("cow_history_init\n");
    uint8_t* page = pages_get_page(VERSION_BLOCK_NUM);
    if (create) {
        int num = 0;
        memset(page, 0, 4);
    }
    chbase = page + 4;
    return 0;
}

slist*
cow_history_getall()
{
    slist* xs = 0;
    int cnt;
    memcpy(&cnt, ((char *)chbase) - 4, 4);
    printf("cnt: %d\n", cnt);
    char temp[256];
    for (int i = (cnt >= VERSION_LIMIT ? VERSION_LIMIT - 1 : cnt - 1); i >= 0; --i) {
        if (strcmp("write", chbase[i].op) == 0) {
            sprintf(temp, "%d %s %s", chbase[i].num, chbase[i].op, chbase[i].file1);
            xs = s_cons(temp, xs);
        }
        else if (strcmp("truncate", chbase[i].op) == 0) {
            sprintf(temp, "%d %s %s", chbase[i].num, chbase[i].op, chbase[i].file1);
            xs = s_cons(temp, xs);
        }
        else if (strcmp("mknod", chbase[i].op) == 0) {
            sprintf(temp, "%d %s %s", chbase[i].num, chbase[i].op, chbase[i].file1);
            xs = s_cons(temp, xs);
        }
        else if (strcmp("unlink", chbase[i].op) == 0) {
            sprintf(temp, "%d %s %s", chbase[i].num, chbase[i].op, chbase[i].file1);
            xs = s_cons(temp, xs);
        }
        else if (strcmp("link", chbase[i].op) == 0) {
            sprintf(temp, "%d %s %s => %s", chbase[i].num, chbase[i].op, chbase[i].file1, chbase[i].file2);
            xs = s_cons(temp, xs);
        }
        else if (strcmp("rename", chbase[i].op) == 0) {
            sprintf(temp, "%d %s %s => %s", chbase[i].num, chbase[i].op, chbase[i].file1, chbase[i].file2);
            xs = s_cons(temp, xs);
        }
        else if (strcmp("chmod", chbase[i].op) == 0) {
            sprintf(temp, "%d %s %s", chbase[i].num, chbase[i].op, chbase[i].file1);
            xs = s_cons(temp, xs);
        }
        else if (strcmp("settime", chbase[i].op) == 0) {
            sprintf(temp, "%d %s %s", chbase[i].num, chbase[i].op, chbase[i].file1);
            xs = s_cons(temp, xs);
        }
    }
    xs = s_rev_free(xs);
    return xs;
}

int
cow_history_add(cow_history_t * history)
{
    if (!history) {
        return -1;
    }
    printf(" cow add op %s\n", history->op);
    int cnt;
    memcpy(&cnt, ((char *)chbase) - 4, 4);
    history->num = cnt == 0 ? 1 : chbase[cnt - 1].num + 1;
    printf(" num: %d\n", history->num);
    if (cnt == VERSION_LIMIT) {
        uint8_t temp[VERSION_LIMIT * sizeof(cow_history_t)];
        memcpy(temp, chbase + 1, (VERSION_LIMIT - 1) * sizeof(cow_history_t));
        memcpy(chbase, temp, (VERSION_LIMIT - 1) * sizeof(cow_history_t));
        --cnt;
    }
    memcpy(chbase + cnt, history, sizeof(cow_history_t));
    ++cnt;
    memcpy(((char *)chbase) - 4, &cnt, 4);
    return 0;
}

void
cow_history_rollback(int ver)
{
    int cnt;
    memcpy(&cnt, ((char *)chbase) - 4, 4);
    // no such version
    if (cnt == 0 || ver < chbase[0].num || ver > chbase[cnt - 1].num) {
        return;
    }
    while (ver < chbase[cnt - 1].num) {
        if (streq(chbase[cnt - 1].op, "write")) {
            struct stat st;
            storage_stat(chbase[cnt - 1].file2, &st);
            char temp[st.st_size];
            storage_read(chbase[cnt - 1].file2, temp, st.st_size, 0);
            storage_unlink(chbase[cnt - 1].file2, 0);
            storage_write(chbase[cnt - 1].file1, temp, st.st_size, 0, 0);
        }
        else if (streq(chbase[cnt - 1].op, "truncate")) {
            struct stat st;
            storage_stat(chbase[cnt - 1].file2, &st);
            char temp[st.st_size];
            storage_read(chbase[cnt - 1].file2, temp, st.st_size, 0);
            storage_unlink(chbase[cnt - 1].file2, 0);
            storage_write(chbase[cnt - 1].file1, temp, st.st_size, 0, 0);
        }
        else if (streq(chbase[cnt - 1].op, "mknod")) {
            storage_unlink(chbase[cnt - 1].file2, 0);
        }
        else if (streq(chbase[cnt - 1].op, "unlink")) {
            if (chbase[cnt - 1].file2[0] == '.') {
                slist* xs = s_split(chbase[cnt - 1].file1, '/');
                xs = s_rev_free(xs);
                slist* ys = xs->next;
                ys = s_rev_free(ys);
                xs->next = 0;
                int inum = directory_lookup(get_inode(0), ys);
                dirent* dirs = pages_get_page(get_inode(inum)->blocks[0]);
                for (int i = 0; i < 4096 / sizeof(dirent); ++i) {
                    if (strlen(dirs[i].name) == 0) {
                        strcpy(dirs[i].name, xs->data);
                        memcpy(&dirs[i].inum, chbase[cnt - 1].file2 + 1, 4);
                        break;
                    }
                }
                s_free(xs);
                s_free(ys);
            }
            else {
                struct stat st;
                storage_stat(chbase[cnt - 1].file2, &st);
                char temp[st.st_size];
                storage_read(chbase[cnt - 1].file2, temp, st.st_size, 0);
                storage_unlink(chbase[cnt - 1].file2, 0);
                storage_write(chbase[cnt - 1].file1, temp, st.st_size, 0, 0);
            }
        }
        else if (streq(chbase[cnt - 1].op, "link")) {
            storage_unlink(chbase[cnt - 1].file1, 0);
        }
        else if (streq(chbase[cnt - 1].op, "rename")) {
            storage_rename(chbase[cnt - 1].file2, chbase[cnt - 1].file1, 0);
        }
        else if (streq(chbase[cnt - 1].op, "chmod")) {
            int mode;
            memcpy(&mode, chbase[cnt - 1].data1, 4);
            storage_chmod(chbase[cnt - 1].file1, mode, 0);
        }
        else if (streq(chbase[cnt - 1].op, "settime")) {
            time_t atime, mtime;
            memcpy(&atime, chbase[cnt - 1].data1, 8);
            memcpy(&mtime, chbase[cnt - 1].data1 + 8, 8);
            struct timespec ts[2];
            ts[0].tv_sec = atime;
            ts[1].tv_sec = mtime;
            storage_set_time(chbase[cnt - 1].file1, ts, 0);
        }
        --cnt;
    }
    memcpy(((char *)chbase) - 4, &cnt, 4);
}
