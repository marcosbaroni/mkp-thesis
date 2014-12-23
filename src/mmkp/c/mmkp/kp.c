#include <math.h>
#include <string.h>
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
	int i, n;
	KPSol *kpsol;
	n = kp->n;

	kpsol = (KPSol*)malloc(sizeof(KPSol));
	kpsol->x = (int*)malloc(n*sizeof(int));
	kpsol->sel = (int*)malloc(n*sizeof(int));
	for( i = 0 ; i < n ; i++ ){
		kpsol->x[i] = 0;
		kpsol->sel[i] = 0;
	}
	kpsol->nx = 0;
	kpsol->profit = 0;
	kpsol->b_left = kp->b;
	kpsol->kp = kp;
	kpsol->find_steps = 0;
	kpsol->proof_steps = 0;

	return kpsol;
}

int kpsol_get(KPSol *sol, int a){
	return(sol->x[a]);
}

KPSol *kpsol_set(KPSol *sol, int a, int val){
	if(sol->x[a] == val){
		fprintf(stderr, "%s error: %d-th variable is already set to %d.\n",
			__PRETTY_FUNCTION__, a+1, val);
		return sol;
	}
	
	if( val ) kpsol_add(sol, a);
	else kpsol_rm(sol, a);

	return sol;
}

long long kpsol_get_profit(KPSol *sol){
	return sol->profit;
}

int kpsol_feasible(KPSol *sol){
	return (sol->b_left >= 0);
}

KPSol *kpsol_add(KPSol *sol, int a){
	if( sol->x[a] ){
		fprintf(stderr, "%s error: item %d-th item already in knapsak.\n",
			__PRETTY_FUNCTION__, a+1);
		return sol;
	}

	sol->x[a] = 1;
	sol->b_left -= sol->kp->w[a];
	sol->profit += sol->kp->p[a];

	return sol;
}

KPSol *kpsol_rm(KPSol *sol, int a){
	if( sol->x[a] ) {
		fprintf(stderr, "%s error: item %d-th item not in knapsak.\n",
			__PRETTY_FUNCTION__, a+1);
		return sol;
	}

	sol->x[a] = 0;
	sol->b_left += sol->kp->w[a];
	sol->profit -= sol->kp->p[a];

	return sol;
}

KPSol *kpsol_new(KP *kp, int *x, long long find_steps, long long proof_steps){
	int i, n;
	KPSol *kpsol;

	n = kp->n;
	kpsol = kpsol_new_empty(kp);
	kpsol->find_steps = find_steps;
	kpsol->proof_steps = proof_steps;

	for( i = 0 ; i < n ; i++ ){
		if( x[i] ){
			kpsol->x[i] = 1;
			kpsol->sel[kpsol->nx++] = i;
			kpsol->profit += kp->p[i];
			kpsol->b_left -= kp->w[i];
		}else
			kpsol->x[i] = 0;
	}

	return kpsol;
}

KPSol *kpsol_new_random(KP *kp){
	KPSol *sol;
	int i, n, *idxs;

	/* new empty sol*/
	sol = kpsol_new_empty(kp);

	/* generating a shuffled index array */
	n = kp->n;
	idxs = (int*)malloc(n*sizeof(int));
	for( i = 0 ; i < n ; i++ )
		idxs[i] = i;
	idxs = int_array_shuffle(idxs, n);

	/* random filling knapsack */
	for( i = 0 ; i < n ; i++ )
		if( sol->b_left >= kp->w[idxs[i]] )
			kpsol_add(sol, idxs[i]);

	free(idxs);

	return sol;
}

KPSol *kpsol_copy(KPSol *kpsol){
	KP *kp;
	KPSol *new;

	kp = kpsol->kp;

	new = kpsol_new_empty(kp);
	new->x = (int*)memcpy(new->x, kpsol->x, kp->n*sizeof(int));
	new->sel = (int*)memcpy(new->x, kpsol->x, kp->n*sizeof(int));
	new->nx = kpsol->nx;
	new->profit = kpsol->profit;
	new->b_left = kpsol->b_left;
	new->find_steps = kpsol->find_steps;
	new->proof_steps = kpsol->proof_steps;
	new->kp = kp;

	return new;
}

