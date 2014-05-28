#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "mkp.h"

double urand(){
	return rand()/((float)RAND_MAX);
}

MKP *mkp_read_from_filename(char *filename){
	FILE *fin = fopen(filename, "r");
	MKP *mkp = mkp_read_from_file(fin);
	fclose(fin);
	return mkp;
}

MKP *mkp_read_from_file(FILE *fin){
	int n, m, i, j, nf;
	MKP *mkp;

	/* Reading sizes */
	nf = fscanf(fin, "%d", &n);
	nf = fscanf(fin, "%d", &m);

	/* allocing */
	mkp = mkp_alloc(n, m);

	/* Reading profits */
	for( i = 0 ; i < n ; i++ )
		nf = fscanf(fin, "%lf", &(mkp->p[i]));

	/* Reading weights */
	for( j = 0 ; j < m ; j++ )
		for( i = 0 ; i < n ; i++ )
			nf = fscanf(fin, "%lf", &(mkp->w[j][i]));

	/* Reading capacities */
	for( j = 0 ; j < m ; j++ )
		nf = fscanf(fin, "%lf", &(mkp->b[j]));

	 return mkp;
}

void mkp_write_to_filename(MKP *mkp, char *filename){
	FILE *fout = fopen(filename, "w");
	mkp_write_to_file(mkp, fout);
	fclose(fout);
	return;
}

void mkp_write_to_file(MKP *mkp, FILE *fout){
	int n, m, i, j, nf;

	n = mkp->n;
	m = mkp->m;

	/* writing  sizes */
	fprintf(fout , "%d ", n);
	fprintf(fout, "%d \n", m);

	/* Writing profits */
	for( i = 0 ; i < n ; i++ )
		fprintf(fout, "%f ", mkp->p[i]);
	fprintf(fout, "\n");

	/* Writing weights */
	for( j = 0 ; j < m ; j++ ){
		for( i = 0 ; i < n ; i++ )
			fprintf(fout, "%lf ", mkp->w[j][i]);
		fprintf(fout, "\n");
	}

	/* Writing capacities */
	for( j = 0 ; j < m ; j++ )
		fprintf(fout, "%lf ", mkp->b[j]);
	fprintf(fout, "\n");
	return;
}

MKP *mkp_random(int n, int m, int seed){
	int i, j;
	double wsum;
	MKP *mkp;
	
	srand(seed);

	mkp = mkp_alloc(n, m);

	/* random profits */
	for( i = 0 ; i < n ; i++ )
		mkp->p[i] = urand();

	/* random weights */
	for( j = 0 ; j < m ; j++){
		wsum = 0.0;
		for( i = 0 ; i < n ; i++ )
			wsum +=  mkp->w[j][i] = urand();
		/* random capacities [0, 0.5]*sum(w[*,m] )*/
		mkp->b[j] = wsum*0.5*urand();
	}

	return mkp;
}

MKP *mkp_alloc(int n, int m){
	int j;

	MKP *mkp = (MKP*)malloc(sizeof(MKP));           /* mkp */
	mkp->p = (double*)malloc(n*sizeof(double));     /* allocing profits */
	mkp->w = (double**)malloc(m*sizeof(double*));   /* allocing weights */
	for( j = 0 ; j < m ; j++ )
		mkp->w[j] = (double*)malloc(n*sizeof(double));
	mkp->b = (double*)malloc(m*sizeof(double*));    /* allocing capacities */

	mkp->n = n;
	mkp->m = m;

	return mkp;
}

void mkp_free(MKP *mkp){
	int j, m;

	m = mkp->m;

	for( j = 0 ; j < m ; j++ )  /* free weights */
		free(mkp->w[j]);
	free(mkp->w);
	free(mkp->b);              /* free capacities */
	free(mkp->p);              /* free profits */
	free(mkp);                 /* free mkp */

	return;
}

void mkp_to_zimpl(MKP *mkp, FILE *fout){
	int i, j, n, m;

	n = mkp->n;
	m = mkp->m;

	/* sizes */
	fprintf(fout, "param n := %d;\n", n);
	fprintf(fout, "param m := %d;\n", m);

	/* sets */
	fprintf(fout, "set N := {1 .. %d};\n", n);
	fprintf(fout, "set M := {1 .. %d};\n", m);

	/* profit */
	fprintf(fout, "param p[N] :=\n");
	zimpl_print_array(fout, n, mkp->p);

	/* weights */
	fprintf(fout, "param w[M*N] :=\n");
	zimpl_print_matrix(fout, m, n, mkp->w);

	/* capacities */
	fprintf(fout, "param b[M] :=\n");
	zimpl_print_array(fout, m, mkp->b);

	/* desicion var */
	fprintf(fout, "var x[N] binary;\n");

	/* capacities constraint */
	fprintf(fout, "subto capacities:\nforall <j> in M do\nsum <i> in N do\nx[i]*w[j, i] <= b[j];\n");

	/* objective function */
	fprintf(fout, "maximize profit:\n sum <i> in N do\n x[i]*p[i];\n");

	return;
}

