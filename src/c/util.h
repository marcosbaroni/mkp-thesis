#ifndef PCOPE_UTIL_H
#define PCOPE_UTIL_H 1

#include <stdlib.h>
#include <stdio.h>

int randint(int a, int b);                     /* Random int in range (a,b) */
double randd();                                /* Random double. */
double randd2(double min, double var);         /* Random double between [min, min+var] */
double gauss();                                /* Random gauss number. */
double gauss2(double med, double std_var);     /* Random gauss number with med and std_var. */
int distributed_rand_int(double *dist, int n);

void fprint_scip_int_array(FILE *fout, int *v, int n);
void fprint_scip_double_array(FILE *fout, double *v, int n);
void fprint_scip_int_matrix(FILE *fout, int **v, int n, int m);
void fprint_scip_double_matrix(FILE *fout, double **v, int n, int m);

#endif

