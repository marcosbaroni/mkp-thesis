#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdarg.h>
#include "util.h"

#define ISNUM(c) ( (c > 47) && (c < 58) )

/* 
 * Returns a random long integer non-negative from 0 to <bound>.
 */
long lrand(long bound){
	return (rand() % (bound+1));
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

long *long_array_random(int n, long *array, long bound){
	int i;

	if(!array) array = (long*)malloc(n*sizeof(long));

	for( i = 0 ; i < n ; i++ )
		array[i] = lrand(bound);
	
	return;
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
	long pivot, aux;
	
	i = a; j = b+1;
	pivot = array[a];

	while( 1 ){
		while( array[++i] < pivot ) if( i == b ) break;
		while( pivot < array[--j] ) if( j == a ) break;
		if( i >= j ) break;            /* Crossed? */
		SWAP_LONG(array, i, j);        /* swap */
	}
	SWAP_LONG(array, j, a);     /* place pivot */

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

long long_matrix_max_col(long **mat, int n, int m, int col){
	int i, j;
	long max;

	max = mat[0][col];
	for( i = 1 ; i < n ; i++ )
		if( max < mat[i][col] )
			max = mat[i][col];
	return max;
}

long long_matrix_max_lin(long **mat, int n, int m, int lin){
	int i, j;
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
	fprintf(out, "%ld\n", array[i]);
	return ;
}

void long_matrix_write(FILE *out, long **mat, int n, int m){
	int i;
	for( i = 0 ; i < n ; i++ )
		long_array_write(out, mat[i], m);

	return;
}

long **long_matrix_read(FILE *in, long **mat, int n, int m){
	int i, j;

	if(!mat) mat = long_matrix_malloc(n, m);
	
	for( i = 0 ; i < n ; i++ )
		mat[i] = long_array_read(in, mat[i], m);
	
	return mat;
}

void *long_array_fprint(FILE *out, long *array, int n){
	int i;
	for( i = 0 ; i < n ; i++ )
		fprintf(out, "%ld%s", array[i], (i < n-1) ? " " : "\n" );
	
	return;
}

void **long_matrix_fprint(FILE *out, long **mat, int n, int m){
	int i;
	for( i = 0 ; i < n ; i++ )
		long_array_fprint(out, mat[i], m);
	
	return;
}

void **long_matrix_fprint_tranlated(FILE *out, long **mat, int n, int m){
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

void long_array_free(long *array){
	free(array);
}

void long_matrix_free(long **mat, int n){
	int i;
	for( i = 0 ; i < n ; i++ )
		free(mat[i]);
	free(mat);
}

/* AVL TREE */
AVLNode *avl_node_new(void *a){
	AVLNode *avl_node;
	avl_node = (AVLNode*)malloc(sizeof(AVLNode));
	avl_node->info = a;
	avl_node->left = NULL;
	avl_node->right = NULL;
	avl_node->father = NULL;
	avl_node->balance = 0;

	return avl_node;
}

AVLTree *avl_new(avl_cmp* cmp_f){
	AVLTree *avl;
	avl = (AVLTree*)malloc(sizeof(AVLTree));
	avl->cmp_f = cmp_f;
	avl->fprt_f = NULL;
	avl->root = NULL;
	avl->n = 0;
	avl->height = 0;

	return avl;
}

AVLTree *avl_set_prt(AVLTree *avl, avl_fprt* fprt_f){
	avl->fprt_f = fprt_f;

	return avl;
}

AVLTree *avl_insert(AVLTree *avl, void *a){
	AVLNode *new_node = avl_node_new(a);
	avl->height += avl_node_insert(avl, avl->root, new_node);
	avl->n++;
	return avl;
}

int avl_node_insert(AVLTree *avl, AVLNode *node, AVLNode *new_node){
	int c, grew = 0;

	c = (*avl->cmp_f)(node->info, new_node->info);
	if(c >= 0){
		if( !node->right ){
			node->right = new_node;
			new_node->father = node;
			node->balance++;
			grew = 1;
		}else{
			grew = avl_node_insert(avl, node->right, new_node);
		}
		node->balance += grew;
	}{
		if( !node->left ){
			node->left = new_node;
			new_node->father = node;
			grew = 1;
		}else{
			grew = avl_node_insert(avl, node->left, new_node);
		}
		node->balance -= grew;
	}
	/* STOPPED HERE */
	return grew;
}


/*
 * Prints a double matrix on the ZIMPL MIP modeling language format.
 *   fout - output FILE
 *   nlin - number of lines on matrix
 *   ncol - numer of columns on matrix
 *   mat  - the double matrix */
void zimpl_print_double_matrix(FILE *fout, double **mat, int nlin, int ncol){
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
		fprintf(fout, "|\n", mat[i][j]);
	}
	fprintf(fout, ";\n");
	return;
}

/*
 * Prints a double array on the ZIMPL MIP modeling language format.
 *   fout  - output FILE
 *   n	 - number of elements on array
 *   array - the double array */
void zimpl_print_double_array(FILE *fout, double *array, int n){
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
		fprintf(fout, "|\n", mat[i][j]);
	}
	fprintf(fout, ";\n");
	return;
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

/*
 * Parses a list of integers (format exemplified below) returning its array of integer.
 *   If the input string is not well formates, returns NULL.
 *   Ex.: "[1,2,3,4]"
 *   str - string to be parsed
 *   n   - size of returning array
 *   (FUNCTION BEST SUITED FOR SMALL ARRAYS) */
int *parse_int_list(char *str, int *n){
	int i, len, nmax, res;
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

