#include <stdlib.h>
#include <stdio.h>

#define ISNUM(c) ( (c > 47) && (c < 58) )

/*
 * Prints a double matrix on the ZIMPL MIP modeling language format.
 *   fout - output FILE
 *   nlin - number of lines on matrix
 *   ncol - numer of columns on matrix
 *   mat  - the double matrix */
void zimpl_print_matrix(FILE *fout, int nlin, int ncol, double **mat){
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
void zimpl_print_array(FILE *fout, int n, double *array){
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

	vec = (int*)malloc(nmax*sizeof(int));

	/* FIXME */
	*n = 0;
	while( res = sscanf(str, "%d", &(vec[(*n)])) > 0){
		if( res > 0 )
			(*n)++;
		while( *str != '\0' && ISNUM(*str) )
			str++;
		while( *str != '\0' && !ISNUM(*str) )
			str++;
	}

	return vec;
}

