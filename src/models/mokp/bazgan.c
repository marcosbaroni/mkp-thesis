#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "../../utils/avl.h"
#include "../../utils/kdtree.h"
#include "../../utils/list.h"

#include "mokp.h"
#include "bazgan.h"

void ulonglongs_bits_set(ulonglong *bits, int idx){
    int i, pos;

    i = idx / (8*sizeof(ulonglong));
    pos = (idx - i*(8*sizeof(ulonglong)));
    bits[i] |= (1ULL << pos);
}

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
    //bnode->profit = (double*)malloc(baz->mokp->np*sizeof(double));
#if SOL_ARRAY_ENABLED
    bnode->sol = (ulonglong*)malloc(baz->solsize*sizeof(ulonglong));
#endif

    return bnode;
}

void bnode_free(BazganNode *bnode){
#if SOL_ARRAY_ENABLED
    free(bnode->sol);
#endif
    //free(bnode->profit);
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
        bnode->profit[i] = 0;
#if SOL_ARRAY_ENABLED
    for( i = 0 ; i < solsize ; i++ )
        bnode->sol[i] = 0ULL;
#endif
    bnode->b_left = baz->mokp->b;

    return bnode;
}

BazganNode *bnode_copy(BazganNode *bnode){
    BazganNode *bnode2;
    Bazgan *baz;
    int i, np, solsize;

    baz = bnode->bazgan;
    solsize = baz->solsize;

    bnode2 = bnode_alloc(baz);
    bnode2->idx = bnode->idx;
    for( i = 0 ; i < baz->mokp->np ; i++ )
        bnode2->profit[i] = bnode->profit[i];
#if SOL_ARRAY_ENABLED
    for( i = 0 ; i < solsize ; i++ )
        bnode2->sol[i] = bnode->sol[i];
#endif
    bnode2->b_left = bnode->b_left;

    return bnode2;
}

BazganNode *bnode_insert_item(BazganNode *bnode, int idx){
    int i;
    MOKP *mokp;

    mokp = bnode->bazgan->mokp;
    for( i = 0 ; i < mokp->np ; i++ )
        bnode->profit[i] += mokp->p[i][idx];
    bnode->b_left -= mokp->w[idx];
#if SOL_ARRAY_ENABLED
    ulonglongs_bits_set(bnode->sol, idx);
#endif

    return bnode;
}

BazganNode *bnode_new_children(BazganNode *bnode, int idx){
    BazganNode *new_bnode;

    new_bnode = bnode_copy(bnode);
    bnode_insert_item(new_bnode, idx);

    return new_bnode;
}

/* (property) */
int bnode_lex_dom(BazganNode *n1, BazganNode *n2){
    int np, i;

    n1->bazgan->_ncomparison++;
    np = n1->bazgan->mokp->np;

    for( i = 0 ; i < np ; i++ )
        if( n1->profit[i] != n2->profit[i] )
            return( n1->profit[i] > n2->profit[i] );

    return 1;
}

int bnode_lex_cmp(BazganNode *n1, BazganNode *n2){
    int np, i;
    mokpnum res;

    n1->bazgan->_ncomparison++;
    res = n1->b_left - n2->b_left;
    np = n1->bazgan->mokp->np;

    if( res > 0 )
        return 1;
    else if( res < 0) 
        return -1;

    for( i = 0 ; i < np ; i++ ){
        if( n1->profit[i] != n2->profit[i] ){
            if( n1->profit[i] > n2->profit[i] ){
                return 1;
            }else if( n1->profit[i] < n2->profit[i] ){
                return -1;
            }
        }
    }

    return 0;
}

int bnode_lex_cmp_inv(BazganNode *n1, BazganNode *n2){
    return bnode_lex_cmp(n2, n1);
}

int bnode_dominates(BazganNode *b1, BazganNode *b2){
    int i, np, has_higher;

    has_higher = 0;
    np = b1->bazgan->mokp->np;
    b1->bazgan->_ncomparison++;
    for( i = 0 ; i < np ; i++ ){
        if( b1->profit[i] < b2->profit[i] )
            return 0;
        else if( b1->profit[i] > b2->profit[i] )
            has_higher = 1;
    }

    return ( has_higher && b1->b_left > b2->b_left);
}

int bnode_is_dominated_by(BazganNode *b1, BazganNode *b2){
    return bnode_dominates(b2, b1);
}

