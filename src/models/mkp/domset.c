/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: december, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <alloca.h>

#include "domset.h"
#include "mkp.h"
#include "mkp-soputils.h"
#include "lbucket.h"
#include "../../utils/ppm.h"

/*****************************************************************************
 *     Dominating Set Node
 *****************************************************************************/
int dsnode_cmp(DomSetNode *dsn1, DomSetNode *dsn2){
	int j, m;
	mkpnum res;

	/* comparing profit */
	res = dsn1->profit - dsn2->profit;
	if( res > 0 ){
		return 1;
	}else if( res < 0 ){
		return -1;
	/* comparing weights */
	}else{
		for( j = 0 ; j < m ; j++ ){
			res = dsn1->b_left[m-j-1] - dsn2->b_left[m-j-1];
			if( res > 0 ){
				return 1;
			}else if( res < 0 ){
				return -1;
			}
		}
	}
	return 0;
}

int dsnode_cmp_by_profit(DomSetNode *dsn1, DomSetNode *dsn2){
    return (dsn1->profit - dsn2->profit);
}
int dsnode_cmp_by_b_left0(DomSetNode *dsn1, DomSetNode *dsn2){
    return (dsn1->b_left[0] - dsn2->b_left[0]);
}
int dsnode_cmp_by_b_left1(DomSetNode *dsn1, DomSetNode *dsn2){
    return (dsn1->b_left[1] - dsn2->b_left[1]);
}
int dsnode_cmp_by_b_left2(DomSetNode *dsn1, DomSetNode *dsn2){
    return (dsn1->b_left[2] - dsn2->b_left[2]);
}

/*
 * obs.:
 *    A dominates B iff:
 *       p(A) >= p(B)    AND    w_i(A) <= w_i(B), for all j in [m]
 * */
int dsnode_dominates(DomSetNode *dsn1, DomSetNode *dsn2){
	int i, m;

	m = dsn1->tree->mkp->m;
    dsn1->tree->n_comparison++;

	/* dsn1 dominates dsn2 ? */
	if( dsn1->profit >= dsn2->profit ){
		/* maybe... */
		for( i = 0 ; i < m ; i++ )
			if( dsn1->b_left[i] < dsn2->b_left[i] ) /* any bleft1 < bleft2? */
				return 0; /* no! */
		return 1; /* yes! */
	}
    return 0;
}

/*
 * idx: index of the item to be inserted in solution.
 * WARINING: returning NULL if new node is not feasible.
 *   - father: father of the new node
 *   - dstree: the holder of all node and problem info
 *   - idx: the index of the new item being inserted
 *
 *   *Obs.: Does not insert node in tree.
 * */
DomSetNode *dsnode_new(DomSetNode *father, int idx){
	DomSetNode *dsnode;
    DomSetTree *dstree;
	int i, m, feasible;
    mkpnum b_left;
    MKP *mkp;

    dstree = father->tree;
    mkp = dstree->mkp;
	m = mkp->m;

	/* allocating new node */
	dsnode = (DomSetNode*)malloc(sizeof(DomSetNode));
	dsnode->b_left = (mkpnum*)malloc(m*sizeof(mkpnum));

	/* initializing node info */
	dsnode->idx = idx;
	dsnode->profit = father->profit + mkp->p[idx];
    feasible = 1;
    for( i = 0 ; i < m ; i++ )
        if( (dsnode->b_left[i] = father->b_left[i] - mkp->w[i][idx]) < 0 )
            feasible = 0;
	/* initializing tree info */
    dsnode->tree = father->tree;
	dsnode->father = father;
	dsnode->prev = NULL;
    dsnode->next = NULL;
    /* KDtree info */
	dsnode->up = NULL;
    dsnode->right = NULL;
    dsnode->left = NULL;
    
    /* dischage node if not feasible */
	if( !feasible ){
		free(dsnode->b_left);
		free(dsnode);
		dsnode = NULL;
	}

	return dsnode;
}

void dsnode_free(DomSetNode *dsnode){
	free(dsnode->b_left);
	free(dsnode);
	return;
}

void dsnode_fprintf_mo(FILE *fout, DomSetNode *dsnode){
    int i, m;
    double *b;
    m = dsnode->tree->mkp->m;
    b = dsnode->tree->mkp->b;

    mkpnum_fprintf(fout, dsnode->profit);
    fprintf(fout, "%x: %.0lf ", dsnode, dsnode->profit);
    for( i = 0 ; i < m-1 ; i++ )
        fprintf(fout, "%.0lf ", b[i]-dsnode->b_left[i]);
    fprintf(fout, "(%.0lf)", dsnode->b_left[m-1]);

   return;
}

