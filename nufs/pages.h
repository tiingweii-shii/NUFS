#ifndef PAGES_H
#define PAGES_H

#include <stdio.h>

void pages_init(const char* path);
void pages_free();
void* pages_get_page(int pnum);
// char* get_freemap();
char* get_datamap();
int alloc_page();
void free_page(int pnum);
// void free_data_page(int pnum);

void free_inode_map(int inum);
char* get_nodemap();

#endif
