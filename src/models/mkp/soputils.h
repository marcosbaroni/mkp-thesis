#ifndef SOPUTILS_H
#define SOPUTILS_H 1

#ifdef __cplusplus
extern "C" {
#endif

double mkp_get_lp_ub(double *p, double **w, double *b, int n, int m, int *fixing); /* LP upper bound*/
double *mkp_get_dual_real(double *p, double **w, double *b, int n, int m);

#ifdef __cplusplus
}
#endif

#endif
