#ifndef DOMSET_H 
#define DOMSET_H 1

#include "../util.h"
#include "mkp.h"

/******************************************************************************
 * Each DomSetNode represents a 'solution' for the MKP.
 * Specifically, it represents the fixation of some variable and value '1'.
 *****************************************************************************/
typedef struct DomSetNode{
	int idx;                   /* the index og variable which was fixed '1' */
	//int nx;                    /* number of itens in whole solution */
	int m;
	long long profit;          /* profit of set/solution */
	long long *b_left;         /* weight left */
	struct DomSetNode *father; /* the father its set/solution */
	struct DomSetNode *prev;   /* previous solution (for list navegation) */
	struct DomSetNode *next;   /* next solution (for list navegation) */
}DomSetNode;

typedef struct DomSetTree{
	MKP *mkp;
	//MKPSol *mkpsol; /* the initial solution */
	int n;
	DomSetNode *root;
	DomSetNode *best;
	DomSetNode *tail;
}DomSetTree;

DomSetTree *dstree_new(MKP *mkp);


/* TODO: implementar container final para guardar as soluções. Testar:
 *       - Binary Tree
 *       - List
 * (STOPPED HERE) */
typedef struct LinkedBucket{
	int n_dsnodes;   /* total number of items (solution) it holds */
	int dim;         /* the dimension it considers */
	long long *max_b_left;  /* increasing order (last is LLONG_MAX) */
	long long minProfit, maxProfit;
	int nsub;               /* number of sub buckets */
	union{
		struct LinkedBucket **sub_buckets; /* array of buckets */
		Array **dsnodes;           /* list of array of dsnodes [leaf bucket case] */
	};
}LinkedBucket;

void lbucket_insert_dsnode(LinkedBucket *lbucket, DomSetNode *dsnode);
LinkedBucket *lbucket_new(long long **max_b_lefts, int nsub, int dims);
void lbucket_free(LinkedBucket *lbucket);

#endif