double bnode_axis_val(BazganNode *n1, int axis){
    if( !axis )
        return (double)n1->b_left;
    return (double)n1->profit[axis-1];
}

int bnode_profit1_cmp(BazganNode *n1, BazganNode *n2){
    return (n1->profit[0] - n2->profit[0]);
}


/***    Profit-only operations    *********************************************/
double bnode_profit_val(BazganNode *n1, int axis){
    return (double)n1->profit[axis];
}

int bnode_profit_dominates(BazganNode *b1, BazganNode *b2){
    int i, np, has_higher;

    has_higher = 0;
    b1->bazgan->_ncomparison++;
    np = b1->bazgan->mokp->np;
    for( i = 0 ; i < np ; i++ ){
        //if( b1->profit[i] < b2->profit[i] )
        if( b1->profit[i] < b2->profit[i] )
            return 0;
        //else if( b1->profit[i] > b2->profit[i] )
        else if( b1->profit[i] > b2->profit[i] )
            has_higher = 1;
    }

    return has_higher;
}

int bnode_profit_lex_cmp(BazganNode *n1, BazganNode *n2){
    int np, i;
    mokpnum res;

    n1->bazgan->_ncomparison++;
    np = n1->bazgan->mokp->np;

    for( i = 0 ; i < np ; i++ )
        if( n1->profit[i] != n2->profit[i] )
            return n1->profit[i] > n2->profit[i] ? 1 : -1;

    return 0;
}
/******************************************************************************/

void bnode_fprintf(FILE *fout, BazganNode *node){
    fprintf(fout, "[");
    mokpnum_array_write(fout, node->profit, node->bazgan->mokp->np);
    fprintf(fout, "] (");
    mokpnum_fprintf(fout, node->b_left);
    fprintf(fout, ") ");
#if SOL_ARRAY_ENABLED
    ulonglongs_bits_fprintf(fout, node->sol, node->bazgan->mokp->n);
#endif
    fprintf(fout, " [%x] ", node);
    //fprintf(fout, "\n");

    return;
}

void bnode_printf(BazganNode *bnode){
    bnode_fprintf(stdout, bnode);
}

void bnode_fprintf2(BazganNode *node, FILE *fout){
    bnode_fprintf(fout, node);
}

double *bnode_get_dominant_bounds(
    BazganNode *bnode,
    int ndim,
    int just_profits
){
    int np, i;
    double *bounds;

    bounds = (double*)malloc(ndim*2*sizeof(double));

    np = bnode->bazgan->mokp->np;
    if( just_profits ){
        for( i = 0 ; i < ndim ; i++ ){
            bounds[i*2] = (double)bnode->profit[i];
            bounds[i*2+1] = INFINITY;
        }
    }else{
        bounds[0] = bnode->b_left;
        bounds[1] = INFINITY;
        for( i = 1 ; i < ndim ; i++ ){
            bounds[i*2] = (double)bnode->profit[i-1];
            bounds[i*2+1] = INFINITY;
        }
    }

    return bounds;
}

double *bnode_get_dominated_bounds(
    BazganNode *bnode,
    int ndim,
    int just_profits
){
    int np, i;
    double *bounds;

    bounds = (double*)malloc(ndim*2*sizeof(double));

    np = bnode->bazgan->mokp->np;
    if( just_profits ){
        for( i = 0 ; i < ndim ; i++ ){
            bounds[i*2] = 0;
            bounds[i*2+1] = (double)bnode->profit[i];
        }
    }else{
        bounds[0] = 0;
        bounds[1] = bnode->b_left;
        for( i = 1 ; i < ndim ; i++ ){
            bounds[i*2] = 0;
            bounds[i*2+1] = (double)bnode->profit[i-1];
        }
    }

    return bounds;
}

BazganNode *bnode_list_find_dominator(
    BazganNode *bnode,
    List *list
){
    BazganNode *dominator, *candidate;
    ListIter *liter;
    int(*dom_checker)(BazganNode*, BazganNode*);

    dom_checker = bnode_profit_dominates;

    dominator = NULL;
    liter = list_get_first(list);
    while( (candidate = (BazganNode*)listiter_forward(liter)) && !dominator )
        if( dom_checker( candidate, bnode ) )
            dominator = candidate;
    listiter_free(liter);

    return dominator;
}

