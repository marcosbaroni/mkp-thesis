#ifndef MOKP_H
#define MOKP_H 1

#include "../../utils/util.h"

/*  Multi-objective Knaspack Problem Instance */
typedef struct MOKP{
    int n;              /* number of itens */
    int np;             /* number of objectives */
    double **p;         /* the profits [np x n] */
    double *w;          /* the weight [n] */
    double b;
}MOKP;

MOKP *mokp_alloc(int n, int np);
MOKP *mokp_random(int n, int np);
void mokp_write(FILE *out, MOKP *mokp);
MOKP *mokp_read(FILE *fin);
void mokp_save(char *filename, MOKP *mokp);
MOKP *mokp_open(char *filename);
void mokp_free(MOKP *mokp);


/* MOKP Node (for Dynamic Programming) */
typedef struct MOKPNode{
    MOKP *mokp;
	int idx;	      /* the index of item which was fixed */
    double *profit;   /* [np] */
    double b_left;

	struct MOKPNode *father;	/* the father its set/solution */
	struct MOKPNode *next;	    /* next solution (for list navegation) */
	struct MOKPNode *prev;	    /* previous solution (for list navegation) */
}MOKPNode;

MOKPNode *mokpnode_new(MOKP *mokp, MOKPNode *father, int idx);
void mokpnode_free(MOKPNode *node);
double mokpnode_axis_val(MOKPNode *node, int h);

/* Solving */
void mokp_dynprog(MOKP *mokp, int use_kdtree, int k, int *idxs);

#endif

