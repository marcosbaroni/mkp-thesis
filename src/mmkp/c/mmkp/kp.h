#ifndef KP_H
#define KP_H

#include "util.h"

typedef struct KP{
	int n;           /* n. of itens */
	long long *w;    /* weight of itens*/
	long long *p;    /* profit of itens*/
	long long b;     /* capacity */
	double *density; /* "profit density" of itens */
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
	int *x;           /* decision variable    */
	int *sel;         /* selected variables   */
	int nx;           /* number of selected itens */
	long long profit; /* current total profit */
	long long b_left; /* units left to sum (0, if "complete" solution) */
	long long find_steps;  /* steps needed to find solution (ps.: custom metric) */
	long long proof_steps;  /* steps to prove optimality (ps.: custom metric) */
	KP *kp;
}KPSol;

KPSol *kpsol_read_file(char *filename);
KPSol *kpsol_read(FILE *in);
KPSol *kpsol_new_empty(KP *kp);
KPSol *kpsol_new(KP *kp, int *x, long long steps, long long proof_steps);
void kpsol_write(FILE *in, KPSol *kpsol);
void kpsol_free(KPSol *kpsol);
void kpsol_fprint(FILE *out, KPSol *kpsol);

/* Enumerate all KP solutions (backtrack alg) */
Array *kp_backtrack(KP *kp, int enumerate);

#endif

