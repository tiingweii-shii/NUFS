
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

#include "directory.h"
#include "pages.h"
#include "slist.h"
#include "util.h"
#include "inode.h"

#define ROOT_NODE 0

void
directory_init()
{
    inode* rn = get_inode(ROOT_NODE);

    if (rn->mode == 0) {
        rn->mode = 040755;
        rn->size = 0;
        rn->ents = 0;
        rn->pnum = alloc_page();
    }
}

char*
skip_string(char* data)
{
    while (*data != 0) {
        data++;
    }
    return data + 1;
}

int
slash_at(const char* name) {
    for(int ii = 0; ii < strlen(name); ++ii) {
        if(name[ii] == '/') {
            return ii;
        }
    }
    return -1;
}

int
directory_lookup(const char* name, int node_num)
{
    if(*name == 0) {
        printf("LOOKING FOR NULL IN NODE %d\n", node_num);
    }
    else
    {
        printf("LOOKING FOR %s IN NODE %d\n", name, node_num);
    }
    
    char imm_dir[strlen(name) + 1];
    char rest[strlen(name) + 1];
    int slash = slash_at(name);
    if(slash < 0) {
        strcpy(imm_dir, name);
    }
    else
    {
        //puts("GOT OUT!!");
        //puts("GOT HERE!!");
        char tmp[strlen(name) + 1];
        strcpy(tmp, name);
        printf("BEFORE REMOVING /: %s\n", tmp);
        tmp[slash] = 0;
        printf("AFTER REMOVING /: %s\n", tmp);
        strcpy(imm_dir, tmp);
        strcpy(rest, &(tmp[slash + 1]));
        //rest = &(tmp[slash + 1]);
        printf("REST /: %s\n", rest);
    }
    
    inode* rn = get_inode(node_num);
    char* data = inode_get_page(rn, 0);
    char* text = data;

    puts("PRE-FOR");
    for (int ii = 0; ii < rn->ents; ++ii) {
    puts("IN-FOR");
        //printf(" ++ lookup '%s' =? '%s' (%p)\n", name, text, text);
        printf("TEXT: %s; IMMEDIATE: %s\n", text, imm_dir);
        if (streq(text, imm_dir)) {
            text = skip_string(text);
            int* inum = (int*)(text);
            //puts("YEET");
            if(slash < 0) {
                printf("LOOKUP YIELDED: %d\n", *inum);
                return *inum;
            }
            else
            {
                printf("GOING INTO %s", rest);
                return directory_lookup(rest, *(inum));
            }
        }

        text = skip_string(text);
        text += 4;
    }
    return -ENOENT;
}

// int
// directory_lookup(const char* name)
// {
//     inode* rn = get_inode(ROOT_NODE);
//     char* data = inode_get_page(rn, 0);
//     char* text = data;

//     for (int ii = 0; ii < rn->ents; ++ii) {
//         //printf(" ++ lookup '%s' =? '%s' (%p)\n", name, text, text);

//         if (streq(text, name)) {
//             text = skip_string(text);
//             int* inum = (int*)(text);
//             return *inum;
//         }

//         text = skip_string(text);
//         text += 4;
//     }
//     return -ENOENT;
// }

int
tree_lookup(const char* path)
{
    assert(path[0] == '/');

    if (streq(path, "/")) {
        return 0;
    }

    return directory_lookup(path + 1, 0);
}

int
last_slash(const char* name) {
    int idx = -1;
    for(int ii = 0; ii < strlen(name); ++ii) {
        if(name[ii] == '/') {
            idx = ii;
        }
    }
    return idx;
}