void dsnode_fprintf(FILE *fout, DomSetNode *dsnode){
    int i, m;
    m = dsnode->tree->mkp->m;

    mkpnum_fprintf(fout, dsnode->profit);
    fprintf(fout, "  [");
    mkpnum_array_write(fout, dsnode->b_left, m);
    fprintf(fout, "] (%x)", dsnode);

   return;
}

/*
 * Extracts the MKP solution from a dominating set node.
*/
MKPSol *dsnode_get_mkpsol(DomSetNode *dsnode){
	MKPSol *mkpsol;

	mkpsol = mkpsol_new(dsnode->tree->mkp);

	while( dsnode->idx > -1 ){
		mkpsol_add_item(mkpsol, dsnode->idx);
		dsnode = dsnode->father;
	}

	return mkpsol;
}

/*****************************************************************************
 *     Dominating Set Tree
 *****************************************************************************/
/*
 * Initialize a DomSetTree (to use in case of stack allocation)
 * */
DomSetTree *dstree_init(
    DomSetTree *dstree,
    MKP *mkp,
    unsigned char *x,
    mkpnum *p,
    mkpnum **w)
{
	DomSetNode *root;
	mkpnum profit, weight;
	int i, j, n, m;

	n = mkp->n;
	m = mkp->m;

	/* heap alloc */
	if( !dstree )
		dstree = (DomSetTree*)malloc(sizeof(DomSetTree));

	/* first solution (empty node) */
	root = (DomSetNode*)malloc(sizeof(DomSetNode));
	root->idx = -1;
	root->profit = 0;
	root->b_left = mkpnum_array_copy(NULL, mkp->b, mkp->m);
    root->tree = dstree;
	root->father = root->prev = root->next = NULL;

	/* INITIALIZATION */
	/* problem instance info */
	dstree->mkp = mkp;

	/* structure info */
	dstree->n = 1;
	dstree->root = root;
	dstree->best = root;
	dstree->tail = root;
    /* performance info */
    dstree->n_comparison = 0ULL;

    dstree->kdtree = NULL;
    dstree->lbucket = NULL;

	return dstree;
}

DomSetTree *dstree_new(MKP *mkp){
	return dstree_init(NULL, mkp, NULL, NULL, NULL);
}

void dstree_set_lbucket(DomSetTree *dstree, LinkedBucket *lbucket){
    dstree->lbucket = lbucket;
}

void dstree_set_kdtree(DomSetTree *dstree, DomSetKDTree *kdtree){
    dstree->kdtree = kdtree;
    kdtree->dstree = dstree;
}

DomSetTree *dstree_insert(DomSetTree *dstree, DomSetNode *dsnode){
	if( dstree->root ){
		dstree->tail->next = dsnode;
		dsnode->prev = dstree->tail;
        if( dsnode->profit > dstree->best->profit )
            dstree->best = dsnode;
	}else{
		dstree->root = dsnode;
        dstree->best = dsnode;
    }
	dstree->tail = dsnode;

	dstree->n++;

	/* checking best */
	if( dsnode->profit > dstree->best->profit )
		dstree->best = dsnode;

    /* INSERTING ON KDTREE */
    if(dstree->kdtree)
        dskdtree_insert(dstree->kdtree, dsnode);

	return dstree;
}

DomSetTree *dstree_remove(DomSetTree *dstree, DomSetNode *dsnode){
	if( dstree->root == dsnode )
		dstree->root = dsnode->next; /* update root (if needed) */
	else
		dsnode->prev->next = dsnode->next; /* if not root, has a prev. */
	
	if( dsnode->next ) /* update next, if needed */
		dsnode->next->prev = dsnode->prev;
	else               /* if no next, its a tail. */
		dstree->tail = dsnode->prev;

	dstree->n--;

    /* REMOVING FROM KD TREE */
    if(dstree->kdtree)
        dskdtree_remove(dstree->kdtree, dsnode);

	return dstree;
}

void dstree_free(DomSetTree *dstree){
	DomSetNode *dsnode, *dsn_next;

	dsnode = dstree->root;
	while( dsnode ){
		dsn_next = dsnode->next;
		dsnode_free(dsnode);
		dsnode = dsn_next;
	}

    if(dstree->kdtree)
        free(dstree->kdtree);

	free(dstree);

	return;
}

