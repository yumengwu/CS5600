
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "storage.h"
#include "slist.h"
#include "util.h"
#include "version.h"

slist*
image_ls_tree(const char* base)
{
    struct stat st;
    slist* zs = 0;
    slist* xs = storage_list(base);
    for (; xs; xs = xs->next) {
        char* path = path_join(base, xs->data);
        slist* ys = s_split(path, '/');
        ys = s_rev_free(ys);
        zs = s_cons(path, zs);
        if (ys && !(streq(ys->data, ".") || streq(ys->data, ".."))) {
            struct stat st;
            storage_stat(path, &st);
            if (S_ISDIR(st.st_mode)) {
                zs = s_cat_free(zs, image_ls_tree(path));
            }
        }
        s_free(ys);
    }
    return zs;
}

void
print_usage(const char* name)
{
    fprintf(stderr, "Usage: %s cmd ...\n", name);
    exit(1);
}

int
main(int argc, char* argv[])
{
    if (argc < 3) {
        print_usage(argv[0]);
    }

    const char* cmd = argv[1];
    const char* img = argv[2];

    if (streq(cmd, "new")) {
        assert(argc == 3);

        storage_init(img, 1);
        printf("Created disk image: %s\n", img);
        return 0;
    }

    if (access(img, R_OK) == -1) {
        fprintf(stderr, "No such image: %s\n", img);
        return 1;
    }

    storage_init(img, 0);

    if (streq(cmd, "ls")) {
        slist* xs = image_ls_tree("/");
        printf("List for /\n");
        for (slist* it = xs; it != 0; it = it->next) {
            printf("%s\n", it->data);
        }
        s_free(xs);
        return 0;
    }

    if (streq(cmd, "versions")) {
        slist* xs = cow_history_getall();
        for (slist* it = xs; it != 0; it = it->next) {
            printf("%s\n", it->data);
        }
        s_free(xs);
        return 0;
    }

    if (streq(cmd, "rollback") && argc == 4) {
        int ver = atoi(argv[3]);
        cow_history_rollback(ver);
        return 0;
    }

    print_usage(argv[0]);
}
