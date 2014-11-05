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
	long long *p, *w, b, llsum, profit;

	n = kp->n;
	p = kp->p;
	w = kp->w;
	b = kp->b;
	dens = kp->density;

	llsum = 0;
	profit = 0;
	fprintf(out, "   i |     p |     w |    p/w| profit | b_left\n");
	fprintf(out, "-----+-------+-------+-------+--------+-------\n");
	for( i = 0 ; i < n ; i++ ){
		profit += p[i];
		llsum += w[i];
		fprintf(out, "% 4d |", i+1);
		fprintf(out, "% 6lld |", p[i]);
		fprintf(out, "% 6lld |", w[i]);
		fprintf(out, " %5.1lf |", dens[i]);
		fprintf(out, "% 7lld |", profit);
		fprintf(out, "% 7lld\n", b-llsum);
	}
	findent(out, 6, ' ');
	fprintf(out, "       | % 5lld (%.3lf)\n", b, b/((double)llsum));

	return;
}

void kp_to_zimpl(FILE *fout, KP *kp){
	int n;
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
	int m;
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

	kpsol = (KPSol*)malloc(sizeof(KPSol));
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

void kpsol_free(KPSol *kpsol){
	free(kpsol->x);
	free(kpsol->sel);
	free(kpsol);
	return;
}

void kpsol_fprint(FILE *out, KPSol *kpsol){
	int i;
	fprintf(out, "%lld\n", kpsol->p);
	for( i = 0 ; i < kpsol->nx ; i++ )
		fprintf(out, "%d%s", kpsol->sel[i]+1, (i+1 == kpsol->nx) ? "\n": " ");
	return;
}

/* Enumerate all KP solutions (backtrack alg)
 *
 * while (k <= n) and (b_left > w[i]):
 *     x[i] = 1            "select k-th item"
 *     profit += p[i]         "update profit"
 *     b_left -= w[i]         "update weight"
 *     i++                 "proceed to next item"
 * if (i > n):
 *     x_best = x          "best found (otherwise would be backtracked before)
 * //else:
 * //    x[i] = 0          "??? set 0"
 * while (lp_bound <= best_profit): "if solut. cant be improved on current branch"
 *     while (i != 0) and (x[i] == 0):   "backtrack: find previous selected item"
 *         i--
 *     if (i == 0):          "root reached"
 *         return best_x;    "HALTS!"
 *     x[i] = 0              "deselect the item (update bound!!!)"
 *     b_left += w[i]
 *     profit -= p[i]
 * i++
 */
Array *kp_backtrack(KP *kp, int enumerate){
	int i, j, n;
	int backtrack;            /* if algorithm is backtracking */
	int *x;                   /* current solution */
	int *best_x;              /* best solution found */
	long long *w, *p, b;      /* auxiliary (instance of problem) */
	long long best_profit;    /* profit of best solution found */
	long long b_left;         /* resource left (current solution) */
	long long profit;         /* profit of current solution */
	long long count;          /* counter, for number of nodes */
	double lp_bound, b_aux;   /* profit of the LP-relax on current solution */
	double *dens;             /* profit density of itens */
	Array *sols;              /* Array os solutions found */

	/* problem variables (n, w, p, b, density) */
	n = kp->n; w = kp->w; p = kp->p; b = kp->b; dens = kp->density;

	/* initialization */
	x = int_array_malloc(n);
	x = int_array_init(x, n, 0);
	best_x = int_array_malloc(n);
	best_x = int_array_copy(best_x, x, n);
	sols = array_new();
	b_left = b;
	best_profit = profit = 0;
	lp_bound = 0.0;
	i = 0;
	backtrack = 0;
	count = 0;

	/* search 0.2 */
	while(1){
		/* DRILLING LEFT TREE... */
		while( i < n && b_left >= w[i] ){
			x[i] = 1;
			b_left -= w[i];
			profit += p[i];
			i++;
		}
		/* new best solution */
		if( profit > best_profit ){
			best_x = int_array_copy(best_x, x, n);
			best_profit = profit;
		}
		/*** UPDATE LP_BOUND ***/
		count++;
		lp_bound = (double)profit;
		b_aux = b_left;
		j = i+1;
		while( b_aux && j < n ){ /* while knapsack is not full... */
			if( b_aux >= w[j] ){  /* if can fit whole next item */
				lp_bound += p[j];
				b_aux -= w[j];
			}else{                /* split item */
				lp_bound += p[j]*w[j]/(double)b_aux;
				b_aux = 0;
			}
			j++;
		}
		/***********************/
		/* BACKTRACKING... */
		while( lp_bound <= (double)best_profit ){  /* while lp_bound is not good enough */
			/* BACKTRACKING... */
			while( (i > 0) && (x[i] == 0)) i--;
			if( i == 0 ){       /* root reached. halt! */
				printf("%lld;%lld\n", count, best_profit);
				sols = array_insert(sols, kpsol_new(kp, best_x));
				free(x);
				free(best_x);
				return sols;
			}
			/* DRILLING RIGHT TREE */
			x[i] = 0;
			b_left += w[i];
			profit -= p[i];

			/*** UPDATE LP_BOUND ***/
			count++;
			b_aux = b_left;
			j = i+1;
			while( b_aux && j < n ){ /* while knapsack is not full... */
				if( b_aux >= w[j] ){  /* if can fit whole next item */
					lp_bound += p[j];
					b_aux -= w[j];
				}else{                /* split item */
					lp_bound += p[j]*w[j]/(double)b_aux;
					b_aux = 0;
				}
				j++;
			}
			/***********************/
		}
		i++;
	}


	/* search! 0.1-beta */
	while( 1 ){
		/* root reached? */
		if( i == 0 )
			if( backtrack )
				if( x[0] == 0 )
					break;
		if( backtrack ){
			/* if is setted  */
			if( x[i] > 0){
				x[i] = 0;
				b_left += w[i];
				profit -= p[i];
				backtrack = 0;
				/* if fixed variable was the last available */
				if( i+1 == n ){
					backtrack = 1;
					/* find the immediately not fixed var to mark as backtrack */
					while( x[i] == 0 && i ) i--;
				}else i++;
			}else i--;
		/* drilling down the tree */
		}else{
			/* if child is feasible */
			if( w[i] <= b_left ){
				x[i] = 1;
				b_left -= w[i];
				profit += p[i];
				/* if node is solution */
				if( profit > best_profit ){
					/* clean solutions */
					array_apply(sols, (void(*)(void*))kpsol_free);
					array_empty(sols);
					array_insert(sols, kpsol_new(kp, x));
				}
			/* TODO */
			}
		}
	}

	free(x);

	return sols;
}

