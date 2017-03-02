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
    for( i = 0 ; i < solsize ; i++ )
        bnode2->sol[i] = bnode->sol[i];
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
    ulonglongs_bits_set(bnode->sol, idx);

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

    np = n1->bazgan->mokp->np;

    for( i = 0 ; i < np ; i++ )
        if( n1->profit[i] != n2->profit[i] )
            return( n1->profit[i] > n2->profit[i] );

    return 1;
}

int bnode_lex_cmp(BazganNode *n1, BazganNode *n2){
    int res, np, i;

    res = n1->b_left - n2->b_left;

    if( res )
        return res;

    return bnode_lex_dom(n1, n2);
}

int bnode_lex_cmp_inv(BazganNode *n1, BazganNode *n2){
    return bnode_lex_cmp(n2, n1);
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
    ulonglongs_bits_fprintf(fout, node->sol, node->bazgan->mokp->n);
    fprintf(fout, " [%x] ", node);
    fprintf(fout, "\n");

    return;
}

void bnode_fprintf2(BazganNode *node, FILE *fout){
    bnode_fprintf(fout, node);
}

BazganNode *bnode_get_lower_bound(
    BazganNode *bnode,
    int fst_idx,            /* index of the fts item to be added */
    int *idxs
){
    int i, np, n;
    BazganNode *_bnode;
    double *w;

    _bnode = bnode_copy(bnode);
    n = bnode->bazgan->mokp->n;
    for( i = fst_idx ; i < n ; i++ )
        if( _bnode->b_left >= w[idxs[i]] )
            _bnode = bnode_insert_item(_bnode, idxs[i]);

    return _bnode;
}

BazganNode *bnode_get_upper_bound(
    BazganNode *bnode,
    int fst_idx,            /* index of the fts item to be added */
    int **idxs              /* worst proft-cost order [np][<n] */
){
    MOKP *mokp;
    double b_left, *w, **p, portion;
    int n, i, np, idx;
    BazganNode *_bnode;

    mokp = bnode->bazgan->mokp;
    _bnode = bnode_copy(bnode);
    np = mokp->np;
    n = mokp->n;
    w = mokp->w;
    p = mokp->p;
    i = 0;

    /* greedy per-dimension packing */
    while( np-- ){
        b_left = _bnode->b_left;
        for( i = fst_idx ; i < n && b_left > .0; i++ ){
            idx = idxs[np][i];
            portion = fmin(1., b_left/w[idx]);
            _bnode->profit[np] += portion*p[np][idx];
            b_left -= portion*w[idx];
        }
    }
    _bnode->b_left = .0;

    return _bnode;
}

double *bnode_get_dominant_bounds(BazganNode *bnode, int ndim){
    int np, i;
    double *bounds;

    bounds = (double*)malloc(ndim*2*sizeof(double));

    np = bnode->bazgan->mokp->np;
    bounds[0] = bnode->b_left;
    bounds[1] = INFINITY;
    for( i = 1 ; i < ndim ; i++ ){
        bounds[i*2] = bnode->profit[i-1];
        bounds[i*2+1] = INFINITY;
    }

    return bounds;
}