BazganNode *bnode_avl_find_dominator(
    BazganNode *upper,
    AVLTree *avl_pool
){
    BazganNode *dominator, *bnode;
    AVLIter *iter;

    dominator = NULL;
    iter = avl_get_lower_higher_than(avl_pool, upper);
    while( (bnode = avliter_forward(iter)) && !dominator )
        if( bnode_profit_dominates(bnode, upper) )
            dominator = bnode;

    avliter_free(iter);
    return dominator;
}

BazganNode *bnode_kdtree_find_dominator(
    BazganNode *bnode,
    KDTree *kdtree
){
    int ndim;
    double *bounds;
    BazganNode *dominator;
    property_f_r dom_func;

    bounds = bnode_get_dominant_bounds(bnode, kdtree->ndim, 1);
    dom_func = (property_f_r)bnode_profit_dominates;

    dominator = kdtree_range_search_r(
        kdtree,
        bounds, 
        dom_func,
        bnode);

    free(bounds);

    return dominator;
}

BazganNode *bnode_get_lower_bound(
    BazganNode *bnode,
    int *idxs,       /* array of available items */
    int nleft        /* items left */
){
    int i, np, n;
    BazganNode *_bnode;
    mokpnum *w;

    w = bnode->bazgan->mokp->w;
    _bnode = bnode_copy(bnode);
    n = bnode->bazgan->mokp->n;
    for( i = 0 ; i < nleft ; i++ )
        if( _bnode->b_left >= w[idxs[i]] )
            _bnode = bnode_insert_item(_bnode, idxs[i]);

    return _bnode;
}

BazganNode *bnode_get_upper_bound(
    BazganNode *bnode,
    int fst_idx,            /* index of the fts item to be added */
    int **idxs              /* available itens (best profit-cost order) [np][<n] */
){
    MOKP *mokp;
    mokpnum b_left, *w, **p, portion;
    int n, i, j, np, idx;
    BazganNode *_bnode;

    mokp = bnode->bazgan->mokp;
    _bnode = bnode_copy(bnode);
    n = mokp->n;
    w = mokp->w;
    p = mokp->p;
    i = 0;

    /* greedy per-dimension packing */
    np = mokp->np;
    for( j = 0 ; j < np ; j++ ){
        b_left = _bnode->b_left;
        for( i = 0 ; i < (n-fst_idx) && b_left > .0; i++ ){
            idx = idxs[j][i];
            portion = fmin(1., b_left/w[idx]);
            _bnode->profit[j] += portion*p[j][idx];
            b_left -= portion*w[idx];
        }
    }
    _bnode->b_left = b_left;

    return _bnode;
}

void _avl_remove2(void *bnode, void *avl){
    AVLTree *ptr;
    ptr = avl_remove( (AVLTree*)avl, (AVLNode*)bnode);
    if( !ptr ){
        printf("ERROR!!! avl\n");
        bnode_fprintf(stdout, (BazganNode*)bnode);
    }
}

/* Remove from node list those which are not promising, (using KDTree)
 *   based on their upper-bound and current lower-bounds.
 *   1. Buils upper-bound pool
 *   2. (For each node) Compute upper bound & check its potencial
 * */
