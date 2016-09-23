#ifndef DOMSET_H 
#define DOMSET_H 1

#include "../../utils/util.h"
#include "mkp.h"

/******************************************************************************
 * Each DomSetNode represents a 'solution' for the MKP.
 * Specifically, it represents the fixation of some variable and value '1'.
 *****************************************************************************/
typedef struct DomSetNode{
	/* NODE INFO */
	int idx;			/* the index of variable which was fixed */
	mkpnum profit;		/* profit of set/solution */
	mkpnum *b_left;		/* weight left */

	/* TREE INFO */
    struct DomSetTree *tree;    /* the domSetTree */
	struct DomSetNode *father;	/* the father its set/solution */
	struct DomSetNode *prev;	/* previous solution (for list navegation) */
	struct DomSetNode *next;	/* next solution (for list navegation) */
}DomSetNode;

int dsnode_dominates(DomSetNode *dsn1, DomSetNode *dsn2);   /* If dsn1 dominates dsn2 */
DomSetNode *dsnode_new(DomSetNode* father, int idx);
void dsnode_fprintf(FILE *fout, DomSetNode *dsnode);
MKPSol *dsnode_get_mkpsol(DomSetNode *dsnode);
void dsnode_free(DomSetNode *dsnode);
int dsnode_cmp(DomSetNode *dsn1, DomSetNode *dsn2);

/******************************************************************************
 * Tree structure, holding all openned nodes on the enumeration procedure.
 *****************************************************************************/
typedef struct DomSetTree{
	/* PROBLEM INSTANCE INFO */
	MKP *mkp;			        /* the problem instance */

	/* STRUCTURE INFO */
	int n;				        /* total number of solution in tree */
	DomSetNode *root;	        /* the iinitial solution node */
	DomSetNode *best;	        /* the current best solution node */
	DomSetNode *tail;	        /* last added node (for append operation) */
    
    /* PERFORMANCE ANALISYS INFO */
    unsigned long long n_comparison;
}DomSetTree;

DomSetTree *dstree_new(MKP *mkp); /* creates a new empty tree */
DomSetTree *dstree_insert(DomSetTree *dstree, DomSetNode *dsnode);
DomSetTree *dstree_remove(DomSetTree *dstree, DomSetNode *dsnode);
DomSetNode *dstree_exists_dominance(DomSetTree *dstree, DomSetNode *dsnode);
void dstree_fprint(FILE *out, DomSetTree *dstree);
/* Dynamic Programming */
DomSetTree *dstree_dynprog(DomSetTree *dstree, int idx);
MKPSol *mkp_dynprog(MKP *mkp, int *idxs);
void dstree_free(DomSetTree *dstree);

#endif

