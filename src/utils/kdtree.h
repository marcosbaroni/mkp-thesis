/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: june, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#ifndef KDTREE_H
#define KDTREE_H 1

#include "util.h"

/* used to insert element, to decide each side of tree */
typedef double (*kdtree_eval_f)(void*, int dim); 

typedef struct KDNode{
    void *info;
	unsigned char deleted;
    //double val;

    struct KDNode *up;
    struct KDNode *right;
    struct KDNode *left;
}KDNode;

typedef struct KDTree{
    int n;
	int ndeleted;
    int ndim;
    KDNode *root;
    kdtree_eval_f eval_f;
}KDTree;

KDTree *kdtree_new( int ndim, kdtree_eval_f eval_f);
KDTree *kdtree_insert( KDTree*, void*);
void *kdtree_range_search(KDTree*, double*, property_f);
void *kdtree_range_search_r(KDTree*, double*, property_f_r prop_f, void*); /* prop_arg will be second argument of prop_f */
void kdtree_apply_to_all(KDTree*, void(*func)(void*) );
void kdtree_apply_to_all_r(KDTree*, void(*func)(void*, void*), void*);
void kdtree_fprint_pretty(FILE*,KDTree*);
void kdtree_remove(KDTree*, void*);
void kdtree_rebuild(KDTree *kdtree, void(*)(void*));
void kdtree_free(KDTree*);
void kdtree_balance(KDTree*);
void **kdtree_get_all(KDTree*);
void *kdtree_find_closest(KDTree*, double(*)(void*, void*));

typedef struct KDTreeIter{
	KDTree *kdtree;
	KDNode *node;
}KDTreeIter;
KDTreeIter *kdtiter_new(KDTree *kdtree);
void *kdtiter_forward(KDTreeIter*);
void *kdtiter_get(KDTreeIter *kdti);
void kdtiter_free(KDTreeIter*);
void kdtiter_remove(KDTreeIter*);

#endif

