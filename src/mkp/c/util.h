#ifndef UTIL_H
#define UTIL_H 1

#include <stdlib.h>
#include <stdio.h>

long *malloc_long_array(int n);
long **malloc_long_matrix(int n, int m);

long *read_long_array(FILE *in, int n);
long **read_long_matrix(FILE *in, int n, int m);

/* Printing */
long *fprint_long_array(FILE *out, long *array, int n);
long **fprint_long_matrix(FILE *out, long **mat, int n, int m);
long **fprint_long_matrix_tranlated(FILE *out, long **mat, int n, int m);

/* Freeing */
void free_long_array(long *array);
void free_long_matrix(long **mat, int n);

/* random NUMBERs */
long lrand(long bound);
double drand();

double *random_normalized_double_array(int n);

/* ZIMPL FORMAT */
void zimpl_print_matrix(FILE *fout, double **mat, int nlin, int ncol);
void zimpl_print_array(FILE *fout, double *array, int n);

/* GENERAL PARSING */
int *parse_int_list(char *str, int *n);

/* Returns the microsecs from */
int getmillis();

/* ZIPING */
void gzip(FILE *in, FILE *out);
void gunzip(FILE *in, FILE *out);

#endif

