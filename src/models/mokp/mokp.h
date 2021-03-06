/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: december, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
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
    mokpnum **p;        /* the profits [np x n] */
    mokpnum *w;         /* the weight [n] */
    mokpnum b;
	int *idxs;          /* int array for random index operations */
}MOKP;

MOKP *mokp_alloc(int n, int np);
MOKP *mokp_random(int n, int np, char option);
//MOKP *mokp_from_mkp(MKP *mkp);
void mokp_write(FILE *out, MOKP *mokp);
void mokp_write_zitzler(FILE *fout, MOKP *mokp);
void mokp_write_bazgan(FILE *fout, MOKP *mokp);
MOKP *mokp_read(FILE *fin);
MOKP *mokp_read_bazgan(FILE *fin);
void mokp_save(char *filename, MOKP *mokp);
MOKP *mokp_open(char *filename);
MOKP *mokp_new_reordered(MOKP *mokp, int *new_idx_order);
MOKP *mokp_reord_by_type(MOKP *mokp, char ordering_type);
int *mokp_get_item_order(MOKP *mokp, char ordering_type);
void mokp_free(MOKP *mokp);
MOKP *mokp_shuffle_idxs(MOKP *mokp);


/* Solution for the MOKP */
typedef struct MOKPSol{
	MOKP *mokp;
	mokpval *x;
	mokpnum *profit;
	mokpnum b_left;
	int rank;
	mokpnum agg_profit;
	struct MOKPSolIndexer *msi;
}MOKPSol;

MOKPSol *mokpsol_new_empty(MOKP*);
MOKPSol *mokpsol_new_random(MOKP*);
MOKPSol *mokpsol_copy(MOKPSol*);
MOKPSol *mokpsol_flip_item(MOKPSol*, int);
MOKPSol *mokpsol_cross(MOKPSol*, MOKPSol*, int);
MOKPSol *mokpsol_insert_item(MOKPSol*, int);
MOKPSol *mokpsol_rm_item(MOKPSol*, int);
void mokpsol_free(MOKPSol*);
void mokpsol_fprintf(FILE*, MOKPSol*);
void mokpsol_printf(MOKPSol*);
void mokpsol_profit_write(MOKPSol* sol);
int mokpsol_dom_cmp(MOKPSol*, MOKPSol*);
double mokpsol_spacing(MOKPSol*, MOKPSol*); /* IF same element THEN INFINITY */
KDTree *mokpsol_new_kdtree(int);
MOKPSol *mokpsol_find_dominant_kdtree(MOKPSol*, KDTree*, int accept_equal);
int mokpsol_e_dominates(MOKPSol*, MOKPSol*);


/* MOKP Solution Indexer */
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
int msi_pareto_update(MOKPSolIndexer *msi, MOKPSol*);
void msi_remove(MOKPSolIndexer*, MOKPSol*);
int msi_get_n(MOKPSolIndexer*);
void msi_free(MOKPSolIndexer*);
void msi_apply_all(MOKPSolIndexer*, void(*)(MOKPSol*));
void msi_apply_all_r(MOKPSolIndexer*, void(*)(MOKPSol*,void*), void*);
MOKPSol **msi_get_all(MOKPSolIndexer*);
MOKPSol *msi_find_dominant(MOKPSolIndexer*, MOKPSol*, int accept_equal);
MOKPSol *msi_find_dominated(MOKPSolIndexer*, MOKPSol*);
int msi_set_coverage(MOKPSolIndexer*, MOKPSolIndexer*);
double msi_spacing(MOKPSolIndexer*);
double msi_hvolume(MOKPSolIndexer*);


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
MOKPSol *msiiter_get(MSIIter*);
MOKPSol *msiiter_forward(MSIIter*);
void msiiter_free(MSIIter*);
void msiiter_remove(MSIIter*);


/* MOKP Solution Archive */
typedef struct Archive{
	int nmax;
	MOKP *mokp;
	MOKPSolIndexer *pareto;
}Archive;
Archive *archive_new(MOKP* mokp, int nmax, int ndim);
int archive_get_n(Archive *arch);
void archive_propose_sol(Archive*, MOKPSol*);
MOKPSolIndexer *archive_free(Archive*);


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

