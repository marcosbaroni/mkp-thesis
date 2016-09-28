#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "util.h"

#define ISNUM(c) ( (c > 47) && (c < 58) )

int verbose = 0;

void findent(FILE *fout, int times, char c){
	while( times-- )
		fprintf(fout, "%c", c);
	return;
}

inline void SWAP_LONG(long *array, int a, int b){
	long aux;
	aux = array[a];
	array[a] = array[b];
	array[b] = aux;
	return;
}

inline void SWAP_LONG_LONG(long long *array, int a, int b){
	long long aux;
	aux = array[a];
	array[a] = array[b];
	array[b] = aux;
	return;
}

/******************************************************************************
*         RANDOM NUMBERS
******************************************************************************/

/* raffle a number, using triangular probability between 0 and "bound" */
/*
* p_n = 2*(n-j+1)/n*(n+1)
* 
* 1(6) 0OOOOOO
* 2(5) 00OOOOO
* 3(4) 000OOOO
* 4(3) 0000OOO    A = (4,5) = (5,4)
* 5(2) 00000OO
* 6(1) 000000O
* 
* (n+1)n/2
*/
int triang_raffle(int bound){
	int lin, col;

	lin = lrand(bound);
	col = lrand(bound+1);
	if( lin <= col ) return (bound-lin);
	return lin;
}

/* 
 * Returns a random long integer non-negative from 0 to <bound>.
 */
long lrand(long bound){
	return labs(((rand() << 16) + rand()) % (bound+1));
}

/* 
 * Returns a random long long integer non-negative from 0 to <bound>.
 */
long long llrand(long long bound){
	return llabs(
		((rand() << 16) |
		 ((long long)rand() << 32) |
		 ((long long)rand() << 48) |
		  rand()) % (bound))+1;
}

/* 
 * Returns a random double in [0,1[.
 */
double drand(){
	return rand()/((double)RAND_MAX);
}

/* 
 * Returns a random double taken from a normal distribution with mean 0 and
 * standard deviation 1.
 */
double normal_dist(){
	fprintf(stderr, "function \"normal_dist\" not implemented yet.\n");
	return 0.0;
}

int *int_array_malloc(int n){
	int *array;
	array = (int*)malloc(n*sizeof(int));

	return array;
}

int *int_array_read(FILE *in, int *array, int n){
	int i;
	array = int_array_malloc(n);

	for( i = 0 ; i < n ; i++ )
		fscanf(in, "%d", &(array[i]));

	return array;
}

void int_array_write(FILE *out, int *array, int n){
	int i;

	if( !array )
		array = int_array_malloc(n);

	for( i = 0 ; i < n ; i++ )
		fscanf(out, "%d", &(array[i]));

	return;
}

int *int_array_init(int *array, int n, int x){
	int i;
	if(array == NULL)
		array = int_array_malloc(n);
	for( i = 0 ; i < n ; i++ )
		array[i] = x;

	return array;
}

int *int_array_copy(int *dest, int *src, int n){
	int i;
	if(!dest) dest = (int*)int_array_malloc(n*sizeof(int));
	for( i = 0 ; i < n ; i++ )
		dest[i] = src[i];

	return dest;
}

void int_array_fprint(FILE *out, int *array, int n){
	int i;
	fprintf(out, "[");
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(out, "%d, ", array[i]);
	if(n) fprintf(out, "%d", array[n-1]);
	fprintf(out, "]");

	return;
}

void int_array_free(int *array){
	free(array);
	return;
}

int int_array_max(int *array, int n){
	int i, max;
	max = -999999999;
	for( i = 0 ; i < n ; i++ )
		if( array[i] > max )
			max = array[i];

	return max;
}

int *int_array_random(int n, int *array, int bound){
	int i;
	if(!array)
		array = int_array_malloc(n);
	for( i = 0 ; i < n ; i++ )
		array[i] = lrand(bound);

	return array;
}

int int_array_is_sorted(int *array, int n){
	int i;
	for( i = 1 ; i < n ; i++ )
		if( array[i-1] > array[i] )
			return 0;
	return 1;
}

int *int_array_qsort(int *array, int n){
	unimplemented();
	return array;
}

int *int_array_shuffle(int *array, int n){
	int i, idx, aux;
	for( i = 0 ; i < n ; i++ ){
		idx = lrand(n-i-1);
		aux = array[idx];
		array[idx] = array[i];
		array[i] = aux;
	}

	return array;
}

int int_array_sum(int *array, int n){
	int i, sum;
	sum = 0;
	for( i = 0 ; i < n ; i++ )
		sum += array[i];
	return sum;
}


