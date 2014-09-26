#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "mmkp.h"

/*
 * Reads a MMKP instance from a file, givens its name.
 *  filename: name of file
 */
MMKP *mmkp_read_from_filename(char *filename){
	FILE *fin = fopen(filename, "r");
	MMKP *mmkp = mmkp_read_from_file(fin);
	fclose(fin);
	return mmkp;
}

/*
 * Reads a MMKP instance from a FILE.
 *   fin: input FILE
 */
MMKP *mmkp_read_from_file(FILE *fin){
	int n, m, o, i, j, nf;
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
	long_array_read(fin, mmkp->p, n);
	/* Reading weights */
	long_matrix_read(fin, mmkp->w, m, n);
	/* Reading capacities */
	long_matrix_read(fin, mmkp->b, m, o);

	 return mmkp;
}

/*
 * Saves a MMKP instance into a file.
 *   mmkp: the MMKP instance
 *   filename: name of the output file
 */
void mmkp_write_to_filename(MMKP *mmkp, char *filename){
	FILE *fout = fopen(filename, "w");
	mmkp_write_to_file(mmkp, fout);
	fclose(fout);
	return;
}

/*
 * Reads a MMKP instance from a gzipped file.
 *   filename: name of the file
 */
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
	int n, m, o, i, j, nf;

	n = mmkp->n;
	m = mmkp->m;
	o = mmkp->o;

	/* writing  sizes */
	fprintf(fout , "%d\n", n);
	fprintf(fout, "%d\n", m);
	fprintf(fout, "%d\n", o);

	/* Writing profits */
	long_array_fprint(fout, mmkp->p, n);
	/* Writing weights */
	long_matrix_fprint(fout, mmkp->w, m, n);
	/* Writing capacities */
	long_matrix_fprint(fout, mmkp->b, m, o);

	return;
}

/*
 * Returns a random MMKP instance.
 *   n: number of itens
 *   m: number of dimensions
 *   o: number of knapsacks
 *   beta: ratio of knapsack capacity over total weight of itens
 */
MMKP *mmkp_random(int n, int m, int o, double beta){
	int i, j, k;
	long tot_weights[m];
	double wsum;
	double *cdist; // capacities distribution

	MMKP *mmkp;
	
	mmkp = mmkp_alloc(n, m, o);

	/* Random profits */
	for( i = 0 ; i < n ; i++ )
		mmkp->p[i] = lrand(MAX_RAND_PROFIT);

	/* Random weights */
	for( j = 0 ; j < m ; j++)
		for( i = 0 ; i < n ; i++ )
			mmkp->w[j][i] = lrand(MAX_RAND_WEIGHT);
	
	/* Computing total weight for each dimension */
	for( j = 0 ; j < m ; j++){
		tot_weights[j] = 0;
		for( i = 0 ; i < n ; i++ )
			tot_weights[j] += mmkp->w[j][i];
	}

	/* Random capacities (each knapsack has a "global" size
	 * which is used to set the capacity for each demantion). */
	cdist = random_normalized_double_array(o);

	for( k = 0 ; k < o ; k++ )
		for( j = 0 ; j < m ; j++ )
			mmkp->b[j][k] = (long)(cdist[k] * tot_weights[j] * beta);
	free(cdist);

	return mmkp;
}

MMKP *mmkp_alloc(int n, int m, int o){
	int j;

	MMKP *mmkp = (MMKP*)malloc(sizeof(MMKP));
	mmkp->p = long_array_malloc(n);
	mmkp->w = long_matrix_malloc(m, n);
	mmkp->b = long_matrix_malloc(m, o);

	mmkp->n = n;
	mmkp->m = m;
	mmkp->o = o;

	return mmkp;
}

/*
 * Frees an MMKP from memory.
 *   mmkp: the MMKP instance
 */
void mmkp_free(MMKP *mmkp){
	int j, m;

	m = mmkp->m;

	long_matrix_free(mmkp->w, mmkp->m);
	long_matrix_free(mmkp->b, mmkp->m);
	free(mmkp->p);
	free(mmkp);

	return;
}

