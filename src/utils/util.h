#ifndef UTIL_H
#define UTIL_H 1

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_COEFFICIENT 1000
#define feq(x, y) (fabs(x-y) < 0.0000001)

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

extern int verbose;

void SWAP_LONG(long *array, int a, int b);
void SWAP_LONG_LONG(long long *array, int a, int b);

void findent(FILE *fout, int times, char c);

/* MEGA POWER QSORT */
typedef void (*mp_swap_f)(void*, int, int);
typedef int (*mp_cmp_r_f)(void*, int, int, void*);
typedef int (*mp_cmp_f)(void*, int, int);
void mp_qsort_r(void *collection, int n, mp_cmp_r_f, mp_swap_f, void *arg, char reverse);
void mp_qsort(void *collection, int n, mp_cmp_f, mp_swap_f, char reverse);
void pointer_swap(void **ps, int a, int b);
void void_sort(void **array, int n, int(*f)(void*, void*));

/* unsigned char */
typedef unsigned char uchar;

/* INT ARRAY */
typedef void(*array_apply_f)(void*);
int *int_array_malloc(int n);                    /* mallocs a int array*/
int *int_array_read(FILE *in, int *array, int n);
void int_array_write(FILE *out, int *array, int n);
int *int_array_init(int *array, int n, int x);   /* init int array */
int *int_array_copy(int *dest, int *src, int n); /* copy a int array */
void int_array_fprint(FILE *out, int *array, int n);
void int_array_free(int *array);
int int_array_max(int *array, int n);            /* max number on array */
int *int_array_random(int n, int *array, int bound);
int int_array_is_sorted(int *array, int n);
int *int_array_qsort(int *array, int n);
int *int_array_shuffle(int *array, int n);
int int_array_sum(int *array, int n);

/* LONG ARRAY */
long *long_array_malloc(int n);                      /* mallocs a long array*/
long *long_array_read(FILE *in, long *array, int n);
void long_array_write(FILE *out, long *array, int n);
long *long_array_init(long *array, int n, long x);   /* init long array */
long *long_array_copy(long *dest, long *src, int n); /* copy a long array */
void long_array_write(FILE *out, long *array, int n);
void long_array_fprint(FILE *out, long *array, int n);
void long_array_free(long *array);
long long_array_max(long *array, int n);         /* max number on array */
long *long_array_random(int n, long *array, long bound);
int long_array_is_sorted(long *array, int n);
long *long_array_qsort(long *array, int n);

/* LONG LONG ARRAY */
long long *long_long_array_malloc(int n);                      /* mallocs a long array*/
long long *long_long_array_read(FILE *in, long long *array, int n);
void long_long_array_write(FILE *out, long long *array, int n);
long long *long_long_array_init(long long *array, int n, long long x);   /* init long array */
long long *long_long_array_copy(long long *dest, long long *src, int n); /* copy a long array */
void long_long_array_write(FILE *out, long long *array, int n);
void long_long_array_fprint(FILE *out, long long *array, int n);
void long_long_array_zimpl_print(FILE *fout, long long *array, int n);
void long_long_array_free(long long *array);
long long long_long_array_max(long long *array, int n);         /* max number on array */
long long *long_long_array_random(int n, long long *array, long long bound);
int long_long_array_is_sorted(long long *array, int n);
long long *long_long_array_qsort(long long *array, int n);
int *long_long_array_count_digits(long long *array, int n);
long long long_long_array_sum(long long *array, int n);

/* INT MATRIX */
int *int_matrix_max_cols(int **mat, int n, int m); /* max elems of EACH COLUMN */

/* LONG MATRIX*/
long **long_matrix_malloc(int n, int m);
long **long_matrix_read(FILE *in, long **mat, int n, int m);
void long_matrix_write(FILE *out, long **mat, int n, int m);
long **long_matrix_init(long **mat, int n, int m, long x);
long **long_matrix_copy(long **dest, long **src, int n, int m);
void long_matrix_fprint(FILE *out, long **mat, int n, int m);
void long_matrix_fprint_tranlated(FILE *out, long **mat, int n, int m);
void long_matrix_free(long **mat, int n);
long long_matrix_max(long **mat, int n, int m);      /* max number on matrix */
long long_matrix_max_col(long **mat, int n, int m, int col); /* max elem of given column */
long long_matrix_max_lin(long **mat, int n, int m, int lin); /* max elem of given line */