void bazgan_ub_filter(
    Bazgan *bazgan,
    int fst_idx,
    int ndim,        /* 0 for list */
    int run_exactly
){
    KDTree *lower_bounds_kdtree;
    List *non_promissings, *lower_bounds_list;
    AVLTree *avl_nodes, *lower_bounds_avl;
    AVLIter *nodes_iter;
    BazganNode *bnode, *lower, *upper, *dominator;
    int i, j, k, n, np, *idxs_max_, *idxs_sum_;
    int *max_order, *sum_order;
    int isum, imax;
    int **upper_idxs, **best_pc_order;
    double *bounds;
    int dominance_failed;

    n = bazgan->mokp->n;
    np = bazgan->mokp->np;
    lower_bounds_kdtree = NULL;
    lower_bounds_avl = NULL;
    lower_bounds_list = NULL;
    non_promissings = list_new();

    /***************************************************************************
    * Initialize
    ***************************************************************************/
    avl_nodes = bazgan->avl_lex;
    nodes_iter = avl_get_first(avl_nodes);
    /* Initilaizing lowerbound pool structure */
    if( !ndim )
        lower_bounds_list = list_new();
    else if( ndim == 1 )
        lower_bounds_avl = new_avltree((avl_cmp_f)bnode_profit1_cmp);
    else
        lower_bounds_kdtree = kdtree_new(ndim, (kdtree_eval_f)bnode_profit_val);

    /* preparing ARRAY of available items */
    idxs_max_ = (int*)malloc(n*sizeof(int));
    idxs_sum_ = (int*)malloc(n*sizeof(int));
    max_order = bazgan->max_order;
    sum_order = bazgan->sum_order;
    isum = imax = 0;
    for( i = 0 ; i < n ; i++ ){
        if( max_order[i] >= fst_idx )
            idxs_max_[imax++] = max_order[i];
        if( sum_order[i] >= fst_idx )
            idxs_sum_[isum++] = sum_order[i];
    }

    /* TODO: EXLUIR DA LISTA DE ITEM AQUELES QUE JA FORAM ITERADOS. */

    /***************************************************************************
    * Computing lower-bound pool
    ***************************************************************************/
    while( bnode = avliter_forward(nodes_iter) ){
        /* Lower-bound using SUM_ */
        lower = bnode_get_lower_bound(bnode, idxs_sum_, n-fst_idx);
        /* Inserting lower-bounds node */
        if( !ndim )
            list_insert(lower_bounds_list, lower);
        else if( ndim == 1 )
            avl_insert(lower_bounds_avl, lower);
        else
            kdtree_insert(lower_bounds_kdtree, lower);

        /* Lower-bound using MAX_ */
        lower = bnode_get_lower_bound(bnode, idxs_max_, n-fst_idx);
        if( !ndim )
            list_insert(lower_bounds_list, lower);
        else if( ndim == 1 )
            avl_insert(lower_bounds_avl, lower);
        else
            kdtree_insert(lower_bounds_kdtree, lower);
    }
    free(idxs_sum_);
    free(idxs_max_);

    /***************************************************************************
    * Building sorted index array of available items 
    * (for upper-bound computation)
    ***************************************************************************/
    best_pc_order = bazgan->best_profit_cost_order;
    upper_idxs = (int**)malloc(np*sizeof(int*));
    for( i = 0 ; i < np ; i++ ){
        k = 0;
        upper_idxs[i] = (int*)malloc(n*sizeof(int));
        for( j = 0 ; j < n ; j++ )
            if( best_pc_order[i][j] >= fst_idx )
                upper_idxs[i][k++] = best_pc_order[i][j];
    }
    avliter_free(nodes_iter);

    /***************************************************************************
    * Computing upper-bound pool / Checking potencial of each node.
    ***************************************************************************/
    nodes_iter = avl_get_first(avl_nodes);
    dominance_failed = 0;
    //while( (bnode = avliter_get(nodes_iter)) ){
    while( bnode = avliter_get(nodes_iter) ){ /* heuristic */
        upper = bnode_get_upper_bound(bnode, fst_idx, upper_idxs);
        /* check if exist a LB dominating the UB */
        if( !ndim )
            dominator = bnode_list_find_dominator(upper, lower_bounds_list);
        else if( ndim == 1 )
            dominator = bnode_avl_find_dominator(upper, lower_bounds_avl);
        else
            dominator = bnode_kdtree_find_dominator(upper, lower_bounds_kdtree);

        bnode_free(upper);
        /* if a dominator exists (node is not promising) */
        if( dominator ){
            list_insert(non_promissings, bnode);
        }else{
            dominance_failed = 1;
            if( !run_exactly )
                break;
        }
        avliter_forward(nodes_iter);
    }
    
    /***************************************************************************
    * Freeing
    ***************************************************************************/
    for( i = 0 ; i < np ; i++ )
        free(upper_idxs[i]);
    free(upper_idxs);
    avliter_free(nodes_iter);
    /* LB nodes */
    if( lower_bounds_avl )
        avl_apply_to_all(lower_bounds_avl, (void(*)(void*))bnode_free);
    else if( lower_bounds_kdtree )
        kdtree_apply_to_all(lower_bounds_kdtree, (void(*)(void*))bnode_free);
    else
        list_apply(lower_bounds_list, (void(*)(void*))bnode_free);
    /* LB pool structure */
    if( lower_bounds_list )
        list_free(lower_bounds_list);
    if( lower_bounds_avl )
        avl_free(lower_bounds_avl);
    if( lower_bounds_kdtree )
        kdtree_free(lower_bounds_kdtree);

    /* removing marked nodes */
    list_apply_r( non_promissings, _avl_remove2, avl_nodes);
    list_apply( non_promissings, (void(*)(void*))bnode_free);
    list_free(non_promissings);

    return;
}