int
directory_put(const char* name, int inum)
{
    printf("PUTTING: %s\n", name);
    char place_in[strlen(name) + 1];
    strcpy(place_in, name);
    int last = last_slash(place_in);
    place_in[last] = 0;
    int node_num;
    printf("/ AT IDX: %d\n", inum);
    printf("PLACE_IN: %s\n", place_in);
    char tmp[strlen(name)];
    strcpy(tmp, name);
    char* new_name = &(tmp[last + 1]);
    printf("FILE NAME: %s\n", new_name);

    if(strlen(place_in) == 0) {
        node_num = ROOT_NODE; 
    }
    else
    {  
        //printf("FILE NAME: %s\n", place_in[last+1]);
        node_num = tree_lookup(place_in); 
    }
    inode* rn = get_inode(node_num);
    int nlen = strlen(new_name) + 1;
    if (rn->size + nlen + sizeof(inum) > 4096) {
        return -ENOSPC;
    }

    char* data = inode_get_page(rn, 0);
    memcpy(data + rn->size, new_name, nlen);
    // memcpy(data + rn->size, name, nlen);
    printf("+ dirent = '%s'\n", new_name);
    rn->size += nlen;

    memcpy(data + rn->size, &inum, sizeof(inum));
    rn->size += sizeof(inum);

    rn->ents += 1;
    //rn->links_count += 1;
    int nnum = tree_lookup(name);
    inode* new_node = get_inode(nnum);
    new_node->links_count += 1;
    printf("INODE %d NOW HAS %d LINKS TO IT\n", nnum, new_node->links_count);

    printf("+ directory_put(..., %s, %d) -> 0\n", name, inum);
    print_inode(rn);

    return 0;
}

int
directory_delete(const char* name)
{
    printf(" + directory_delete(%s)\n", name);
    int last = last_slash(name);
    char dir[strlen(name)];
    char file[strlen(name)];

    if(last == 0) {
        strcpy(dir, "/");
        strcpy(file, name + 1);
    }
    else
    {
        strcpy(dir, name);
        dir[last] = 0;
        strcpy(file, name + last + 1);
    }
    
    printf("DELETING FROM DIR: %s\n", dir);
    int num = tree_lookup(dir);
    inode* rn = get_inode(num);
    char* data = inode_get_page(rn, 0);
    char* text = data;
    char* eend = 0;

    for (int ii = 0; ii < rn->ents; ++ii) {
        puts("IN FOR");
        if (streq(text, file)) {
            puts("GOTO");
            goto delete_found;
        }

        text = skip_string(text);
        text += 4;
    }
    puts("NOT GOTO");
    return -ENOENT;

delete_found:
    eend = skip_string(text);
    int inum = *((int*)eend);
    eend += sizeof(int);

    int epos = (int)(eend - data);
    memmove(text, eend, rn->size - epos);

    int elen = (int)(eend - text);
    rn->size -= elen;
    rn->ents -= 1;

    return 0;
}

slist*
directory_list(inode* rn)
{
    // int node_num = directory_lookup(path, 0);
    // inode* rn = get_inode(node_num);
    //assert(rn->mode == 40755);
    char* data = inode_get_page(rn, 0);
    char* text = data;

    printf("+ directory_list()\n");
    slist* ys = 0;

    for (int ii = 0; ii < rn->ents; ++ii) {
        char* name = text;
        text = skip_string(text);
        int pnum = *((int*) text);
        text += sizeof(int);

        printf(" - %d: %s [%d]\n", ii, name, pnum);
        ys = s_cons(name, ys);
    }

    return ys;
}

// slist*
// directory_list()
// {
//     inode* rn = get_inode(ROOT_NODE);
//     char* data = inode_get_page(rn, 0);
//     char* text = data;

//     printf("+ directory_list()\n");
//     slist* ys = 0;

//     for (int ii = 0; ii < rn->ents; ++ii) {
//         char* name = text;
//         text = skip_string(text);
//         int pnum = *((int*) text);
//         text += sizeof(int);

//         printf(" - %d: %s [%d]\n", ii, name, pnum);
//         ys = s_cons(name, ys);
//     }

//     return ys;
// }

void
print_directory(inode* dd)
{
    printf("Contents:\n");
    slist* items = directory_list(dd);
    for (slist* xs = items; xs != 0; xs = xs->next) {
        printf("- %s\n", xs->data);
    }
    printf("(end of contents)\n");
    s_free(items);
}
