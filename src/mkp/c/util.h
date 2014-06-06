#ifndef UTIL_H
#define UTIL_H 1

#include <stdlib.h>
#include <stdio.h>

/* ZIMPL FORMAT */
void zimpl_print_matrix(FILE *fout, double **mat, int nlin, int ncol);
void zimpl_print_array(FILE *fout, double *array, int n);

/* GENERAL PARSING */
int *parse_int_list(char *str, int *n);

/* ZIPING */
void gzip(FILE *in, FILE *out);
void gunzip(FILE *in, FILE *out);

#endif

