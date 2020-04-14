#include "pages.h"
#include "inode.h"
#include "bitmap.h"
#include "util.h"

const int INODE_COUNT = 254;

inode*
get_inode(int inum)
{
    uint8_t* base = (uint8_t*) (pages_get_page(0) + INODE_OFFSET);
    inode* nodes = (inode*)(base);
    return &(nodes[inum]);
}

int
alloc_inode()
{
    // for (int ii = 1; ii < INODE_COUNT; ++ii) {
    //     inode* node = get_inode(ii);
    //     if (node->mode == 0) {
    //         memset(node, 0, sizeof(inode));
    //         node->mode = 010644;
    //         printf("+ alloc_inode() -> %d\n", ii);
    //         return ii;
    //     }
    // }

    // get block bitmap and inode bitmap
    char* block_bm = pages_get_page(0);
    char* inode_bm = block_bm + INODE_TABLE_OFFSET;

    for (int i = 0; i < INODE_COUNT; ++i) {
        // find the first empty inode through inode bitmap
        if (bitmap_get(inode_bm, i) == 0) {
            inode* node = get_inode(i);
            memset(node, 0, sizeof(inode));
            // alloc one block
            for (int j = 2; j < BLOCK_COUNT; ++j) {
                if (bitmap_get(block_bm, j) == 0) {
                    bitmap_put(block_bm, j, 1);
                    node->blocks[0] = j;
                    break;
                }
            }
            if (node->blocks[0] == 0) {
                return -1;
            }
            node->mode = 010644;
            node->count = 1;
            bitmap_put(inode_bm, i, 1);
            printf("+ alloc_inode() -> %d\n", i);
            return i;
        }
    }

    return -1;
}

void
free_inode(int inum)
{
    printf("+ free_inode(%d)\n", inum);
    uint8_t* block_bm = pages_get_page(0);
    uint8_t* inode_bm = block_bm + INODE_TABLE_OFFSET;

    inode* node = get_inode(inum);

    // first, free indirect pointer
    // it is safe to assume that only file have indirect pointer
    if (node->size > 4096 * 3) {
        uint16_t* page = pages_get_page(node->blocks[3]);
        int indirect_cnt = (node->size - 4096 * 3) / 4096 + 1;
        for (int i = 0; i < indirect_cnt; ++i) {
            if (page[i] < BLOCK_COUNT) {
                bitmap_put(block_bm, page[i], 0);
            }
        }
        node->size = 4096 * 3;
    }

    // second, free direct pointer
    if (node->size > 4096 * 2) bitmap_put(block_bm, node->blocks[2], 0);
    if (node->size > 4096) bitmap_put(block_bm, node->blocks[1], 1);
    bitmap_put(block_bm, node->blocks[0], 0);

    // free inode
    bitmap_put(inode_bm, inum, 0);

    memset(node, 0, sizeof(inode));
}

void
print_inode(inode* node)
{
    if (node) {
        printf("node{mode: %04o, size: %d}\n",
               node->mode, node->size);
    }
    else {
        printf("node{null}\n");
    }
}

