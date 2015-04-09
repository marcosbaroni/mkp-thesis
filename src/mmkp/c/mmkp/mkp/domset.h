#ifndef DOMSET_H 
#define DOMSET_H 1

#include "../util.h"
#include "mkp.h"

typedef struct DomSetNode{
	int idx;                   /* the index which was fixed in '1' */
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
	int n_sub_buckets;     /* number of sub buckets (on the 'next' dimension) */
	struct LinkedBucket *sub_buckets;
}LinkedBucket;

#endif

