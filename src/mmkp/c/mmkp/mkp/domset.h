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
	int n;                 /* total number of items (solution) it holds */
	int dim;               /* the dimension it considers */
	long long minW;        /* min weight their elements has */
	long long maxW;
	//long long minProfit;
	//long long maxProfit;
	union{
		int n_sub_buckets;     /* number of sub buckets (on the 'next' dimension) */
		int n_dsnodes;         /* number of dsnodes (solution) [leaf bucket case]*/
	};
	union{
		struct LinkedBucket *sub_buckets; /*array of buckets */
		Array *dsnodes;           /* array of dsnodes [leaf bucket case] */
	};
}LinkedBucket;

#endif