/**
 * Returns a random normalized double array (random numbers totaling 1.0).
 * Must be freed.
 * Ex.: [0.19, 0.1, 0.5, 0.21]
 *   n: length of array
 */
double *random_normalized_double_array(int n){
	int i;
	double sum, *rarray;

	rarray = (double*)malloc(n*sizeof(double));

	sum = 0.0;
	for( i = 0 ; i < n ; i++)
		sum += (rarray[i] = drand());
	
	for( i = 0 ; i < n ; i++)
		rarray[i] /= sum;
	
	return rarray;
}

/*******************************************************************************
 *                                LONG ARRAY                                   *
*******************************************************************************/
long *long_array_malloc(int n){
	long *array;

	array = (long*)malloc(n*sizeof(long));

	return array;
}

long *long_array_init(long *array, int n, long x){
	int i;
	for( i = 0 ; i < n ; i++ )
		array[i] = x;
	return array;
}

long *long_array_copy(long *dest, long *src, int n){
	int i;

	if(!dest)
		dest = long_array_malloc(n);

	for( i = 0 ; i < n ; i++ )
		dest[i] = src[i];
	return dest;
}

long long_array_max(long *array, int n){
	int i;
	long max;

	max = array[0];
	for( i = 1 ; i < n ; i++ )
		if( max < array[i] )
			max = array[i];
	return max;
}

long *long_array_read(FILE *in, long *array, int n){
	int i, nf;

	if(!array) array = (long*)malloc(n*sizeof(long));

	for( i = 0 ; i < n ; i++ )
		nf = fscanf(in, "%ld", &(array[i]));
	
	return array;
}

void long_array_write(FILE *out, long *array, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(out, "%ld ", array[i]);
	if(n) fprintf(out, "%ld", array[n-1]);
	fprintf(out, "\n");
	return ;
}


long *long_array_random(int n, long *array, long bound){
	int i;

	if(!array) array = (long*)malloc(n*sizeof(long));

	for( i = 0 ; i < n ; i++ )
		array[i] = lrand(bound);
	
	return array;
}

int long_array_is_sorted(long *array, int n){
	int i;
	for( i = 1 ; i < n ; i++ )
		if( array[i-1] > array[i] )
			return 0;
	return 1;
}

int long_array_partition(long *array, int a, int b){
	int i, j;
	long pivot;
	
	i = a; j = b+1;
	pivot = array[a];

	while( 1 ){
		while( array[++i] < pivot ) if( i == b ) break;
		while( pivot < array[--j] ) if( j == a ) break;
		if( i >= j ) break;            /* Crossed? */
		SWAP_LONG(array, i, j);        /* swap i-j */
	}
	SWAP_LONG(array, j, a);     /* place pivot (swap j-a) */

	return j;
}

long *long_array_sub_qsort(long *array, int a, int b){
	int m;
	if( b <= a ) return array;
	m = long_array_partition(array, a, b);
	long_array_sub_qsort(array, a, m-1);
	long_array_sub_qsort(array, m+1, b);
	return array;
}

long *long_array_qsort(long *array, int n){
	long_array_sub_qsort(array, 0, n-1);
	return array;
}

void long_array_fprint(FILE *out, long *array, int n){
	int i;
	fprintf(out, "[");
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(out, "%ld, ", array[i]);
	if(n) fprintf(out, "%ld", array[n-1]);
	fprintf(out, "]\n");
	
	return;
}

void long_array_free(long *array){
	free(array);
}

/*
 * Prints a longint array on the ZIMPL MIP modeling language format.
 *   fout  - output FILE
 *   n	 - number of elements on array
 *   array - the longint array */
void long_array_zimpl_print(FILE *fout, long *array, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(fout, "<%d> %ld,\n", i+1, array[i]);
	fprintf(fout, "<%d> %ld;\n", i+1, array[n-1]);

	return;
}

/*******************************************************************************
 *                             LONG LONG ARRAY                                 *
*******************************************************************************/
long long *long_long_array_malloc(int n){
	long long *array;

	array = (long long*)malloc(n*sizeof(long long));

	return array;
}

long long *long_long_array_init(long long *array, int n, long long x){
	int i;
	if(!array)
		array = (long long*)malloc(n*sizeof(long long));
	for( i = 0 ; i < n ; i++ )
		array[i] = x;
	return array;
}

long long *long_long_array_copy(long long *dest, long long *src, int n){
	int i;

	if(!dest)
		dest = long_long_array_malloc(n);

	for( i = 0 ; i < n ; i++ )
		dest[i] = src[i];
	return dest;
}

