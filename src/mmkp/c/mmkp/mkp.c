#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
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
	MKP *mkp;
	FILE *fin;

	assert_faccess(filename, R_OK);
	fin = fopen(filename, "r");

	mkp = mkp_read_from_file(fin);
	fclose(fin);

	return mkp;
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
	assert_faccess(filename, W_OK);
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

void mkp_fprint(FILE *fout, MKP *mkp){
	int i, j, n, m, ndigs[mkp->n+1];
	char format[20];
	long max;

	n = mkp->n;
	m = mkp->m;

	/* Deciding ndigits */
	for( i = 0 ; i < n ; i++ ){
		max = max_long_matrix_col(mkp->w, m, n, i);
		max = MAX(max, mkp->p[i]);
		ndigs[i] = 1+(int)(floor(log(max)/log(10.)));
	}
	ndigs[n] = (int)(ceil(log(max_long_array(mkp->b, mkp->m)) / log(10.)));

	/* print profits*/
	for( i = 0 ; i < n ; i++ ){
		sprintf(format, "%%%dd ", ndigs[i]);
		fprintf(fout, format, mkp->p[i]);
	}
	fprintf(fout, "\n");

	for( i = 0 ; i < n-1 ; i++ ){
		for( j = 0 ; j < ndigs[i] ; j++ )
			fprintf(fout, "-");
		fprintf(fout, "-");
	}
	for( j = 0 ; j < ndigs[n-1] ; j++ )
		fprintf(fout, "-");
	fprintf(fout, "\n");

	/* print constraints */
	for( i = 0 ; i < m ; i++ ){
		/* weights */
		for( j = 0 ; j < n ; j++ ){
			sprintf(format, "%%%dd ", ndigs[j]);
			fprintf(fout, format, mkp->w[i][j]);
		}
		/* capacity */
		sprintf(format, "%%%dd ", ndigs[n]);
		fprintf(fout, "| ");
		fprintf(fout, format, mkp->b[i]);
		fprintf(fout, "\n");
	}

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

MKPSol *mkpsol_add_item(MKPSol *mkpsol, int a){
	MKP *mkp;
	int i, m;

	mkp = mkpsol->mkp;
	m = mkp->m;

	if(mkpsol->x[a]) {
		fprintf(stderr, "%s error: item %d-th item already in knapsak.\n",
			__PRETTY_FUNCTION__, a+1);
		return mkpsol;
	}

	mkpsol->x[a] = 1;
	mkpsol->obj += mkp->p[a];
	for( i = 0 ; i < m ; i++ ){
		mkpsol->b_left -= mkp->w[i][a];
		if(mkpsol->b_left <= 0)
			mkpsol->viable = 0;
	}

	return mkpsol;
}

MKPSol *mkpsol_rm_item(MKPSol *mkpsol, int a){
	MKP *mkp;
	int i, m;

	mkp = mkpsol->mkp;
	m = mkp->m;

	if(mkpsol->x[a]) {
		fprintf(stderr, "%s error: item %d-th item not in knapsak.\n",
			__PRETTY_FUNCTION__, a+1);
		return mkpsol;
	}

	mkpsol->x[a] = 0;
	mkpsol->obj -= mkp->p[a];
	for( i = 0 ; i < m ; i++ ){
		mkpsol->b_left += mkp->w[i][a];
	}

	return mkpsol;
}

MKPSol *mkpsol_read_from_filename(char *filename, MKP *mkp){
	MKPSol *mkpsol;
	FILE *fin;

	assert_faccess(filename, R_OK);
	fin = fopen(filename, "r");
	mkpsol = mkpsol_read_from_file(fin, mkp);
	fclose(fin);

	return mkpsol;
}

MKPSol *mkpsol_read_from_file(FILE *fin, MKP *mkp){
	MKPSol *mkpsol;
	int i, a, n;

	n = mkp->n;
	mkpsol = mkpsol_new(mkp);
	for( i = 0 ; i < n ; i++ ){
		fscanf(fin, "%d", &a);
		mkpsol_add_item(mkpsol, a);
	}

	return mkpsol;
}

MKPSol *mkpsol_copy(MKPSol *mkpsol){
	MKPSol *mkpsol_new;

	mkpsol_new = (MKPSol*)malloc(sizeof(MKPSol));
	mkpsol_new->x = copy_long_array(NULL, mkpsol->x, mkpsol->mkp->n);
	mkpsol_new->b_left = copy_long_array(NULL, mkpsol->b_left, mkpsol->mkp->m);
	mkpsol_new->obj = mkpsol->obj;
	mkpsol_new->viable = mkpsol->viable;
	mkpsol_new->mkp = mkpsol->mkp;

	return mkpsol_new;
}

void mkpsol_print(FILE *fout, MKPSol *mkpsol){
	
}



void mkpsol_free(MKPSol *mkpsol){
	free(mkpsol->x);
	free(mkpsol->b_left);
	free(mkpsol);
	return;
}

MKPSol *tabu_mkp(MKPSol *mkpsol){
	unimplemented();
}

