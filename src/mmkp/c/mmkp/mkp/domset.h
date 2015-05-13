#ifndef DOMSET_H 
#define DOMSET_H 1

#include "../util.h"
#include "mkp.h"


/******************************************************************************
 * Each DomSetNode represents a 'solution' for the MKP.
 * Specifically, it represents the fixation of some variable and value '1'.
 *****************************************************************************/
typedef struct DomSetNode{
	/* NODE INFO */
	int idx;					/* the index og variable which was fixed '1' */
	//int nx;					/* number of itens in whole solution */
	int m;						/* dimension of problem */
	mkpnum profit;				/* profit of set/solution */
	mkpnum *b_left;				/* weight left */

	/* SURR. RELAXATION INFO */
	mkpnum relax_profit;		/* current upper bound of node */
	int last_idx;				/* last item added on relaxed solution */
	double partial;				/* the fraction of last added item */
	//double relax_card;		/* cardinality of relaxed solution */

	/* TREE INFO */
	struct DomSetNode *father;	/* the father its set/solution */
	struct DomSetNode *prev;	/* previous solution (for list navegation) */
	struct DomSetNode *next;	/* next solution (for list navegation) */
}DomSetNode;


/******************************************************************************
 * Tree structure, holding all openned nodes on the enumeration procedure.
 *****************************************************************************/
typedef struct DomSetTree{
	/* PROBLEM INSTANCE INFO */
	MKP *mkp;
	mkpnum *relax_w;			/* weight of itens on surrogate relax */
	mkpnum *relax_p;			/* profit of itens */

	/* EXECUTION INFO */
	mkpnum upper_profit;		/* current upper bound of problem */

	/* STRUCTURE INFO */
	int n;						/* total number of solution in tree */
	DomSetNode *root;			/* the iinitial solution node */
	DomSetNode *best;			/* the current best solution node */
	DomSetNode *tail;			/* last added node (for append operation) */
}DomSetTree;

DomSetTree *dstree_new(MKP *mkp);	/* create a new empty tree */


/* PROFILING VARIABLES */
int _hitted;	/* total times which a new node had to be compared with existant solution */
int _n_comps;	/* total number of comparison between solutions */


/* TODO: implementar container final para guardar as soluções. Testar:
 *       - Binary Tree
 *       - List */
typedef struct LinkedBucket{
	int n_dsnodes;				/* total number of items (solution) it holds */
	int _total_hits;			/* performance analysis */
	int _total_compares;		/* performance analysis */
	int dim;					/* the dimension it considers */
	mkpnum *max_b_left;			/* increasing order (last is LLONG_MAX) */
	mkpnum minProfit, maxProfit;/* min/max profit of solutions on the bucket */
	int nsub;					/* number of sub buckets */
	union{
		struct LinkedBucket **sub_buckets; /* array of buckets */
		Array **dsnodes;		/* list of array of dsnodes [leaf bucket case] */
	};
}LinkedBucket;

void lbucket_insert_dsnode(LinkedBucket *lbucket, DomSetNode *dsnode);
LinkedBucket *lbucket_new(mkpnum **max_b_lefts, int nsub, int dims);
void lbucket_free(LinkedBucket *lbucket);
MKPSol *mkp_fast_domsets_enum(MKP *mkp);
MKPSol *mkp_fast_domsets_enum_lbucket(MKP *mkp, int ndim, int nsub, char type);

#endif