long long *long_long_array_read(FILE *in, long long *array, int n){
	int i, nf;

	if(!array) array = (long long*)malloc(n*sizeof(long long));

	for( i = 0 ; i < n ; i++ )
		nf = fscanf(in, "%lld", &(array[i]));
	
	return array;
}

void long_long_array_write(FILE *out, long long *array, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(out, "%lld ", array[i]);
	if(n) fprintf(out, "%lld", array[n-1]);
	fprintf(out, "\n");
	return ;
}

void long_long_array_fprint(FILE *out, long long *array, int n){
	int i;
	fprintf(out, "[");
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(out, "%lld, ", array[i]);
	if(n) fprintf(out, "%lld", array[n-1]);
	fprintf(out, "]\n");
	
	return;
}

long long long_long_array_max(long long *array, int n){
	int i;
	long long max;

	max = array[0];
	for( i = 1 ; i < n ; i++ )
		if( max < array[i] )
			max = array[i];
	return max;
}

long long *long_long_array_random(int n, long long *array, long long bound){
	int i;

	if(!array) array = (long long*)malloc(n*sizeof(long long));

	for( i = 0 ; i < n ; i++ )
		array[i] = llrand(bound);
	
	return array;
}

int long_long_array_is_sorted(long long *array, int n){
	int i;
	for( i = 1 ; i < n ; i++ )
		if( array[i-1] > array[i] )
			return 0;
	return 1;
}

int long_long_array_partition(long long *array, int a, int b){
	int i, j;
	long long pivot;
	
	i = a; j = b+1;
	pivot = array[a];

	while( 1 ){
		while( array[++i] < pivot ) if( i == b ) break;
		while( pivot < array[--j] ) if( j == a ) break;
		if( i >= j ) break;            /* Crossed? */
		SWAP_LONG_LONG(array, i, j);        /* swap */
	}
	SWAP_LONG_LONG(array, j, a);     /* place pivot */

	return j;
}

long long *long_long_array_sub_qsort(long long *array, int a, int b){
	int m;
	if( b <= a ) return array;
	m = long_long_array_partition(array, a, b);
	long_long_array_sub_qsort(array, a, m-1);
	long_long_array_sub_qsort(array, m+1, b);
	return array;
}

long long *long_long_array_qsort(long long *array, int n){
	long_long_array_sub_qsort(array, 0, n-1);
	return array;
}

/*
 * Prints a long long int array on the ZIMPL MIP modeling language format.
 *   fout  - output FILE
 *   n	 - number of elements on array
 *   array - the longint array */
void long_long_array_zimpl_print(FILE *fout, long long *array, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(fout, "<%d> %lld,\n", i+1, array[i]);
	fprintf(fout, "<%d> %lld;\n", i+1, array[n-1]);

	return;
}

/*
 * Count the digits for each number on array.
 * (This is a helper for pretty printers)
 *
 * Ex.: input: [0, 13, 2, 24, 1904, 013, 3442]
 *     output: [1, 2, 1, 2, 3, 2, 4]
 */
int *long_long_array_count_digits(long long *array, int n){
	int i;
	int *digits;

	digits = (int*)malloc(n*sizeof(int));
	for( i = 0 ; i < n ; i++){
		digits[i] = logl(((double)array[i]+1))/log(10);
	}

	return digits;
}

long long long_long_array_sum(long long *array, int n){
	int i;
	long long sum;

	sum = 0.;
	for( i = 0 ; i < n ; i++ )
		sum += array[i];

	return sum;
}

/*******************************************************************************
 *                                INT MATRIX                                   *
*******************************************************************************/
/* Return max elems of EACH COLUMN. */
int *int_matrix_max_cols(int **mat, int n, int m){
	int i, j, max, *maxs;

	maxs = (int*)malloc(m*sizeof(int));

	for( i = 0 ; i < m ; i++ ){
		max = -32760;
		for( j = 0 ; j < n ; j++ )
			if( mat[j][i] > max )
				max = mat[j][i];
		maxs[i] = max;
	}

	return maxs;
}

/*******************************************************************************
 *                                LONG MATRIX                                  *
*******************************************************************************/
long long_matrix_max_col(long **mat, int n, int m, int col){
	int i;
	long max;

	max = mat[0][col];
	for( i = 1 ; i < n ; i++ )
		if( max < mat[i][col] )
			max = mat[i][col];
	return max;
}

long long_matrix_max_lin(long **mat, int n, int m, int lin){
	int i;
	long max;

	max = mat[lin][0];
	for( i = 1 ; i < n ; i++ )
		if( max < mat[lin][0] )
			max = mat[lin][0];
	return max;
}

