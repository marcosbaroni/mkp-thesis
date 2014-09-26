#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"
#include "ssum.h"

/*
 * Reads a SubSet-sum instance from a file, given its name.
 */
SSum *ssum_read_file(char *filename){
	SSum *ssum;
	FILE *fin;

	fin = fopen(filename, "r");
	ssum = ssum_read(fin);
	fclose(fin);

	return ssum;
}

/*
 * Reads a SubSet-sum instance from and input FILE.
 */
SSum *ssum_read(FILE *in){
	SSum *ssum;
	ssum = (SSum*)malloc(sizeof(SSum));

	fscanf(in, "%d", &(ssum->n));
	fscanf(in, "%ld", &(ssum->b));
	ssum->w = long_array_read(in, NULL, ssum->n);

	return ssum;
}

/*
 * Generates a random Subset-sum instance.
 * n: number of itens
 * boudnd: size of
 */
SSum *ssum_new_random(int n, long bound, double b_ratio){
	SSum *ssum;
	long sum;
	int i;

	ssum = (SSum*)malloc(sizeof(SSum));
	ssum->n = n;
	ssum->w = (long*)malloc(n*sizeof(long));
	for( i = 0 ; i < n ; i++ ){
		ssum->w[i] = 1+lrand(bound-1);
		sum+= ssum->w[i];
	}
	
	long_array_qsort(ssum->w, ssum->n);
	ssum->b = (long)(floor(sum*b_ratio));

	return ssum;
}

/*
 * Outputs a Subset-sum instance on a machine friendly format.
 * Format:
 *    "<n>
 *    <a1> <a2> <a3> ... <an> <B>"
 */
void ssum_write(FILE *out, SSum *ssum){
	int i;
	fprintf(out, "%d\n", ssum->n);
	for( i = 0 ; i < ssum->n ; i++ )
		fprintf(out, "%ld ", ssum->w[i]);
	fprintf(out, "%ld\n", ssum->b);
	return ;
}

/*
 * Dealloc a Subset-sum instance.
 */
void ssum_free(SSum *ssum){
	free(ssum->w);
	free(ssum);
	return;
}

/*
 * Prints a Subset-sum instance on human friendly format.
 */
void ssumsol_fprint(FILE *out, SSumSol *sumsol){
	unimplemented();
	return;
}

/* 
 * Outputs an Subset-sum instance on ZIMPL linear programming modeling format.
 */
void ssum_to_zimpl(FILE *fout, SSum *ssum){
	int n, i;
	n = ssum->n;

	/* SIZES */
	fprintf(fout, "param n := %d;\n", n);

	/* SETS */
	fprintf(fout, "set N := {1 .. %d};\n", n);

	/* PARAMETERS */
	fprintf(fout, "param w[N] :=\n"); /* weights */
	long_array_zimpl_print(fout, ssum->w, n);

	/* DECISION VARIABLES */
	fprintf(fout, "var x[N] binary;\n");

	/* CONSTRAINT */
	fprintf(fout, /* max sum */
		"subto capacities:\n\
			forall <i> in N do\n\
				x[i]*w[i] <= b;\n");

	/* OBJECTIVE FUNCTION */
	fprintf(fout, /* total sum of selected itens */
		"maximize profit:\n\
			sum <i> in N do\n\
				x[i]*w[i];\n");
	
	return;
}

