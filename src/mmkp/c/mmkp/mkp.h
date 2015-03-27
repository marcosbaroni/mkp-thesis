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
	int n;            /* Number of itens */
	int m;            /* Number of dimensions */
	long long *p;     /* Profit of itens [n] */
	long long **w;    /* Weight of itens [m x n] */
	long long *b;     /* Knapsack capacities [m] */
	int *idxs;        /* item indexs (no order) */
	double *em;       /* efficienct measure */
	double *lp_sol;   /* array with result of lp relaxation */
	double lp_obj;    /* lp-relaxation upperbound */
	struct MKPSol *lp_trunc;
}MKP;

/*** Memory Management ***/
MKP *mkp_alloc(int n, int m);
MKP *mkp_random(int n, int m, double alpha, double beta, long long max_coefs);
void mkp_sort_by_profit(MKP *mkp);
void mkp_free(MKP *mkp);

/*** read/write functions ***/
MKP *mkp_read_from_filename(char *filename);
MKP *mkp_read_from_file(FILE *fin);
void mkp_write_to_filename(MKP *mkp, char *filename);
void mkp_write_to_file(MKP *mkp, FILE *fout);

/*** misc ***/
void mkp_fprint(FILE *fout, MKP *mkp);
void mkp_to_zimpl(FILE *fout, MKP *mkp, double max_opt, double capacity_scale, char linear);
double *mkp_solve_with_scip(MKP *mkp, double maxtime, double capacity_scale, char linear);
void mkp_dual_to_zimpl(FILE *fout, MKP *mkp, char linear);
double *mkp_solve_dual_with_scip(MKP *mkp);
double* mkp_get_lp_sol(MKP *mkp);
double mkp_get_lp_obj(MKP *mkp);
double *mkp_get_em(MKP *mkp);
int mkp_max_items(MKP *mkp);

/* generates a reduced MKP, with a subset of variables.
 *  mkp: original problem
 *  var_vals: values of i-th variables:
 *    - 1: fixed on 1
 *    - 0: fixed on 0
 *    - other: free
 * */
MKP *mkp_reduced(MKP *mkp, int *var_vals);
MKP *mkp_core_problem(MKP *mkp, int core_size, int **vars_fix);
MKP *mkp_select_contraints(MKP *mkp, int *cons, int m2);

/*** Core functions ***/
#define MKP_CORE_SIMPLE 1
#define MKP_CORE_SCALED 2
#define MKP_CORE_ST 3
#define MKP_CORE_FP 4
#define MKP_CORE_DUALS 5
#define MKP_CORE_LP 6
#define MKP_CORE_LP2 7
/* returns an array of variables index, sorted by descending "efficienct measure". 
 *   - mkp: the problem
 *   - type: efficiency measure choosen
 * */
int *mkp_core_val(MKP *mkp, char type);

LP *mkp2lp(MKP *mkp, double capacity_scale); /* MKP to LP relaxation */


/*** MKP SOLUTION ***/
typedef struct MKPSol{
	int *x;                /* The solution vector [n] */
	int nx;
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
MKPSol *mkpsol_solve_with_scip(MKP *mkp, double maxtime, double capacity_scale, char linear);
MKPSol *mkpsol_from_lp(MKP *mkp);               /* truncated from LP */
MKPSol *mkpsol_local_search(MKPSol *mkpsol, int niter);
int mkpsol_get_core_size(MKPSol *mkpsol, int *first_0p, int *last_1p);
MKPSol *mkpsol_from_mkp_core(MKPSol *core_sol, MKP *orig_mkp, int *vars_fix);
int mkpsol_cmp_profit(MKPSol *ms1, MKPSol *ms2); /* cmp of solution profits */
int mkpsol_cmp_weight(MKPSol *ms1, MKPSol *ms2, int *j);
MKPSol *mkpsol_greedy_fill(MKPSol *mkpsol);

/* MKP TABU SEARCH*/

/* MKP NEMHAUSER-ULLMAN */
Array *mkp_nemull(MKP *mkp);

DES_Interface *mkp_des_interface(); /* DES interface for MKP */
SFL_Interface *mkp_sfl_interface(); /* Shuffled Frog Leaping interf. for MKP */

#endif

