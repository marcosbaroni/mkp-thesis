#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"
#include "mkp.h"

long MAX_MKP_COEFICIENT = 100;    /* for random generation */

/*** memory management ***/
MKP *mkp_alloc(int n, int m){
	MKP *mkp;

	mkp = (MKP*)malloc(sizeof(MKP));
	mkp->p = malloc_long_array(n);
	mkp->w = malloc_long_matrix(m, n);
	mkp->b = malloc_long_array(n);
	mkp->n = n;
	mkp->m = m;

	return mkp;
}

/*
 * Returns a random MKP problem with coeficients taken from uniform
 *    distribution.
 */
MKP *mkp_random(int n, int m, double beta){
	int i, j;
	long lsum;
	MKP *mkp = mkp_alloc(n, m);

	/* profit */
	for( i = 0 ; i < n ; i++ )
		mkp->p[i] = lrand(MAX_MKP_COEFICIENT);
	/* weight */
	for( i = 0 ; i < m ; i++ ){
		lsum = 0;
		for( j = 0 ; j < n ; j++ )
			lsum += mkp->w[i][j] = lrand(MAX_MKP_COEFICIENT);
		mkp->b[i] = (long)(ceil(lsum*beta));
	}

	return mkp;
}

void mkp_free(MKP *mkp){
	free_long_matrix(mkp->w, mkp->m);
	free(mkp->p);
	free(mkp->b);
	free(mkp);

	return;
}

MKP *mkp_read_from_filename(char *filename){
	unimplemented();

	return mkp_random(10, 1, 0.5);
}

MKP *mkp_read_from_file(FILE *fin){
	int n, m;
	MKP *mkp;
	
	fscanf(fin, "%d", &n);
	fscanf(fin, "%d", &m);

	mkp = mkp_alloc(n, m);
	mkp->n = n;
	mkp->m = m;
	read_long_array(fin, mkp->p, n);
	read_long_matrix(fin, mkp->w, m, n);
	read_long_array(fin, mkp->b, n);

	return mkp;
}

void mkp_write_to_filename(MKP *mkp, char *filename){
	FILE *fout;
	fout = fopen(filename, "w");
	mkp_write_to_file(mkp, fout);
	return ;
}

void mkp_write_to_file(MKP *mkp, FILE *fout){
	int i, j;

	fprintf(fout, "%d\n%d\n", mkp->n, mkp->m);
	write_long_array(fout, mkp->p, mkp->n);
	write_long_matrix(fout, mkp->w, mkp->m, mkp->n);
	write_long_array(fout, mkp->b, mkp->m);

	return;
}

void mkp_to_zimpl(FILE *fout, MKP *mkp){
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
	zimpl_print_long_array(fout, mkp->p, n);

	/* weights */
	fprintf(fout, "param w[M*N] :=\n");
	zimpl_print_long_matrix(fout, mkp->w, m, n);

	/* capacities */
	fprintf(fout, "param b[M] :=\n");
	zimpl_print_long_array(fout, mkp->b, m);

	/* desicion var */
	fprintf(fout, "var x[N] binary;\n");

	/* capacities constraint */
	fprintf(fout,
		"subto capacities:\n\
			forall <j> in M do\n\
				sum <i> in N do\n\
					x[i]*w[j, i] <= b[j];\n");

	/* objective function */
	fprintf(fout,
		"maximize profit:\n\
			sum <i> in N do\n\
				x[i]*p[i];\n");

	return;
}

MKPSol *mkpsol_new(MKP *mkp){
	int i;
	MKPSol *mkpsol;

	mkpsol = (MKPSol*)malloc(sizeof(MKPSol));
	mkpsol->x = malloc_long_array(mkp->n);
	mkpsol->b_left = copy_long_array(mkpsol->b_left, mkp->b, mkp->m);
	mkpsol->obj = 0;
	mkpsol->viable = 1;
	mkpsol->mkp = mkp;

	return mkpsol;
}

void mkpsol_free(MKPSol *mkpsol){
	free(mkpsol->x);
	free(mkpsol->b_left);
	free(mkpsol);
	return;
}

