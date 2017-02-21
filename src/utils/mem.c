#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem.h"
#include "avl.h"

struct MemPool *_mempool = NULL;

int memnode_cmp(MemNode *na, MemNode *nb){
    if ( na->addr > nb->addr ) return 1;
    if ( na->addr < nb->addr ) return -1;
    return 0;
}

int memlblnode_cmp(MemLblNode *lna, MemLblNode *lnb){
    return( strcmp(lna->label, lnb->label) );
}

void memnode_free(MemNode *nd){
    free(nd);
}

void memlblnode_free(MemLblNode *ln){
    free(ln->label);
    free(ln);
}

MemPool *_mempool_new(){
    MemPool *mempool;

    mempool = (MemPool*)malloc(sizeof(MemPool));
    mempool->pool = new_avltree( (avl_cmp_f)memnode_cmp );
    mempool->labels = new_avltree( (avl_cmp_f)memlblnode_cmp );
    return mempool;
}

void _mempool_free(MemPool *mempool){
    avl_apply_to_all( mempool->pool, (void(*)(void*))memnode_free);
    avl_apply_to_all( mempool->labels, (void(*)(void*))memlblnode_free);
    avl_free( mempool->pool );
    avl_free( mempool->labels );
    free( mempool );
}

void _mempool_init(){
    _mempool = _mempool_new();
}

void _mempool_close(){
    if(_mempool)
        _mempool_free(_mempool);
    _mempool = NULL;
}

void *_mymalloc(size_t size, char *label){
    char *rlabel;
    void *addr;
    MemNode *mnode;
    MemLblNode *mlnode, *rmlnode;

    addr = malloc(size);

    if( !_mempool ){
        fprintf(stderr, "Error: asked for _mymalloc but _mempool is not allocated. (_myalloc)\n");
        return addr;
    }

    mlnode = (MemLblNode*)malloc(sizeof(MemLblNode));
    mlnode->label = label;
    rmlnode = avl_has(_mempool->labels, mlnode);
    if( !rmlnode ){
        rmlnode = (MemLblNode*)malloc(sizeof(MemLblNode));
        rmlnode->label = (char*)malloc(32*sizeof(char));
        rmlnode->n = rmlnode->ntot = 0;
        if( strlen(label) > 30 )
            fprintf(stderr, "Warning: mem labels must be smaller than 30 chars\n");
        strcpy(rmlnode->label, label);
        avl_insert(_mempool->labels, rmlnode);
    }
    free(mlnode);

    mnode = (MemNode*)malloc(sizeof(MemNode));
    mnode->addr = addr;
    mnode->labelnode = rmlnode;
    rmlnode->n++;
    rmlnode->ntot++;
    avl_insert(_mempool->pool, mnode);

    return addr;
}

void _myfree(void *addr){
    MemNode *mnode;
    MemNode *rmnode;
    MemLblNode *ln;
    MemLblNode *rln;

    mnode = (MemNode*)malloc(sizeof(MemNode));
    mnode->addr = addr;

    free(addr);
    if( !_mempool ){
        fprintf(stderr,
            "Error: asked for _myfree but _mempool is not allocated. (_myfree)\n");
        return;
    }

    rmnode = avl_has(_mempool->pool, mnode);
    if( !rmnode ){
        fprintf(stderr, "Error: memory addr not found in _mempool. (_myfree)\n");
        return;
    }

    rmnode->labelnode->n--;
    avl_remove(_mempool->pool, rmnode);
    free(mnode);
    free(rmnode);

    return;
}

void _mempool_analyse(FILE *out){
    AVLIter *iter;
    MemLblNode *mln;

    if( !_mempool ){
        fprintf(stderr, "Error: asked for _myfree but _mempool is not allocated.\n");
    }
    iter = avl_get_first(_mempool->labels);

    while( mln = avliter_forward(iter) ){
        if( mln->n != 0 ){
            fprintf(out, "%s: %d/%d yet allocated blocks !!!\n",
                mln->label,
                mln->n,
                mln->ntot);
        }else{
            fprintf(out, "%s: all %d proper freeded\n", mln->label, mln->ntot);
        }
    }
    avliter_free(iter);
    return;
}

/******************   FUNCTION-BINDINGS   ************************/
#ifdef USE_MEMPOOL
void mempool_init(){
    _mempool_init();
}
void mempool_close(){
    _mempool_close();
}
void myfree(void *addr){
    _myfree(addr);
}
void *mymalloc(size_t size, char *label){
    return _mymalloc(size, label);
}
void mempool_analyse(FILE *out){
    _mempool_analyse(out);
}
#else
void mempool_init(){}
void mempool_close(){}
void myfree(void *addr){
    free(addr);
}
void *mymalloc(size_t size, char *label){
    return malloc(size);
}
void mempool_analyse(FILE *out){}
#endif
/**********************************************************/
