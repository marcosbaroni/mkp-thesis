#ifndef MOKP_H
#define MOKP_H 1

#include "../../utils/util.h"
#include "../../utils/kdtree.h"
#include "../../utils/list.h"
#include "../../utils/avl.h"

#define SOL_DOMINATES 0
#define SOL_EQUAL     1
#define SOL_DOMINATED 2
#define SOL_NOTHING   3

typedef int mokpnum;
typedef unsigned char mokpval;
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


/* Solution for the MOKP */
typedef struct MOKPSol{
	MOKP *mokp;
	mokpval *x;
	mokpnum *profit;
	mokpnum b_left;
	int rank;
}MOKPSol;

MOKPSol *new_empty(MOKP*);
MOKPSol *mokpsol_new_random(MOKP*);
MOKPSol *mokpsol_copy(MOKPSol*);
MOKPSol *mokpsol_insert_item(MOKPSol*, int);
MOKPSol *mokpsol_rm_item(MOKPSol*, int);
void mokpsol_free(MOKPSol*);
void mokpsol_fprintf(FILE*, MOKPSol*);
void mokpsol_printf(MOKPSol*);
int mokpsol_dom_cmp(MOKPSol*, MOKPSol*);
double mokpsol_spacing(MOKPSol*, MOKPSol*); /* IF same element THEN INFINITY */
KDTree *mokpsol_new_kdtree(int);
MOKPSol *mokpsol_find_dominant_kdtree(MOKPSol*, KDTree*);


typedef struct MOKPSolIndexer{
	int ndim;
	union {
		AVLTree *avl;
		KDTree *kdt;
		List *list;
	}tad;
}MOKPSolIndexer;

MOKPSolIndexer *msi_new(int);
MOKPSolIndexer *msi_insert(MOKPSolIndexer*, MOKPSol*);
int msi_get_n(MOKPSolIndexer*);
void msi_free(MOKPSolIndexer*);
void msi_apply_all(MOKPSolIndexer*, void(*)(void*));
MOKPSol **msi_get_all(MOKPSolIndexer*);
MOKPSol *msi_find_dominant(MOKPSolIndexer*, MOKPSol*);
int msi_set_coverage(MOKPSolIndexer*, MOKPSolIndexer*);
double msi_spacing(MOKPSolIndexer*);

/* MOKP Solution Indexer Iterator */
typedef struct MSIIter{
	MOKPSolIndexer *msi;
	union {
		ListIter *listi;
		AVLIter *avli;
		KDTreeIter *kdti;
	}tad;
}MSIIter;

MSIIter *msiiter_new(MOKPSolIndexer*);
MOKPSol *msiiter_next(MSIIter*);
void msiiter_free(MSIIter*);

/****************************
*    DYNAMIC PROGRAMMING    *
****************************/
/* MOKP Node */
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

