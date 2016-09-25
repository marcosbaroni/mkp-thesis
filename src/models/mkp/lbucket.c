#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "domset.h"
#include "mkp.h"
#include "lbucket.h"
#include "../../utils/util.h"

#define LLONG_MAX 1000000000;

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
 * max_b_lefts[i]: the points on i-th dimension the space is segmented.
 * dims: number of indexed dimensions
 * nsub: number of subbuckets (or arrays)
 */
LinkedBucket *_lbucket_new(
	mkpnum **max_b_lefts, int nsub, int ndims)
{
	/* final container */
	LinkedBucket *lbucket;
	int i;

	lbucket = (LinkedBucket*)malloc(sizeof(LinkedBucket));
	lbucket->n_dsnodes = 0;
	lbucket->_total_hits = 0;
	lbucket->_total_compares = 0;
	lbucket->dim = ndims-1;
	lbucket->minProfit = LLONG_MAX;
	lbucket->maxProfit = 0;
	lbucket->max_b_left = mkpnum_array_copy(NULL, max_b_lefts[ndims-1], nsub);
	lbucket->nsub = nsub;

	/* final bucket, holding a set of solutions (dsnodes) */
	if( !lbucket->dim ){
		lbucket->dsnodes = (Array**)malloc(nsub*sizeof(Array*));
		for( i = 0 ; i < nsub ; i++ )
			lbucket->dsnodes[i] = array_new();
	}else{
		/* creating sub buckets */
		lbucket->sub_buckets = (LinkedBucket**)malloc(nsub*sizeof(LinkedBucket*));
		for( i = 0 ; i < nsub ; i++ )
			lbucket->sub_buckets[i] = _lbucket_new(
				max_b_lefts, nsub, ndims-1);
	}
	return lbucket;
}

LinkedBucket *lbucket_new(MKP *mkp, int nsub, int ndims, char type){
    int i;
    LinkedBucket *lbucket;
    mkpnum **max_b_lefts;

    max_b_lefts = lbucket_prepare_max_b_left(mkp, ndims, nsub, type);
    lbucket = _lbucket_new(max_b_lefts, nsub, ndims);
    for( i = 0 ; i < ndims ; i++ ) /* freeing max_b_lefts */
        free(max_b_lefts[i]);
    free(max_b_lefts);

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

DomSetNode *lbucket_exists_dominator(LinkedBucket *lbucket, DomSetNode *dsnode){
	int i, j, n, nsub, dim, _hitted;
	mkpnum b_left, *max_b_left;
	Array *a;
    unsigned long long _n_comps;

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

    _n_comps = 0;
    _hitted = 0;
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
					return dsnode;
			}
		}
	}
	if(_hitted){
		lbucket->_total_hits++;
		lbucket->_total_compares += _n_comps;
        fflush(stdout);
	}
	return NULL;
}

/*
 * (does not free DomSetNodes)
 */
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
 * Print the configuration and size of the linked bucket
 */
void lbucket_fprintf(FILE *out, LinkedBucket *lbucket){
    int nsub, ndims;
    int i, j;

    nsub = lbucket->nsub;
    ndims = lbucket->dim;
    for( i = 0 ; i < nsub ; i++ ){
    }

    return;
}

/*
 * Prints some performance information about the linekd bucket.
 */
void lbucket_fprintf_profile(FILE *out, LinkedBucket *lbucket, MKP *mkp){
	int i, j, tot, dim, nsub;
    int n, m;
    int idx_zero;
	int idxs[20];
	LinkedBucket *buk;
    unsigned long long g_total_comp;
    Array *array;
    DomSetNode *dsnode;

	/* variables initialization */
    g_total_comp = 0;
	dim = lbucket->dim;
	nsub = lbucket->nsub;
    n = mkp->n;
    m = mkp->m;
	tot = ipow(nsub, dim+1);      // total number of lbuckets
	for( i = 0 ; i <= dim ; i++ ) // setting the indexs in first position
		idxs[i] = 0;

    fprintf(out, "- PROFILING:\n(index);<max b_left>;<tot hits>;<tot compares>\n");
    fprintf(out, " - total buckets: %d\n", tot);
    fprintf(out, " - total dsnodes: %d\n", lbucket->n_dsnodes);

    /* nonrecursive navegation throughout all lbuckets
     *   index [0,0,...,0] is first
     */
    idx_zero = 0;
    do{
        /* printing the index */
        int_array_fprint(out, idxs, dim+1);
        /* locating the current Array<dsnode> */
        buk = lbucket;
        for( i = 0 ; i < dim ; i++ )
            buk = buk->sub_buckets[idxs[i]];
        fprintf(out, ":%d [", array_get_size(buk->dsnodes[idxs[dim]]));
        for( i = 0 ; i < dim+1 ; i++ ){
            mkpnum_fprintf(stdout, lbucket->max_b_left[idxs[i]]);
            printf(" ");
        }
        printf("]\n");
        array = buk->dsnodes[idxs[dim]];
        array_bubble_sort(array, (int (*)(void *, void *))dsnode_cmp);
        for( j = 0 ; j < array_get_size(array) ; j++ ){
            dsnode = (DomSetNode*)array_get(array, j);
            printf("  %d: (", j+1);
            mkpnum_fprintf(stdout, dsnode->profit);
            printf(") ");
            mkpnum_array_write(stdout, dsnode->b_left, m);
        }

        /* update indexs (set next) */
        i = dim;
        idxs[i]++;
        while( idxs[i] == nsub ){
            idxs[i] = 0;
            i--;
            if( i >= 0 )
                idxs[i]++;
        }
    }while( i >= 0 );

	return;
}

