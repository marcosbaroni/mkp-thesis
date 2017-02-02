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
    MOKP *mokp;
    int i, np, pos;

    mokp = bnode->bazgan->mokp;
    np = mokp->np;

    new_bnode = bnode_alloc(bnode->bazgan);
    new_bnode->idx = idx;
    for( i = 0 ; i < np ; i++ )
        new_bnode->profit[i] = bnode->profit[i] + mokp->p[i][idx];
    new_bnode->b_left = bnode->b_left - mokp->w[idx];
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
    ulonglongs_bits_fprintf(fout, node->sol, node->bazgan->mokp->n);
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
    int n, np;
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

/* Simple brute-force execution. To validate method. */
List *bazgan_exec_simple(MOKP *mokp, int k){
    KDTree *kdtree, *old_kdtree;
    List *list, *old_list, *all;
    ListNode *lnode;
    Bazgan *bazgan;
    BazganNode *bnode, *new_bnode, *dominant;
    int i, n, ndim;
    double *bounds;

    n = mokp->n;
    ndim = 3;

    bazgan = bazgan_new(mokp);
    list = list_new();
    all = list_new();
    kdtree = kdtree_new(ndim, (kdtree_eval_f)bnode_axis_val);

    bnode = bnode_new_empty(bazgan);
    list = list_insert(list, bnode);
    all = list_insert(all, bnode);
    kdtree = kdtree_insert(kdtree, bnode);

    for( i = 0 ; i < k ; i++ ){
        old_list = list;
        old_kdtree = kdtree;
        lnode = old_list->first;

        list = list_new();
        kdtree = kdtree_new(ndim, (kdtree_eval_f)bnode_axis_val);
        printf("\nINSERT IDX %d\n", i);
        printf("Nodes are:\n");
        list_apply_r(old_list, (void(*)(void*,void*))bnode_fprintf2, stdout);
        printf("OLD KDTREE\n");
        kdtree_fprint_pretty(stdout, old_kdtree);

        do{
            bnode = lnode->info;
            new_bnode = bnode_new_children(bnode, i);
            all = list_insert(all, new_bnode);
            printf("new node is: ");
            bnode_fprintf(stdout, new_bnode);
            _bazgan_insert_if_nondominated(bnode, kdtree, list, old_kdtree, old_list);
            _bazgan_insert_if_nondominated(new_bnode, kdtree, list, old_kdtree, old_list);
        }while( lnode = lnode->next );

        kdtree_free(old_kdtree);
        list_free(old_list);
    }
    printf("Nodes are:\n", i);
    list_apply_r(list, (void(*)(void*,void*))bnode_fprintf2, stdout);
    printf("nodes = %d\n", list->n);

    kdtree_free(kdtree);
    list_free(list);
    list_apply(all, (void(*)(void*))bnode_free);
    list_free(all);
    bazgan_free(bazgan);

    return NULL;
}

void bazgan_exec(MOKP *mokp, char ordering_type, int kmax){
    Bazgan *bazgan;
    MOKP *reord_mokp;
    int *idxs;

    BazganNode *n1, *n2, *n3;

    /* reordering indexes */
    idxs = get_mokp_new_ordering(mokp, ordering_type);
    reord_mokp = mokp_reorder(mokp, idxs);
    free(idxs);
    mokp_write(stdout, reord_mokp);

    /* creating bazgan execution instance */
    bazgan = bazgan_new(reord_mokp);

    /* freeing structures */
    bazgan_free(bazgan);
    mokp_free(reord_mokp);

    return;
}

