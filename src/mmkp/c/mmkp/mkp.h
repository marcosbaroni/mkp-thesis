#ifndef MKP_H
#define MKP_H 1

#include "util.h"
#include "des.h"
#include "sfl.h"

/*** MKP PROBLEM INSTANCE ***/
typedef struct MKP{
	int n;       /* Number of itens */
	int m;       /* Number of dimensions */
	long long *p;     /* Profit of itens [n] */
	long long **w;    /* Weight of itens [m x n] */
	long long *b;     /* Knapsack capacities [m] */
	int *idxs;        /* item indexs (no order) */
}MKP;

/*** Memory Management ***/
MKP *mkp_alloc(int n, int m);
MKP *mkp_random(int n, int m, double beta);
void mkp_free(MKP *mkp);

/*** read/write functions ***/
MKP *mkp_read_from_filename(char *filename);
MKP *mkp_read_from_file(FILE *fin);
void mkp_write_to_filename(MKP *mkp, char *filename);
void mkp_write_to_file(MKP *mkp, FILE *fout);

/*** Print functions ***/
void mkp_fprint(FILE *fout, MKP *mkp);
void mkp_to_zimpl(FILE *fout, MKP *mkp, double max_opt, double capacity_scale, char linear);


/*** MKP SOLUTION ***/
typedef struct MKPSol{
	int *x;                /* The solution vector [n] */
	long long *b_left;     /* Resource left on the knapsack [m] */
	long long obj;         /* Objective function */
	int feasible;            /* If solution is feasible*/
	MKP *mkp;              /* The problem instance */
}MKPSol;

/*** MKP SOLUTION FUNCTIONS ***/
MKPSol *mkpsol_new(MKP *mkp);                   /* new empty solution */
MKPSol *mkpsol_new_random(MKP *mkp);                /* new random solution */
MKPSol *mkpsol_add_item(MKPSol *mkpsol, int a); /* add item */
MKPSol *mkpsol_rm_item(MKPSol *mkpsol, int a);  /* remove item */
MKPSol *mkpsol_copy(MKPSol *mkpsol);            /* copies a solution */
MKPSol *mkpsol_read_from_filename(char *filename, MKP *mkp);
MKPSol *mkpsol_read_from_file(FILE *fin, MKP *mkp);
//int mkpsol_dominated_by(MKPSol *mkpsol1, MKPSol *mkpsol2); /* if 1st is dominated by 2nd */
int mkpsol_dominates(MKPSol *ms1, MKPSol *ms2);
void mkpsol_fprint(FILE *fout, MKPSol *mkpsol);  /* prints a solution */
void mkpsol_free(MKPSol *mkpsol);               /* new empty solution */

/* MKP TABU SEARCH*/
MKPSol *tabu_mkp(MKPSol *mkpsol, int niter);

/* MKP NEMHAUSER-ULLMAN */
Array *mkp_nemull(MKP *mkp);

DES_Interface *mkp_des_interface(); /* DES interface for MKP */
SFL_Interface *mkp_sfl_interface(); /* Shuffled Frog Leaping interf. for MKP */

#endif

