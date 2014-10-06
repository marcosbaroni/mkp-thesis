#ifndef UTIL_H
#define UTIL_H 1

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_COEFFICIENT 1000

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

inline void SWAP_LONG(long *array, int a, int b);

/* INT ARRAY */
int *int_array_malloc(int n);                      /* mallocs a int array*/
int *int_array_read(FILE *in, int *array, int n);
void int_array_write(FILE *out, int *array, int n);
int *int_array_init(int *array, int n, int x);   /* init int array */
int *int_array_copy(int *dest, int *src, int n); /* copy a int array */
void *int_array_fprint(FILE *out, int *array, int n);
void int_array_free(int *array);
int int_array_max(int *array, int n);         /* max number on array */
int *int_array_random(int n, int *array, int bound);
int int_array_is_sorted(int *array, int n);
int *int_array_qsort(int *array, int n);

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

/* LONG MATRIX*/
long **long_matrix_malloc(int n, int m);
long **long_matrix_read(FILE *in, long **mat, int n, int m);
void long_matrix_write(FILE *out, long **mat, int n, int m);
long **long_matrix_init(long **mat, int n, int m, long x);
long **long_matrix_copy(long **dest, long **src, int n, int m);
void **long_matrix_fprint(FILE *out, long **mat, int n, int m);
void **long_matrix_fprint_tranlated(FILE *out, long **mat, int n, int m);
void long_matrix_free(long **mat, int n);
long long_matrix_max(long **mat, int n, int m);      /* max number on matrix */
long long_matrix_max_col(long **mat, int n, int m, int col); /* max elem of column */
long long_matrix_max_lin(long **mat, int n, int m, int lin); /* max elem of line */

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
void array_applay(Array *a, void(apl_f)(void*));
Array *array_insert(Array *a, void *elem);
void array_free(Array *a);

/* RANDOM NUMBERs */
long lrand(long bound);
double drand();
//double normal_dist();
double *random_normalized_double_array(int n);

/* AVL TREE */
typedef int(*avl_cmp)(void*, void*);     /* compares teo objs*/
typedef void(*avl_fprt)(FILE*, void*) ; /* fprints an obj */

typedef struct AVLNode{
	void *info;
	struct AVLNode *right, *left;
	struct AVLNode *father;
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

AVLTree *avl_new(avl_cmp* cmp_f);
AVLTree *avl_set_prt(AVLTree *avl, avl_fprt* fprt_f);
AVLTree *avl_insert(AVLTree *avl, void *a);

/* ZIMPL FORMAT */
void zimpl_print_double_matrix(FILE *fout, double **mat, int nlin, int ncol);
void zimpl_print_double_array(FILE *fout, double *array, int n);
void long_matrix_zimpl_print(FILE *fout, long **mat, int nlin, int ncol);
void long_array_zimpl_print(FILE *fout, long *array, int n);

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

