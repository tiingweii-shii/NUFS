#ifndef INODE_H
#define INODE_H

#include "pages.h"

typedef struct inode {
    int mode; // permission & type; zero for unused
    int size; // bytes
    int pnum; // page number 
    //TODO: replace pnum with a bunch of pointers to paged/data blocks (because pnum only works for small allocations less than 1 page)
    int links_count;
    int ents;// Number of directory entries; unused for non-dirs.
    __time_t time; // last access time
    __time_t mtime; // last modification time
} inode;

void print_inode(inode* node);
inode* get_inode(int inum);
int alloc_inode();
void free_inode(int inum);
int inode_get_pnum(inode* node, int fpn);
void* inode_get_page(inode* node, int fpn);

#endif

/*
All operations:

HW10 reqs:
- Create files.
- List the files in the filesystem root directory (where you mounted it).
- Write to small files (under 4k).
- Read from small files (under 4k).
- Delete files.
- Rename files.

CH03 reqs:
- Read and write from files larger than one block
- Create directories and nested directories
- Remove directories.
- Hard links.
- Symlinks 
- metadata (permissions and timestamps) for files and directories
*/