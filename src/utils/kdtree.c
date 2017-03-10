#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"
#include "kdtree.h"

KDNode *kdnode_new(void *element){
    KDNode *kdn;
    kdn = (KDNode*)malloc(sizeof(KDNode));
    kdn->info = element;
    kdn->up = kdn->right = kdn->left = NULL;

    return kdn;
}

KDTree *kdtree_new( int ndim, kdtree_eval_f eval_f){
    KDTree *kdtree;
    int nhcount = 100;

    kdtree = (KDTree*)malloc(sizeof(KDTree));

    kdtree->eval_f = eval_f;
    kdtree->root = NULL;
    kdtree->n = 0;
    kdtree->ndim = ndim;

    return kdtree;
}

void _kdtree_sub_insert(KDTree *kdtree, KDNode *father, void *element, int h){
    int dim;
    int rval, eval;

    dim = h % kdtree->ndim;
    rval = kdtree->eval_f(father->info, dim);
    eval = kdtree->eval_f(element, dim);

    if( eval > rval ){ /* insert in right branch */
        if(father->right ){
            _kdtree_sub_insert(kdtree, father->right, element, h+1);
        }else{
            father->right = kdnode_new(element);
            father->right->up = father;
        }
    }else{              /* insert in left branch */
        if( father->left ){
            _kdtree_sub_insert(kdtree, father->left, element, h+1);
        }else{
            father->left = kdnode_new(element);
            father->left->up = father;
        }
    }

    return;
}

KDTree *kdtree_insert( KDTree *kdtree, void *element){
    if( !kdtree->root ){
        kdtree->root = kdnode_new(element);
    }else
        _kdtree_sub_insert(kdtree, kdtree->root, element, 0);

    kdtree->n++;

    return kdtree;
}

/*
 * Procedure:
 *   1. check if root is inside range (if true, return root)
 *   2. check if right hypercube intersects range (if true, call for right)
 *   3. check if left hypercube intersects range (if true, call for left)
 *
 * obs.: Consider infinity bounds
 */
void *_kdtree_range_search(
    KDTree *kdtree,
    KDNode *root,
    double *bounds,
    int h,
    void *prop_f,
    void *prop_arg)
{
    double val, lower, upper;
    int i, dim, ndim, meets;
    kdtree_eval_f eval_f;
    void *ret;

    ndim = kdtree->ndim;
    eval_f = kdtree->eval_f;
    ret = NULL;

    /* check if root is inside range */
    meets = 1;
    for( i = 0 ; i < ndim && meets ; i++ ){
        val = eval_f(root->info, i);
        lower = bounds[i*2];
        upper = bounds[i*2+1];
        meets = (val >= lower) && (val <= upper);
    }
    /* if is inside, check if has desired property */
    if( meets ){
        if( prop_arg ) meets = ((property_f_r)prop_f)(root->info, prop_arg);
        else meets = ((property_f)prop_f)(root->info);
    }

    /* if is inside and meets property, return */
    if( meets )
        return root->info;

    /* root does not meet, continuing search throught children... */
    dim = h % kdtree->ndim; /* the dimention index to be used */
    val = kdtree->eval_f(root->info, dim); /* value of root for the dim used */
    lower = bounds[dim*2];
    upper = bounds[dim*2+1];

    /* search right branch */
    if( root->right )
        if( val <= upper )
            ret = _kdtree_range_search(kdtree, root->right, bounds, h+1, prop_f, prop_arg);

    if( ret )
        return ret;

    /* search left branch */
    if( root->left )
        if( val >= lower )
            ret = _kdtree_range_search(kdtree, root->left, bounds, h+1, prop_f, prop_arg);

    return ret;
}


/*
 * kdtree: the KD-Tree
 * bounds: array representing the bounds for search.
 *         Ex.: For first hyper-quadrant on 3 dimensions {0.0, INFINITY, 0,0, INFINITY, 0.0, INFINITY}
 * prop_f: optional function to check if a candidate node, found inside range, has
 *         the desired property. If not, range search wont return it and will keep searching.
 */
int always_has_prop(void *a){ return 1; }
void *kdtree_range_search(KDTree *kdtree, double *bounds, property_f prop_f){
    if( !kdtree->root )
        return NULL;

    if( !prop_f )
        prop_f = always_has_prop;

    else return _kdtree_range_search(kdtree, kdtree->root, bounds, 0, prop_f, NULL);
}

void *kdtree_range_search_r(KDTree *kdtree, double *bounds, property_f_r prop_f, void *prop_arg){
    if( !kdtree->root )
        return NULL;

    return _kdtree_range_search(kdtree, kdtree->root, bounds, 0, prop_f, prop_arg);
}

void _kdtree_node_print(FILE *fout, KDNode *node){
    fprintf(fout, "info: %x ", node->info);
    return;
}

void *_kdtree_branch_getter(KDNode *node, char side){
    if( side == 'r' )
        return node->right;
    if( side == 'l' )
        return node->left;
    return NULL;
}

