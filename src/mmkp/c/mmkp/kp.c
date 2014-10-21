#include <math.h>
#include "kp.h"

KP *kp_new_random(int n, double tightness, long long bound){
	KP *kp;
	int i;
	long long wsum;

	kp = (KP*)malloc(sizeof(KP));
	kp->n = n;
	kp->w = (long long*)malloc(n*sizeof(long long));
	kp->p = (long long*)malloc(n*sizeof(long long));
	kp->density = (double*)malloc(n*sizeof(double));

	wsum = 0;
	for( i = 0 ; i < n ; i++ ){
		kp->w[i] = llrand(bound);
		kp->p[i] = llrand(bound);
		kp->density[i] = kp->p[i]/((double)kp->w[i]);
		wsum += kp->w[i];
	}
	kp->b = (long long)(floor(wsum*tightness));

	return kp;
}

KP *kp_read_file(char *filename){
	KP *kp;
	FILE *fin;

	fin = fopen(filename, "r");
	kp = kp_read(fin);
	fclose(fin);

	return kp;
}

KP *kp_read(FILE *in){
	int n, i;
	KP *kp;

	kp = (KP*)malloc(sizeof(KP));
	fscanf(in, "%d", &n);
	kp->n = n;

	kp->p = long_long_array_read(in, NULL, n);
	kp->w = long_long_array_read(in, NULL, n);
	fscanf(in, "%lld", &(kp->b));
	kp->density = (double*)malloc(n*sizeof(double));

	for( i = 0 ; i < n ; i++ )
		kp->density[i] = kp->p[i]/((double)kp->w[i]);

	return kp;
}

void kp_write(FILE *out, KP *kp){
	fprintf(out, "%d\n", kp->n);
	long_long_array_write(out, kp->p, kp->n);
	long_long_array_write(out, kp->w, kp->n);
	fprintf(out, "%lld\n", kp->b);
	return;
}

void kp_free(KP *kp){
	free(kp->p);
	free(kp->w);
	free(kp->density);
	free(kp);
	return;
}

void kp_fprintf(FILE *out, KP *kp){
	double *dens;
	int i, n;
	long long *p, *w, b, llsum;

	n = kp->n;
	p = kp->p;
	w = kp->w;
	b = kp->b;
	dens = kp->density;

	llsum = 0;
	for( i = 0 ; i < n ; i++ ){
		fprintf(out, "% 5lld | % 5lld | (%.3lf)\n", p[i], w[i], dens[i]);
		llsum += w[i];
	}
	findent(out, 6, ' ');
	fprintf(out, "| % 5lld (%.3lf)\n", b, b/((double)llsum));

	return;
}

void kp_to_zimpl(FILE *fout, KP *kp){
	int n, i;
	n = kp->n;

	/* SIZES */
	fprintf(fout, "param n := %d;\n", n);

	/* SETS */
	fprintf(fout, "set N := {1 .. %d};\n", n);

	/* PARAMETERS */
	fprintf(fout, "param p[N] :=\n"); /* profits */
	long_long_array_zimpl_print(fout, kp->p, n);
	fprintf(fout, "param w[N] :=\n"); /* weights */
	long_long_array_zimpl_print(fout, kp->w, n);
	fprintf(fout, "param b := %lld;\n", kp->b);

	/* DECISION VARIABLES */
	fprintf(fout, "var x[N] binary;\n");

	/* CONSTRAINT */
	fprintf(fout, /* total weight of selected itens */
"subto capacities:\n\
	sum <i> in N do\n\
		x[i]*w[i] <= b;\n");

	/* OBJECTIVE FUNCTION */
	fprintf(fout, /* total profit of selected itens */
"maximize profit:\n\
	sum <i> in N do\n\
		x[i]*p[i];\n");
	
	return;
}

int kp_partition_by_density(KP *kp, int a, int b){
	int i, j;
	double pivot, *dens, daux;
	long long llaux;

	dens = kp->density;

	i = a; j = b+1;
	pivot = dens[a];

	while(1){
		while( dens[++i] > pivot ) if( i == b ) break;
		while( dens[--j] < pivot ) if( j == a ) break;
		if( i >= j ) break;    /* pointer crossed? */
		/* SWAP i-j */
		llaux = kp->w[i]; kp->w[i] = kp->w[j]; kp->w[j] = llaux;
		llaux = kp->p[i]; kp->p[i] = kp->p[j]; kp->p[j] = llaux;
		daux = dens[i]; dens[i] = dens[j]; dens[j] = daux;
	}
	/* PLACE PIVOT (SWAP j-a) */
	llaux = kp->w[a]; kp->w[a] = kp->w[j]; kp->w[j] = llaux;
	llaux = kp->p[a]; kp->p[a] = kp->p[j]; kp->p[j] = llaux;
	daux = dens[a]; dens[a] = dens[j]; dens[j] = daux;

	return j;
}

KP *kp_sub_qsort_by_density(KP *kp, int a, int b){
	int m, n;
	if( b <= a ) return kp;
	m = kp_partition_by_density(kp, a, b);
	kp_sub_qsort_by_density(kp, a, m-1);
	kp_sub_qsort_by_density(kp, m+1, b);
	return kp;
}

/* Sort the itens by density. */
KP *kp_qsort_by_density(KP *kp){
	kp_sub_qsort_by_density(kp, 0, kp->n-1);
	return kp;
}

KPSol *kpsol_new_empty(KP *kp){
	KPSol *kpsol;

	kpsol = (KPSol*)malloc(sizeof(KPSOL));
	kpsol->x = (int*)malloc(kp->n*sizeof(int));
	kpsol->sel = (int*)malloc(kp->n*sizeof(int));
	kpsol->nx = 0;
	kpsol->p = 0;
	kpsol->b_left = kp->b;
	kpsol->kp = kp;

	return kpsol;
}

KPSol *kpsol_new(KP *kp, int *x){
	int i, n;
	KPSol *kpsol;

	n = kp->n;
	kpsol = kpsol_new_empty(kp);

	for( i = 0 ; i < n ; i++ ){
		if( x[i] ){
			kpsol->x[i] = 1;
			kpsol->sel[kpsol->nx++] = i;
			kpsol->p += kp->p[i];
			kpsol->b_left -= kp->w[i];
		}
	}

	return kpsol;
}

/* Enumerate all KP solutions (backtrack alg) */
Array *kp_backtrack(KP *kp, int enumerate){
	int i, n, backtrack, *x;
	long long *w, *p, b, b_left, profit, best_profit;
	double *dens;
	Array *sols;

	/* problem variables */
	n = kp->n;
	w = kp->w;
	p = kp->p;
	b = kp->b;
	dens = kp->density;
	/* auxiliary */

	/* solution */
	x = (int)malloc(n*sizeof(int));
	sols = array_new();
	b_left = b;
	profit = 0;

	backtrack = 0;
	i = 0;
	/* search! */
	while( 1 ){
		/* root reached? */
		if( i == 0 )
			if( backtrack )
				if( x[0] == 0 )
					break;
		if( backtrack ){
		}else{
			if( w[i] <= b_left ){
				x[i] = 1;
				b_left -= w[i];
				profit += p[i];
				/* if node is solution */
				if( profit > best_profit ){
					/* clean solutions */
					array_apply(sols, kpsol_free);
					array_empty(sols);
					array_insert(kpsol_new(kp, x));
				}
				/* TODO */
			}else{
			}
		}
	}

	return sols;
}

