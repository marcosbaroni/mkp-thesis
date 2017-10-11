#ifndef MOKP_H
#define MOKP_H 1

#include "../../utils/util.h"

typedef int mokpnum;
int mokpnum_fscanf(FILE *in, mokpnum *a);
void mokpnum_fprintf(FILE *out, mokpnum x);
void mokpnum_printf(mokpnum x);
void mokpnum_array_write(FILE *out, mokpnum *array, int n);

/*  Multi-objective Knaspack Problem Instance */
typedef struct MOKP{
    int n;              /* number of itens */
    int np;             /* number of objectives */
    mokpnum **p;         /* the profits [np x n] */
    mokpnum *w;          /* the weight [n] */
    mokpnum b;
}MOKP;

MOKP *mokp_alloc(int n, int np);
MOKP *mokp_random(int n, int np, char option);
//MOKP *mokp_from_mkp(MKP *mkp);
void mokp_write(FILE *out, MOKP *mokp);
MOKP *mokp_read(FILE *fin);
MOKP *mokp_read_bazgan_format(FILE *fin);
void mokp_save(char *filename, MOKP *mokp);
MOKP *mokp_open(char *filename);
MOKP *mokp_new_reordered(MOKP *mokp, int *new_idx_order);
MOKP *mokp_reord_by_type(MOKP *mokp, char ordering_type);
int *mokp_get_item_order(MOKP *mokp, char ordering_type);
void mokp_free(MOKP *mokp);

/* MOKP Node (for Dynamic Programming) */
typedef struct MOKPNode{
    struct MOKPTree *tree;
	int idx;	      /* the index of item which was fixed */
    mokpnum *profit;   /* [np] */
    mokpnum b_left;
    struct MOKPNode *father;
    struct MOKPNode *next, *prev;
    /* Algorithm Auxiliar structure */
    unsigned long long *sol; /* Solution bits representation. */
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
    MOKPNode *tail; /* for list appending */
}MOKPTree;
/* mokptree_new: ndim is the number of dimentions indexed
 *   ndim = 0 to not use kdtree */
MOKPTree *mokptree_new(MOKP *mokp, int ndim);

/* Solving */
int mokp_dynprog(MOKP *mokp, int use_kdtree, int k, int *idxs, long long *n_comps);

#endif

