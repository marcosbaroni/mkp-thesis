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
	ssum->w = long_array_read(in, NULL, ssum->n);
	fscanf(in, "%ld", &(ssum->b));

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
	sum = 0;
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
	fprintf(out, "\n%ld\n", ssum->b);
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
void ssumsol_fprint(FILE *out, SSumSol *sol){
	int i, n, nx;
	long *w;

	nx = sol->nx;
	n = sol->ssum->n;
	w = sol->ssum->w;

	i = 0;
	for( i = 1 ; i < nx ; i++ )
		fprintf(out, "%ld + ", w[sol->sel[i]]);
	fprintf(out, "%ld = %ld", w[sol->sel[i]], sol->sum);

	if( sol->sum != sol->ssum->b )
		fprintf(out, "(+ %ld = %ld)", sol->ssum->b - sol->sum, sol->sum );
	fprintf(out, "\n");

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
	fprintf(fout, "param b := %ld;\n", ssum->b);

	/* DECISION VARIABLES */
	fprintf(fout, "var x[N] binary;\n");

	/* CONSTRAINT */
	fprintf(fout, /* max sum */
"subto capacities:\n\
	sum <i> in N do\n\
		x[i]*w[i] <= b;\n");

	/* OBJECTIVE FUNCTION */
	fprintf(fout, /* total sum of selected itens */
"maximize profit:\n\
	sum <i> in N do\n\
		x[i]*w[i];\n");
	
	return;
}

void ssum_fprint(FILE *out, SSum *ssum){
	int i, n;
	n = ssum->n;

	for( i = 0 ; i < n ; i++ )
		printf("%ld ", ssum->w[i]);
	printf("[%ld]\n", ssum->b);

	return;
}

SSumSol *ssumsol_new_empty(SSum *ssum){
	int i, n;
	SSumSol *sol;
	
	n = ssum->n;
	sol = (SSumSol*)malloc(sizeof(SSumSol));
	sol->x = (int*)malloc(n*sizeof(int));
	sol->sel = (int*)malloc(n*sizeof(int));
	sol->nx = 0;

	for( i = 0 ; i < n ; i++)
		sol->x[i] = 0;

	sol->sum = 0;
	sol->b_left = ssum->b;
	sol->ssum = ssum;

	return sol;
}

void ssumsol_free(SSumSol *ssumsol){
	free(ssumsol->x);
	free(ssumsol->sel);
	free(ssumsol);
	return;
}

SSumSol *ssumsol_new(SSum *ssum, int *x){
	int i, n;
	SSumSol *sol;
	n = ssum->n;
	sol = (SSumSol*)malloc(sizeof(SSumSol));
	sol->x = (int*)malloc(n*sizeof(int));
	sol->sel = (int*)malloc(n*sizeof(int));

	sol->sum = 0;
	sol->b_left = ssum->b;
	sol->ssum = ssum;

	for( i = 0 ; i < n ; i++ ){
		if( x[i] ){
			sol->x[i] = 1;
			sol->b_left -= ssum->w[i];
			sol->sum += ssum->w[i];
			sol->sel[sol->nx] = i;
			sol->nx++;
		}else{
			sol->x[i] = 0;
		}
	}

	return sol;
}

Array *ssum_backtrack(SSum *ssum){
	Array *sols = array_new();
	int i, n, *x, backtrack;
	long *w, sum, b_left;

	n = ssum->n;
	w = ssum->w;

	x = (int*)malloc(n*sizeof(int));
	for( i = 0 ; i < n ; i ++ )
		x[i] = 0;

	sum = 0; b_left = ssum->b;

	x[0] = 1;
	b_left -= w[0];
	i = 1;
	backtrack = 0;
	ssum_fprint(stdout, ssum);
	while( 1 ){
		//printf("i=%d, b_left=%ld%s\n", i+1, b_left, backtrack ? " Backtrack": "");
		//fflush(stdout);

		/* root reached? */
		if( i == 0 )
			if( backtrack )
				if( x[0] == 0 )
					break;

		/* if is backtracking */
		if( backtrack ){
			if( x[i] ){
				x[i] = 0;
				b_left += w[i];
				backtrack = 0;
				i++;
			}else{
				i--;
			}
		/* drilling down the tree. */
		}else{
			/* child is feasible. */
			if( w[i] <= b_left ){
				x[i] = 1;
				b_left -= w[i];
				/* if the node is solution */
				if( !b_left ){
					array_insert(sols, ssumsol_new(ssum, x));
					backtrack = 1;
				/* next variable as target */
				}else{
					if( i+1 == n ){
						backtrack = 1;
					}else{
						i++;
					}
				}
			/* if child is unfeasible. */
			}else{
				i--;
				backtrack = 1;
			}
		}
	}

	free(x);

	return sols;
}

/**
 * Function that outputs a (<n_max>X<b_max>) matrix A[nb] giving the probability
 *    of a Subset-sum instance with (n+1) vaiables has a subset summing (b+1)
 *    (considering w_max as maximum weight).
 * */
//double **ssum_calc_ptable(double **mat, int n_max, long b_max, long w_max){
//	int i, j;
//
//	if(!mat){   // need to alloc?
//		mat = (double**)malloc(n_max*sizeof(double*));
//		for( i = 0 ; i < n_max ; i++)
//			mat[i] = (double*)malloc(b_max*sizeof(double));
//	}
//
//	for( i = 0 ; i < n_max ; i++ )
//
//	return mat;
//}

