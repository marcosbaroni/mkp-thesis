#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "domset.h"
#include "mkp.h"

extern int ncomp;

/*****************************************************************************
 *     Dominating Set Node
 *****************************************************************************/
int dsnode_cmp(DomSetNode *dsn1, DomSetNode *dsn2){
	int j, m;
	long long res;

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
 * returns:
 *  -1 : dsn2 dominates dsn1
 *   0 : none dominates the other
 *  +1 : dsn1 dominates dsn2
 *
 * obs.:
 *    A dominates B iff:
 *       p(A) >= p(B)    AND    w_i(A) <= w_i(B), for all j in [m]
 * */
int dsnode_dominates(DomSetNode *dsn1, DomSetNode *dsn2){
	int i, m;
	m = dsn1->m;
	ncomp++;

	/* dsn1 dominates dsn2 ? */
	if( dsn1->profit >= dsn2->profit ){
		/* maybe... */
		for( i = 0 ; i < m ; i++ )
			if( dsn1->b_left[i] < dsn2->b_left[i] ) /* any bleft1 < bleft2? */
				return 0; /* no! */
		return 1; /* yes! */
	}
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
	dsnode->m = m;
	/* setting b_left (checking feasibility) */
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


/*****************************************************************************
 *     Dominating Set Tree
 *****************************************************************************/

DomSetTree *dstree_new(MKP *mkp){
	DomSetTree *dstree;
	DomSetNode *dsnode;

	/* First solution (empty) */
	dsnode = (DomSetNode*)malloc(sizeof(DomSetNode));
	dsnode->idx = -1;
	dsnode->profit = 0;
	dsnode->b_left = long_long_array_copy(NULL, mkp->b, mkp->m);
	dsnode->father = dsnode->prev = dsnode->next = NULL;
	dsnode->m = mkp->m;

	/* allocing */
	dstree = (DomSetTree*)malloc(sizeof(DomSetTree));
	dstree->mkp = mkp;
	dstree->root = dstree->tail = dstree->best = dsnode;
	dstree->n = 1;

	return dstree;
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
		fprintf(out, "%d;%lld", nx, dsn->profit);
		for( i = 0 ; i < m ; i++ )
			fprintf(out, ";%lld", dstree->mkp->b[i] - dsn->b_left[i]);
		fprintf(out, "\n");
	}while(dsn = dsn->next);

	return;
}

/*****************************************************************************
 *     LinkedBucket
 *****************************************************************************/
/*
 * limitss[i]: the points on i-th dimension the space is segmented.
 * dims: number of indexed dimensions
 * nsub: number of subbuckets (or arrays)
 */
LinkedBucket *lbucket_new(
	long long **max_b_lefts, int nsub, int dims)
{
	/* final container */
	LinkedBucket *lbucket;
	int i;

	lbucket = (LinkedBucket*)malloc(sizeof(LinkedBucket));
	lbucket->n_dsnodes = 0;
	lbucket->dim = dims-1;
	lbucket->minProfit = LLONG_MAX;
	lbucket->maxProfit = 0;
	lbucket->max_b_left = long_long_array_copy(NULL, max_b_lefts[dims-1], nsub);
	lbucket->nsub = nsub;

	/* final bucket, holding a set of solutions (dsnodes) */
	if( !lbucket->dim ){
		/* allocing arrays for holding dsnodes */
		lbucket->dsnodes = (Array**)malloc(nsub*sizeof(Array*));
		for( i = 0 ; i < nsub ; i++ ){
			lbucket->dsnodes[i] = array_new();
		}
	}else{
		/* creating sub buckets */
		lbucket->sub_buckets = (LinkedBucket**)malloc(nsub*sizeof(LinkedBucket*));
		for( i = 0 ; i < nsub ; i++ )
			lbucket->sub_buckets[i] = lbucket_new(
				max_b_lefts, nsub, dims-1);
	}
	return lbucket;
}

void lbucket_insert_dsnode(LinkedBucket *lbucket, DomSetNode *dsnode){
	int i, dim, nsub;
	long long b_left;

	dim = lbucket->dim;
	b_left = dsnode->b_left[dim];

	/* seeking proper bucket (sub bucket-or-array) */
	i = 0;
	while( lbucket->max_b_left[i] < b_left )
		i++;

	/* sub bucket case OR array? */
	if( dim )
		lbucket_insert_dsnode(lbucket->sub_buckets[i], dsnode);
	else
		array_insert(lbucket->dsnodes[i], dsnode);
	
	/* updating profit range */
	if( dsnode->profit > lbucket->maxProfit )
		lbucket->maxProfit = dsnode->profit;
	if( dsnode->profit < lbucket->minProfit )
		lbucket->minProfit = dsnode->profit;

	/* updating couting */
	lbucket->n_dsnodes++;

	return;
}

int lbucket_exists_dominator(LinkedBucket *lbucket, DomSetNode *dsnode){
	int i, j, n, nsub, dim;
	long long b_left, *max_b_left;
	Array *a;

	dim = lbucket->dim;
	max_b_left = lbucket->max_b_left;

	/* test possible existance of any dominator */
	if( !lbucket->n_dsnodes )
		return 0;

	/* test profit range */
	if( lbucket->maxProfit < dsnode->profit )
		return 0;

	nsub = lbucket->nsub;
	b_left = dsnode->b_left[dim];

	/* seeking for the first applicable subbucket/array
	      (those having greater-or-equal b_left) */
	i = 0;
	while( max_b_left[i] < b_left )
		i++;

	if( dim ){
		/* sub buckets case */
		for( ; i < nsub ; i++ )
			lbucket_exists_dominator(lbucket->sub_buckets[i], dsnode);
	}else{
		/* arrays of dsnodes case */
		for( ; i < nsub ; i++ ){
			a = lbucket->dsnodes[i];
			n = array_get_size(a);
			for( j = 0 ; j < n ; j++ )
				if( dsnode_dominates(array_get(a, j), dsnode) )
					return 1;
		}
	}
	return 0;
}

void lbucket_free(LinkedBucket *lbucket){
	int i, nsub;

	nsub = lbucket->nsub;
	if( lbucket->dim ){
		for( i = 0 ; i < nsub ; i++ )
			lbucket_free(lbucket->sub_buckets[i] );
		free(lbucket->sub_buckets);
	}else{
		for( i = 0 ; i < nsub ; i++ )
			array_free(lbucket->dsnodes[i]);
		free(lbucket->dsnodes);
	}
	free(lbucket->max_b_left);
	free(lbucket);
}



/*****************************************************************************
 *     Fast Enumeration Algorithm
 *****************************************************************************/
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
				dominance = dsnode_dominates(dsn_new, dsn_iter);
				//if( dominance == 1 ){ /* dsn_new dominates dsn_iter */
				//	if( dsn_iter == dsn_tail ) /* update tail, if needed */
				//		dsn_tail = dsn_tail->prev;
				//	dstree_remove(dstree, dsn_iter);
				//	dsnode_free(dsn_iter);
				//}
				if( dominance == -1 ) /* current domset dominates new */
					promissing = 0;
			}
			if( promissing ) /* new is not dominated */
				dstree_insert(dstree, dsn_new);
			else if( dsn_new )
				dsnode_free(dsn_new);
		}
	}

	/* output */
	printf("Best: %lld\n", dstree->best->profit);
	//dstree_fprint(stdout, dstree);

	/* free */
	dstree_free(dstree);

	return NULL;
}