void kpsol_free(KPSol *kpsol){
	free(kpsol->x);
	free(kpsol->sel);
	free(kpsol);
	return;
}

void kpsol_fprint(FILE *out, KPSol *kpsol){
	int i;
	fprintf(out, "%lld\n", kpsol->profit);
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
KPSol *kp_backtrack(KP *kp, int enumerate, long long upper_bound){
	int i, j, n;
	int backtrack;            /* if algorithm is backtracking */
	int *x;                   /* current solution */
	int *best_x;              /* best solution found */
	long long *w, *p, b;      /* auxiliary (instance of problem) */
	long long best_profit;    /* profit of best solution found */
	long long b_left;         /* resource left (current solution) */
	long long profit;         /* profit of current solution */
	long long count;          /* counter, for number of nodes */
	long long best_count;     /* number of steps when best was found */
	double lp_bound, b_aux;   /* profit of the LP-relax on current solution */
	double *dens;             /* profit density of itens */
	KPSol *kpsol;

	/* problem variables (n, w, p, b, density) */
	n = kp->n; w = kp->w; p = kp->p; b = kp->b; dens = kp->density;

	/* initialization */
	x = int_array_malloc(n);
	x = int_array_init(x, n, 0);
	best_x = int_array_malloc(n);
	best_x = int_array_copy(best_x, x, n);
	b_left = b;
	best_profit = profit = 0;
	lp_bound = 0.0;
	i = 0;
	backtrack = 0;
	count = 0;

	while(1){
		/* DRILLING LEFT TREE. */
		while( i < n && b_left >= w[i] ){
			x[i] = 1;
			b_left -= w[i];
			profit += p[i];
			i++;
		}
		/* NEW BEST SOLUTION */
		if( profit > best_profit ){
			best_count = count;
			best_x = int_array_copy(best_x, x, n);
			best_profit = profit;
			if( best_profit >= upper_bound){
				/* Found a good enough */
				kpsol = kpsol_new(kp, best_x, best_count, count);
				free(x);
				free(best_x);
				return kpsol;
			}
		}
		/* UPDATE LP_BOUND */
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
		/* BACKTRACKING: (i: index of item which was about to insert) */
		while( lp_bound <= (double)best_profit ){  /* while lp_bound is not good enough */
			if( i == 0 ){             /* root reached. halt! */
				kpsol = kpsol_new(kp, best_x, best_count, count);
				free(x);
				free(best_x);
				return kpsol;
			}else do i--;
			while( (i > 0) && (x[i] == 0));
			/* DRILL RIGHT TREE */
			x[i] = 0;
			b_left += w[i];
			profit -= p[i];

			/* UPDATE LP_BOUND */
			count++;
			b_aux = b_left;
			j = i+1;
			while( b_aux && j < n ){  /* while knapsack is not full... */
				if( b_aux >= w[j] ){  /* if can fit whole next item */
					lp_bound += p[j];
					b_aux -= w[j];
				}else{                /* split item */
					lp_bound += p[j]*w[j]/(double)b_aux;
					b_aux = 0;
				}
				j++;
			}
		}
		i++;
	}

	return kpsol;
}

KPSol *kp_dymprog(KP *kp){
	int n, i;
	long long *w, *p, b;
	KPSol *kpsol;

	n = kp->n;
	w = kp->w;
	p = kp->p;
	n = kp->b;

	for(i = 0 ; i < n ; i++){
	}

	return kpsol;
}

DES_Interface *kp_des_interface(){
	DES_Interface *desi;

	desi = (DES_Interface*)malloc(sizeof(DES_Interface));

	desi->des_activate = NULL;
	desi->des_set = (des_set_f)kpsol_set;
	desi->des_get = (des_get_f)kpsol_get;
	desi->des_obj = (des_obj_f)kpsol_get_profit;
	desi->des_feasible = (des_feasible_f)kpsol_feasible;
	desi->des_new_solution = (des_new_solution_f)kpsol_new_random;
	desi->des_copy_solution = (des_copy_solution_f)kpsol_copy;
	desi->des_free_solution = (des_free_solution_f)kpsol_free;

	return desi;
}