int *t_mokp_new_ordering(MOKP *mokp, char ordering_type){
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
int **get_best_profit_cost_order(MOKP *mokp){
    int **order;
    int np, n, i, j, idx;
    double *pcost;

    np = mokp->np;
    n = mokp->n;

    order = (int**)malloc(np*sizeof(int*));
    pcost = (double*)malloc(n*sizeof(double));

    for( i = 0 ; i < np ; i++ ){
        for( j = 0 ; j < n ; j++ )
            pcost[j] = (double)mokp->p[i][j]/(double)mokp->w[j];
        order[i] = double_index_sort(pcost, n);
    }

    free(pcost);

    return order;
}

int _mokp_get_solize(MOKP *mokp){
    return ((mokp->n-1)/8) + 1;
}

Bazgan *bazgan_new(MOKP *mokp){
    int i, n, np;
    Bazgan *baz;

    n = mokp->n;
    baz = (Bazgan*)malloc(sizeof(Bazgan));

    bazgan_pong(baz);
    baz->mokp = mokp;
    baz->solsize = _mokp_get_solize(mokp);
    baz->avl_lex = new_avltree((avl_cmp_f)bnode_lex_cmp_inv);
    baz->best_profit_cost_order = get_best_profit_cost_order(mokp);
    baz->_ncomparison = 0;
    baz->just_profits = 1;
    baz->max_nd = 0;
    baz->max_order = mokp_get_item_order(mokp, 'M');
    baz->sum_order = mokp_get_item_order(mokp, 's');
    bazgan_ping(baz);

    return baz;
}

void bazgan_free(Bazgan *bazgan){
    int np;

    np = bazgan->mokp->np;
    avl_apply_to_all(bazgan->avl_lex, (void(*)(void*))bnode_free);
    avl_free(bazgan->avl_lex);
    while( np-- )
        free(bazgan->best_profit_cost_order[np]);
    free(bazgan->best_profit_cost_order);
    free(bazgan->max_order);
    free(bazgan->sum_order);
    //mokp_free(bazgan->mokp);  /* freeing, assuming its a reordered copy */
    free(bazgan);
}


/* Inserts node in list if no dominator is found. */
BazganNode *bnode_list_insert_if_no_dom(
    BazganNode *bnode,
    List *list,
    List *new_list)
{
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
        lnode = lnode->next;
    }

    if( !dominant )
        list_insert(new_list, bnode);

    return dominant;
}

/*********************************************
 * Aplication of Dominanced 2 (using List)   *
 ********************************************/
BazganNode *_mantain_non_dom_list(
    BazganNode *bnode,
    AVLTree *c_avl,
    List *m_list,
    List *to_be_freeded
){
    ListIter *m_iter;
    BazganNode *m_bnode;
    BazganNode *dominant;
    
    /* Search dominant. */
    dominant = NULL;
    m_iter = list_get_first(m_list);
    m_bnode = listiter_get(m_iter);
    if( !list_is_empty(m_list) ){
        // while( && !dominant ){   /* verificar divergencia... */
        //while( bnode_lex_dom(m_bnode, bnode) && !dominant ){
        while( bnode_lex_cmp(m_bnode, bnode) > 0 && !dominant ){
            if( bnode_profit_dominates(m_bnode, bnode) )
                dominant = m_bnode;
            else 
                m_bnode = listiter_forward(m_iter);

            if( !m_bnode )
                break;
        }
    }

    if( !dominant ){
        avl_insert(c_avl, bnode);
        list_insert_here(m_list, bnode, m_iter);

        listiter_forward(m_iter);
        m_bnode = listiter_get(m_iter);
        /* removing any possible dominated from checking pool */
        while( m_bnode ){
            if( bnode_profit_dominates(bnode, m_bnode) ){
                list_remove(m_list, m_iter);
                m_bnode = listiter_get(m_iter);
            }else{
                m_bnode = listiter_forward(m_iter);
            }
        }
    }else{
        list_insert(to_be_freeded, bnode);
    }
    listiter_free(m_iter);

    return dominant;
}

