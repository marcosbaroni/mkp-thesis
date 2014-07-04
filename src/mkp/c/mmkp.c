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
	mmkp->p = read_long_array(fin, n);
	/* Reading weights */
	mmkp->w = read_long_matrix(fin, n, m);
	/* Reading capacities */
	mmkp->w = read_long_matrix(fin, m, o);

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
	fprintf(fout , "%d ", n);
	fprintf(fout, "%d \n", m);
	fprintf(fout, "%d \n", o);

	/* Writing profits */
	fprint_long_array(fout, mmkp->p, n);
	/* Writing weights */
	fprint_long_matrix(fout, mmkp->w, n, m);
	/* Writing capacities */
	fprint_long_matrix(fout, mmkp->b, o, m);

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
			mmkp->w[i][j] = lrand(MAX_RAND_WEIGHT);
	
	/* Computing total weight for each dimension */
	for( j = 0 ; j < m ; j++){
		tot_weights[j] = 0;
		for( i = 0 ; i < n ; i++ )
			tot_weights[j] += mmkp->w[i][j];
	}

	/* Random capacities (each knapsack has a "global" size
	 * which is used to set the capacity for each demantion). */
	cdist = random_normalized_double_array(o);

	for( k = 0 ; k < o ; k++ )
		printf("%lf ", cdist[o]);
	printf("\n");

	for( k = 0 ; k < o ; k++ )
		for( j = 0 ; j < m ; j++ )
			mmkp->b[k][j] = (long)(cdist[k] * tot_weights[j] * beta);

	return mmkp;
}

MMKP *mmkp_alloc(int n, int m, int o){
	int j;

	MMKP *mmkp = (MMKP*)malloc(sizeof(MMKP));
	mmkp->p = malloc_long_array(n);
	mmkp->w = malloc_long_matrix(n, m);
	mmkp->b = malloc_long_matrix(o, m);

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

	free_long_matrix(mmkp->w, mmkp->n);
	free_long_matrix(mmkp->b, mmkp->o);
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
	fprint_long_array(out, mmkp->p, n);

	fprintf(out, " Weights:\n");
	fprint_long_matrix_tranlated(out, mmkp->w, n, m);

	fprintf(out, " Capacities:\n");
	fprint_long_matrix_tranlated(out, mmkp->b, o, m);

	return;
}

//void mmkp_to_zimpl(MMKP *mmkp, FILE *fout){
//	int i, j, n, m;
//
//	n = mmkp->n;
//	m = mmkp->m;
//
//	/* sizes */
//	fprintf(fout, "param n := %d;\n", n);
//	fprintf(fout, "param m := %d;\n", m);
//
//	/* sets */
//	fprintf(fout, "set N := {1 .. %d};\n", n);
//	fprintf(fout, "set M := {1 .. %d};\n", m);
//
//	/* profit */
//	fprintf(fout, "param p[N] :=\n");
//	zimpl_print_array(fout, mmkp->p, n);
//
//	/* weights */
//	fprintf(fout, "param w[M*N] :=\n");
//	zimpl_print_matrix(fout, mmkp->w, m, n);
//
//	/* capacities */
//	fprintf(fout, "param b[M] :=\n");
//	zimpl_print_array(fout, mmkp->b, m);
//
//	/* desicion var */
//	fprintf(fout, "var x[N] binary;\n");
//
//	/* capacities constraint */
//	fprintf(fout, "subto capacities:\nforall <j> in M do\nsum <i> in N do\nx[i]*w[j, i] <= b[j];\n");
//
//	/* objective function */
//	fprintf(fout, "maximize profit:\n sum <i> in N do\n x[i]*p[i];\n");
//
//	return;
//}
//