void dstree_fprint(FILE *out, DomSetTree *dstree){
	int i, j, n, nx, m;
	DomSetNode *dsn, *fat;

	n = dstree->mkp->n;
	m = dstree->mkp->m;
	dsn = dstree->root->next;

	/*printing header*/

	/* tree is empty */
	if(!dsn)
		return;

	/* for each node... */
	do{
		nx = 0;
		fat = dsn;
		while(fat = fat->father)
			nx++;
		fprintf(out, "%d;", nx);
		mkpnum_fprintf(out, dsn->profit);
		for( i = 0 ; i < m ; i++ ){
			fprintf(out, ";");
			mkpnum_fprintf(out, dstree->mkp->b[i] - dsn->b_left[i]);
		}
		fprintf(out, "\n");
	}while(dsn = dsn->next);

	return;
}

DomSetNode *dstree_exists_dominance(DomSetTree *dstree, DomSetNode *dsn1){
    DomSetNode *dsn2;

    if( dstree->kdtree )
        return dskdtree_find_dominator(dstree->kdtree, dsn1);

    dsn2 = dstree->root;
    do
        if( dsnode_dominates(dsn2, dsn1) )   /* If dsn2 dominates dsn1 */
            return dsn2;
    while( dsn2 = dsn2->next );

    return NULL;
}

/*
 * Apply one iteration of the dynamic programming method on a given DomSetTree.
 *   dstree: the DomSetTree given
 *   idx: the index of the item to be inserted
 */
DomSetTree *dstree_dynprog(DomSetTree *dstree, int idx){
    DomSetNode *father;
    DomSetNode *new;
    int j, n_nodes;

    n_nodes = dstree->n;
    father = dstree->root;

    /* for each existing domset (need couting because tail will grow) */
    for( j = 0 ; j < n_nodes; j++ ){
        new = dsnode_new(father, idx);
        if( new ){
            if( dstree_exists_dominance(dstree, new) ){
                dsnode_free(new);
            }else{
                dstree_insert(dstree, new);
            }
        }
        /* next node */
        father = father ->next;
    }
    return dstree;
}

/*******************************************************************************
 * Multidimensional Knapsack Problem - Dynamic Programming (Nemhauser-Ullman) *
 * - mkp : the MKP problem instance
 * - idxs: optional array of items reorder (for the algorithm)
*******************************************************************************/
MKPSol *mkp_dynprog(MKP *mkp, int *idxs){
    MKPSol *mkpsol;
    DomSetTree *dstree;
    DomSetNode *father;
    DomSetNode *new;
    DomSetNode *root;
    DomSetNode *tail;

    int i, j, n, m;
    int n_nodes;
    int idx;

    /* initialize dstree */
    dstree = dstree_new(mkp);

    n = mkp->n;
    /* for each item */
    for( i = 0 ; i < n ; i++ ){
        if(idxs) idx = idxs[i];
        else idx = i;

        dstree = dstree_dynprog(dstree, idxs[i]);
    }

    mkpsol = dsnode_get_mkpsol(dstree->best);
    dstree_free(dstree);

    return mkpsol;
}

/*******************************************************************************
 * DomSet KD-Tree
 ******************************************************************************/

DomSetKDTree *dskdtree_new(int ndim){
    DomSetKDTree *t;

    t = (DomSetKDTree*)malloc(sizeof(DomSetKDTree));

    t->ndim = ndim;
    t->root = NULL;

    return t;
}

/* 
 * dim = 0: profit
 * dim < 0: b_left[dim-1]
 */
void _dskdtree_insert(DomSetNode *root, DomSetNode *dsnode, int h, int ndim){
    int insert_right;
    int dim;

    insert_right = 1;
    dim = h % ndim;
    if( (!dim && dsnode->profit < root->profit) || /* pofit check */
        (dim && dsnode->b_left[dim-1] < root->b_left[dim-1]) ) /* b_left check */
            insert_right = 0;

    /* inserting */
    if( insert_right )
        if( !root->right )
            { root->right = dsnode; dsnode->up = root; }
        else
            _dskdtree_insert(root->right, dsnode, h+1, ndim);
    else
        if( !root->left )
            { root->left = dsnode; dsnode->up = root; }
        else
            _dskdtree_insert(root->left, dsnode, h+1, ndim);

    return;
}

DomSetKDTree *dskdtree_insert(DomSetKDTree *kdtree, DomSetNode *dsnode){
    if( kdtree->root )
        _dskdtree_insert(kdtree->root, dsnode, 0, kdtree->ndim);
    else
        { kdtree->root = dsnode; dsnode->up = NULL; }

    return kdtree;
}