long **long_matrix_init(long **mat, int n, int m, long x){
	int i;
	for( i = 0 ; i < n ; i++ )
		long_array_init(mat[i], m, x);
	return mat;
}

long **long_matrix_malloc(int n, int m){
	long **mat;
	int i;

	mat = (long**)malloc(n*sizeof(long*));
	for( i = 0 ; i < n ; i++ )
		mat[i] = (long*)malloc(m*sizeof(long));

	return mat;
}

long **long_matrix_copy(long **dest, long **src, int n, int m){
	int i;

	if(!dest)
		dest = long_matrix_malloc(n, m);

	for( i = 0 ; i < n ; i++ )
		long_array_copy(dest[i], src[i], m);
	return dest;
}

void long_matrix_write(FILE *out, long **mat, int n, int m){
	int i;
	for( i = 0 ; i < n ; i++ )
		long_array_write(out, mat[i], m);

	return;
}

long **long_matrix_read(FILE *in, long **mat, int n, int m){
	int i;

	if(!mat) mat = long_matrix_malloc(n, m);
	
	for( i = 0 ; i < n ; i++ )
		mat[i] = long_array_read(in, mat[i], m);
	
	return mat;
}

void long_matrix_fprint(FILE *out, long **mat, int n, int m){
	int i;
	for( i = 0 ; i < n ; i++ )
		long_array_fprint(out, mat[i], m);
	
	return;
}

void long_matrix_fprint_tranlated(FILE *out, long **mat, int n, int m){
	int i, j;

	for( j = 0 ; j < m ; j++ )
		for( i = 0 ; i < n ; i++ )
			fprintf(out, "%ld%s", mat[i][j], (i < n-1) ? " " : "\n" );
	
	return;
}

long long_matrix_get_max(long **mat, int n, int m){
	int i, j;
	long max;

	max = mat[0][0];
	for( i = 0 ; i < n ; i++ )
		for( j = 0 ; j < m ; j++ )
			if( max < mat[i][j] )
				max = mat[i][j];

	return max;
}

void long_matrix_free(long **mat, int n){
	int i;
	for( i = 0 ; i < n ; i++ )
		free(mat[i]);
	free(mat);
}

/*******************************************************************************
*                      LONG LONG MATRIX                                        *
*******************************************************************************/
/* MALLOC */
long long **long_long_matrix_malloc(int n, int m){
	long long **mat;
	int i;

	mat = (long long**)malloc(n*sizeof(long long*));
	for( i = 0 ; i < n ; i++ )
		mat[i] = (long long*)malloc(m*sizeof(long long));

	return mat;
}

/* COPY */
long long **long_long_matrix_copy(long long **dest, long long **src, int n, int m){
	int i;

	if(!dest)
		dest = long_long_matrix_malloc(n, m);

	for( i = 0 ; i < n ; i++ )
		long_long_array_copy(dest[i], src[i], m);
	return dest;
}

/* WRITE */
void long_long_matrix_write(FILE *out, long long **mat, int n, int m){
	int i;
	for( i = 0 ; i < n ; i++ )
		long_long_array_write(out, mat[i], m);
	fprintf(out, "\n");

	return;
}

/* READ */
long long **long_long_matrix_read(FILE *in, long long **mat, int n, int m){
	int i;

	if(!mat) mat = long_long_matrix_malloc(n, m);
	
	for( i = 0 ; i < n ; i++ )
		mat[i] = long_long_array_read(in, mat[i], m);
	
	return mat;
}

/* FPRINT */
void long_long_matrix_fprint(FILE *out, long long **mat, int n, int m){
	int i;
	for( i = 0 ; i < n ; i++ )
		long_long_array_fprint(out, mat[i], m);
	
	return;
}

/* FRPINT (TRANSLATED) */
void long_long_matrix_fprint_tranlated(FILE *out, long long **mat, int n, int m){
	int i, j;

	for( j = 0 ; j < m ; j++ )
		for( i = 0 ; i < n ; i++ )
			fprintf(out, "%lld%s", mat[i][j], (i < n-1) ? " " : "\n" );
	
	return;
}

/* GET MAX ELEMENT */
long long long_long_matrix_get_max(long long **mat, int n, int m){
	int i, j;
	long long max;

	max = mat[0][0];
	for( i = 0 ; i < n ; i++ )
		for( j = 0 ; j < m ; j++ )
			if( max < mat[i][j] )
				max = mat[i][j];

	return max;
}