double *bnode_get_dominated_bounds(BazganNode *bnode, int ndim){
    int np, i;
    double *bounds;

    bounds = (double*)malloc(ndim*2*sizeof(double));

    np = bnode->bazgan->mokp->np;
    bounds[0] = 0;
    bounds[1] = bnode->b_left;
    for( i = 1 ; i < ndim ; i++ ){
        bounds[i*2] = 0;
        bounds[i*2+1] = bnode->profit[i-1];
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
int **get_best_profit_cost_order(MOKP *mokp){
    int **order;
    int np, n, i, j;
    double *pcost;

    np = mokp->np;
    n = mokp->n;

    order = (int**)malloc(np*sizeof(int*));
    pcost = (double*)malloc(n*sizeof(double));

    for( i = 0 ; i < np ; i++ ){
        for( j = 0 ; j < n ; j++ )
            pcost[j] = mokp->w[j]/mokp->p[i][j];
        order[j] = double_index_sort(pcost, n);
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

    baz->mokp = mokp;
    baz->solsize = _mokp_get_solize(mokp);
    baz->avl_lex = new_avltree((avl_cmp_f)bnode_lex_cmp_inv);
    baz->best_profit_cost_order = get_best_profit_cost_order(mokp);

    return baz;
}

void bazgan_free(Bazgan *bazgan){
    int np;

    np = bazgan->mokp->np;
    avl_apply_to_all(bazgan->avl_lex, (void(*)(void*))bnode_free);
    if(bazgan->avl_lex)
        avl_free(bazgan->avl_lex);
    while( np-- )
        free(bazgan->best_profit_cost_order[np]);
    free(bazgan->best_profit_cost_order);
    free(bazgan);
}

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
        while( bnode_lex_dom(m_bnode, bnode) && !dominant ){
            if( bnode_dominates(m_bnode, bnode) )
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
        while( m_bnode ){
            if( bnode_dominates(bnode, m_bnode) ){
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

BazganNode *_mantain_non_dom_kdtree(
    BazganNode *bnode,
    AVLTree *c_avl,
    KDTree *m_kdtree,
    List *to_be_freeded
){
    BazganNode *dominant;
    double *bounds;
    int ndim;

    ndim = m_kdtree->ndim;
    bounds = bnode_get_dominant_bounds(bnode, ndim);

    /* Search dominant. */
    dominant = kdtree_range_search_r(
        m_kdtree,
        bounds, 
        (property_f_r)bnode_dominates,
        bnode);

    if( !dominant ){
        avl_insert(c_avl, bnode);
        kdtree_insert(m_kdtree, bnode);
    }

    free(bounds);

    return dominant;
}

BazganNode *_mantain_non_dom(
    BazganNode *bnode,
    AVLTree *c_avl,
    List *m_list,
    KDTree *m_kdtree,
    List *to_be_freeded
){
    BazganNode *bnode2;
    BazganNode *list_dominant, *kdtree_dominant;

    list_dominant = kdtree_dominant = NULL;

    if( m_list )
        list_dominant = _mantain_non_dom_list(bnode, c_avl, m_list, to_be_freeded);
    if( m_kdtree )
        kdtree_dominant = _mantain_non_dom_kdtree(bnode, c_avl, m_kdtree, to_be_freeded);

    if( list_dominant )
        return list_dominant;
    return kdtree_dominant;
}

/*
 * Bazgan2009, p. 14.
 */
Bazgan *_bazgan_iter(Bazgan *bazgan, int idx, int ndim){
    double max_b_left_needed, min_b_left_feasible;
    int n, i;
    BazganNode *j_node, *i_node, *child;
    AVLIter *j_iter, *i_iter;
    MOKP *mokp;
    AVLTree *new_avl_lex, *old_avl_lex;
    List *to_be_freeded;
    List *m_list;
    KDTree *m_kdtree;

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

    /* allocing desired structure */
    m_list = NULL;
    m_kdtree = NULL;
    if( ndim ) m_kdtree = kdtree_new(ndim, (kdtree_eval_f)bnode_axis_val);
    else m_list = list_new();

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

            _mantain_non_dom(j_node, new_avl_lex, m_list, m_kdtree, to_be_freeded);
            j_node = avliter_forward(j_iter);
        }
        _mantain_non_dom(child, new_avl_lex, m_list, m_kdtree, to_be_freeded);
        i_node = avliter_forward(i_iter);
    }

    /* Inserting remaining j_nodes */
    while( j_node ){
        _mantain_non_dom(j_node, new_avl_lex, m_list, m_kdtree, to_be_freeded);
        j_node = avliter_forward(j_iter);
    }
    /**********************************************************************
    * BAZGAN UPDATE
    **********************************************************************/
    old_avl_lex = bazgan->avl_lex;
    bazgan->avl_lex = new_avl_lex;

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

Bazgan *bazgan_exec(MOKP *mokp, char ordering_type, int kmax, int ndim){
    Bazgan *bazgan;
    MOKP *reord_mokp;
    int i;
    int *idxs;

    /* Reordering MOKP indexes */
    idxs = get_mokp_new_ordering(mokp, ordering_type);
    reord_mokp = mokp_reorder(mokp, idxs);
    free(idxs);
    //mokp_write(stdout, reord_mokp);

    /* Creating bazgan execution instance */
    bazgan = bazgan_new(reord_mokp);
    avl_insert(bazgan->avl_lex, bnode_new_empty(bazgan));

    /* Method iterations... */
    bazgan_ping(bazgan);
    for( i = 0 ; i < kmax ; i++ ){
        //printf("iter %d/%d\n", i+1, kmax);
        //bazgan_fprint_nodes(stdout, bazgan);
        _bazgan_iter(bazgan, i, ndim);
    }
    bazgan_pong(bazgan);
    //bazgan_fprint_nodes(stdout, bazgan);

    /* Freeing structures */
    mokp_free(reord_mokp);
    //bazgan_free(bazgan);

    return bazgan;
}

