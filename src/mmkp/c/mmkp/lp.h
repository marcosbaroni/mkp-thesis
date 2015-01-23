#ifndef LP_H
#define LP_H 1

/*
 * A Linear Programming Problem.
 * Max. cx
 * S.t. Ax <= b
 *       x >= 0
 * */
typedef struct LP{
	int nvars;   /* number of original variables: |c| */
	int ncs;     /* n. of contraints: |b| */
	double *c;   /* c [nvars] */
	double **a;  /* A [ncs x nvars] */
	double *b;   /* b [ncs] (capacities) */
}LP;

double *lp_simplex(LP *lp);
double *lp_scip(LP *lp);
void lp_fprint(FILE *out, LP *lp);
void lp_free(LP *lp);


#endif

