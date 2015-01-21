#ifndef MKP_H
#define MKP_H 1

#include "util.h"
#include "des.h"
#include "sfl.h"
#include "lp.h"

/*** MKP PROBLEM INSTANCE ***
*   Itens are sorting by decreasing order of profit (for greedy propose).
*/
typedef struct MKP{
	int n;       /* Number of itens */
	int m;       /* Number of dimensions */
	long long *p;     /* Profit of itens [n] */
	long long **w;    /* Weight of itens [m x n] */
	long long *b;     /* Knapsack capacities [m] */
	int *idxs;        /* item indexs (no order) */
	struct MKPSol *lp_trunc;
}MKP;

/*** Memory Management ***/
MKP *mkp_alloc(int n, int m);
MKP *mkp_random(int n, int m, double beta, long long max_coefs);
void mkp_sort_by_profit(MKP *mkp);
void mkp_free(MKP *mkp);

/*** read/write functions ***/
MKP *mkp_read_from_filename(char *filename);
MKP *mkp_read_from_file(FILE *fin);
void mkp_write_to_filename(MKP *mkp, char *filename);
void mkp_write_to_file(MKP *mkp, FILE *fout);

/*** Print functions ***/
void mkp_fprint(FILE *fout, MKP *mkp);
void mkp_to_zimpl(FILE *fout, MKP *mkp, double max_opt, double capacity_scale, char linear);

/*** Core functions ***/
#define MKP_CORE_SIMPLE 1
#define MKP_CORE_SCALED 2
#define MKP_CORE_ST 3
#define MKP_CORE_FP 4
#define MKP_CORE_DUALS 5
#define MKP_CORE_LP 6
double *mkp_core_val(MKP *mkp, char type);

LP *mkp2lp(MKP *mkp); /* MKP to LP relaxation */


/*** MKP SOLUTION ***/
typedef struct MKPSol{
	int *x;                /* The solution vector [n] */
	long long *b_left;     /* Resource left on the knapsack [m] */
	long long obj;         /* Objective function */
	int feasible;          /* If solution is feasible*/
	MKP *mkp;              /* The problem instance */
}MKPSol;

/*** MKP SOLUTION FUNCTIONS ***/
MKPSol *mkpsol_new(MKP *mkp);                   /* new empty solution */
MKPSol *mkpsol_new_random(MKP *mkp);                /* new random solution */
MKPSol *mkpsol_add_item(MKPSol *mkpsol, int a);   /* add item */
MKPSol *mkpsol_rm_item(MKPSol *mkpsol, int a);    /* remove item */
MKPSol *mkpsol_flip_item(MKPSol *mkpsol, int a);  /* remove or add item */
MKPSol *mkpsol_copy(MKPSol *mkpsol);            /* copies a solution */
MKPSol *mkpsol_read_from_filename(char *filename, MKP *mkp);
MKPSol *mkpsol_read_from_file(FILE *fin, MKP *mkp);
//int mkpsol_dominated_by(MKPSol *mkpsol1, MKPSol *mkpsol2); /* if 1st is dominated by 2nd */
int mkpsol_dominates(MKPSol *ms1, MKPSol *ms2);
void mkpsol_fprint(FILE *fout, MKPSol *mkpsol, char ptr_sol); /* prints a solution */
void mkpsol_free(MKPSol *mkpsol);               /* new empty solution */
MKPSol *mkpsol_solve_with_scip(MKP *mkp, double maxtime, char linear);
MKPSol *mkpsol_from_lp(MKP *mkp);               /* truncated from LP */
MKPSol *mkpsol_local_search(MKPSol *mkpsol, int niter);
int mkpsol_get_core_size(MKPSol *mkpsol, int *first_0p, int *last_1p);

/* MKP TABU SEARCH*/

/* MKP NEMHAUSER-ULLMAN */
Array *mkp_nemull(MKP *mkp);

DES_Interface *mkp_des_interface(); /* DES interface for MKP */
SFL_Interface *mkp_sfl_interface(); /* Shuffled Frog Leaping interf. for MKP */

#endif