MKPSol *mkp_fast_domsets_enum_lbucket(MKP *mkp){
	DomSetTree *dstree;
	DomSetNode *dsnode, *dsn_tail, *dsn_new, *dsn_iter;
	LinkedBucket *lbucket;
	int i, j, n, m, nsub, ndim, dominance, promissing;
	long long **max_b_lefts;

	n = mkp->n;
	m = mkp->m;

	nsub = 10;
	ndim = 2;

	/* preparing max_b_lefts */
	max_b_lefts = (long long**)malloc(ndim*sizeof(long long*));
	for( i = 0 ; i < ndim ; i++ ){
		max_b_lefts[i] = (long long*)malloc(nsub*sizeof(long long));
		for( j = 0 ; j < nsub-1 ; j++ )
			max_b_lefts[i][j] = mkp->w[i][j]*(j+1)/nsub;
		max_b_lefts[i][nsub-1] = LLONG_MAX;
	}

	/* creating linked bucket */
	lbucket = lbucket_new(max_b_lefts, nsub, ndim);

	/* freeing max_b_lefts */
	for( i = 0 ; i < ndim ; i++ )
		free(max_b_lefts[i]);
	free(max_b_lefts);

	/* initializing domsets tree */
	dstree = dstree_new(mkp);

	lbucket_insert_dsnode(lbucket, dstree->root);

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
			dsn_new = dsnode_new(dsnode, mkp, i); /* null, if unfeasible */
			promissing = dsn_new ? 1 : 0;

			/* checking dominance */
			if(promissing)
				promissing = !lbucket_exists_dominator(lbucket, dsn_new);

			if( promissing ){ /* new is not dominated */
				dstree_insert(dstree, dsn_new);
				lbucket_insert_dsnode(lbucket, dsn_new);
			}else if( dsn_new ){
				dsnode_free(dsn_new);
			}
		}
	}

	/* output */
	printf("Best: %lld\n", dstree->best->profit);
	//dstree_fprint(stdout, dstree);

	/* free */
	dstree_free(dstree);
	lbucket_free(lbucket);

	return NULL;
}

