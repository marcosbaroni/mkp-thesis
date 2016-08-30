#ifndef KP_H
#define KP_H

#include "util.h"
#include "des.h"

/*******************************************************************************
 * A Knapsack Problem instance
 * ps.: itens are sorted on decreasing density (p/w)
 ******************************************************************************/
typedef struct KP{
	int n;           /* n. of itens */
	long long *w;    /* weight of itens*/
	long long *p;    /* profit of itens*/
	long long b;     /* capacity */
	double *density; /* "profit density" of itens */
	int *idxs;       /* list of idx (no order) */
}KP;

KP *kp_new_random(int n, double tightness, long long bound);
KP *kp_read_file(char *filename);
KP *kp_read(FILE *in);
void kp_write(FILE *out, KP *kp);
void kp_free(KP *kp);
void kp_fprintf(FILE *out, KP *kp);
void kp_to_zimpl(FILE *out, KP *kp);
KP *kp_qsort_by_density(KP *kp);

/* SubSet-Sum Solution */
typedef struct KPSol{
	int *x;           /* decision variable [n]    */
	int *sel;         /* selected variables [n]   */
	int nx;           /* number of selected itens */
	long long profit; /* current total profit     */
	long long b_left; /* units left to sum (0, if "complete" solution) */
	long long find_steps;  /* steps needed to find solution (ps.: custom metric) */
	long long proof_steps;  /* steps to prove optimality (ps.: custom metric) */
	KP *kp;
}KPSol;

KPSol *kpsol_read_file(char *filename);
KPSol *kpsol_read(FILE *in);
KPSol *kpsol_new_empty(KP *kp);
int kpsol_get(KPSol *sol, int a);
KPSol *kpsol_set(KPSol *sol, int a, int val);
KPSol *kpsol_add(KPSol *sol, int a);
KPSol *kpsol_rm(KPSol *sol, int a);
KPSol *kpsol_new_random(KP *kp);
KPSol *kpsol_greedy_fill(KPSol *sol);
KPSol *kpsol_new(KP *kp, int *x, long long steps, long long proof_steps);
KPSol *kpsol_copy(KPSol *kpsol);
void kpsol_write(FILE *in, KPSol *kpsol);
void kpsol_free(KPSol *kpsol);
void kpsol_fprint(FILE *out, KPSol *kpsol);

/* Enumerate all KP solutions (backtrack alg) */
KPSol *kp_backtrack(KP *kp, int enumerate, long long upper_bound);
KPSol *kp_dymprog(KP *kp);

/* Discrete Evolution Strategy */
DES_Interface *kp_des_interface();

#endif

