#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../../utils/avl.h"
#include "../../utils/kdtree.h"
#include "../../utils/list.h"

#include "mokp.h"
#include "bazgan.h"

void ulonglongs_bits_fprintf(FILE *fout, ulonglong *sol, int nbits){
    int i, j, ullsize, n, pos;
    ulonglong ull;

    ullsize = 8*sizeof(ulonglong);
    for( i = 0 ; i < nbits ; i++ ){
        j = i/ullsize;
        ull = sol[j];
        pos = i - (j*ullsize);
        fprintf(fout, "%c", ((1ULL << pos) & ull) ? '1' : '0');
    }
}

/*******************************************************************************
 *      BAZGAN NODE
*******************************************************************************/
BazganNode *bnode_alloc(Bazgan *baz){
    BazganNode *bnode;

    bnode = (BazganNode*)malloc(sizeof(BazganNode));
    bnode->bazgan = baz;
    bnode->profit = (double*)malloc(baz->mokp->np*sizeof(double));
    bnode->sol = (ulonglong*)malloc(baz->solsize*sizeof(ulonglong));

    return bnode;
}

void bnode_free(BazganNode *bnode){
    free(bnode->profit);
    free(bnode->sol);
    free(bnode);
    
    return;
}

BazganNode *bnode_new_empty(Bazgan *baz){
    BazganNode *bnode;
    int i, np, solsize;

    solsize = baz->solsize;

    bnode = bnode_alloc(baz);
    bnode->idx = -1;
    for( i = 0 ; i < baz->mokp->np ; i++ )
        bnode->profit[i] = 0.0;
    for( i = 0 ; i < solsize ; i++ )
        bnode->sol[i] = 0ULL;
    bnode->b_left = baz->mokp->b;

    return bnode;
}

BazganNode *bnode_new_children(BazganNode *bnode, int idx){
    BazganNode *new_bnode;
    double b_left;
    MOKP *mokp;
    int i, np, pos;

    mokp = bnode->bazgan->mokp;
    np = mokp->np;
    b_left = bnode->b_left - mokp->w[idx];
    if( b_left < 0 )
        return NULL;

    new_bnode = bnode_alloc(bnode->bazgan);
    new_bnode->idx = idx;
    for( i = 0 ; i < np ; i++ )
        new_bnode->profit[i] = bnode->profit[i] + mokp->p[i][idx];
    new_bnode->b_left = b_left;
    memcpy(new_bnode->sol, bnode->sol, bnode->bazgan->solsize*sizeof(ulonglong));
    i = idx / (8*sizeof(ulonglong));
    pos = (idx - i*(8*sizeof(ulonglong)));
    new_bnode->sol[i] |= (1ULL << pos);

    return new_bnode;
}

int bnode_lex_cmp(BazganNode *n1, BazganNode *n2){
    int res, solsize, i;

    res = n1->b_left - n2->b_left;

    if( res )
        return res;

    solsize = n1->bazgan->solsize;
    for( i = 0 ; i < solsize ; i++ )
        if( res = (n1->sol[i] > n2->sol[i]) )
            return res;

    return 0;
}

int bnode_dominates(BazganNode *b1, BazganNode *b2){
    int i, np;
    np = b1->bazgan->mokp->np;

    for( i = 0 ; i < np ; i++ )
        if( b1->profit[i] < b2->profit[i] )
            return 0;

    return b1->b_left > b2->b_left;
}

int bnode_is_dominated_by(BazganNode *b1, BazganNode *b2){
    return bnode_dominates(b2, b1);
}

double bnode_axis_val(BazganNode *n1, int axis){
    if( !axis )
        return n1->b_left;
    return n1->profit[axis-1];
}

void bnode_fprintf(FILE *fout, BazganNode *node){
    double_array_fprint(fout, node->profit, node->bazgan->mokp->np);
    fprintf(fout, " (%lf) ", node->b_left);
    //ulonglongs_bits_fprintf(fout, node->sol, node->bazgan->mokp->n);
    fprintf(fout, " [%x] ", node);
    fprintf(fout, "\n");

    return;
}

void bnode_fprintf2(BazganNode *node, FILE *fout){
    bnode_fprintf(fout, node);
}

double *bnode_get_dominant_bounds(BazganNode *bnode, int ndim){
    int np, i;
    double *bounds;

    bounds = (double*)malloc(ndim*2*sizeof(double));

    np = bnode->bazgan->mokp->np;
    bounds[0] = -1;
    bounds[1] = bnode->b_left;
    for( i = 1 ; i < ndim ; i++ ){
        bounds[i*2] = bnode->profit[i-1];
        bounds[i*2+1] = INFINITY;
    }

    return bounds;
}

int *get_mokp_new_ordering(MOKP *mokp, char ordering_type){
    int *idxs;
    int i, n, np;

    n = mokp->n;
    np = mokp->np;

    idxs = (int*)malloc(n*sizeof(int));
    for( i = 0 ; i < n ; i++ )
        idxs[i] = i;

    return idxs;
}

/*******************************************************************************
 *      BAZGAN INSTANCE
*******************************************************************************/
Bazgan *bazgan_new(MOKP *mokp){
    int i, n, np;
    Bazgan *baz;

    n = mokp->n;
    baz = (Bazgan*)malloc(sizeof(Bazgan));

    baz->mokp = mokp;
    baz->solsize = ((n-1)/8)+1;
    baz->avl_lex = new_avltree((avl_cmp_f)bnode_lex_cmp);

    return baz;
}

void bazgan_free(Bazgan *bazgan){
    if(bazgan->avl_lex)
        avl_free(bazgan->avl_lex);
    free(bazgan);
}