void _sub_kdtree_apply_to_all(KDNode *node, void(*func)(void*) ){
    if( node->left)
        _sub_kdtree_apply_to_all(node->left, func);
    func( node->info );
    if( node->right )
        _sub_kdtree_apply_to_all(node->right, func);
}

void kdtree_apply_to_all(KDTree *kdtree, void(*func)(void*) ){
    if( kdtree->root )
        _sub_kdtree_apply_to_all(kdtree->root, func);
}

void kdtree_fprint_pretty(FILE *fout, KDTree *kdtree){
    if( kdtree->root )
        tree_pretty_printer(
            fout,
            kdtree->root,
            (void*(*)(void*, char))_kdtree_branch_getter,
            (void(*)(FILE*,void*))_kdtree_node_print);
}

void _kdnode_free(KDNode *kdn){
    if(kdn->right) _kdnode_free(kdn->right);
    if(kdn->left) _kdnode_free(kdn->left);
    free(kdn);

    return;
}

void kdtree_free(KDTree *kdtree){
    _kdnode_free(kdtree->root);
    free(kdtree);

    return;
}

void _sub_kdtree_get_elems(KDNode *root, void **elems, int *k){
    if( root->left ) _sub_kdtree_get_elems(root->left , elems, k);
    elems[(*k)++] = root->info;
    if( root->right ) _sub_kdtree_get_elems(root->right, elems, k);
}

void _kdtree_get_elems(KDNode *root, void **elems){
    int k = 0;
    _sub_kdtree_get_elems(root, elems, &k);
}

void varray_shell_iter_r(void **v, int a, int b, int k, cmp_r_f cmp_r, void *arg){
    int i, n, igap, gap;

    n = b-a+1;
    igap = 7;
    while( n*2 < shell_gaps[igap] )
        igap--;

    gap = shell_gaps[igap];
    for( i = 1 ; i < gap ; i++ ){}
    // TODO: implement parcial shellsort...

    return;
}

int _kdtree_cmp_r( void *a, void *b, void *args){
    double aval, bval;

    aval = ((kdtree_eval_f)((void**)args)[0])(a, *(int*)((void**)args)[1] );
    bval = ((kdtree_eval_f)((void**)args)[0])(b, *(int*)((void**)args)[1] );

    if( aval < bval ) return -1;
    else if( aval > bval ) return 1;
    return 0;
}

void _kdtree_median_insert(KDTree *kdtree, void **elems, int n, int dim){
    int i, mid;
    Heap *minh;
    Heap *maxh;
    void *median, *elem;
    double mval, val;
    kdtree_eval_f eval_f;
    void **args[2];

    mid = n/2;
    eval_f = kdtree->eval_f;
    args[0] = (void*)(kdtree->eval_f);
    args[1] = (void*)(&dim);

    i = 0;

    /* too short array */
    if( n < 3 ){
        while( i < n )
            kdtree_insert(kdtree, elems[i++]);
        return;
    }

    /* preparing heaps */
    minh = heap_new_r(mid, _kdtree_cmp_r, args, 1);
    maxh = heap_new_r(mid, _kdtree_cmp_r, args, 0);

    median = elems[i++];
    mval = eval_f(median, dim);
    while( i < n ){
        elem = elems[i++];
        val = eval_f(elem, dim);
        /* inserting in right heap */
        if( val < mval ) heap_insert( minh, elem);
        else heap_insert( maxh, elem);

        /* balancing */
        if( maxh->n - minh->n > 1 ){
            heap_insert(minh, median);
            median = heap_pop_peak(maxh);
            mval = eval_f(median, dim);
        }else if( minh->n - maxh->n > 1 ){
            heap_insert(maxh, median);
            median = heap_pop_peak(minh);
            mval = eval_f(median, dim);
        }
    }

    /* insert median and recusevly call for splited */
    kdtree_insert(kdtree, median);
    memcpy(elems, minh->arr, minh->n*sizeof(void*));
    memcpy(&(elems[minh->n]), maxh->arr, maxh->n*sizeof(void*));
    i = minh->n;
    heap_free(minh);
    heap_free(maxh);

    _kdtree_median_insert(kdtree, elems, i, (dim+1)%(kdtree->ndim));
    _kdtree_median_insert(kdtree, &(elems[i]), n-i-1, (dim+1)%(kdtree->ndim));

    return;
}

void kdtree_balance(KDTree *kdtree){
    void **elems;
    int n, ndim;
    kdtree_eval_f eval_f;
    
    /* init variables */
    ndim = kdtree->ndim;
    eval_f = kdtree->eval_f;
    n = kdtree->n;
    elems = (void**)malloc(n*sizeof(void*));

    /* get all elements out */
    _kdtree_get_elems(kdtree->root, elems);

    /* renewing the kdtree */
    kdtree->root = NULL;
    kdtree->n = 0;

    /* find median */
    _kdtree_median_insert(kdtree, elems, n, 0);
    free(elems);

    return;
}

