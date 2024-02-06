// based on cs3650 starter code

#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
// gets the iith bit
int bitmap_get(char* bm, int ii);

// vv can only be 0 or 1 and this changes the iith bit to be vv
void bitmap_put(char* bm, int ii, int vv);

// prints the bitmap
void bitmap_print(char* bm, int size);

// unint64_t* allocate_bitmap(size_t bits);

#endif
