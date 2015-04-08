#ifndef MKP_H
#define MKP_H 1

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

#endif

