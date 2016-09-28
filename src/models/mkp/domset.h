#ifndef DOMSET_H 
#define DOMSET_H 1

#include "../../utils/util.h"
#include "mkp.h"
#include "lbucket.h"

/******************************************************************************
 * Each DomSetNode represents a 'solution' for the MKP.
 * Specifically, it represents the fixation of some variable and value '1'.
 *****************************************************************************/
typedef struct DomSetNode{
	/* NODE INFO */
	int idx;			        /* the index of variable which was fixed */
	mkpnum profit;		        /* profit of set/solution */
	mkpnum *b_left;		        /* weight left */

	/* TREE INFO */
    struct DomSetTree *tree;    /* the domSetTree */
	struct DomSetNode *father;	/* the father its set/solution */
	struct DomSetNode *next;	/* next solution (for list navegation) */
	struct DomSetNode *prev;	/* previous solution (for list navegation) */

    /* KD TREE INFO */
	struct DomSetNode *up;      /* the up node in kdtree */
	struct DomSetNode *right;	/* the right node in kdtree */
	struct DomSetNode *left;	/* the left node in kdtree */
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

    /* AUXILIARY DATA STRUCTER INFO*/
    struct LinkedBucket *lbucket;
    struct DomSetKDTree *kdtree;
}DomSetTree;

DomSetTree *dstree_new(MKP *mkp);
void dstree_set_lbucket(DomSetTree *dstree, LinkedBucket *lbucket);
void dstree_set_kdtree(DomSetTree *dstree, struct DomSetKDTree *kdtree);
DomSetTree *dstree_insert(DomSetTree *dstree, DomSetNode *dsnode);
DomSetTree *dstree_remove(DomSetTree *dstree, DomSetNode *dsnode);
DomSetNode *dstree_exists_dominance(DomSetTree *dstree, DomSetNode *dsnode);
void dstree_fprint(FILE *out, DomSetTree *dstree);
/* Dynamic Programming */
DomSetTree *dstree_dynprog(DomSetTree *dstree, int idx);
MKPSol *mkp_dynprog(MKP *mkp, int *idxs);
void dstree_free(DomSetTree *dstree);
void dstree_dp_iter(DomSetTree *dstree, int idx);
/* comparison functions */
int dsnode_cmp_by_profit(DomSetNode *dsn1, DomSetNode *dsn2);
int dsnode_cmp_by_b_left0(DomSetNode *dsn1, DomSetNode *dsn2);
int dsnode_cmp_by_b_left1(DomSetNode *dsn1, DomSetNode *dsn2);
int dsnode_cmp_by_b_left2(DomSetNode *dsn1, DomSetNode *dsn2);

/******************************************************************************
 * KDTree structure, holding all openned nodes on the enumeration procedure.
 *****************************************************************************/
typedef struct DomSetKDTree{
	/* PROBLEM INSTANCE INFO */
    DomSetTree *dstree;         /* the plain DomSet Tree */

	/* STRUCTURE INFO */
    int ndim;                   /* number of dimension to be considered (first is profit) */
	DomSetNode *root;	        /* the initial solution node */
}DomSetKDTree;

DomSetKDTree *dskdtree_new(int ndim);
DomSetKDTree *dskdtree_insert(DomSetKDTree *dskdtree, DomSetNode *dsnode);
DomSetKDTree *dskdtree_remove(DomSetKDTree *dskdtree, DomSetNode *dsnode);
DomSetNode *dskdtree_find_dominator(DomSetKDTree *dskdtree, DomSetNode *dsnode);
DomSetKDTree *dskdtree_balance(DomSetKDTree *dskdtree);
void dskdtree_fprintf_balance_profile(FILE *fout, DomSetKDTree *kdtree);

#endif