/* LONG LONG MATRIX */
long long **long_long_matrix_malloc(int n, int m);
long long **long_long_matrix_read(FILE *in, long long **mat, int n, int m);
void long_long_matrix_write(FILE *out, long long **mat, int n, int m);
long long **long_long_matrix_init(long long **mat, int n, int m, long long x);
long long **long_long_matrix_copy(long long **dest, long long **src, int n, int m);
void long_long_matrix_fprint(FILE *out, long long **mat, int n, int m);
void long_long_matrix_fprint_tranlated(FILE *out, long long **mat, int n, int m);
void long_long_matrix_free(long long **mat, int n);
long long long_long_matrix_max(long long **mat, int n, int m);      /* max number on matrix */
long long long_long_matrix_max_col(long long **mat, int n, int m, int col); /* max elem of given column */
long long long_long_matrix_max_lin(long long **mat, int n, int m, int lin); /* max elem of given line */

/* DOUBLE ARRAY */
double *double_array_alloc(int n);
double *double_array_copy(double *array, int n);
double double_array_sum(double *array, int n);
double *double_array_init(double *array, int n, double x);
void double_array_fprint(FILE *out, double *a, int n);
double *double_array_from_scip(double *array, FILE *in);    /*reads a double array from scip solution output format */
double *double_array_read(FILE *fin, double *p, int n);
void double_array_write(FILE *out, double *p, int n);
int *double_index_sort(double *array, int n); /* returns the ordering (desc) of the given array */

/* DOUBLE MATRIX */
double **double_matrix_alloc(int n, int m);
double **double_matrix_init(double **mat, int n, int m, double x);

/* ABSTRACT ARRAY */
typedef struct Array{
	void **a;
	int n;
	int nmax;
}Array;

Array *array_new();
int array_get_size(Array *array);
void *array_get_array(Array *array);
void *array_get(Array *array, int i);
void array_apply(Array *array, void(apl_f)(void*));
void array_bubble_sort(Array *array, int(*compar)(void *, void *));
void array_sort(Array *array, int(*compar)(void *, void *));
void array_sort_r(Array *array, int(*compar)(void *obj1, void *obj2, void *cmp_arg), void *arg);
Array *array_insert(Array *array, void *elem);
Array *array_remove(Array *array, int a);
Array *array_empty(Array *array);
void array_free(Array *array);

/* RANDOM NUMBERS */
int triang_raffle(int bound);
long lrand(long bound);
long long llrand(long long bound);
double drand();                          /* Returns a random double in [0,1[. */
//double normal_dist();
double *random_normalized_double_array(int n);

/* ZIMPL FORMAT */
void double_matrix_zimpl_print(FILE *fout, double **mat, int nlin, int ncol);
void double_array_zimpl_print(FILE *fout, double *array, int n);
void long_matrix_zimpl_print(FILE *fout, long **mat, int nlin, int ncol);
void long_array_zimpl_print(FILE *fout, long *array, int n);
void long_long_matrix_zimpl_print(FILE *fout, long long **mat, int nlin, int ncol);

/* GENERAL PARSING */
int *parse_int_list(char *str, int *n);

/* TIMING */
int getmillis();

/* INPUT-OUTPUT */
void gzip(FILE *in, FILE *out);
void gunzip(FILE *in, FILE *out);

/* MISCELANIOUS */
int debug_lvl;
void setdebug_lvl(char lvl);
void unimplemented();
void error(const char* format, ...);
void assert_faccess(char *filename, int mode);
void myassert(int express);
void debug(char *msg);
double sigmoid(double x);             /* logistic sigmoid */
int ipow(int base, int exp);

#endif