BazganNode *_mantain_non_dom_avl(
    BazganNode *bnode,
    AVLTree *c_avl,
    AVLTree *m_avl,
    List *to_be_freeded
){
    BazganNode *dominant;

    dominant = bnode_avl_find_dominator(bnode, m_avl);

    if( !dominant ){
        avl_insert(c_avl, bnode);
        avl_insert(m_avl, bnode);
    }

    return dominant;
}

/* 
 * Aplication of Dominanced 2 (using KDTree)
 * */
BazganNode *_mantain_non_dom_kdtree(
    BazganNode *bnode,
    AVLTree *c_avl,
    KDTree *m_kdtree,
    List *to_be_freeded
){
    Bazgan *bazgan;
    BazganNode *dominant;
    double *bounds;
    int ndim;
    property_f_r dom_func;

    ndim = m_kdtree->ndim;
    bazgan = bnode->bazgan;
    bounds = bnode_get_dominant_bounds(bnode, ndim, bazgan->just_profits);

    if( bazgan->just_profits )
        dom_func = (property_f_r)bnode_profit_dominates;
    else
        dom_func = (property_f_r)bnode_dominates;

    /* Search dominant. */
    dominant = kdtree_range_search_r(
        m_kdtree,
        bounds, 
        dom_func,
        bnode);

    if( !dominant ){
        avl_insert(c_avl, bnode);
        kdtree_insert(m_kdtree, bnode);
    }

    free(bounds);

    return dominant;
}

/* 
 * Aplication of Dominanced 2 (dispatcher)
 * */
BazganNode *_mantain_non_dom(
    BazganNode *bnode,  /* Candidate node*/
    AVLTree *c_avl,     /* New node pool*/
    List *m_list,       /* Current node pool List (optional) */
    AVLTree *m_avl,     /* Current node pool AVL (optional) */
    KDTree *m_kdtree,   /* Current node pool KDTree (optional) */
    List *to_be_freeded /* Non-promessing nodes ("node garbage") */
){
    BazganNode *bnode2;
    BazganNode *list_dominant, *avl_dominant, *kdtree_dominant;

    list_dominant = kdtree_dominant = avl_dominant = NULL;

    if( m_list )
        list_dominant = _mantain_non_dom_list(bnode, c_avl, m_list, to_be_freeded);
    if( m_avl )
        avl_dominant = _mantain_non_dom_avl(bnode, c_avl, m_avl, to_be_freeded);
    if( m_kdtree )
        kdtree_dominant = _mantain_non_dom_kdtree(bnode, c_avl, m_kdtree, to_be_freeded);

    if( list_dominant )
        return list_dominant;
    if( avl_dominant )
        return avl_dominant;
    return kdtree_dominant;
}

/*
 * Bazgan2009, p. 14.
 */
