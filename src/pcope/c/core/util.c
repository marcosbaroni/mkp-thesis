#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"

/********************************************************************
 *                     RANDOM NUMBER GENERATION
 ********************************************************************/
double gauss_stash = 0.0;

int randint(int a, int b){
	int d = (b-a+1);
	return (a + (rand() % d));
}

double randd(){
	return (rand()/((double)RAND_MAX));
}

double randd2(double min, double var){
	return min+var*randd();
}

double gauss(){
	float x1, x2, w, y;

	if( gauss_stash != 0.0 ){
		y = gauss_stash;
		gauss_stash = 0.0;
		return y;
	}

	do{
		x1 = randd2(-1., 2.);
		x2 = randd2(-1., 2.);
		w = x1*x1 + x2*x2;
	}while( w >= 1.0);
	w = sqrt((-2.*log(2))/w);
	y = x1*w;
	gauss_stash = x2*w;
	return y;
}

double gauss2(double med, double std_var){
	return med + std_var*gauss();
}

/* Random fill a double vector with values totaling "total".
 *   Each value will have a min value of (min_rate*total)/n.
 * */
double *double_rand_fill_with_total(double *v, int n, double total, double min_rate){
	int i;
	double sum, min, rest;

	min = (min_rate*total)/((double)n);
	rest = total*(1.0 - min_rate);

	sum = 0;
	/* Random filling */
	for( i = 0 ; i < n ; i++ )
		{ v[i] = randd(); sum += v[i]; }
	for( i = 0 ; i < n ; i++ )
		v[i] = min + (v[i]*rest)/sum;
	return v;
}

/*
 * Fills an int array with random values, totaling "total"
 *    on a way each array element was a rate of "min_rate" of the mean.
 *
 *    Ex.:
 *    	int_rand_fill_with_total(v, 5, 50, 1.0) -> [10, 10, 10, 10, 10] (min=10)
 *    	int_rand_fill_with_total(v, 5, 50, 0.0) -> [12, 14, 00, 22, 02]   (min=0)
 *    	int_rand_fill_with_total(v, 5, 50, 0.0) -> [08, 15, 12, 10, 05]   (min=5)
 */
int *int_rand_fill_with_total(int *v, int n, int total, double min_rate){
	int i;
	int min, sum, tot_rest, left;
	if(DEBUG) printf(" -total:%d\n", total);

	min = (min_rate*total)/n;   // minimum value for each element (rate of total)
	tot_rest = total - (n*min);

	sum = 0;
	/* Random filling */
	for( i = 0 ; i < n ; i++ )
		{ v[i] = randint(0, 100000); sum += v[i]; }
	left = total;
	for( i = 0 ; i < n-1 ; i++ ){
		v[i] = rint(min + v[i]*(tot_rest/(double)sum));
		left -= v[i];
	}
	v[i] = left;
	return v;
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

/********************************************************************
 *                     ARRAY / MATRIX DEBBUGING UTILS
 ********************************************************************/
 /* Prints an array of int.  */
void fprint_int_array(FILE *out, int *v, int n, char conf){
	int i;
	for( i = 0 ; i < n ; i++ )
		fprintf(out, "%d ", v[i]);
	fprintf(out, "\n");
	return;
}

 /* Returns the sum of an int array. */
int sum_int_array(int *v, int n){
	int i, sum;
	sum = 0;
	for( i = 0 ; i < n ; i++ )
		sum += v[i];
	return sum;
}

 /* Prints an array of double.  */
void fprint_double_array(FILE *out, double *v, int n, char conf){
	int i;
	for( i = 0 ; i < n ; i++ )
		fprintf(out, "%.2f ", v[i]);
	fprintf(out, "\n");
	return;
}

 /* Returns the sum of a double array. */
double sum_double_array(double *v, int n){
	int i;
	double sum;
	sum = 0;
	for( i = 0 ; i < n ; i++ )
		sum += v[i];
	return sum;
}

/********************************************************************
 *                     SCIP FORMATTING
 ********************************************************************/
 /* Prints an array of int on scip format.  */
void fprint_scip_int_array(FILE *fout, int *v, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(fout, "<%d> %d,\n", i+1, v[i]);
	fprintf(fout, "<%d> %d;\n", i+1, v[i]);
	return;
}

 /* Prints a matrix of int on scip format.  */
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

 /* Prints an array of double on scip format.  */
void fprint_scip_double_array(FILE *fout, double *v, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ )
		fprintf(fout, "<%d> %lf,\n", i+1, v[i]);
	fprintf(fout, "<%d> %lf;\n", i+1, v[i]);
	return;
}

 /* Prints double matrix on scip format.
  *   - rev: If index is reversed */
void fprint_scip_double_matrix(FILE *fout, double **v, int n, int m, char rev){
	int i, j;

	if(rev)
		{ i = n; n = m; m = i; } /* swap */

	/* header */
	fprintf(fout, "|");
	for( j = 0 ; j < m-1 ; j++)
		fprintf(fout, "%d,", j+1);
	fprintf(fout, "%d|\n", j+1);

	/* rows */
	for( i = 0 ; i < n ; i++ ){
		fprintf(fout, "|%d|", i+1);
		for( j = 0 ; j < m-1 ; j++ )
			fprintf(fout, "%f, ", rev ? v[j][i] : v[i][j]);
		fprintf(fout, "%f|%s\n", rev ? v[j][i] : v[i][j], (i == n-1) ? ";" : "" );
	}
	return;
}

