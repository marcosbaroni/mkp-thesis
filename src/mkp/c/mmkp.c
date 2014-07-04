#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "mmkp.h"

MMKP *mmkp_read_from_filename(char *filename){
	FILE *fin = fopen(filename, "r");
	MMKP *mmkp = mmkp_read_from_file(fin);
	fclose(fin);
	return mmkp;
}

MMKP *mmkp_read_from_file(FILE *fin){
	int n, m, i, j, nf;
	MMKP *mmkp;
	mmkp = (MMKP*)malloc(sizeof(MMKP));

	/* Reading sizes */
	nf = fscanf(fin, "%d", &n);
	nf = fscanf(fin, "%d", &m);
	nf = fscanf(fin, "%d", &o);

	mmkp->n = n;
	mmkp->m = m;
	mmkp->o = o;

	/* Reading profits */
	mmkp->p = read_NUMBER_array(fin, n);
	/* Reading weights */
	mmkp->w = read_NUMBER_matrix(fin, n, m);
	/* Reading capacities */
	mmkp->w = read_NUMBER_matrix(fin, m, o);

	 return mmkp;
}

void mmkp_write_to_filename(MMKP *mmkp, char *filename){
	FILE *fout = fopen(filename, "w");
	mmkp_write_to_file(mmkp, fout);
	fclose(fout);
	return;
}

MMKP *mmkp_read_from_gzip(char *filename){
	char cmd[300];
	FILE *pipe;
	MMKP *mmkp;

	sprintf(cmd, "zcat %s", filename);
	pipe = popen(cmd, "r");
	mmkp = mmkp_read_from_file(pipe);
	pclose(pipe);

	return mmkp;
}

void mmkp_write_to_gzip(MMKP *mmkp, char *filename){
	char cmd[300];
	FILE *pipe;

	sprintf(cmd, "gzip -cf > %s", filename);
	pipe = popen(cmd, "w");
	mmkp_write_to_file(mmkp, pipe);
	pclose(pipe);

	return;
}

void mmkp_write_to_file(MMKP *mmkp, FILE *fout){
	int n, m, i, j, nf;

	n = mmkp->n;
	m = mmkp->m;

	/* writing  sizes */
	fprintf(fout , "%d ", n);
	fprintf(fout, "%d \n", m);

	/* Writing profits */
	for( i = 0 ; i < n ; i++ )
		fprintf(fout, "%f ", mmkp->p[i]);
	fprintf(fout, "\n");

	/* Writing weights */
	for( j = 0 ; j < m ; j++ ){
		for( i = 0 ; i < n ; i++ )
			fprintf(fout, "%lf ", mmkp->w[j][i]);
		fprintf(fout, "\n");
	}

	/* Writing capacities */
	for( j = 0 ; j < m ; j++ )
		fprintf(fout, "%lf ", mmkp->b[j]);
	fprintf(fout, "\n");
	return;
}

MMKP *mmkp_random(int n, int m){
	int i, j;
	double wsum;
	MMKP *mmkp;
	
	mmkp = mmkp_alloc(n, m);

	/* random profits */
	for( i = 0 ; i < n ; i++ )
		mmkp->p[i] = drand();

	/* random weights */
	for( j = 0 ; j < m ; j++){
		wsum = 0.0;
		for( i = 0 ; i < n ; i++ )
			wsum +=  (mmkp->w[j][i] = drand());
		/* random capacities [0, 0.5]*sum(w[*,m] )*/
		mmkp->b[j] = wsum*0.5*drand();
	}

	return mmkp;
}

MMKP *mmkp_alloc(int n, int m){
	int j;

	MMKP *mmkp = (MMKP*)malloc(sizeof(MMKP));           /* mmkp */
	mmkp->p = (double*)malloc(n*sizeof(double));     /* allocing profits */
	mmkp->w = (double**)malloc(m*sizeof(double*));   /* allocing weights */
	for( j = 0 ; j < m ; j++ )
		mmkp->w[j] = (double*)malloc(n*sizeof(double));
	mmkp->b = (double*)malloc(m*sizeof(double*));    /* allocing capacities */

	mmkp->n = n;
	mmkp->m = m;

	return mmkp;
}

void mmkp_free(MMKP *mmkp){
	int j, m;

	m = mmkp->m;

	for( j = 0 ; j < m ; j++ )  /* free weights */
		free(mmkp->w[j]);
	free(mmkp->w);
	free(mmkp->b);              /* free capacities */
	free(mmkp->p);              /* free profits */
	free(mmkp);                 /* free mmkp */

	return;
}

void mmkp_to_zimpl(MMKP *mmkp, FILE *fout){
	int i, j, n, m;

	n = mmkp->n;
	m = mmkp->m;

	/* sizes */
	fprintf(fout, "param n := %d;\n", n);
	fprintf(fout, "param m := %d;\n", m);

	/* sets */
	fprintf(fout, "set N := {1 .. %d};\n", n);
	fprintf(fout, "set M := {1 .. %d};\n", m);

	/* profit */
	fprintf(fout, "param p[N] :=\n");
	zimpl_print_array(fout, mmkp->p, n);

	/* weights */
	fprintf(fout, "param w[M*N] :=\n");
	zimpl_print_matrix(fout, mmkp->w, m, n);

	/* capacities */
	fprintf(fout, "param b[M] :=\n");
	zimpl_print_array(fout, mmkp->b, m);

	/* desicion var */
	fprintf(fout, "var x[N] binary;\n");

	/* capacities constraint */
	fprintf(fout, "subto capacities:\nforall <j> in M do\nsum <i> in N do\nx[i]*w[j, i] <= b[j];\n");

	/* objective function */
	fprintf(fout, "maximize profit:\n sum <i> in N do\n x[i]*p[i];\n");

	return;
}

