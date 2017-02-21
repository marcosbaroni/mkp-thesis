#ifndef MEM_H
#define MEM_H 1

#include "avl.h"


typedef struct MemPool{
    AVLTree *pool;
    AVLTree *labels;
}MemPool;

typedef struct MemLblNode{
    char *label;
    int n;
}MemLblNode;

typedef struct MemNode{
    void *addr;
    MemLblNode *labelnode;
}MemNode;

void mempool_init();
void mempool_close();
void myfree(void *addr);
void *mymalloc(size_t size, char *label);
void mempool_analyse(FILE *out);

#endif