void mmkp_fprint(FILE *out, MMKP *mmkp){
	int i, j, n, m, o;

	n = mmkp->n;
	m = mmkp->m;
	o = mmkp->o;

	fprintf(out, " Itens: %d\n", mmkp->n);
	fprintf(out, " Dimen: %d\n", mmkp->m);
	fprintf(out, " Knapsacks: %d\n", mmkp->o);

	fprintf(out, " Profits:\n");
	long_array_fprint(out, mmkp->p, n);

	fprintf(out, " Weights:\n");
	long_matrix_fprint(out, mmkp->w, m, n);

	fprintf(out, " Capacities:\n");
	long_matrix_fprint(out, mmkp->b, m, o);

	return;
}

void mmkp_to_zimpl(FILE *fout, MMKP *mmkp){
	int i, j, o, n, m;

	n = mmkp->n;
	m = mmkp->m;
	o = mmkp->o;

	/* SIZES */
	fprintf(fout, "param n := %d;\n", n); /* # of itens */
	fprintf(fout, "param m := %d;\n", m); /* # of dimensions */
	fprintf(fout, "param o := %d;\n", o); /* # of knapsacks */

	/* SETS */
	fprintf(fout, "set N := {1 .. %d};\n", n); /* itens */
	fprintf(fout, "set M := {1 .. %d};\n", m); /* dimensions */
	fprintf(fout, "set O := {1 .. %d};\n", o); /* knapsacks */

	/* PARAMETERS*/
	fprintf(fout, "param p[N] :=\n"); /* profit */
	long_array_zimpl_print(fout, mmkp->p, n);

	fprintf(fout, "param w[M*N] :=\n"); /* weights */
	long_matrix_zimpl_print(fout, mmkp->w, m, n);

	fprintf(fout, "param b[M*O] :=\n"); /* capacities */
	long_matrix_zimpl_print(fout, mmkp->b, m, o);

	/* DECISION VARIABLES */
	fprintf(fout, "var x[O*N] binary;\n");

	/* CONSTRAINTS */
	fprintf(fout, /* capacities constraint */
		"subto capacities:\n\
			forall <j, k> in M*O do\n\
				sum <i> in N do\n\
					x[k, i]*w[j, i] <= b[j, k];\n");

	fprintf(fout, /* Single-Knapsack-item constraint */
		"subto singleknapsack:\n\
			forall <i> in N do\n\
				sum <k> in O do\n\
					x[k, i] <= 1;\n");

	/* OBJECTIVE FUNCTION */
	fprintf(fout, /* total profit of selected itens */
		"maximize profit:\n\
			sum <k, i> in O*N do\n\
				x[k, i]*p[i];\n");

	return;
}

MMKPSol *mmkpsol_new(MMKP *mmkp){
	int i, j, k, n, m, o;
	long lsum;
	MMKPSol *mmkpsol;

	n = mmkp->n;
	m = mmkp->m;
	o = mmkp->o;
	
	/* allocs */
	mmkpsol = (MMKPSol*)malloc(sizeof(MMKPSol));
	mmkpsol->x = long_matrix_malloc(o, n);
	mmkpsol->x_used = long_array_malloc(n);
	mmkpsol->b_left = long_matrix_malloc(m, o);

	/* assingnments*/
	long_matrix_init(mmkpsol->x, o, n, 0);
	long_array_init(mmkpsol->x_used, n, 0);
	long_matrix_copy(mmkpsol->b_left, mmkp->b, m, o);
	mmkpsol->obj = 0;
	mmkpsol->viable = 1;
	mmkpsol->mmkp = mmkp;

	return mmkpsol;
}

void mmkpsol_free(MMKPSol *mmkpsol){
	int n, m, o;

	n = mmkpsol->mmkp->n;
	m = mmkpsol->mmkp->m;
	o = mmkpsol->mmkp->o;

	long_matrix_free(mmkpsol->x, o);
	long_array_free(mmkpsol->x_used);
	long_matrix_free(mmkpsol->b_left, m);
	free(mmkpsol);

	return;
}