/* FREE */
void long_long_matrix_free(long long **mat, int n){
	int i;
	for( i = 0 ; i < n ; i++ )
		free(mat[i]);
	free(mat);
}

/* GET MAX ELEMENT OF A GIVEN COLUMN */
long long long_long_matrix_max_col(long long **mat, int n, int m, int col){
	int i;
	long long max;

	max = mat[0][col];
	for( i = 1 ; i < n ; i++ )
		if( max < mat[i][col] )
			max = mat[i][col];
	return max;
}

/* GET MAX ELEMENT OF A GIVEN LINE */
long long long_long_matrix_max_lin(long long **mat, int n, int m, int lin){
	int i;
	long long max;

	max = mat[lin][0];
	for( i = 1 ; i < n ; i++ )
		if( max < mat[lin][0] )
			max = mat[lin][0];
	return max;
}

/* DOUBLE ARRAY */
double *double_array_alloc(int n){
	return (double*)malloc(n*sizeof(double));
}

double double_array_sum(double *array, int n){
	int i;
	double sum;

	sum = 0.;
	for( i = 0 ; i < n ; i++ )
		sum += array[i];
	
	return sum;
}

double *double_array_copy(double *array, int n){
	double *cop;
	cop = (double*)malloc(n*sizeof(double));
	cop = memcpy(cop, array, n*sizeof(double));
	return cop;
}

double *double_array_init(double *array, int n, double x){
	int i;
	if(!array)
		array = (double*)malloc(n*sizeof(double));
	for( i = 0 ; i < n ; i++ )
		array[i] = x;
	return array;
}

void double_array_fprint(FILE *out, double *a, int n){
	int i;
	fprintf(out, "[");
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(out, "%.3f, ", a[i]);
	if(n) fprintf(out, "%.3f", a[n-1]);
	fprintf(out, "]");

	return;
}

double *double_array_from_scip(double *array, FILE *in){
	int a;
	double val;
	
	while(!feof(in)){
		fscanf(in, "%d %lf", &a, &val);
		array[a] = val;
	}

	return array;
}

int dis_cmp(void *arg, int a, int b){
	double da, db;
	int ia, ib;

	ia = (((int**)arg)[1])[a];
	ib = (((int**)arg)[1])[b];
	da = (((double**)arg)[0])[ia];
	db = (((double**)arg)[0])[ib];
	return da > db ? 1 : da < db ? -1 : 0;
}
void dis_swap(void *arg, int a, int b){
	int iaux;
	iaux = (((int**)arg)[1])[a];
	(((int**)arg)[1])[a] = (((int**)arg)[1])[b];
	(((int**)arg)[1])[b] = iaux;
	return;
}
int *double_index_sort(double *array, int n){
	void **mp_arg[2];
	int *idxs, i;

	idxs = (int*)malloc(n*sizeof(int));

	/* initialize index */
	for( i = 0 ; i < n ; i++ )
		idxs[i] = i;

	/* sort */
	mp_arg[0] = (void*)array;
	mp_arg[1] = (void*)idxs;
	mp_qsort(mp_arg, n, dis_cmp, dis_swap, 1);

	return idxs;
}

/* DOUBLE MATRIX */
double **double_matrix_alloc(int n, int m){
	double **mat;
	int i;

	mat = (double**)malloc(n*sizeof(double*));
	for( i = 0 ; i < n ; i++ )
		mat[i] = (double*)malloc(m*sizeof(double));

	return mat;
}

double **double_matrix_init(double **mat, int n, int m, double x){
	int i, new;

	new = 0; /* if mem blobk is new one */
	if(!mat)
		{mat = (double**)malloc(n*sizeof(double*)); new = 1;}
	for( i = 0 ; i < n ; i++ )
		mat[i] = double_array_init( (new ? NULL : mat[i]), m, x);

	return mat;
}

void long_long_matrix_zimpl_print(FILE *fout, long long **mat, int nlin, int ncol){
	int i, j;
	
	/* header */
	fprintf(fout, "|1");
	for( i = 1 ; i < ncol ; i++ )
		fprintf(fout, ",%d", i+1);
	fprintf(fout, "|\n");

	/* values */
	for( i = 0 ; i < nlin ; i++ ){
		fprintf(fout, "|%d|%lld", i+1, mat[i][0]);
		for( j = 1 ; j < ncol ; j++ )
			fprintf(fout, ",%lld", mat[i][j]);
		fprintf(fout, "|\n");
	}
	fprintf(fout, ";\n");
	return;
}

