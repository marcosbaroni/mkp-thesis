#include <stdlib.h>
#include <string.h>
#include "util.h"

double randd(){
	return (rand()/((double)RAND_MAX));
}

double randd2(double min, double var){
	return min+var*randd();
}

int distributed_rand_int(double *dist, int n){
	double aux, r, sum;
	int i;

	sum = 0;
	for( i = 0 ; i < n ; i++ )
		sum += dist[i];

	r = (rand()/((double)RAND_MAX))*sum;
	aux = dist[0];
	i = 0;

	while( r > aux )
		aux += dist[++i];

	return i;
}

void fprint_scip_int_array(FILE *fout, int *v, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(fout, "<%d> %d,\n", i+1, v[i]);
	fprintf(fout, "<%d> %d;\n", i+1, v[i]);
	return;
}

void fprint_scip_double_array(FILE *fout, double *v, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(fout, "<%d> %lf,\n", i+1, v[i]);
	fprintf(fout, "<%d> %lf;\n", i+1, v[i]);
	return;
}

void fprint_scip_int_matrix(FILE *fout, int **v, int n, int m){
	int i, j;
	/* header */
	fprintf(fout, "|");
	for( j = 0 ; j < m-1 ; j++)
		fprintf(fout, "%d,", j+1);
	fprintf(fout, "%d|\n", j+1);

	/* rows */
	for( i = 0 ; i < n ; i++ ){
		fprintf(fout, "|%d|", i+1);
		for( j = 0 ; j < m-1 ; j++ )
			fprintf(fout, "%d, ", v[i][j]);
		fprintf(fout, "%d|%s\n", v[i][j], (i == n-1) ? ";" : "" );
	}
	return;
}

void fprint_scip_double_matrix(FILE *fout, double **v, int n, int m){
	int i, j;
	/* header */
	fprintf(fout, "|");
	for( j = 0 ; j < m-1 ; j++)
		fprintf(fout, "%d,", j+1);
	fprintf(fout, "%d|\n", j+1);

	/* rows */
	for( i = 0 ; i < n ; i++ ){
		fprintf(fout, "|%d|", i+1);
		for( j = 0 ; j < m-1 ; j++ )
			fprintf(fout, "%f, ", v[i][j]);
		fprintf(fout, "%f|%s\n", v[i][j], (i == n-1) ? ";" : "" );
	}
	return;
}

