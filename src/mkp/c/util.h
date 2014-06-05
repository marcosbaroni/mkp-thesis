#ifndef UTIL_H
#define UTIL_H 1

#include <stdlib.h>
#include <stdio.h>

/* ZIMPL FORMAT */
void zimpl_print_matrix(FILE *fout, int nlin, int ncol, double **mat);
void zimpl_print_array(FILE *fout, int n, double *array);

/* GENERAL PARSING */
int *parse_int_list(char *str, int *n);

#endif

