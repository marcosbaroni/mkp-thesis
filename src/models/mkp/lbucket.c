#include <stdio.h>
#include <stdlib.h>

#include "domset.h"
#include "mkp.h"

/*
 * Auxiliary function to prepare max_b_left table, for Linked Bucket creating.
 */
mkpnum **lbucket_prepare_max_b_left(MKP *mkp, int ndim, int nsub, char type){
	int i, j;
	mkpnum **max_b_lefts, last_max, sum;

	/* allocs */
	max_b_lefts = (mkpnum**)malloc(ndim*sizeof(mkpnum*));
	for( i = 0 ; i < ndim ; i++ )
		max_b_lefts[i] = (mkpnum*)malloc(nsub*sizeof(mkpnum));

	last_max = mkp->b[0]*0.48;

	/* preparing */
	for( i = 0 ; i < ndim ; i++ ){
		for( j = 0 ; j < nsub-1 ; j++ ){
			switch( type ){
				case 'l': /* linear */
				max_b_lefts[i][j] = last_max*(j+1)/nsub;
				break;

				case 's': /* square root */
				max_b_lefts[i][j] = last_max*sqrt((double)((j+1)/nsub));
				break;

				case 'q': /* quadratic */
				max_b_lefts[i][j] = last_max*((j+1)*(j+1)/(nsub*nsub));
				break;
			}
		}
		max_b_lefts[i][nsub-1] = LLONG_MAX;
	}
	return max_b_lefts;
}


/*
 * limitss[i]: the points on i-th dimension the space is segmented.
 * dims: number of indexed dimensions
 * nsub: number of subbuckets (or arrays)
 */
