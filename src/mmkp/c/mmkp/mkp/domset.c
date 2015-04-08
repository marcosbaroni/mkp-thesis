#include <stdio.h>
#include <stdlib.h>
#include "domset.h"

DomSetTree *dstree_new(MKPSol *mkpsol){
	DomSetTree *dstree;

	dstree = (DomSetTree*)malloc(sizeof(DomSetTree));
	dstree->mkpsol = mkpsol;
	dstree->mkp = mkpsol->mkp;
	dstree->n = 0;
	dstree->root = NULL;

	return dstree;
}

/*
 * returns:
 *  -1 : dns2 dominates dns1
 *   0 : none dominates the other
 *  +1 : dns1 dominates dns2
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
		if( dsn2->b_left[i] < dns1->b_left[i] )
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

DomSetTree *dstree_insert(DomSetTree *dstree, DomSetNode *dsnode){
	dstree->tail->next = dsnode;
	dsnode->prev = dstree->tail;
	dstree->tail = dsnode;
	dstree->n++;

	if( dsnode->profit > dstree->best->profit )
		dstree->best = dsnode;

	return dstree;
}

DomSetTree *dstree_remove(DomSetTree *dstree, DomSetNode *dsnode){
	if( dstree->root == dsnode )
		dstree->root = dsnode->next; /* update root (if needed) */
	else
		dsnode->prev->next = dsnode->next; /* update prev, if nedded */
	
	if( dsnode->next )
		dsnode->next->prev = denode->prev; /* update next, if needed */

	return dstree;
}

	/* TODO: iniciar enumeração das soluções... 
	 * 	    - iniciar de uma solução vazia (por enquanto);
	 * 	    - criar novo nó apenas no caso de inserção de item. */
MKPSol *mkp_fast_domsets_enum(MKP *mkp){
	DomSetTree *dstree;
	DomSetNode *dsnode, *dsn_tail, *dsnode_new, *dsnode_iter;
	MKPSol *mkpsol;
	MKP *mkp;
	int i, j, n, m, dominance, not_dominated;

	n = mkp->n;
	m = mkp->m;
	mkp = mkpsol->mkp;
	dstree = dstree_new(mkpsol);

	/* First solution (empty) */
	dsnode = (DomSetNode*)malloc(sizeof(DomSetNode));
	dsnode->idx = -1;
	dsnode->profit = 0;
	dsnode->b_left = long_long_array_copy(NULL, mkp->b, m);
	dsnode->father = dsnode->prev = dsnode->next = NULL;
	/* insert first solution */
	dstree->root = dstree->best = dstree->tail = dsnode;
	dstree->n++;

	/* for each item... */
	for( i = 0 ; i < n ; i++ ){
		/* for each existent dominating set... */
		for( dsnode = dstree->root, dsn_tail = dstree->tail;
			dsnode->next != dsn_tail ;
			dsnode = dsnode->next)
		{
			/* new 'child' solution (adding item 'i') */
			dsnode_new = dsnode_new(dsnode, mkp, i);
			not_dominated = dsnode_new;
			/* for each existent dominating set... */
			/* FIXME: tail is out of comparison. */
			for( dsnode_iter = dstree->root;
				dsnode_iter != dsn_tail && not_dominated;
				dsnode_iter = dsnode_iter->next)
			{
				/* check relative dominance */
				dominance = dsnode_dominates(dsnode_new, dsnode_iter);
				if( dominance == 1 ) /* new dominates iter */
					dstree_remove(dstree, dsnode_iter);
				if( dominance == -1 ) /* iter dominates new */
					not_dominated = 0;
			}
			if( not_dominated ) /* new is not dominated */
				dstree_insert(dstree, dsnode_new);
			else
				{ free(dsnode_new->b_left); free(dsnode_new); }
		}
	}
	/* TODO: Select best and build solution... */

	return mkpsol;
}