Bazgan *_bazgan_iter(Bazgan *bazgan, int idx, int ndim){
    mokpnum max_b_left_needed, min_b_left_feasible;
    int n, i;
    BazganNode *j_node, *i_node, *child;
    AVLIter *j_iter, *i_iter;
    MOKP *mokp;
    AVLTree *new_avl_lex, *old_avl_lex;
    List *to_be_freeded;
    List *m_list;
    KDTree *m_kdtree;
    AVLTree *m_avl;

    mokp = bazgan->mokp;
    n = mokp->n;

    /*********************************************************************
    * PRE-PROCESSING
    * Computing minimum b_left needed (based on remaining weights)
    *********************************************************************/
    min_b_left_feasible = mokp->w[idx];
    max_b_left_needed = 0;
    for( i = idx ; i < n ; i++ )
        max_b_left_needed += mokp->w[i];

    /*********************************************************************
    * INITILALIZING
    * Computing minimum b_left needed (based on remaining weights)
    *********************************************************************/
    i_iter = avl_get_first(bazgan->avl_lex);
    j_iter = avl_get_first(bazgan->avl_lex);
    new_avl_lex = new_avltree((avl_cmp_f)bnode_lex_cmp_inv);
    to_be_freeded = list_new();

    m_list = NULL;
    m_kdtree = NULL;
    m_avl = NULL;
    /* allocing desired structure */
    if( !ndim )
        m_list = list_new();
    else if( ndim == 1 )
        m_avl = new_avltree((avl_cmp_f)bnode_profit1_cmp);
    else
        m_kdtree = kdtree_new(ndim, (kdtree_eval_f)bnode_profit_val);

    /**********************************************************************
    * DOM 1
    * Finding the first with "min_b_left_needed" (i.e., not D-r-Dominated) 
    **********************************************************************/
    j_node = avliter_forward(j_iter);
    while( j_node ){
        if( j_node->b_left <= max_b_left_needed )
            break;
        j_node = avliter_forward(j_iter);
    }

    /**********************************************************************
    * DOM 2
    * Generating children, following lexographic order
    **********************************************************************/
    i_node = avliter_forward(i_iter);
    while( i_node ){
        /* Stops when fathers start to generate unfeasible nodes */
        if( i_node->b_left < min_b_left_feasible )
            break;

        /* Children generation */
        child = bnode_new_children(i_node, idx);

        /* Testing "old nodes" (j_node) */
        while( j_node ){
            /* Stops if actual j_node is lexLess than children */
            if( bnode_lex_cmp(j_node, child) < 0 )
                break;

            _mantain_non_dom(j_node, new_avl_lex, m_list, m_avl, m_kdtree, to_be_freeded);
            j_node = avliter_forward(j_iter);
        }
        _mantain_non_dom(child, new_avl_lex, m_list, m_avl, m_kdtree, to_be_freeded);
        i_node = avliter_forward(i_iter);
    }

    /* Inserting remaining j_nodes */
    while( j_node ){
        _mantain_non_dom(j_node, new_avl_lex, m_list, m_avl, m_kdtree, to_be_freeded);
        j_node = avliter_forward(j_iter);
    }
    /***   BAZGAN UPDATE   ***/
    old_avl_lex = bazgan->avl_lex;
    bazgan->avl_lex = new_avl_lex;

    /**********************************************************************
    * DOM 3
    * Filtering not-promising nodes
    **********************************************************************/
    bazgan_ub_filter(bazgan, idx+1, ndim, (idx == n-1) );
    /* Max ND update */
    if( bazgan->max_nd < bazgan->avl_lex->n )
        bazgan->max_nd = bazgan->avl_lex->n;

    /**********************************************************************
    * FREEING
    **********************************************************************/
    list_apply(to_be_freeded, (void(*)(void*))bnode_free);
    list_free(to_be_freeded);
    avl_free(old_avl_lex);
    avliter_free(i_iter);
    avliter_free(j_iter);
    if( m_kdtree )
        kdtree_free(m_kdtree);
    if( m_list )
        list_free(m_list);

    return bazgan;
}

void bazgan_ping(Bazgan *bazgan){
    bazgan->ping = clock();
}

void bazgan_pong(Bazgan *bazgan){
    bazgan->pong = clock();
}

double bazgan_get_seconds(Bazgan *bazgan){
    return (bazgan->pong - bazgan->ping)/(double)CLOCKS_PER_SEC;
}

void bazgan_fprint_nodes(FILE *out, Bazgan *bazgan){
    AVLIter *avliter;
    BazganNode *bnode;
    int i;

    fprintf(out, "  Nodes: %d\n", bazgan->avl_lex->n);
    avliter = avl_get_first(bazgan->avl_lex);
    i = 0;
    while( bnode = avliter_forward(avliter) ){
        fprintf(out, "    %d: ", ++i);
        bnode_fprintf(out, bnode);
    }
    fprintf(out, "\n");
    avliter_free(avliter);

    return;
}

void bazgan_print_nodes( Bazgan *bazgan){
    return bazgan_fprint_nodes(stdout, bazgan);
}

Bazgan *bazgan_exec(MOKP *mokp, int kmax, int ndim){
    Bazgan *bazgan;
    int i;

    /* Creating bazgan execution instance */
    bazgan = bazgan_new(mokp);
    avl_insert(bazgan->avl_lex, bnode_new_empty(bazgan));

    /* Method iterations... */
    bazgan_ping(bazgan);
	long long last_cmp;
	last_cmp = 0;
    for( i = 0 ; i < kmax ; i++ ){
        printf("\riter %d/%d (nd: %d) cmp: %lld"
			, i+1
			, kmax
			, bazgan->avl_lex->n
			, (bazgan->_ncomparison - last_cmp) );
		fflush(stdout);
		last_cmp = bazgan->_ncomparison;
        //bazgan_fprint_nodes(stdout, bazgan);
        _bazgan_iter(bazgan, i, ndim);
    }
	printf("\n");
    bazgan_pong(bazgan);
    //bazgan_fprint_nodes(stdout, bazgan);

    /* Freeing structures */
    //mokp_free(reord_mokp);
    //bazgan_free(bazgan);

    return bazgan;
}

