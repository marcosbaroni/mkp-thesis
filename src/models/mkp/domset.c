#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <alloca.h>

#include "domset.h"
#include "mkp.h"
#include "soputils.h"

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

/*
 * obs.:
 *    A dominates B iff:
 *       p(A) >= p(B)    AND    w_i(A) <= w_i(B), for all j in [m]
 * */
int dsnode_dominates(DomSetNode *dsn1, DomSetNode *dsn2){
	int i, m;

	m = dsn1->tree->mkp->m;  /* FIXME: check performance impact */

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
 * */
DomSetNode *dsnode_new(DomSetNode *father, int idx){
	DomSetNode *dsnode;
    DomSetTree *dstree;
	int i, m, feasible;
    mkpnum b_left;
    MKP *mkp;
	double relax_w_free, relax_w_item;

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
	dsnode->prev = dsnode->next = NULL;
	dsnode->father = father;
    
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

void dsnode_fprintf(FILE *fout, DomSetNode *dsnode){
    int i, m;
    m = dsnode->tree->mkp->m;

    fprintf(fout, "- dsnode: %x\n", dsnode);
    fprintf(fout, "  profi: ");
    mkpnum_fprintf(fout, dsnode->profit);
    fprintf(fout, "  b_left:");
   mkpnum_array_write(fout, dsnode->b_left, m);

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
    mkpnum **w,
    mkpnum *relax_w)
{
	DomSetNode *root;
	mkpnum profit, weight;
	int i, j, n, m;

	n = mkp->n;
	m = mkp->m;

	/* heap alloc */
	if( !dstree )
		dstree = (DomSetTree*)malloc(sizeof(DomSetTree));
	if( !x )
		x = (unsigned char*)malloc(n*sizeof(unsigned char));
	if( !p )
		p = (mkpnum*)malloc(n*sizeof(mkpnum));
	if( !w ){
		w = (mkpnum**)malloc(m*sizeof(mkpnum*));
		for( i = 0 ; i < m ; i++ )
		w[i] = (mkpnum*)malloc(n*sizeof(mkpnum));
	}

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
	dstree->p = mkpnum_array_copy(NULL, mkp->p, n);
    dstree->w = mkpnum_matrix_copy(NULL, mkp->w, mkp->m, mkp->n);

	/* structure info */
	dstree->n = 1;
	dstree->root = root;
	dstree->best = root;
	dstree->tail = root;

	return dstree;
}

DomSetTree *dstree_new(MKP *mkp){
	return dstree_init(NULL, mkp, NULL, NULL, NULL, NULL);
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

    dsn2 = dstree->root;
    while( dsn2 ){
        if( dsnode_dominates(dsn1, dsn2) < 0 )   /* If dsn2 dominates dsn1 */
            return dsn2;
        dsn2 = dsn2->next;
    }

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
    printf("total nodes: %d\n", dstree->n);
    dstree_free(dstree);

    return mkpsol;
}