/*******************************************************************************
*                            ARRAY                                             *
*******************************************************************************/
Array *array_new(){
	Array *a;
	a = (Array*)malloc(sizeof(Array));
	a->n = 0;
	a->nmax = 100;
	a->a = (void**)malloc(a->nmax*sizeof(void*));

	return a;
}

int array_get_size(Array *array){
	return array->n;
}

void *array_get_array(Array *array){
	return array->a;
}

void *array_get(Array *array, int i){
	return array->a[i];
}

void array_apply(Array *a, void(*apl_f)(void*)){
	int i;
	for( i = 0 ; i < a->n ; i++ )
		apl_f(a->a[i]);
	return;
}

void pointer_swap(void **ps, int a, int b){
	void *p;
	p = ps[a]; ps[a] = ps[b]; ps[b] = p;
}

void array_bubble_sort(Array *array, int(*compar)(void *, void *)){
    int i, j, imin,  n;
    void **arr;
    void *aux;

    n = array->n;

    if( array->n < 2 )
        return;

    arr = array->a;
    for( i = 0 ; i < n-1 ; i++ ){
        imin = i;
        for( j = i+1 ; j < n ; j++ )
            if( compar(arr[imin], arr[j]) > 0 )
                imin = j;
        aux = arr[imin];
        arr[imin] = arr[i];
        arr[i] = aux;
    }
    return;
}

void array_sort(
	Array *array,
	int(*compar)(void *, void *))
{
	mp_qsort(
		array->a,
		array->n,
		(mp_cmp_f)compar,
		(mp_swap_f)pointer_swap,
		0);
}

int pointer_cmp_r(
	void *ps_,
	int a,
	int b,
	void *arg)
{
	void **ps;
	void **args;
	int(*cmp)(void*, void*, void*);
	ps = (void**)ps_;
	args = (void**)arg;
	cmp = (int(*)(void*,void*,void*))args[0];
	return cmp(ps[a], ps[b], args[1]);
}

void pointer_swp(void **ps, int a, int b){
	void *aux;
	aux = ps[a];
	ps[a] = ps[b];
	ps[b] = aux;
	return;
}

void array_sort_r(
	Array *array,
	int(*compar)(void *obj1, void *obj2, void *cmp_arg),
	void *arg)
{
	void *args[2];
	args[0] = compar;
	args[1] = arg;
	mp_qsort_r(
		array->a,
		array->n,
		(mp_cmp_r_f)pointer_cmp_r,
		(mp_swap_f)pointer_swp,
		args,
		0);
}

Array *array_insert(Array *array, void *elem){
	if( array->n == array->nmax ){
		array->nmax *= 2;
		array->a = (void**)realloc(array->a, array->nmax*sizeof(void*));
	}
	array->a[array->n] = elem;
	array->n++;

	return array;
}

Array *array_remove(Array *array, int a){
	myassert( a < array->n );

	array->a[a] = array->a[array->n-1];
	array->n--;
	return array;
}

Array *array_empty(Array *a){
	a->n = 0;
	return a;
}

void array_free(Array *a){
	free(a->a);
	free(a);
	return;
}

/* MEGA POWER QSORT */
int mp_partition_r(void *collection, int a, int b, mp_cmp_r_f mp_cmp_r, mp_swap_f mp_swap, void *arg, char reverse){
	int i, j, pivot;

	pivot = i = a; j = b+1;
	while( 1 ){
		while( reverse*mp_cmp_r(collection, ++i, pivot, arg) < 0 ) if( i == b ) break;
		while( reverse*mp_cmp_r(collection, pivot, --j, arg) < 0 ) if( j == a ) break;
		if( i >= j ) break;        /* pointers crossed? */
		mp_swap(collection, i, j);
	}
	mp_swap(collection, j, a);     /* place pivot */

	return j;
}

void sub_mp_qsort_r(void *collection, int a, int b, mp_cmp_r_f mp_cmp_r, mp_swap_f mp_swap, void *arg, char reverse){
	int m;
	if( b <= a ) return;
	m = mp_partition_r(collection, a, b, mp_cmp_r, mp_swap, arg, reverse);
	sub_mp_qsort_r(collection, a, m-1, mp_cmp_r, mp_swap, arg, reverse);
	sub_mp_qsort_r(collection, m+1, b, mp_cmp_r, mp_swap, arg, reverse);
	return;
}

void mp_qsort_r(void *collection, int n, mp_cmp_r_f mp_cmp_r, mp_swap_f mp_swap, void *arg, char reverse){
	reverse = reverse ? -1 : 1;
	sub_mp_qsort_r(collection, 0, n-1, mp_cmp_r, mp_swap, arg, reverse);
	return;
}