DomSetKDTree *dskdtree_remove(DomSetKDTree *kdtree, DomSetNode *dsnode){

    return kdtree;
}

DomSetNode *_dskdtree_find_dominator(DomSetNode *root, DomSetNode *dsnode, int h, int ndim){
    int dim;
    DomSetNode *dominant = NULL;

    if( ! root )
        return NULL;

    if( dsnode_dominates(root, dsnode) )
        return root;

    dim = h % ndim;

    /* always check right tree */
    dominant = _dskdtree_find_dominator(root->right, dsnode, h+1, ndim);

    /* if not found, check if left tree may have dominant */
    if( !dominant )
        if( (!dim && dsnode->profit < root->profit) || /* pofit check */
            (dim && dsnode->b_left[dim-1] < root->b_left[dim-1]) ) /* b_left check */
            dominant = _dskdtree_find_dominator(root->left, dsnode, h+1, ndim);

    return dominant;
}

DomSetNode *dskdtree_find_dominator(DomSetKDTree *dskdtree, DomSetNode *dsnode){
    return _dskdtree_find_dominator(dskdtree->root, dsnode, 0, dskdtree->ndim);
}

void _dskdtree_draw(DomSetNode *root, PPM *ppm, int h, int ndim){
    int x, y, dim;
    int x0, xf, y0, yf;
    int rad = 10;

    if( !root)
        return;

    dim = h % ndim;

    if( !dim ){ /* profit: horizontal line */
        /* defining x0 and xy */
        if( root->up ){
            // TODO: find bounds (recursively?)
        }else{
            x0 = 0;
            xf = ppm_get_maxx(ppm);
        }
        ppm_paint_dot(ppm, x, y, PPM_RED, rad);
    }else{ /* b_left[0]: vertical line */
        if( root->up ){
        }else{
            y0 = 0;
            yf = ppm_get_maxy(ppm);
        }
        ppm_paint_dot(ppm, x, y, PPM_BLUE, rad);
    }


    _dskdtree_draw(root->right, ppm, h+1, ndim);
    _dskdtree_draw(root->left, ppm, h+1, ndim);
}

void dskdtree_draw(DomSetKDTree *kdtree, char *filename){
    PPM *ppm;
    int nx, ny;
    MKP *mkp;
    int n, m;
    int i, j;

    n = mkp->n;
    m = mkp->m;

    nx = ny = 0;
    for(i = 0 ; i < n ; i++ ){
        nx += mkp->p[i];
        ny += mkp->w[0][i];
    }

    ppm = ppm_new(nx, ny);
    _dskdtree_draw(kdtree->root, ppm, 0, kdtree->ndim);
    ppm_write(ppm, filename);
    ppm_free(ppm);

    return;
}

void _dstree_dp_iter(DomSetTree *dstree, int idx){
    DomSetNode *current;
    DomSetNode *dominant;
    DomSetNode *new_dsnode;
    int n_nodes;
    int i;

    n_nodes = dstree->n;
    current = dstree->root;
    printf(" %d: fixing\n", idx+1);
    for( i = 0 ; i < n_nodes ; i++ ){
        new_dsnode = dsnode_new(current, idx);

        /* IF NEW NODE IS FEASIBLE */
        if( new_dsnode ){
            /* TRY TO FIND A DOMINANT NODE */
            if( dstree->kdtree )
                dominant = dskdtree_find_dominator(dstree->kdtree, new_dsnode);
            else if( dstree->lbucket )
                dominant = lbucket_exists_dominator(dstree->lbucket, new_dsnode);
            else
                dominant = dstree_exists_dominance(dstree, new_dsnode);

            printf("  %d:%s", i+1, dominant ? "*DISCHAGED " : " ADDED ");
            dsnode_fprintf_mo(stdout, new_dsnode);
            if( dominant ){
                printf("  dom: ");
                dsnode_fprintf_mo(stdout, dominant);
            }
            printf("\n");

            /* INSERT THE NEW NODE, IF HAS NO DOMINANT */
            if( !dominant ) dstree_insert(dstree, new_dsnode);
            else dsnode_free(new_dsnode);
        }
        current = current->next;
    }

    return;
}

void dstree_dp_iter(DomSetTree *dstree, int idx){
    _dstree_dp_iter(dstree, idx);
}

