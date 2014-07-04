#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define ISNUM(c) ( (c > 47) && (c < 58) )

double irand(int bound){
	return (rand() % bound);
}

double drand(){
	return rand()/((float)RAND_MAX);
}

int *read_int_array(FILE *in, int n){
	int i, nf;
	int *array;
	array = (int*)malloc(n*sizeof(int));

	for( i = 0 ; i < n ; i++ )
		nf = fscanf(in, "%d", &(array[i]));
	
	return array;
}

int **read_int_matrix(FILE *in, int n, int m){
	int i, j;
	int **mat;

	mat = (int**)malloc(n*sizeof(int*));
	
	for( i = 0 ; i < n ; i++ )
		mat[i] = read_int_array(in, m);
	
	return mat;
}

int *fprint_int_array(FILE *out, int *array, int n){
	int i;
	for( i = 0 ; i < n ; i++ )
		fprintf(out, "%d%s", array[i], (i < n-1) ? " " : "\n" );
	
	return;
}

int **fprint_int_matrix(FILE *out, int **mat, int n, int m){
	int i, j;
	for( i = 0 ; i < n ; i++ )
		fprint_int_array(out, mat[i], m);
	
	return;
}

void free_int_array(int *array){
	free(array);
}

void free_int_matrix(int **mat, int n){
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
void zimpl_print_matrix(FILE *fout, double **mat, int nlin, int ncol){
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
 *   n     - number of elements on array
 *   array - the double array */
void zimpl_print_array(FILE *fout, double *array, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(fout, "<%d> %lf,\n", i+1, array[i]);
	fprintf(fout, "<%d> %lf;\n", i+1, array[n-1]);

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

void gunzip(FILE *in, FILE *out){
}

