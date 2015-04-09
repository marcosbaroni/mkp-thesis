#include <stdio.h>
#include <stdlib.h>

#include "domset.h"
#include "mkp.h"

DomSetTree *dstree_new(MKP *mkp){
	DomSetTree *dstree;
	DomSetNode *dsnode;

	/* First solution (empty) */
	dsnode = (DomSetNode*)malloc(sizeof(DomSetNode));
	dsnode->idx = -1;
	dsnode->profit = 0;
	dsnode->b_left = long_long_array_copy(NULL, mkp->b, mkp->m);
	dsnode->father = dsnode->prev = dsnode->next = NULL;

	/* allocing */
	dstree = (DomSetTree*)malloc(sizeof(DomSetTree));
	dstree->mkp = mkp;
	dstree->root = dstree->tail = dstree->best = dsnode;
	dstree->n = 1;

	return dstree;
}

/*
 * returns:
 *  -1 : dsn2 dominates dsn1
 *   0 : none dominates the other
 *  +1 : dsn1 dominates dsn2
 *
 * obs.:
 *    A dominates B iff:
 *       p(A) >= p(B)    AND    w_i(A) <= w_i(B), for all j in [m]
 * */
int dsnode_dominates(DomSetNode *dsn1, DomSetNode *dsn2, MKP *mkp){
	int i, m;
	m = mkp->m;

	/* dsn1 dominates dsn2 ? */
	if( dsn1->profit >= dsn2->profit ){
		/* maybe... */
		for( i = 0 ; i < m ; i++ )
			if( dsn1->b_left[i] < dsn2->b_left[i] )
				return 0; /* no! */
		return 1; /* yes! */
	}

	/* maybe dsn2 dominates dsn1... */
	for( i = 0 ; i < m ; i++ )
		if( dsn2->b_left[i] < dsn1->b_left[i] )
			return 0; /* no! */
	return -1; /* yes! */
}

/*
 * idx: index od the item to be inserted in solution.
 * WARINING: returning NULL if new node is not feasible.
 * */
DomSetNode *dsnode_new(DomSetNode *father, MKP *mkp, int idx){
	DomSetNode *dsnode;
	int i, m, feasible;

	m = mkp->m;

	dsnode = (DomSetNode*)malloc(sizeof(DomSetNode));
	dsnode->profit = father->profit + mkp->p[idx];
	dsnode->b_left = (long long*)malloc(m*sizeof(long long));
	feasible = 1;
	for( i = 0 ; i < m ; i++ ){
		dsnode->b_left[i] = father->b_left[i] - mkp->w[i][idx];
		if( dsnode->b_left[i] < 0 )
			feasible = 0;
	}
	dsnode->father = father;
	dsnode->prev = dsnode->next = NULL;

	/* if new solution is not feasible... */
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

DomSetTree *dstree_insert(DomSetTree *dstree, DomSetNode *dsnode){
	if( dstree->root ){
		dstree->tail->next = dsnode;
		dsnode->prev = dstree->tail;
	}else{
		dstree->root = dsnode;
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

	/* TODO: iniciar enumeração das soluções... 
	 * 	    - iniciar de uma solução vazia (por enquanto);
	 * 	    - criar novo nó apenas no caso de inserção de item.
	 * 	    - utilizar LinkedBucket na enumeração
	 * 	    */
MKPSol *mkp_fast_domsets_enum(MKP *mkp){
	DomSetTree *dstree;
	DomSetNode *dsnode, *dsn_tail, *dsn_new, *dsn_iter;
	int i, j, n, m, dominance, promissing;

	n = mkp->n;
	m = mkp->m;

	/* initializing domsets tree */
	dstree = dstree_new(mkp);

	/* ENUMERATING SOLUTIONS */
	/* for each item... */
	for( i = 0 ; i < n ; i++ ){
		/* for each existent dominating set... */
		for( dsnode = dstree->root, dsn_tail = dstree->tail
			; dsnode
				?(dsnode->prev /* if has a prev node and... */
					?dsnode->prev != dsn_tail /* ... its not the old tail. */
					:1)
				:0
			; dsnode = dsnode->next)
		{
			/* new 'child' solution (adding item 'i') */
			dsn_new = dsnode_new(dsnode, mkp, i);
			promissing = dsn_new ? 1 : 0;

			/* checking dominance */
			/* FIXME: tail is out of comparison. */
			for( dsn_iter = dstree->root;
				dsn_iter != dsn_tail && promissing;
				dsn_iter = dsn_iter->next)
			{
				/* check relative dominance */
				dominance = dsnode_dominates(dsn_new, dsn_iter, mkp);
				//if( dominance == 1 ){ /* dsn_new dominates dsn_iter */
				//	if( dsn_iter == dsn_tail ) /* update tail, if needed */
				//		dsn_tail = dsn_tail->prev;
				//	dstree_remove(dstree, dsn_iter);
				//	dsnode_free(dsn_iter);
				//}
				if( dominance == -1 ) /* iter dominates new */
					promissing = 0;
			}
			if( promissing ) /* new is not dominated */
				dstree_insert(dstree, dsn_new);
			else if( dsn_new )
				dsnode_free(dsn_new);
		}
		printf("%d - %d sets (best=%lld)\n", i+1, dstree->n, dstree->best->profit);
	}

	/* output */
	printf("Best: %lld\n", dstree->best->profit);

	/* free */
	dstree_free(dstree);

	return NULL;
}

