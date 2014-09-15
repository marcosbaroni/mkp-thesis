#ifndef UTIL_H
#define UTIL_H 1

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

/* LONG ARRAY */
long *malloc_long_array(int n);                      /* mallocs a long array*/
long *read_long_array(FILE *in, long *array, int n);
void write_long_array(FILE *out, long *array, int n);
long *init_long_array(long *array, int n, long x);   /* init long array */
long *copy_long_array(long *dest, long *src, int n); /* copy a long array */
void *fprint_long_array(FILE *out, long *array, int n);
void free_long_array(long *array);
long max_long_array(long *array, int n);         /* max number on array */
long *random_long_array(int n, long *array, long bound);
long *qsort_long_array(long *array, int n);

/* LONG MATRIX*/
long **malloc_long_matrix(int n, int m);
long **read_long_matrix(FILE *in, long **mat, int n, int m);
void write_long_matrix(FILE *out, long **mat, int n, int m);
long **init_long_matrix(long **mat, int n, int m, long x);
long **copy_long_matrix(long **dest, long **src, int n, int m);
void **fprint_long_matrix(FILE *out, long **mat, int n, int m);
void **fprint_long_matrix_tranlated(FILE *out, long **mat, int n, int m);
void free_long_matrix(long **mat, int n);
long max_long_matrix(long **mat, int n, int m);      /* max number on matrix */
long max_long_matrix_col(long **mat, int n, int m, int col); /* max elem of column */
long max_long_matrix_lin(long **mat, int n, int m, int lin); /* max elem of line */

/* RANDOM NUMBERs */
long lrand(long bound);
double drand();
//double normal_dist();
double *random_normalized_double_array(int n);

/* AVL TREE */
typedef (avl_cmp*)(int)(void* a, void *b);     /* compares teo objs*/
typedef (avl_fprt*)(void)(FILE *out, void* a); /* fprints an obj */

typedef struct AVLNode{
	void *info;
	AVLNode *right, *left;
	AVLNode *father;
	unsigned char balance;
}AVLNode;

AVLNode *avl_node_new(void *a);

typedef struct AVLTree{
	AVLNode *root;
	avl_cmp *cmp_f;
	avl_fprt *fprt_f;
	int n;
	int height;
}AVLTree;

AVLTree *avl_new(cmp* cmp_f);
AVLTree *avl_set_prt(AVLTree *avl, avl_fprt* fprt_f);
AVLTree *avl_insert(AVLTree *avl, void *a);

/* ZIMPL FORMAT */
void zimpl_print_double_matrix(FILE *fout, double **mat, int nlin, int ncol);
void zimpl_print_double_array(FILE *fout, double *array, int n);
void zimpl_print_long_matrix(FILE *fout, long **mat, int nlin, int ncol);
void zimpl_print_long_array(FILE *fout, long *array, int n);

/* GENERAL PARSING */
int *parse_int_list(char *str, int *n);

/* TIMING */
int getmillis();

/* INPUT-OUTPUT */
void gzip(FILE *in, FILE *out);
void gunzip(FILE *in, FILE *out);

/* MISCELANIOUS */
void unimplemented();
void error(const char* format, ...);
void assert_faccess(char *filename, int mode);

#endif

