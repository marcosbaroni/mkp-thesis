#ifndef KP_H
#define KP_H

#include "util.h"

typedef struct KP{
	int n;
	long long *w;
	long long *p;
	long long b;
}KP;

KP *kp_read_file(char *filename);
KP *kp_read(FILE *in);
void kp_write(FILE *out, KP *kp);
void kp_free(KP *kp);
void kp_fprintf(FILE *out, KP *kp);
void kp_to_zimpl(FILE *out, KP *kp);

/* SubSet-Sum Solution */
typedef struct KPSol{
	int *x;           /* decision variable    */
	int *sel;         /* selected variables   */
	int nx;           /* number of selected itens */
	long long p;      /* current total profit */
	long long b_left; /* units left to sum (0, if "complete" solution) */
	KP *kp;
}KP;

KPSol *ssumsol_read_file(char *filename);
KPSol *ssumsol_read(FILE *in);
KPSol *ssumsol_new_empty(KP *kp);
KPSol *ssumsol_new(SSum *ssum, int *x);
void ssumsol_write(FILE *in, KPSol *kpsol);
void ssumsol_free(KPSol *kpsol);
void ssumsol_fprint(FILE *out, KPSol *kpsol);

/* Enumerate all KP solutions (backtrack alg) */
Array *kp_backtrack(KP *kp, int enumerate);
#endif

