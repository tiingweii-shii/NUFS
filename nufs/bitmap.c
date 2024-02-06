#include <malloc.h>
#include "bitmap.h"

// gets the iith bit
int bitmap_get(char* bm, int ii) {
    // uint64_t lshift = sizeof(uint64_t) - (ii % 64) - 1;
    int lshift = ii % 8;
    int idx = ii / 8;
    int bit = bm[idx] & (1 << lshift);
    return bit > 0;
}

// vv can only be 0 or 1 and this changes the iith bit to be vv
//bit
void bitmap_put(char* bm, int ii, int vv) {

    // int bit = bitmap_get(bm, ii);
    // if(bit != vv) {
        // uint64_t lshift = sizeof(uint64_t) - (ii % 64) - 1;
        int lshift = ii % 8;
        int idx = ii / 8;
        // uint64_t bb = bm[idx];
        bm[idx] = bm[idx] ^ (1 << lshift);
    //     return 0;
    // }else{
    //     return -1;
    // }

}

// prints the bitmap
void bitmap_print(char* bm, int size) {
    char strmap[size];
    for (int ii = 0; ii < size; ++ii) {
        strmap[ii] = bitmap_get(bm, ii) + '0';
    }
    puts(strmap);
}

// unint64_t* allocate_bitmap(size_t bits)
// {
//     size_t num_ints = ceil(bits/64);
//     unint64_t* [num_ints]
//     return u;

// }

// int main(int argc, char const *argv[])
// {
//     char* bm = malloc(32 * sizeof(char));
//     bitmap_print(bm, 256);
//     //printf("get bit 25 %d\n", bitmap_get(bm, 24));

//     bitmap_put(bm, 32, 1);
//     bitmap_print(bm, 256);
// }
//     bitmap_print(bm, 128);
//     puts("BM2");
//     bitmap_print(bm2, 128);
//     bitmap_put(bm, 3, 1);
//     bitmap_print(bm, 128);
//     puts("BM2");
//     bitmap_print(bm2, 128);
//     bitmap_put(bm, 3, 0);
//     bitmap_print(bm, 128);
//     puts("BM2");
//     bitmap_print(bm2, 128);
//     bitmap_put(bm, 67, 1);
//     bitmap_print(bm, 128);
//     puts("BM2");
//     bitmap_print(bm2, 128);
//     bitmap_put(bm, 67, 1);
//     bitmap_print(bm, 128);
//     puts("BM2");
//     bitmap_print(bm2, 128);

//     // put("BM2")
//     // bitmap_print(bm2, 256);
//     // bitmap_put(bm, 3, 1);
//     // bitmap_print(bm, 256);
//     // bitmap_put(bm, 3, 0);
//     // bitmap_print(bm, 256);
//     // bitmap_put(bm, 67, 1);
//     // bitmap_print(bm, 256);
//     // bitmap_put(bm, 67, 1);
//     // bitmap_print(bm, 256);
//     /* code */
//     return 0;
// }
