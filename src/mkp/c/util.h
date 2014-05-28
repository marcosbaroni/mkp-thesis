#ifndef UTIL_H
#define UTIL_H 1

#include <stdlib.h>
#include <stdio.h>

void zimpl_print_matrix(FILE *fout, int nlin, int ncol, double **mat);
void zimpl_print_array(FILE *fout, int n, double *array);

#endif

