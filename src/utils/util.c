#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "util.h"

#define ISNUM(c) ( (c > 47) && (c < 58) )

int verbose = 0;
int shell_gaps[8] = {1, 4, 10, 23, 57, 132, 301, 701};

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

int *int_array_reverse(int *array, int n){
    int aux, i, mid;

    mid = n/2;
    for( i = 0 ; i < mid ; i++ ){
        aux = array[i];
        array[i] = array[n-i-1];
        array[n-i-1] = aux;
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

int int_matrix_max(int **mat, int n, int m){
    int i, j, max;
    max = mat[0][0];

    for( i = 0 ; i < n ; i++ )
        for( j = 0 ; j < n ; j++ )
            if( max < mat[i][j] )
                max = mat[i][j];

    return max;
}

void int_matrix_fprintf(FILE *out, int **mat, int n, int m){
    int i, j, ndigits;
    char format[20];

    ndigits = (int)(ceil(log10(int_matrix_max(mat, n, m))));
    sprintf(format, "%% %dd", ndigits);

    for( i = 0 ; i < n ; i++ ){
        for( j = 0 ; j < m ; j++ )
            fprintf(out, format, mat[i][j]);
        fprintf(out, "\n");
    }

    return;
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
	if( !array )
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

double *double_array_read(FILE *fin, double *p, int n){
    int i;
    int nerr;

    if(!p)
        p = (double*)malloc(n*sizeof(double));

    for( i = 0 ; i < n ; i++ )
        nerr = fscanf(fin, "%lf", &(p[i]));

    return p;
}

void double_array_write(FILE *out, double *p, int n){
    int i;

    for( i = 0 ; i < n ; i++ )
        fprintf(out, "%.3lf ", p[i]);

    return;
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

double double_array_max(double *mat, int n){
    double max;
    int i;

    max = mat[0];
    for( i = 1 ; i < n ; i++ )
        if( max < mat[i] )
            max = mat[i];

    return max;
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

double double_matrix_max(double **mat, int n, int m){
    double max, r;
    int i;

    max = double_array_max(mat[0], m);
    for( i = 1 ; i < n ; i++ ){
        r = double_array_max(mat[i], m);
        if( r > max )
            max = r;
    }

    return max;
}

void double_matrix_fprintf(FILE *fout, double **mat, int n, int m){
    int i, j;
    double max;
    int ndigits;
    char format[20];

    ndigits = (int)(ceil(log10(double_matrix_max(mat, n, m))));
    sprintf(format, "%%%d.3f ", ndigits);
    for( i = 0 ; i < n ; i++ ){
        for( j = 0 ; j < m ; j++ ){
            fprintf(fout, format, mat[i][j]);
        }
        fprintf(fout, "\n");
    }

    return;
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

/*******************************************************************************
 ***     KD-TREE
*******************************************************************************/
KDTree *kdtree_new( int ndim, kdtree_eval_f eval_f){
    KDTree *kdtree;
    int nhcount = 100;

    kdtree = (KDTree*)malloc(sizeof(KDTree));

    kdtree->eval_f = eval_f;
    kdtree->root = NULL;
    kdtree->n = 0;
    kdtree->ndim = ndim;
#if KDTREE_STATS
    kdtree->hcount = long_long_array_init(NULL, nhcount, 0);
    kdtree->nhcount = nhcount;
#endif

    return kdtree;
}

#if KDTREE_STATS
void _kdtree_increment_hcount(KDTree *kdtree, int h){
    long long *new_hcount;

    if( kdtree->nhcount < h ){
        new_hcount = (long long*)malloc(2*kdtree->nhcount*sizeof(long long));
        long_long_array_copy(new_hcount, kdtree->hcount, kdtree->nhcount);
        kdtree->nhcount *= 2;
        free(kdtree->hcount);
        kdtree->hcount = new_hcount;
    }

    kdtree->hcount[h]++;
}
#endif

void _kdtree_insert(KDTree *kdtree, KDNode *root, KDNode *kdn, int h){
    double rval;
    double eval;
    int dim;

    dim = h % kdtree->ndim;
    rval = kdtree->eval_f(root->info, dim);
    eval = kdtree->eval_f(kdn->info, dim);

    if( eval > rval ){ /* insert in right branch */
        if( root->right ){
            _kdtree_insert(kdtree, root->right, kdn, h+1);
        }else{
            root->right = kdn;
            kdn->up = root;
#if KDTREE_STATS
            _kdtree_increment_hcount(kdtree, h+1);
#endif
        }
    }else{              /* insert in left branch */
        if( root->left ){
            _kdtree_insert(kdtree, root->left, kdn, h+1);
        }else{
            root->left = kdn;
            kdn->up = root;
#if KDTREE_STATS
            _kdtree_increment_hcount(kdtree, h+1);
#endif
        }
    }

    return;
}

KDTree *kdtree_insert( KDTree *kdtree, void *element){
    KDNode *kdn;

    kdn = (KDNode*)malloc(sizeof(KDNode));
    kdn->info = element;
    kdn->up = kdn->right = kdn->left = NULL;

    if( !kdtree->root ){
        kdtree->root = kdn;
        kdn->val = kdtree->eval_f(element, 0);
#if KDTREE_STATS
        _kdtree_increment_hcount(kdtree, 0);
#endif
    }else
        _kdtree_insert(kdtree, kdtree->root, kdn, 0);

    kdtree->n++;

    return kdtree;
}

/*
 * Procedure:
 *   1. check if root is inside range (if true, return root)
 *   2. check if right hypercube intersects range (if true, call for right)
 *   3. check if left hypercube intersects range (if true, call for left)
 *
 * obs.: Consider infinity bounds
 */
void *_kdtree_range_search(KDTree *kdtree, KDNode *root, double *bounds, int h, void *prop_f, void *prop_arg){
    double val, lower, upper;
    int i, dim, ndim, meets;
    kdtree_eval_f eval_f;
    void *ret;

    ndim = kdtree->ndim;
    eval_f = kdtree->eval_f;
    ret = NULL;

#ifdef MOKP_DEBUG
    printf("     root is ");
    mokpnode_fprintf(stdout, root->info);
    printf("\n");
#endif

    /* check if root is inside range */
    meets = 1;
    for( i = 0 ; i < ndim && meets ; i++ ){
        val = eval_f(root->info, i);
        lower = bounds[i*2];
        upper = bounds[i*2+1];
        meets = (val >= lower) && (val <= upper);
    }
#ifdef MOKP_DEBUG
    printf("     inside bounds: %s\n", meets ? "yes" : "no");
#endif
    /* if is inside, check if has desired property */
    if( meets ){
        if( prop_arg ) meets = ((property_f_r)prop_f)(root->info, prop_arg);
        else meets = ((property_f)prop_f)(root->info);
    }

    /* if is inside and meets property, return */
    if( meets )
        return root->info;

    /* root does not meet, continuing search throught children... */
    dim = h % kdtree->ndim; /* the dimention index to be used */
    val = kdtree->eval_f(root->info, dim); /* value of root for the dim used */
    lower = bounds[dim*2];
    upper = bounds[dim*2+1];

    /* search right branch */
    if( root->right )
        if( val <= upper )
            ret = _kdtree_range_search(kdtree, root->right, bounds, h+1, prop_f, prop_arg);

    if( ret )
        return ret;

    /* search left branch */
    if( root->left )
        if( val >= lower )
            ret = _kdtree_range_search(kdtree, root->left, bounds, h+1, prop_f, prop_arg);

    return ret;
}


/*
 * kdtree: the KD-Tree
 * bounds: array representing the bounds for search.
 *         Ex.: For first hyper-quadrant on 3 dimensions {0.0, INFINITY, 0,0, INFINITY, 0.0, INFINITY}
 * prop_f: optional function to check if a candidate node, found inside range, has
 *         the desired property. If not, range search wont return it and will keep searching.
 */
int always_has_prop(void *a){ return 1; }
void *kdtree_range_search(KDTree *kdtree, double *bounds, property_f prop_f){
    if( !kdtree->root )
        return NULL;

    if( !prop_f )
        prop_f = always_has_prop;

    else return _kdtree_range_search(kdtree, kdtree->root, bounds, 0, prop_f, NULL);
}

void *kdtree_range_search_r(KDTree *kdtree, double *bounds, property_f_r prop_f, void *prop_arg){
    if( !kdtree->root )
        return NULL;

    return _kdtree_range_search(kdtree, kdtree->root, bounds, 0, prop_f, prop_arg);
}

void _kdnode_free(KDNode *kdn){
    if(kdn->right) _kdnode_free(kdn->right);
    if(kdn->left) _kdnode_free(kdn->left);
    free(kdn);

    return;
}

void kdtree_free(KDTree *kdtree){
    _kdnode_free(kdtree->root);
#if KDTREE_STATS
    free(kdtree->hcount);
#endif
    free(kdtree);

    return;
}

double kdtree_avg_height(KDTree *kdtree){
#if KDTREE_STATS
    long long total;
    double mean;
    int i;

    total = 0;
    for( i = 0 ; i < kdtree->nhcount ; i++ )
        total += i*kdtree->hcount[i];
    mean = total/((double)kdtree->n);

    return mean;
#else
    fprintf(stderr, "%s error: kd-tree statistics not enabled.\n", __FUNCTION__);
#endif
}

double kdtree_ideal_avg_height(KDTree *kdtree){
#if KDTREE_STATS
    long long n_left;
    double avg;
    int i;

    n_left = kdtree->n;
    i = 0;
    avg = 0;
    while( n_left > 0 ){
        if( n_left < ipow(2, i)) avg += n_left*i;
        else avg += ipow(2, i)*i;
        n_left -= ipow(2, i);
        i++;
    }
    avg = avg/((double)kdtree->n);
    return avg;

#else
    fprintf(stderr, "%s error: kd-tree statistics not enabled.\n", __FUNCTION__);
#endif
}

void _sub_kdtree_get_elems(KDNode *root, void **elems, int *k){
    if( root->left ) _sub_kdtree_get_elems(root->left , elems, k);
    elems[(*k)++] = root->info;
    if( root->right ) _sub_kdtree_get_elems(root->right, elems, k);
}

void _kdtree_get_elems(KDNode *root, void **elems){
    int k = 0;
    _sub_kdtree_get_elems(root, elems, &k);
}

void varray_shell_iter_r(void **v, int a, int b, int k, cmp_r_f cmp_r, void *arg){
    int i, n, igap, gap;

    n = b-a+1;
    igap = 7;
    while( n*2 < shell_gaps[igap] )
        igap--;

    gap = shell_gaps[igap];
    for( i = 1 ; i < gap ; i++ ){}
    // TODO: implement parcial shellsort...

    return;
}

int _kdtree_cmp_r( void *a, void *b, void *args){
    double aval, bval;

    aval = ((kdtree_eval_f)((void**)args)[0])(a, *(int*)((void**)args)[1] );
    bval = ((kdtree_eval_f)((void**)args)[0])(b, *(int*)((void**)args)[1] );

    if( aval < bval ) return -1;
    else if( aval > bval ) return 1;
    return 0;
}

void _kdtree_median_insert(KDTree *kdtree, void **elems, int n, int dim){
    int i, mid;
    Heap *minh;
    Heap *maxh;
    void *median, *elem;
    double mval, val;
    kdtree_eval_f eval_f;
    void **args[2];

    mid = n/2;
    eval_f = kdtree->eval_f;
    args[0] = (void*)(kdtree->eval_f);
    args[1] = (void*)(&dim);

    i = 0;

    /* too short array */
    if( n < 3 ){
        while( i < n )
            kdtree_insert(kdtree, elems[i++]);
        return;
    }

    /* preparing heaps */
    minh = heap_new_r(mid, _kdtree_cmp_r, args, 1);
    maxh = heap_new_r(mid, _kdtree_cmp_r, args, 0);

    median = elems[i++];
    mval = eval_f(median, dim);
    while( i < n ){
        elem = elems[i++];
        val = eval_f(elem, dim);
        /* inserting in right heap */
        if( val < mval ) heap_insert( minh, elem);
        else heap_insert( maxh, elem);

        /* balancing */
        if( maxh->n - minh->n > 1 ){
            heap_insert(minh, median);
            median = heap_pop_peak(maxh);
            mval = eval_f(median, dim);
        }else if( minh->n - maxh->n > 1 ){
            heap_insert(maxh, median);
            median = heap_pop_peak(minh);
            mval = eval_f(median, dim);
        }
    }

    /* insert median and recusevly call for splited */
    kdtree_insert(kdtree, median);
    memcpy(elems, minh->arr, minh->n*sizeof(void*));
    memcpy(&(elems[minh->n]), maxh->arr, maxh->n*sizeof(void*));
    i = minh->n;
    heap_free(minh);
    heap_free(maxh);

    _kdtree_median_insert(kdtree, elems, i, (dim+1)%(kdtree->ndim));
    _kdtree_median_insert(kdtree, &(elems[i]), n-i-1, (dim+1)%(kdtree->ndim));

    return;
}

void kdtree_balance(KDTree *kdtree){
    printf("balaingin\n");
    void **elems;
    int n, ndim;
    kdtree_eval_f eval_f;
    
    /* init variables */
    ndim = kdtree->ndim;
    eval_f = kdtree->eval_f;
    n = kdtree->n;
    elems = (void**)malloc(n*sizeof(void*));

    /* get all elements out */
    _kdtree_get_elems(kdtree->root, elems);

    /* renewing the kdtree */
    kdtree->root = NULL;
    kdtree->n = 0;
    long_long_array_init(kdtree->hcount, kdtree->nhcount, 0);

    /* find median */
    _kdtree_median_insert(kdtree, elems, n, 0);
    free(elems);

    return;
}

void kdtree_fprint_stats(FILE *out, KDTree *kdtree){
    double avg_h, ideal_avg_h;
    avg_h = kdtree_avg_height(kdtree);
    ideal_avg_h = kdtree_ideal_avg_height(kdtree);

    fprintf(out, "avg h: %.1lf, ideal h: %.1lf\n", avg_h, ideal_avg_h);

    return;
}


/*******************************************************************************
 ***     (MIN/MAX) HEAP
*******************************************************************************/
Heap *heap_new(int nmax, cmp_f cmp, char is_min){
    Heap *heap;

    heap = (Heap*)malloc(sizeof(Heap));
    heap->n = 0;
    heap->nmax = nmax;
    heap->arr = (void**)malloc(nmax*sizeof(void*));
    heap->cmp = cmp;
    heap->cmp_r = NULL;
    heap->arg = NULL;
    heap->is_min = is_min;

    return heap;
}

Heap *heap_new_r(int nmax, cmp_r_f cmp_r, void *arg, char is_min){
    Heap *heap;

    heap = heap_new(nmax, NULL, is_min);
    heap->cmp_r = cmp_r;
    heap->arg = arg;

    return heap;
}

int _heap_eval(Heap *heap, void *elem1, void* elem2){
    if( heap->is_min ){
        if( heap->cmp ){
            return (heap->cmp(elem1, elem2));
        }else{
            return (heap->cmp_r(elem1, elem2, heap->arg));
        }
    }else{
        if( heap->cmp ){
            return -heap->cmp(elem1, elem2);
        }else{
            return -heap->cmp_r(elem1, elem2, heap->arg);
        }
    }
}

void heap_insert(Heap *heap, void *elem){
    double val, val2;
    void **arr;
    void *aux;
    int n, i, father_i;

    if( heap->n == heap->nmax ){
        heap->nmax = 2*heap->nmax;
        heap->arr = (void**)realloc(heap->arr, heap->nmax*sizeof(void*));
    }

    arr = heap->arr;

    arr[heap->n] = elem;
    i = heap->n;
    father_i = (i-1)/2;
    while( i && (_heap_eval(heap, arr[father_i], elem) < 0) ){
        aux = arr[father_i];
        arr[father_i] = arr[i];
        arr[i] = aux;
        i = father_i;
        father_i = (i-1)/2;
    }

    heap->n++;
    return;
}

void heap_fprintf(FILE *out, Heap *heap, prt_f prt){
    int i, n;
    void **arr;

    n = heap->n;
    arr = heap->arr;

    fprintf(out, "1: ");
    prt(out, arr[0]);
    fprintf(out, " (root)\n");
    for( i = 1 ; i < n ; i++ ){
        fprintf(out, "%d: ", i+1);
        prt(out, arr[i]);
        fprintf(out, " (%s ", heap->is_min ? "<" : ">");
        prt(out, arr[(i-1)/2]);
        fprintf(out, ")\n");
    }

    return;
}

void *heap_pop_peak(Heap *heap){
    void **arr;
    void *aux, *min;
    int i, n, imin, need_check;
    double val;

    arr = heap->arr;

    min = arr[0];
    n = --heap->n;
    arr[0] = arr[n];

    /* heapfying */
    i = 0;
    need_check = 1;
    while( i*2+1 < n && need_check ){
        need_check = 0;
        /* checking lesser child */
        imin = i*2+1;
        if( imin+1 < n )
            if( _heap_eval(heap, arr[imin+1], arr[imin]) > 0 )
                imin++;

        /* checking if needs change */
        if( _heap_eval(heap, arr[i], arr[imin]) < 0 ){
            aux = arr[i];    /* swaping */
            arr[i] = arr[imin];
            arr[imin] = aux;
            need_check = 1;   /* flag check */
            i = imin;
        }
    }

    return min;
}

void heap_free(Heap *heap){
    free(heap->arr);
    free(heap);
}