LinkedBucket *lbucket_new(
	mkpnum **max_b_lefts, int nsub, int dims)
{
	/* final container */
	LinkedBucket *lbucket;
	int i;

	lbucket = (LinkedBucket*)malloc(sizeof(LinkedBucket));
	lbucket->n_dsnodes = 0;
	lbucket->_total_hits = 0;
	lbucket->_total_compares = 0;
	lbucket->dim = dims-1;
	lbucket->minProfit = LLONG_MAX;
	lbucket->maxProfit = 0;
	lbucket->max_b_left = mkpnum_array_copy(NULL, max_b_lefts[dims-1], nsub);
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
	mkpnum b_left;

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
	mkpnum b_left, *max_b_left;
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
			_hitted = 1;
			a = lbucket->dsnodes[i];
			n = array_get_size(a);
			for( j = 0 ; j < n ; j++ ){
				_n_comps++;
				if( dsnode_dominates(array_get(a, j), dsnode) )
					return 1;
			}
		}
	}
	if(_hitted){
		lbucket->_total_hits++;
		lbucket->_total_compares += _n_comps;
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

/*
 * Prints some information about
 */
void lbucket_fprintf_profile(FILE *out, LinkedBucket *lbucket){
	int count, i, j, tot, dim, nsub;
	int idxs[100];
	LinkedBucket *buk;

	/* initialization */
	dim = lbucket->dim; /* should be "->dim+1", but array has no profile yet */
	nsub = lbucket->nsub;
	tot = ipow(nsub, dim);
	for( i = 0 ; i < dim ; i++ )
		idxs[i] = 0;

	for( count = 0 ; count < tot ; count++ ){
		/* outputs */
		buk = lbucket;
		for( j = 0 ; j < dim ; j++ ){
			buk = buk->sub_buckets[idxs[j]];
			fprintf(out, "%d;", idxs[j]);
		}
		fprintf(out, "%d;%d\n", buk->_total_hits, buk->_total_compares);

		/* update line */
		i = dim-1;
		idxs[i]++;
		while( idxs[i] == nsub ){
			idxs[i] = 0;
			i--;
			idxs[i]++;
		}
	}

	return;
}


/*****************************************************************************
 *     Fast Enumeration Algorithm
 *  Nemhauser-Ullman using node representation
 *****************************************************************************/
	/* TODO: iniciar enumeração das soluções... 
	 * 	    - iniciar de uma solução vazia (por enquanto);
	 * 	    - criar novo nó apenas no caso de inserção de item.
	 * 	    - utilizar LinkedBucket na enumeração
	 * 	    */
MKPSol *mkp_fast_domsets_enum(MKP *mkp){
	DomSetTree *dstree;
	DomSetNode *dsnode, *dsn_tail, *dsn_new, *dsn_iter;
	MKPSol *mkpsol;
	int i, j, n, m, dominance, promissing;

	n = mkp->n;
	m = mkp->m;

	/* initializing domsets tree */
	mkpsol = mkpsol_new(mkp); /* empty solution */
	dstree = dstree_new(mkpsol);
	mkpsol_free(mkpsol);

	/* ENUMERATING SOLUTIONS */
	/* for each item... */
	for( i = 0 ; i < n ; i++ ){

		/* for each existent dominating set (dsnode)... */
		for( dsnode = dstree->root, dsn_tail = dstree->tail
			; dsnode ? (dsnode->prev /* if has a prev node and... */
					? dsnode->prev != dsn_tail /* ... its not the old tail. */
					:1) :0
			; dsnode = dsnode->next) {

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

	/* extracting solution from best node */
	mkpsol = dsnode_get_mkpsol(dstree, dstree->best);

	/* free */
	dstree_free(dstree);

	return mkpsol;
}

MKPSol *mkp_fast_domsets_enum_lbucket(MKP *mkp, int ndim, int nsub, char type){
	DomSetTree *dstree;
	DomSetNode *dsnode, *dsn_tail, *dsn_new, *dsn_iter;
	LinkedBucket *lbucket;
	MKPSol *mkpsol;
	int i, j, n, m, promissing;
	mkpnum **max_b_lefts, sum;

	n = mkp->n;
	m = mkp->m;

	/* initializing linked buckets */
	max_b_lefts = lbucket_prepare_max_b_left(mkp, ndim, nsub, type);
	lbucket = lbucket_new(max_b_lefts, nsub, ndim);
	for( i = 0 ; i < ndim ; i++ ) /* freeing max_b_lefts */
		free(max_b_lefts[i]);
	free(max_b_lefts);

	/* initializing domsets tree */
	dstree = dstree_new(mkpsol);
	lbucket_insert_dsnode(lbucket, dstree->root);
	mkpsol_free(mkpsol);

	/* ENUMERATING SOLUTIONS */
	/* for each item... */
	for( i = 0 ; i < n ; i++ ){
		/* for each existent dominating set... */
		printf("fixing %d/%d (%d, %f)\n", i+1, n, dstree->n,
			log((float)dstree->n)/log(2));
		fflush(stdout);
		for( dsnode = dstree->root, dsn_tail = dstree->tail
			; dsnode
				?(dsnode->prev /* if has a prev node and... */
					?dsnode->prev != dsn_tail /* ... its not the old tail. */
					:1)
				:0
			; dsnode = dsnode->next)
		{
			/* Creating new child solution (adding item 'i') */
			dsn_new = dsnode_new(dsnode, mkp, i); /* null, if unfeasible */
			promissing = dsn_new ? 1 : 0;

			/* checking dominance */
			if(promissing){
				_hitted = 0;
				_n_comps = 0;
				promissing = !lbucket_exists_dominator(lbucket, dsn_new);
			}

			/* Is it a promissing node? */
			if( promissing ){
				dstree_insert(dstree, dsn_new);
				lbucket_insert_dsnode(lbucket, dsn_new);
			}else if( dsn_new ){
				dsnode_free(dsn_new);
			}
		}
	}

	/* extracing solution from best node */
	mkpsol = dsnode_get_mkpsol(dstree, dstree->best);

	/* printing profile */
	//lbucket_fprintf_profile(stdout, lbucket);

	/* free */
	dstree_free(dstree);
	lbucket_free(lbucket);

	return mkpsol;
}