int mp_partition(void *collection, int a, int b, mp_cmp_f mp_cmp, mp_swap_f mp_swap, char reverse){
	int i, j, pivot;
    printf("partitin...\n"); fflush(stdout);

	pivot = i = a; j = b+1;
	while( 1 ){
		while( reverse*mp_cmp(collection, ++i, pivot) < 0 ) if( i == b ) break;
		while( reverse*mp_cmp(collection, pivot, --j) < 0 ) if( j == a ) break;
		if( i >= j ) break;        /* pointers crossed? */
		mp_swap(collection, i, j);
	}
	mp_swap(collection, j, a);     /* place pivot */

	return j;
}

void sub_mp_qsort(void *collection, int a, int b, mp_cmp_f mp_cmp, mp_swap_f mp_swap, char reverse){
	int m;
	if( b <= a ) return;
	m = mp_partition(collection, a, b, mp_cmp, mp_swap, reverse);
	sub_mp_qsort(collection, a, m-1, mp_cmp, mp_swap, reverse);
	sub_mp_qsort(collection, m+1, b, mp_cmp, mp_swap, reverse);
	return;
}

void mp_qsort(void *collection, int n, mp_cmp_f mp_cmp, mp_swap_f mp_swap, char reverse){
	reverse = reverse ? -1 : 1;
	sub_mp_qsort(collection, 0, n-1, mp_cmp, mp_swap, reverse);
	return;
}

int _void_partition(void **array, int a, int b, int(*cmp)(void*,void*)){
	int i, j;
	void *pivot, *aux;
	
	i = a; j = b+1;
	pivot = array[a];

	while( 1 ){
		while( cmp(array[++i], pivot) < 0 ) if( i == b ) break;
		//while( array[++i] < pivot ) if( i == b ) break;
		while( cmp(pivot, array[--j]) < 0 ) if( j == a ) break;
		//while( pivot < array[--j] ) if( j == a ) break;
		if( i >= j ) break;            /* Crossed? */
        /* swap */
        aux = array[i];
        array[i] = array[j];
        array[j] = aux;
	}
    /* swap */
    aux = array[i];
    array[i] = array[a];
    array[a] = aux;

	return j;
}

void _void_qsort(void **array, int left, int right, int(*f)(void*, void*)){
    int pivot;
    if( right <= left ) return;
    if ( right > left ){
        pivot = _void_partition(array, left, right, f);
        _void_qsort(array, left, pivot-1, f);
        _void_qsort(array, pivot+1, right, f);
    }
}

void void_qsort(void **array, int n, int(*f)(void*, void*)){
    _void_qsort(array, 0, n-1, f);
}

void void_shell_sort(void **array, int n, int(*cmp)(void*,void*)){
    int gaps[8] = {701, 203, 132, 57, 23, 10, 4, 1};
    int gap;
    int k, i, j;
    void *aux;

    for( k = 0 ; k < 8 ; k++ ){
        gap = gaps[k];
        printf("gap %d\n", gap);
        for( i = gap ; i < n ; i++ ){
            aux = array[i];
            for( j = i ; j >= gap && cmp(array[j-gap], aux) > 0 ; j -= gap )
                array[j] = array[j-gap];
            array[j] = aux;
        }
    }
}

void void_sort(void **array, int n, int(*cmp)(void*,void*)){
    void_shell_sort(array, n, cmp);
}

/*
 * Prints a double matrix on the ZIMPL MIP modeling language format.
 *   fout - output FILE
 *   nlin - number of lines on matrix
 *   ncol - numer of columns on matrix
 *   mat  - the double matrix */
void double_matrix_zimpl_print(FILE *fout, double **mat, int nlin, int ncol){
	int i, j;
	
	/* header */
	fprintf(fout, "|1");
	for( i = 1 ; i < ncol ; i++ )
		fprintf(fout, ",%d", i+1);
	fprintf(fout, "|\n");

	/* values */
	for( i = 0 ; i < nlin ; i++ ){
		fprintf(fout, "|%d|%lf", i+1, mat[i][0]);
		for( j = 1 ; j < ncol ; j++ )
			fprintf(fout, ",%lf", mat[i][j]);
		fprintf(fout, "|\n");
	}
	fprintf(fout, ";\n");
	return;
}

/*
 * Prints a double array on the ZIMPL MIP modeling language format.
 *   fout  - output FILE
 *   n	 - number of elements on array
 *   array - the double array */