void _bazgan_insert_if_nondominated(
    BazganNode *node,
    KDTree *kdtree,
    List *list,
    KDTree *old_kdtree,
    List *old_list)
{
    double *bounds;
    int ndim;
    BazganNode *dominant;
    ListNode *lnode;

    ndim = kdtree->ndim;

    lnode = old_list->first;
    dominant = NULL;
    bounds = bnode_get_dominant_bounds(node, kdtree->ndim);
    dominant = kdtree_range_search_r(old_kdtree, bounds, (property_f_r)bnode_is_dominated_by, node);
    free(bounds);

    if( !dominant && node->b_left >= 0 ){
        list_insert(list, node);
        kdtree_insert(kdtree, node);
    }
}

BazganNode *bnode_list_insert_if_no_dom(BazganNode *bnode, List *list, List *new_list){
    BazganNode *bnode2, *dominant;
    ListNode *lnode;

    if( !bnode )
        return NULL;

    dominant = NULL;
    lnode = list->first;
    while( lnode && !dominant ){
        bnode2 = lnode->info;
        if( bnode_is_dominated_by(bnode, bnode2) )
            dominant = bnode2;
    }

    if( dominant ) list_insert(new_list, bnode);
    else bnode_free( bnode );

    return dominant;
}

/* Simple brute-force execution. To validate method. */
List *bazgan_exec_simple(MOKP *mokp, int k){
    List *list, *old_list;
    ListNode *lnode, *lnode2;
    Bazgan *bazgan;
    BazganNode *bnode, *new_bnode, *dominant;
    int i, n, ndim;
    double *bounds;

    n = mokp->n;
    ndim = 3;

    bazgan = bazgan_new(mokp);
    list = list_new();

    bnode = bnode_new_empty(bazgan);
    list = list_insert(list, bnode);

    /* Iterations */
    for( i = 0 ; i < k ; i++ ){
        lnode = list->first;
        old_list = list;
        list = list_new();
        while( lnode ){
            bnode = lnode->info;
            bnode_list_insert_if_no_dom(bnode, old_list, list);
            new_bnode = bnode_new_children(bnode, i);
            bnode_list_insert_if_no_dom(new_bnode, old_list, list);
            lnode = lnode->next;
        }
    }
    //printf("Nodes are:\n");
    //list_apply_r(list, (void(*)(void*,void*))bnode_fprintf2, stdout);
    //printf("Total nodes = %d\n", list->n);
    //printf("(all = %d)\n", all->n);

    list_apply(list, (void(*)(void*))bnode_free);
    list_free(list);

    return NULL;
}

BNodeIter bnodeiter_get_next(BNodeIter iter){
    return avlnode_get_next(iter);
}

BazganNode *bnodeiter_get_bnode(BNodeIter iter){
    if( iter )
        return (BazganNode*)iter->info;
    return NULL;
}

BNodeIter _bazgan_get_first(Bazgan *bazgan){
    return avl_get_first(bazgan->avl_lex);
}

/*
 * Bazgan2009, p. 14.
 */
Bazgan *_bazgan_iter(Bazgan *bazgan, int idx){
    BazganNode *j_node, *i_node, *father, *child;
    BNodeIter j_iter, i_iter;
    MOKP *mokp;
    int n, i;
    double min_b_left_needed, min_b_left_feasible;

    mokp = bazgan->mokp;
    n = mokp->n;

    /* Computing minimum b_left needed (based on remaining weights) */
    min_b_left_feasible = mokp->b - mokp->w[idx];
    min_b_left_needed = min_b_left_feasible;
    for( i = idx+1 ; i < n ; i++ )
        min_b_left_needed -= mokp->w[i];

    /* Finding the first with min b_left (i.e., not D-r-Dominated) */
    j_iter = _bazgan_get_first(bazgan);
    j_node = bnodeiter_get_bnode(j_iter);
    while( j_node->b_left <= min_b_left_needed ){
        j_iter = bnodeiter_get_next(j_iter);
        j_node = bnodeiter_get_bnode(j_iter);
    }

    i_iter = _bazgan_get_first(bazgan);
    i_node = bnodeiter_get_bnode(i_iter);
    while( i_node->b_left <= min_b_left_feasible ){
        child = bnode_new_children(i_node, idx);
        if( j_node ){
            while( bnode_lex_cmp(j_node, child) >= 0 ){
                /* TODO: mantainNonDominated(j_node) ... */
                j_iter = bnodeiter_get_next(j_iter);
                j_node = bnodeiter_get_bnode(j_iter);
            }
            /* TODO: mantainNonDominated(i_node) ... */
        }
        i_iter = bnodeiter_get_next(i_iter);
        i_node = bnodeiter_get_bnode(i_iter);
        if( !i_node ) break;
    }

    return bazgan;
}

void bazgan_exec(MOKP *mokp, char ordering_type, int kmax){
    Bazgan *bazgan;
    MOKP *reord_mokp;
    int i;
    int *idxs;

    /* Reordering MOKP indexes */
    idxs = get_mokp_new_ordering(mokp, ordering_type);
    reord_mokp = mokp_reorder(mokp, idxs);
    free(idxs);
    mokp_write(stdout, reord_mokp);

    /* Creating bazgan execution instance */
    bazgan = bazgan_new(reord_mokp);

    /* Method iterations... */
    for( i = 0 ; i < kmax ; i++ ){
        _bazgan_iter(bazgan, i);
    }

    /* Freeing structures */
    bazgan_free(bazgan);
    mokp_free(reord_mokp);

    return;
}

