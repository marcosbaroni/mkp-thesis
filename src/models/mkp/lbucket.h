/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: december, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#ifndef LBUCKET_H
#define LBUCKET_H 1

#include <stdlib.h>
#include <stdio.h>

#include "mkp.h"
#include "domset.h"

struct DomSetNode;

/* PROFILING VARIABLES */

/*
 * Each bucket records its range of profit (minProft - maxProfit) which can be used
 *    to decide if the dsnode has no dominator (if maxProfit < dsnode->profit).
 *
 * TODO: implementar container final para guardar as soluções. Testar:
 *       - Binary Tree
 *       - List */
typedef struct LinkedBucket{
    int n_dsnodes;				    /* total number of items (solution) it holds */
    unsigned long long _total_hits;			/* number of times the bucket is "hitted" (performance analysis) */
	unsigned long long _total_compares;		/* number of total node comparison (performance analysis) */
	int dim;		    		    /* the dimension it considers */
	mkpnum *max_b_left;			    /* increasing order (last is LLONG_MAX) */
	mkpnum minProfit, maxProfit;	/* min/max profit of solutions on the bucket (for statistics and shortcuts) */
	int nsub;				        /* number of sub buckets */
	union{
		struct LinkedBucket **sub_buckets; /* array of buckets */
		Array **dsnodes;		  /* list of array of dsnodes [leaf bucket case] */
	};
}LinkedBucket;

LinkedBucket *lbucket_new(MKP *mkp, int nsub, int dims, char type);
LinkedBucket *_lbucket_new(mkpnum **max_b_lefts, int nsub, int dims);
mkpnum **lbucket_prepare_max_b_left(MKP *mkp, int ndim, int nsub, char type);
void lbucket_fprintf_profile(FILE *out, LinkedBucket *lbucket, MKP *mkp);
void lbucket_insert_dsnode(LinkedBucket *lbucket, struct DomSetNode *dsnode);
void lbucket_free(LinkedBucket *lbucket);
//MKPSol *mkp_fast_domsets_enum(MKP *mkp);
//MKPSol *mkp_fast_domsets_enum_lbucket(MKP *mkp, int ndim, int nsub, char type);
struct DomSetNode *lbucket_exists_dominator(LinkedBucket *lbucket, struct DomSetNode *dsnode);

#endif
