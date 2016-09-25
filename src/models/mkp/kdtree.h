#ifndef DOMSET_H 
#define DOMSET_H 1

#include "mkp.h"

typedef struct KDTree{
	/* NODE INFO */
	int idx;			/* the index of variable which was fixed */
	mkpnum profit;		/* profit of set/solution */
	mkpnum *b_left;		/* weight left */

	/* TREE INFO */
    struct DomSetTree *tree;    /* the domSetTree */
	struct DomSetNode *father;	/* the father its set/solution */
	struct DomSetNode *prev;	/* previous solution (for list navegation) */
	struct DomSetNode *next;	/* next solution (for list navegation) */
}KDTree;

#endif