List *bnodes_list_filter(List *bnodes, int just_profits){
    List *to_free, *new_bnodes;
    ListIter *liter, *liter2;
    BazganNode *bnode, *bnode2, *dominant;
    int(*comp_f)(BazganNode*, BazganNode*);

    comp_f = bnode_dominates;
    if( just_profits )
        comp_f = bnode_profit_dominates;

    to_free = list_new();
    new_bnodes = list_new();
    liter = list_get_first(bnodes);
    while( bnode = listiter_forward(liter) ){
        dominant = NULL;
        /* Looking for a dominant */
        liter2 = list_get_first(bnodes);
        while( (bnode2 = listiter_forward(liter2)) && !dominant )
            if( comp_f(bnode2, bnode) )
                dominant = bnode2;
        if( !dominant && bnode->b_left >= 0.0 )
            list_insert(new_bnodes, bnode);
        else
            list_insert(to_free, bnode);
        listiter_free(liter2);
    }
    /* Freeing */
    listiter_free(liter);
    list_free(bnodes);
    list_apply(to_free, (void(*)(void*))bnode_free);
    list_free(to_free);

    return new_bnodes;
}

Bazgan *bazgan_brute(MOKP *mokp, int k){
    Bazgan *bazgan;
    BazganNode *bnode, *bnode2, *new_bnode, *dominant;
    int i, n;

    List *bnodes, *new_bnodes, *to_free;
    ListIter *liter, *liter2;

    bazgan = bazgan_new(mokp);
    n = mokp->n;
    
    /* Checking input */
    if( k > n || k < 1 )
        k = n;
    bazgan_ping(bazgan);

    /* Initializing */
    bnode = bnode_new_empty(bazgan);
    bnodes = list_new();
    list_insert(bnodes, bnode);

    /* For each item */
    for( i = 0 ; i < k ; i++ ){
        new_bnodes = list_new();

        /* Generating */
        liter = list_get_first(bnodes);
        while( bnode = listiter_forward(liter) ){
            list_insert(new_bnodes, bnode);
            new_bnode = bnode_copy(bnode);
            bnode_insert_item(new_bnode, i);
            list_insert(new_bnodes, new_bnode);
        }
        listiter_free(liter);
        list_free(bnodes);

        /* Filtering */
        bnodes = bnodes_list_filter(new_bnodes, 0);

        if( bnodes->n > bazgan->max_nd )
            bazgan->max_nd = bnodes->n;
    }

    /* Last filtering */
    bnodes = bnodes_list_filter(bnodes, 1);

    /* Outputing */
    liter = list_get_first(bnodes);
    while( bnode = listiter_forward(liter) )
        avl_insert(bazgan->avl_lex, bnode);
    listiter_free(liter);
    list_free(bnodes);
    bazgan_pong(bazgan);

    return bazgan;
}

void _avl_insert( AVLTree *avl, BazganNode *n1){
    avl_insert(avl, n1);
}

void bazgan_fprint_nodes_lex(FILE *out, Bazgan *bazgan){
    BazganNode **nodes;
    AVLTree *avl;

    avl = new_avltree( (avl_cmp_f)bnode_profit_lex_cmp );
    avl_apply_to_all_r( bazgan->avl_lex, (void(*)(void*,void*))_avl_insert, avl);
    avl_apply_to_all( avl, (void(*)(void*))bnode_printf);
    printf("\n");

    avl_free(avl);

    return;
}

void bazgan_print_nodes_lex(Bazgan *bazgan){
    return bazgan_fprint_nodes_lex(stdout, bazgan);
}

void bazgan_fprint_summary(FILE *out, Bazgan *bazgan){
    fprintf(out, "%d;%lld;%.3lf;%d",
        bazgan->avl_lex->n,
        bazgan->_ncomparison,
        bazgan_get_seconds(bazgan),
        bazgan->max_nd);
}

void bazgan_print_summary(Bazgan *bazgan){
    return bazgan_fprint_summary(stdout, bazgan);
}

