#ifndef MOKP_H
#define MOKP_H 1

#include "../../utils/util.h"
#include "../mkp/mkp.h"

/*  Multi-objective Knaspack Problem Instance */
typedef struct MOKP{
    int n;              /* number of itens */
    int np;             /* number of objectives */
    double **p;         /* the profits [np x n] */
    double *w;          /* the weight [n] */
    double b;
}MOKP;

MOKP *mokp_alloc(int n, int np);
MOKP *mokp_random(int n, int np, int option);
MOKP *mokp_from_mkp(MKP *mkp);
void mokp_write(FILE *out, MOKP *mokp);
MOKP *mokp_read(FILE *fin);
void mokp_save(char *filename, MOKP *mokp);
MOKP *mokp_open(char *filename);
void mokp_free(MOKP *mokp);

/* MOKP Node (for Dynamic Programming) */
typedef struct MOKPNode{
    struct MOKPTree *tree;
	int idx;	      /* the index of item which was fixed */
    double *profit;   /* [np] */
    double b_left;

    /* list */
	struct MOKPNode *father;	/* the father its set/solution */
	struct MOKPNode *next;	    /* next solution (for list navegation) */
	struct MOKPNode *prev;	    /* previous solution (for list navegation) */
    /* kdtree */
    struct MOKPNode *right;     /* holds those with geater feature */
    struct MOKPNode *left;      /* holds those with lesser feature */
}MOKPNode;
MOKPNode *mokpnode_new(MOKPNode *father, int idx);
void mokpnode_free(MOKPNode *node);
double mokpnode_axis_val(MOKPNode *node, int h);

/* MOKP Tree (for holding MOKP nodes) */
typedef struct MOKPTree{
    MOKP *mokp;
    int n_nodes;
    long long n_comparisons;
    int ndim;

    MOKPNode *root;
    MOKPNode *kdtree_root;
    MOKPNode *tail; /* for list appending */
}MOKPTree;
/* mokptree_new: ndim is the number of dimentions indexed
 *   ndim = 0 to not use kdtree */
MOKPTree *mokptree_new(MOKP *mokp, int ndim);

/* Solving */
int mokp_dynprog(MOKP *mokp, int use_kdtree, int k, int *idxs, long long *n_comps);

#endif