void _dskdtree_get_profile(DomSetNode *root, int h, int *count, int *nmax){
    int i;
    if( root->right )
        _dskdtree_get_profile( root->right, h+1, count, nmax);

    /* alloing more space */
    if( h > *nmax-1 ){
        count = realloc(count, 2*(*nmax)*sizeof(int));
        for( i = *nmax ; i < 2*(*nmax) ; i++ )
            count[i] = 0;
        *nmax = *nmax*2;
    }

    count[h]++;

    if( root->left)
        _dskdtree_get_profile( root->left, h+1, count, nmax);

    return;
}

DomSetKDTree *dskdtree_balance(DomSetKDTree *dskdtree){
    DomSetNode ***dsn_ord;
    DomSetNode *dsn;
    DomSetTree *dstree;
    int i, j;
    int n_nodes, ndim;

    dstree = dskdtree->dstree;
    ndim = dskdtree->ndim;
    n_nodes = dstree->n;

    dsn_ord = (DomSetNode***)malloc(ndim*sizeof(DomSetNode**));
    for( i = 0 ; i < ndim ; i++ )
        dsn_ord[i] = (DomSetNode**)malloc(n_nodes*sizeof(DomSetNode*));

    dsn = dstree->root;
    switch(ndim){
        case 2:
        for( i = 0 ; i < n_nodes ; i++ ){
            dsn_ord[0][i] =
                dsn_ord[1][i] =  dsn;
            dsn = dsn->next;
        }
        break;

        case 3:
        for( i = 0 ; i < n_nodes ; i++ ){
            dsn_ord[0][i] =
                dsn_ord[1][i] =
                dsn_ord[2][i] = dsn;
            dsn = dsn->next;
        }
        break;

        case 4:
        for( i = 0 ; i < n_nodes ; i++ ){
            dsn_ord[0][i] =
                dsn_ord[1][i] =
                dsn_ord[2][i] =
                dsn_ord[3][i] = dsn;
            dsn = dsn->next;
        }
        break;

        case 5:
        for( i = 0 ; i < n_nodes ; i++ ){
            dsn_ord[0][i] =
                dsn_ord[1][i] =
                dsn_ord[2][i] =
                dsn_ord[3][i] =
                dsn_ord[4][i] = dsn;
            dsn = dsn->next;
        }
        break;
    }

    return dskdtree;
}

double dskdtree_mean_h(DomSetKDTree *kdtree){
    int i, hmax;
    int *count;
    double mean_h;
    hmax = 100;

    count = (int*)malloc(hmax*sizeof(int));
    for( i = 0 ; i < hmax ; i++ )
        count[i] = 0;
    _dskdtree_get_profile(kdtree->root, 0, count, &hmax);

    mean_h = 0;
    for( i = 0 ; i < hmax ; i++ )
        if( count[i] )
            mean_h += i*((double)count[i]/((double)kdtree->dstree->n));

    free(count);

    return mean_h;
}

void _get_median(DomSetNode **dsns, DomSetNode **dsns_ord, int *k, int a, int b, int h, int ndim){
    int(*cmp)(DomSetNode*,DomSetNode*);

    /* setting compare function to be used */
    switch( h % ndim ){
        case 0: cmp = dsnode_cmp_by_profit; break;
        case 1: cmp = dsnode_cmp_by_b_left0; break;
        case 2: cmp = dsnode_cmp_by_b_left1; break;
        case 3: cmp = dsnode_cmp_by_b_left2; break;
    }

    if( b-a < 2 ){
        // put ...
    }

}

DomSetKDTree *dskdtree_build_balanced(DomSetNode **dsns, int n_nodes, int ndim){
    DomSetKDTree *kdtree;
    DomSetNode **dsns_ord;
    int k;

    dsns_ord = (DomSetNode**)malloc(n_nodes*sizeof(DomSetNode*));
    kdtree = dskdtree_new(3);

    k = 0;
    _get_median(dsns, dsns_ord, &k, 0, n_nodes-1, 0, ndim);

    return kdtree;
}
void dstree_balance_kdtree(DomSetTree *dstree){
    DomSetKDTree *kdtree2;
    DomSetNode *dsnode;
    DomSetNode **dsns;
    int n_nodes;
    int i;

    n_nodes = dstree->n;
    dsns = (DomSetNode**)malloc(n_nodes*sizeof(DomSetNode*));

    i = 0;
    do dsns[i++] = dsnode;
    while( dsnode = dsnode->next );

    kdtree2 = dskdtree_build_balanced(dsns, n_nodes, 3);

    free(dsns);
}

