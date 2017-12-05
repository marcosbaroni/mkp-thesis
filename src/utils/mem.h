/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: november, 2016
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#ifndef MEM_H
#define MEM_H 1

#include "avl.h"


typedef struct MemPool{
    AVLTree *pool;
    AVLTree *labels;
}MemPool;

typedef struct MemLblNode{
    char *label;
    int n, ntot;
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

