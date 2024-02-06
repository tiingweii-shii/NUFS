
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
#include "slist.h"
#include "util.h"
#include "pages.h"
#include "inode.h"
#include "directory.h"

void
storage_init(const char* path)
{
    pages_init(path);
    directory_init();
}

int
storage_stat(const char* path, struct stat* st)
{
    int inum = tree_lookup(path);
    if (inum < 0) {
        printf("+ storage_stat(%s) -> %d\n", path, inum);
        return inum;
    }

    inode* node = get_inode(inum);
    printf("+ storage_stat(%s) -> 0; inode %d\n", path, inum);
    print_inode(node);

    memset(st, 0, sizeof(struct stat));
    st->st_uid   = getuid();
    st->st_mode  = node->mode;
    st->st_size  = node->size;
    st->st_atime = node->time;
    st->st_mtime = node->mtime;
    printf("AT %s, THERE ARE %d LINKS\n", path, node->links_count);
    st->st_nlink = 1;

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
    printf("+ storage_read(%s); inode %d\n", path, inum);
    print_inode(node);

    if (offset >= node->size) {
        return 0;
    }

    if (offset + size >= node->size) {
        size = node->size - offset;
    }

    int pnum = inode_get_pnum(node, 0);
    uint8_t* data = pages_get_page(pnum);
    printf(" + reading from page: %d\n", pnum);
    memcpy(buf, data + offset, size);

    return size;
}

int
storage_write(const char* path, const char* buf, size_t size, off_t offset)
{
    int trv = storage_truncate(path, offset + size);
    if (trv < 0) {
        return trv;
    }

    int inum = tree_lookup(path);
    if (inum < 0) {
        return inum;
    }

    inode* node = get_inode(inum);
    int pnum = inode_get_pnum(node, 0);
    uint8_t* data = pages_get_page(pnum);

    printf("+ writing to page: %d\n", pnum);
    // time_t rawtime;
    // struct tm * timeinfo;

    // time ( &rawtime );
    // timeinfo = localtime ( &rawtime );
    // printf ( "Current local time and date: %s", asctime (timeinfo) );
    // node->mtime = mktime(timeinfo);
    memcpy(data + offset, buf, size);

    return size;
}

int
storage_truncate(const char *path, off_t size)
{
    int inum = tree_lookup(path);
    if (inum < 0) {
        return inum;
    }

    inode* node = get_inode(inum);
    node->size = size;
    return 0;
}

int
storage_mknod(const char* path, int mode)
{
    char* tmp1 = alloca(strlen(path));
    char* tmp2 = alloca(strlen(path));
    strcpy(tmp1, path);
    strcpy(tmp2, path);

    const char* name = path + 1;

    if (directory_lookup(name, 0) != -ENOENT) {
        printf("mknod fail: already exist\n");
        return -EEXIST;
    }

    int    inum = alloc_inode();
    inode* node = get_inode(inum);
    node->mode = mode;
    node->size = 0;
    node->ents = 0;
    node->pnum = alloc_page();

    printf("+ mknod create %s [%04o] - #%d\n", path, mode, inum);

    return directory_put(path, inum);
    // return directory_put(name, inum);
}

slist*
storage_list(const char* path)
{
    printf("NOW LOOKING FOR: %s\n", path);
    return directory_list(get_inode(tree_lookup(path)));
}

int
storage_unlink(const char* path)
{
    //const char* name = path + 1;

    int inum = tree_lookup(path);
    printf("AT THIS POINT, THERE ARE %d LINKS\n", get_inode(inum)->links_count);
    free_inode(inum);
    return directory_delete(path);
}

int
storage_link(const char* from, const char* to)
{
    int inum = tree_lookup(from);
    //put in directory
    return directory_put(to, inum);
}

int
storage_rename(const char* from, const char* to)
{
    int inum = tree_lookup(from);
    int rv = directory_put(to, inum);
    // int rv = directory_put(to + 1, inum);
    if (rv != 0) {
        return rv;
    }
   
    return directory_delete(from);
}

int
storage_set_time(const char* path, const struct timespec ts[2])
{
    int inum = tree_lookup(path);
    inode* node = get_inode(inum);

    // time_t rawtime;
    // struct tm * timeinfo;

    // time ( &rawtime );
    // timeinfo = localtime ( &rawtime );
    // printf ( "Current local time and date: %s", asctime (timeinfo) );
    //node->atime = mktime(timeinfo);
    node->time = ts[0].tv_sec;
    node->mtime = ts[1].tv_sec;
    // Maybe we need space in a inode for timestamps.
    return 0;
}
