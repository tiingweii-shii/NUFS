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
#include <stdint.h>

#include "bitmap.h"
#include "pages.h"
#include "util.h"

const int PAGE_COUNT = 256;
const int NUFS_SIZE  = 4096 * 256; // 1MB

static int   pages_fd   = -1;
static void* pages_base =  0;
static int   alloc_ptr  = 32;

void
pages_init(const char* path)
{
    pages_fd = open(path, O_CREAT | O_RDWR, 0644);
    assert(pages_fd != -1);

    int rv = ftruncate(pages_fd, NUFS_SIZE);
    assert(rv == 0);

    pages_base = mmap(0, NUFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, pages_fd, 0);
    assert(pages_base != MAP_FAILED);

    // mark inode 0 as taken
    char* nodemap = get_nodemap();
    bitmap_put(nodemap, 0, 1);

    // mark page 0 as taken
    char* datamap = get_datamap();
    bitmap_put(datamap, 0, 1);
}

void
pages_free()
{
    int rv = munmap(pages_base, NUFS_SIZE);
    assert(rv == 0);
}

void*
pages_get_page(int pnum)
{
    return pages_base + 4096 * pnum;
}

// char*
// get_freemap()
// {
//     return (char*) pages_get_page(0) + 2048;
// }

char*
get_datamap()
{
    return (char*) pages_get_page(0) + 2048;
}

char*
get_nodemap()
{
    // data map is 256 bits (32 void*)/32 bytes
    return (char*) ((void*) get_datamap() + 32);
}

// void
// free_page(int pnum)
// {
//     char* freemap = get_freemap();
//     freemap[pnum] = 0;
// }

void
free_page(int pnum)
{
    char* datamap = get_datamap();
    bitmap_put(datamap, pnum, 0);
}

void
free_inode_map(int inum)
{
    char* nodemap = get_nodemap();
    bitmap_put(nodemap, inum, 0);
}

int
alloc_page()
{
    char* datamap = get_datamap();
    for (int jj = 0; jj < 256; ++jj) {
        if (bitmap_get(datamap, alloc_ptr) == 0) {
            bitmap_put(datamap, alloc_ptr, 1);
            return alloc_ptr;
        }
        alloc_ptr = (alloc_ptr * 13 + 5) % 256;
    }

    return -ENOSPC;
}
