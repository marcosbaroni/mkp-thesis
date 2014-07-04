#ifndef UTIL_H
#define UTIL_H 1

#include <stdlib.h>
#include <stdio.h>

#define NUMBER int

int *read_int_array(FILE *in, int n);
int **read_int_matrix(FILE *in, int n, int m);

/* Printing */
int *fprint_int_array(FILE *out, int *array, int n);
int **fprint_int_matrix(FILE *out, int **mat, int n, int m);

/* Freeing */
void free_int_array(int *array);
void free_int_matrix(int **mat, int n);

/* random NUMBERs */
double drand();
double irand();

/* ZIMPL FORMAT */
void zimpl_print_matrix(FILE *fout, double **mat, int nlin, int ncol);
void zimpl_print_array(FILE *fout, double *array, int n);

/* GENERAL PARSING */
int *parse_int_list(char *str, int *n);

/* ZIPING */
void gzip(FILE *in, FILE *out);
void gunzip(FILE *in, FILE *out);

#endif

