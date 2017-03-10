#ifndef KDTREE_H
#define KDTREE_H 1

#include "util.h"

/* used to insert element, to decide each side of tree */
typedef double (*kdtree_eval_f)(void*, int dim); 

typedef struct KDNode{
    void *info;
    //double val;

    struct KDNode *up;
    struct KDNode *right;
    struct KDNode *left;
}KDNode;

typedef struct KDTree{
    int n;
    int ndim;
    KDNode *root;
    kdtree_eval_f eval_f;
}KDTree;

KDTree *kdtree_new( int ndim, kdtree_eval_f eval_f);
KDTree *kdtree_insert( KDTree *kdtree, void *element);
void *kdtree_range_search(KDTree *kdtree, double *bounds, property_f);
void *kdtree_range_search_r(KDTree *kdtree, double *bounds, property_f_r prop_f, void *prop_arg); /* prop_arg will be second argument of prop_f */
void kdtree_apply_to_all(KDTree *kdtree, void(*func)(void*) );
void kdtree_fprint_pretty(FILE *fout, KDTree *kdtree);
void kdtree_free(KDTree *kdtree);
void kdtree_balance(KDTree *kdtree);

#endif

