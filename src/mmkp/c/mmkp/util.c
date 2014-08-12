#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdarg.h>

#define ISNUM(c) ( (c > 47) && (c < 58) )

/* 
 * Returns a random long integer non-negative less than <bound>.
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

long *malloc_long_array(int n){
	long *array;

	array = (long*)malloc(n*sizeof(long));

	return array;
}

long *init_long_array(long *array, int n, long x){
	int i;
	for( i = 0 ; i < n ; i++ )
		array[i] = x;
	return array;
}

long *copy_long_array(long *dest, long *src, int n){
	int i;

	if(!dest)
		dest = malloc_long_array(n);

	for( i = 0 ; i < n ; i++ )
		dest[i] = src[i];
	return dest;
}

long **init_long_matrix(long **mat, int n, int m, long x){
	int i;
	for( i = 0 ; i < n ; i++ )
		init_long_array(mat[i], m, x);
	return mat;
}

long **malloc_long_matrix(int n, int m){
	long **mat;
	int i;

	mat = (long**)malloc(n*sizeof(long*));
	for( i = 0 ; i < n ; i++ )
		mat[i] = (long*)malloc(m*sizeof(long));

	return mat;
}

long **copy_long_matrix(long **dest, long **src, int n, int m){
	int i;

	if(!dest)
		dest = malloc_long_matrix(n, m);

	for( i = 0 ; i < n ; i++ )
		copy_long_array(dest[i], src[i], m);
	return dest;
}

long *read_long_array(FILE *in, long *array, int n){
	int i, nf;

	if(!array) array = (long*)malloc(n*sizeof(long));

	for( i = 0 ; i < n ; i++ )
		nf = fscanf(in, "%ld", &(array[i]));
	
	return array;
}

void write_long_array(FILE *out, long *array, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(out, "%ld ", array[i]);
	fprintf(out, "%ld\n", array[i]);
	return ;
}

void write_long_matrix(FILE *out, long **mat, int n, int m){
	int i;
	for( i = 0 ; i < n ; i++ )
		write_long_array(out, mat[i], m);

	return;
}

long **read_long_matrix(FILE *in, long **mat, int n, int m){
	int i, j;

	if(!mat) mat = malloc_long_matrix(n, m);
	
	for( i = 0 ; i < n ; i++ )
		mat[i] = read_long_array(in, mat[i], m);
	
	return mat;
}

long *fprint_long_array(FILE *out, long *array, int n){
	int i;
	for( i = 0 ; i < n ; i++ )
		fprintf(out, "%ld%s", array[i], (i < n-1) ? " " : "\n" );
	
	return;
}

int **fprint_long_matrix(FILE *out, long **mat, int n, int m){
	int i;
	for( i = 0 ; i < n ; i++ )
		fprint_long_array(out, mat[i], m);
	
	return;
}

long **fprint_long_matrix_tranlated(FILE *out, long **mat, int n, int m){
	int i, j;

	for( j = 0 ; j < m ; j++ )
		for( i = 0 ; i < n ; i++ )
			fprintf(out, "%ld%s", mat[i][j], (i < n-1) ? " " : "\n" );
	
	return;
}

void free_long_array(long *array){
	free(array);
}

void free_long_matrix(long **mat, int n){
	int i;
	for( i = 0 ; i < n ; i++ )
		free(mat[i]);
	free(mat);
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
void zimpl_print_long_matrix(FILE *fout, long **mat, int nlin, int ncol){
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
void zimpl_print_long_array(FILE *fout, long *array, int n){
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
