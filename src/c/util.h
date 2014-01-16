#ifndef PCOPE_UTIL_H
#define PCOPE_UTIL_H 1

#include <stdlib.h>
#include <stdio.h>

double randd();                            /* Random double. */
double randd2(double min, double var);     /* Random double between [min, min+var] */
int distributed_rand_int(double *dist, int n);

void fprint_scip_int_array(FILE *fout, int *v, int n);
void fprint_scip_double_array(FILE *fout, double *v, int n);
void fprint_scip_int_matrix(FILE *fout, int **v, int n, int m);
void fprint_scip_double_matrix(FILE *fout, double **v, int n, int m);

#endif

