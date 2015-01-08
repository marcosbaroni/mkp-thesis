#include <stdio.h>
#include <stdlib.h>
#include "lp.h"

void print_tableau(FILE *out, double **tab, double *b, double z, int n, int m){
	int i, j;

	/* the tableau */
	for( i = 0 ; i < m ; i++ ){
		for( j = 0 ; j < n ; j++ )
			fprintf(out, "\t%.2lf", tab[i][j]);
		fprintf(out, "| %.2lf\n", b[i]);
	}

	/* ruler */
	fprintf(out, "\t");
	for( i = 0 ; i < n ; i++ )
		fprintf(out, "--------");
	fprintf(out, "\n");

	/* last line*/
	for( j = 0 ; j < n ; j++ )
		fprintf(out, "\t%.2lf", tab[m][j]);
	fprintf(out, "| %.2lf\n", z);

	return;
}

void build_tableau(LP *lp, double **tab, double *b){
	int i, j, n, m, nvars;

	nvars = lp->nvars;
	m = lp->ncs;
	n = m + nvars;      /* n. of variables (with slacks) */

	for( i = 0 ; i < m ; i++ ){
		/* coping "A" table */
		for( j = 0 ; j < nvars ; j++ )
			tab[i][j] = lp->a[i][j];
		/* initializin slack variable columns */
		for( j = nvars ; j < n ; j++ )
			tab[i][j] = 0.0;
		tab[i][nvars+i] = 1.0;
		/* copying the "b" column */
		b[i] = lp->b[i];
	}

	/* the additional line (with profits) */
	for( j = 0 ; j < nvars ; j++ )
		tab[m][j] = -lp->c[j];      /* to negative */

	/* "profit" of slack variables */
	for( j = nvars ; j < n ; j++ )
		tab[m][j] = 0.0;
	
	return;
}

/*
 * [ tab | b ]
 * [ tab | z ]   <- additional line | z
 * */
double *simplex(LP *lp){
	int i, j, n, nvars, m;
	double **tab; /* the tableau (with addicional line, for pivoting ruling) */
	double *b;    /* right side "b"s */
	double z;     /* value of z */
	double *x;    /* the solution */
	double most_neg, min_rate, rate, pivot;
	int pivot_i, pivot_j, ones;

	nvars = lp->nvars;
	m = lp->ncs;
	n = m + nvars;  /* total variables (with slacks) */

	/* allocing tableau */
	tab = (double**)malloc((m+1)*sizeof(double*));
	for( i = 0 ; i < m+1 ; i++ )
		tab[i] = (double*)malloc(n*sizeof(double));
	b = (double*)malloc(m*sizeof(double));
	x = (double*)malloc(nvars*sizeof(double));
	
	/* building tableau */
	build_tableau(lp, tab, b);

	/* finding column of first pivot */
	pivot_j = 0;
	most_neg = lp->c[0];
	for( j = 0 ; j < nvars ; j++ )
		if( tab[m][j] < most_neg )
			{ pivot_j = j; most_neg = tab[m][j]; }

	/* finding line of first pivot */
	pivot_i = 0;
	min_rate = b[pivot_i]/tab[pivot_i][pivot_j];
	for( i = 1 ; i < m ; i++ ){
		rate = b[i]/tab[i][pivot_j];
		if( rate < min_rate && rate > 0.0 ){
			min_rate = rate;
			pivot_i = i;
		}
	}

	print_tableau(stdout, tab, b, z, n, m);
	printf("pivot i=%d  j=%d\n\n", pivot_i+1, pivot_j+1);
	
	z = 0.0;
	/* solving */
	while( most_neg < 0.0 && min_rate > 0.0 ){
		/*** PIVOTING ***/
		/* normalizing the pivot line */
		pivot = tab[pivot_i][pivot_j];
		for( j = 0 ; j < n ; j++ )
			tab[pivot_i][j] /= pivot;
		b[pivot_i] /= pivot;
		/* further lines (before pivot line) */
		for( i = 0 ; i < pivot_i ; i++ ){
			pivot = -tab[i][pivot_j];
			for( j = 0 ; j < n ; j++ )
				tab[i][j] += pivot*tab[pivot_i][j];
			b[i] += pivot*b[pivot_i];
		}
		/* further lines (after pivot line) */
		for( i = pivot_i+1 ; i < m ; i++ ){ // further lines (pivot+1..nc)
			pivot = -tab[i][pivot_j];         // multiplier
			for( j = 0 ; j < n ; j++ )
				tab[i][j] += pivot*tab[pivot_i][j];
			b[i] += pivot*b[pivot_i];
		}

		/* the additional line */
		pivot = -tab[m][pivot_j];          // multiplier
		most_neg = 1.0;
		for( j = 0 ; j < n ; j++ ){
			tab[m][j] += pivot*tab[pivot_i][j];   /* finding pivot column */
			if( tab[m][j] < most_neg ){
				most_neg = tab[m][j];
				pivot_j = j;
			}
		}
		z += pivot*b[pivot_i];

		/* finding pivot line */
		pivot_i = 0;
		min_rate = b[pivot_i]/tab[pivot_i][pivot_j];
		for( i = 1 ; i < m ; i++ ){
			rate = b[i]/tab[i][pivot_j];
			if( rate < min_rate && rate > 0.0 ){
				min_rate = rate;
				pivot_i = i;
			}
		}
	
		print_tableau(stdout, tab, b, z, n, m);
		printf("pivot i=%d  j=%d, most_neg=%.3lf min_rate=%.3lf %s\n\n",
			pivot_i+1, pivot_j+1, most_neg, min_rate,
			(most_neg < 0.0 && min_rate > 0.0 ) ? "YES" : "NO" );
	}

	/* extracting solution */
	for( j = 0 ; j < nvars ; j++ ){
		ones = 0;
		for( i = 0 ; i < m ; i++ ){
			if( tab[i][j] == 1.0 ){
				ones++;
				x[j] = b[i];
			}
		}
		if( ones != 1 )        /* not basic */
			x[j] = 0.0;
	}
	
	/* freeing */
	for( i = 0 ; i < m+1 ; i++ )
		free(tab[i]);
	free(tab);
	free(b);

	return x;
}

void lp_fprint(FILE *out, LP *lp){
	int i, j, n, m;

	n = lp->nvars;
	m = lp->ncs;

	for( i = 0 ; i < n ; i++ )
		fprintf(out, "%1.1lf ", lp->c[i]);
	fprintf(out, "= Z\n");
	for( i = 0 ; i < m ; i++ ){
		for( j = 0 ; j < n ; j++ )
			fprintf(out, "%1.1lf ", lp->a[i][j]);
		fprintf(out, "<= %lf\n", lp->b[i]);
	}

	return;
}

void lp_free(LP *lp){
	int i;

	for( i = 0 ; i < lp->ncs ; i++ )
		free(lp->a[i]);
	free(lp->a);
	free(lp->c);
	free(lp->b);
	free(lp);

	return;
}