void double_array_zimpl_print(FILE *fout, double *array, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(fout, "<%d> %lf,\n", i+1, array[i]);
	fprintf(fout, "<%d> %lf;\n", i+1, array[n-1]);

	return;
}

/*
 * Prints a longint matrix on the ZIMPL MIP modeling language format.
 *   fout - output FILE
 *   nlin - number of lines on matrix
 *   ncol - numer of columns on matrix
 *   mat  - the longint matrix */
void long_matrix_zimpl_print(FILE *fout, long **mat, int nlin, int ncol){
	int i, j;
	
	/* header */
	fprintf(fout, "|1");
	for( i = 1 ; i < ncol ; i++ )
		fprintf(fout, ",%d", i+1);
	fprintf(fout, "|\n");

	/* values */
	for( i = 0 ; i < nlin ; i++ ){
		fprintf(fout, "|%d|%ld", i+1, mat[i][0]);
		for( j = 1 ; j < ncol ; j++ )
			fprintf(fout, ",%ld", mat[i][j]);
		fprintf(fout, "|\n");
	}
	fprintf(fout, ";\n");
	return;
}

/*
 * Parses a list of integers (format exemplified below) returning its array of integer.
 *   If the input string is not well formates, returns NULL.
 *   Ex.: "[1,2,3,4]"
 *   str - string to be parsed
 *   n   - size of returning array
 *   (FUNCTION BEST SUITED FOR SMALL ARRAYS) */
int *parse_int_list(char *str, int *n){
	int len, nmax, res;
	int *vec;
	char c;

	/* calculating size of str and counting ','s for upper-bound on size of returning array. */
	nmax = 1;
	c = str[len=0];
	while( c != '\0' ){
		if( c == ',')
			nmax++;
		if( !ISNUM(c) )
			str[len] = ' ';
		c = str[++len];
	}

	printf("nmax=%d\n", nmax);
	vec = (int*)malloc((nmax+10)*sizeof(int));

	/* FIXME */
	*n = res = 0;
	do{
		if( res > 0 )
			(*n)++;
		while( *str != '\0' && ISNUM(*str) )
			str++;
		while( *str != '\0' && !ISNUM(*str) )
			str++;
	}while( res = sscanf(str, "%d", &(vec[(*n)])) > 0);

	return vec;
}

int getmillis(){
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);

	return tv.tv_sec*1000+tv.tv_usec;
}

/* ZIPING */
void gzip(FILE *f_in, FILE *f_out){
	FILE *pipe, *ftemp;
	size_t nr;
	char ftemp_name[200];
	char buffer[1000];
	char cmd[1000];

	/* FIXME... */
	/* opening a temp file */
	pipe = popen("mktemp", "r");
	fscanf(pipe, "%s", ftemp_name);
	pclose(pipe);

	/* zips the input into temp file */
	sprintf(cmd, "gzip -f > %s", ftemp_name);
	pipe = popen(cmd, "w");
	printf("printinf on pipe"); fflush(stdout);
	while(!feof(f_in)){
		nr = fread(buffer, sizeof(char), 1000, f_in);
		fwrite(buffer, sizeof(char), nr, pipe);
	}
	pclose(pipe);

	printf("rediring"); fflush(stdout);
	/* redirects the zipped file content to output */
	ftemp = fopen(ftemp_name, "r");
	while(!feof(ftemp)){
		nr = fread(buffer, sizeof(char), 1000, ftemp);
		fwrite(buffer, sizeof(char), nr, f_out);
	}
	fclose(ftemp);

	/* deletes temporary file */
	//remove(ftemp_name);

	return;
}

void setdebug_lvl(char lvl){
    debug_lvl = lvl;
}

void unimplemented(){
	fprintf(stderr, "function \"%s\" not implemented yet.\n",
		__PRETTY_FUNCTION__);
}
 
void gunzip(FILE *in, FILE *out){
	unimplemented();
}

void error( const char* format, ... ){
	va_list args;
	fprintf(stderr, "Error: ");
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	exit(1);
}

void assert_faccess(char *filename, int mode){
	if(access(filename, mode))
		error("Could not %s file %s.\n",
			mode == W_OK ? "write" : "read", filename);
	return;
}

void myassert(int express){
	if(!express){
		fprintf(stderr, "Fatal error on function %s: condition not meet!\n", __PRETTY_FUNCTION__);
		exit(1);
	}
}

void debug(char *msg){
    if( debug_lvl ){
        fprintf(stdout, "%s", msg);
        fflush(stdout);
    }
	return;
}

double sigmoid(double x){
	return (1./(1.+pow(M_E, -x)));
}

int ipow(int base, int exp){
    int result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}
