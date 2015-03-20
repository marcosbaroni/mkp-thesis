#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "mkp.h"

#include "util.h"
#include "des.h"
#include "sfl.h"
#include "lp.h"

/*** memory management ***/
MKP *mkp_alloc(int n, int m){
	int i;
	MKP *mkp;

	mkp = (MKP*)malloc(sizeof(MKP));
	mkp->p = long_long_array_malloc(n);
	mkp->w = long_long_matrix_malloc(m, n);
	mkp->b = long_long_array_malloc(n);
	mkp->idxs = (int*)malloc(n*sizeof(int));
	for( i = 0 ; i < n ; i++ )
		mkp->idxs[i] = i;
	mkp->n = n;
	mkp->m = m;
	mkp->lp_sol = NULL;
	mkp->lp_trunc = NULL;

	return mkp;
}

/*
 * Returns a random MKP problem with coeficients taken from uniform
 *    distribution.
 */
MKP *mkp_random(int n, int m, double beta, long long max_coefs){
	int i, j;
	long long lsum;
	MKP *mkp;

	mkp = mkp_alloc(n, m);

	/* profit */
	for( i = 0 ; i < n ; i++ )
		mkp->p[i] = llrand(max_coefs);
	/* weight */
	for( i = 0 ; i < m ; i++ ){
		lsum = 0;
		for( j = 0 ; j < n ; j++ )
			lsum += mkp->w[i][j] = llrand(max_coefs);
		mkp->b[i] = (long)(ceil(lsum*beta));
	}

	return mkp;
}

int mkp_cmp_profit(MKP *mkp, int a, int b){
	if( mkp->p[a] < mkp->p[b] ) return -1;
	else if(mkp->p[a] > mkp->p[b] ) return 1;
	return 0;
}

void mkp_swap_itens(MKP *mkp, int a, int b){
	long long w[mkp->m], p;
	int i, m;

	m = mkp->m;
	/* swaping profits */
	p = mkp->p[a]; mkp->p[a] = mkp->p[b]; mkp->p[b] = p;
	/* swaping weights */
	for( i = 0 ; i < m ; i++ ) w[i] = mkp->w[i][a];
	for( i = 0 ; i < m ; i++ ) mkp->w[i][a] = mkp->w[i][b];
	for( i = 0 ; i < m ; i++ ) mkp->w[i][b] = w[i];

	return ;
}

void mkp_sort_by_profit(MKP *mkp){
	mp_qsort(mkp, mkp->n, (mp_cmp_f)mkp_cmp_profit, (mp_swap_f)mkp_swap_itens, 1);
	return;
}

void mkp_free(MKP *mkp){
	long_long_matrix_free(mkp->w, mkp->m);
	if(mkp->lp_sol)
		free(mkp->lp_sol);
	if(mkp->lp_trunc)
		mkpsol_free(mkp->lp_trunc);
	free(mkp->p);
	free(mkp->b);
	free(mkp->idxs);
	free(mkp);

	return;
}

MKPSol *mkp_get_lp_trunc(MKP *mkp){
	if(!mkp->lp_trunc)
		mkp->lp_trunc = mkpsol_from_lp(mkp);
	return mkp->lp_trunc;
}

LP *mkp2lp(MKP *mkp, double capacity_scale){
	LP *lp;
	int i, j, n, m;

	if( capacity_scale < 0.0 )
		capacity_scale = 0.0;
	n = mkp->n;
	m = mkp->m;

	/* allocs */
	lp = (LP*)malloc(sizeof(LP));
	lp->nvars = n;
	lp->ncs = m+n;
	lp->c = double_array_alloc(n);
	lp->a = double_matrix_alloc(m+n, n);
	lp->b = double_array_alloc(m+n);

	/* the profit */
	for( i = 0 ; i < n ; i++ )
		lp->c[i] = mkp->p[i];
	/* the weights constraints */
	for( i = 0 ; i < m ; i++ )
		for( j = 0 ; j < n ; j++ )
			lp->a[i][j] = mkp->w[i][j];
	/* the "binary" contraint of each variable */
	for( i = 0 ; i < n ; i++ ){
		for( j = 0 ; j < n ; j++ )
			lp->a[m+i][j] = 0.0;
		lp->a[m+i][i] = 1.0;
	}
	/* the capacities */
	for( i = 0 ; i < m ; i++ )
		lp->b[i] = capacity_scale*mkp->b[i];
	for( i = 0 ; i < n ; i++ )
		lp->b[m+i] = 1.0;

	return lp;
}

void mkpsol_write(FILE *fout, MKPSol *mkpsol){
	int i, n;

	n = mkpsol->mkp->n;

	for( i = 0 ; i < n ; i++ )
		if(mkpsol->x[i])
			fprintf(fout, "%d ", i);
	fprintf(fout, "\n");

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
	int i, n, m;
	MKP *mkp;
	
	fscanf(fin, "%d", &n);
	fscanf(fin, "%d", &m);

	mkp = mkp_alloc(n, m);
	mkp->n = n;
	mkp->m = m;
	long_long_array_read(fin, mkp->p, n);
	long_long_matrix_read(fin, mkp->w, m, n);
	long_long_array_read(fin, mkp->b, n);
	for( i = 0 ; i < n ; i++ )
		mkp->idxs[i] = i;

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
	long_long_array_write(fout, mkp->p, mkp->n);
	long_long_matrix_write(fout, mkp->w, mkp->m, mkp->n);
	long_long_array_write(fout, mkp->b, mkp->m);

	return;
}

/*
 * Prints a MKP instance on a human friendly format.
 */
void mkp_fprint(FILE *fout, MKP *mkp){
	int i, j, n, m, ndigs[mkp->n+1];
	char format[20];
	long max;

	n = mkp->n;
	m = mkp->m;

	/* Deciding ndigits */
	for( i = 0 ; i < n ; i++ ){
		max = long_long_matrix_max_col(mkp->w, m, n, i);
		max = MAX(max, mkp->p[i]);
		ndigs[i] = 1 + (int)(floor(log(max)/log(10.)));
	}
	ndigs[n] = (int)(ceil(log(MAX(long_long_array_max(mkp->b, mkp->m), mkp->m))) / log(10.));

	/* print profits*/
	for( i = 0 ; i < n ; i++ ){
		sprintf(format, "%%%dlld ", ndigs[i]);
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
			sprintf(format, "%%%dlld ", ndigs[j]);
			fprintf(fout, format, mkp->w[i][j]);
		}
		/* capacity */
		sprintf(format, "%%%dlld ", ndigs[n]);
		fprintf(fout, "| ");
		fprintf(fout, format, mkp->b[i]);
		fprintf(fout, "\n");
	}

	return;
}

/* Prints MKP dual problem in ZIMPL format */
void mkp_dual_to_zimpl(FILE *fout, MKP *mkp, char linear){
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
	long_long_array_zimpl_print(fout, mkp->p, n);

	/* weights */
	fprintf(fout, "param w[M*N] :=\n");
	long_long_matrix_zimpl_print(fout, mkp->w, m, n);

	/* capacities */
	fprintf(fout, "param b[M] :=\n");
	long_long_array_zimpl_print(fout, mkp->b, m);

	/* desicion var */
	if(linear) {
		fprintf(fout, "var x[M] real;\n");
		fprintf(fout, "var v[N] real;\n");
	} else {
		fprintf(fout, "var x[M] binary;\n");
		fprintf(fout, "var v[N] binary;\n");
	}

	/* capacities constraint */
	fprintf(fout,
		"subto constraints:\n\
			forall <j> in N do\n\
				sum <i> in M do\n\
					x[i]*w[i, j] + v[j] >= p[j];\n");

	/* objective function */
	fprintf(fout,
		"minimize obj:\n\
			sum <i> in M do\n\
				x[i]*b[i] +\n\
			sum <j> in N do\n\
				v[j];\n");

	return;
}

double *mkp_solve_dual_with_scip(MKP *mkp){
	double *x, val, maxtime;
	int i, a, m, nread;
	char tempf[200], buff[400];
	FILE *pip, *out;

	m = mkp->m;
	x = double_array_init(NULL, m, 0.0);  /* solution array */
	maxtime = 60.0;

	/* temp file for mkp model */
	pip = popen("tempfile", "r");
	fscanf(pip, "%s", tempf);
	fclose(pip);

	/* write mkp dual model on temp file*/
	out = fopen(tempf, "w");
	mkp_dual_to_zimpl(out, mkp, 1);
	fclose(out);

	/* solve model */
	sprintf(buff, "zpl2lp %s | runscip %lf | scip2summary -s ", tempf, maxtime);
	pip = popen(buff, "r");

	/* reading solution */
	fscanf(pip, "%s", buff);    /* throwing away first line */
	i = 0;
	nread = fscanf(pip, "%d %lf", &a, &val);
	while( nread == 2 ){
		x[a-1] = val;
		nread = fscanf(pip, "%d %lf", &a, &val);
	}
	fclose(pip);

	return x;
}

void mkp_to_zimpl(FILE *fout, MKP *mkp, double max_opt, double capacity_scale, char linear){
	int i, j, n, m;
	double *b;
	double total_profit;

	n = mkp->n;
	m = mkp->m;
	total_profit = (double)long_long_array_sum(mkp->p, n);
	if( max_opt <= 0.0 ) max_opt = 1.0e+40;

	/* sizes */
	fprintf(fout, "param n := %d;\n", n);
	fprintf(fout, "param m := %d;\n", m);

	/* sets */
	fprintf(fout, "set N := {1 .. %d};\n", n);
	fprintf(fout, "set M := {1 .. %d};\n", m);

	/* profit */
	fprintf(fout, "param p[N] :=\n");
	long_long_array_zimpl_print(fout, mkp->p, n);

	/* weights */
	fprintf(fout, "param w[M*N] :=\n");
	long_long_matrix_zimpl_print(fout, mkp->w, m, n);

	/* scaling */
	if(capacity_scale == 0.0)
		capacity_scale = 1.0;
	b = (double*)malloc(m*sizeof(double));
	for( i = 0 ; i < m ; i++ )
		b[i] = mkp->b[i]*capacity_scale;

	/* capacities */
	fprintf(fout, "param b[M] :=\n");
	double_array_zimpl_print(fout, b, m);

	/* max objective (optional) */
	if( max_opt < total_profit )
		fprintf(fout, "param maxobj := %lf;\n", max_opt);

	/* desicion var */
	if(linear) fprintf(fout, "var x[N] real >= 0 <= 1;\n");
	else fprintf(fout, "var x[N] binary;\n");

	/* capacities constraint */
	fprintf(fout,
		"subto capacities:\n\
			forall <j> in M do\n\
				sum <i> in N do\n\
					x[i]*w[j, i] <= b[j];\n");

	/* objective constraint (optional) */
	if( max_opt < total_profit )
		fprintf(fout,
			"subto maxprofit:\n\
				sum <i> in N do\n\
					x[i]*p[i] <= maxobj;\n");

	/* objective function */
	fprintf(fout,
		"maximize profit:\n\
			sum <i> in N do\n\
				x[i]*p[i];\n");

	free(b);

	return;
}

double* mkp_get_lp_sol(MKP *mkp){
	LP *lp;
	if(!mkp->lp_sol){
		lp = mkp2lp(mkp, 1.0);
		mkp->lp_sol = lp_simplex(lp);
		lp_free(lp);
	}
	return (double*)memcpy(malloc(mkp->n*sizeof(double)), mkp->lp_sol, mkp->n*sizeof(double));
}

void _couting_fracs(double *x, int n,
	double *assigned,
	int *nnew_assigned,
	int *greater_var,
	double *greater_val){
	int j;

	*nnew_assigned = 0;
	*greater_val = 0.0;
	for( j = 0 ; j < n ; j++ ){
		if( x[j] > 0.0 && !assigned[j] ){
			/* the new non-zero is '1' */
			if( x[j] >= 1.0 )
				(*nnew_assigned)++;
			/* the new non-zero (is fractional) greater than others fractionals */
			else if( x[j] > *greater_val ){
				*greater_val = x[j];
				*greater_var = j;
			}
		}
	}
}

double *mkp_my_core_vals(MKP *mkp){
	int i, j, n;
	double *x, tic, scale, *assigned;
	int greater_var;      /* index of the greater fractional variable */
	double greater_val;   /* value of the greater fractional variable */
	int nnew_assigned;    /* number of new variables assigned to one */
	LP *lp;

	n = mkp->n;
	assigned = double_array_init(NULL, n, 0.0);

	scale = 0.0;
	for( i = 0 ; i < n ; i++ ){
		tic = 2.0/(double)n;
		nnew_assigned = 1;

		/* while none new fractional appeared OR a new '1' appeared .*/
		while( greater_val == 0.0 || nnew_assigned ){
			/* solving relaxation */
			x = mkp_get_lp_sol(mkp);

			/* counting new variables assigned to '1' or fractional */
			_couting_fracs(x, n,
				assigned,
				&nnew_assigned,
				&greater_var,
				&greater_val);

			/* adjust scaling (if needed) */
			if( nnew_assigned ) tic /= 2.0; /* new '1' appeared */
			else if( greater_val == 0.0 ) scale += tic; /* no new fractional */

			free(x);
		}
		/* updating assigment */
		assigned[greater_var] = (double)(n-i);

		/* updating next initial scale */
		scale += tic;
	}

	return assigned;
}

int _find_lesser_nonzero_nonassigned(double *x, double *assigned, int n){
	int i;
	int lesser_var;
	double lesser_val;

	lesser_val = 5.0;
	lesser_var = -1;
	for( i = 0 ; i < n ; i++ )
		if( x[i] > 0.0 && !assigned[i] )
			if( x[i] < lesser_val )
				{ lesser_val = x[i]; lesser_var = i; }

	return lesser_var;
}

double _do_right_search(MKP *mkp, int *assigned, int *var){
	int i, n;
	int n_std_ones;
	int n_ones;
	int n_ones_found;
	int n_fracs_found;
	int lesser_r_var;
	double lesser_val;
	double *x;
	double tic;
	double scale;
	LP *lp;

	n = mkp->n;

	/* solve std lp problem */
	lp = mkp2lp(mkp, 1.0);
	x = lp_simplex(lp);
	lp_free(lp);

	/* counting 'std' ones assigned variables */
	n_std_ones = 0;
	for( i = 0 ; i < n ; i++ )
		if( x[i] == 1.0 && !assigned[i] )
			n_std_ones++;


	scale = 1.0;
	tic = 2./(double)n;
	/* searching for not assigned fracional variables */
	do{
		/* solving lp problem */
		lp = mkp2lp(mkp, scale+tic);
		x = lp_simplex(lp);
		lp_free(lp);

		fprintf(debugout, "scale is %.3f ", scale+tic);
		double_array_fprint(debugout, x, n);
		fprintf(debugout, "\n");

		lesser_val = 1.0;
		n_fracs_found = n_ones_found = n_ones = 0;
		/* counting n_fracs_found not assigned */
		for( i = 0 ; i < n ; i++ ){
			if( x[i] == 1.0 )
				n_ones++;
			if( x[i] >= 1.0 && !assigned[i] )
				n_ones_found++;
			else if( x[i] > 0.0 && !assigned[i] ){
				n_fracs_found++;
				if( x[i] < lesser_val ){
					lesser_val = x[i];
					lesser_r_var = i;
				}
			}
		}

		/* adjust scale */
		if( n_ones_found > n_std_ones ) tic /= 2;
		else if( !n_fracs_found ) { scale += tic; tic *= 2; }
	/* while: no frac was found
	 *   AND some 0.0 variable turned to one
	 *   AND problem is not 'unbounded' yet */
	/* FIXME: check while expression */
	}while( !n_fracs_found && (n_ones < n) );

	/* lesser nonassigned frac is 'lesser_r_var' and its scale point is 'scale+tic' */
	if( n_ones == n) *var = -1;
	else *var = lesser_r_var;

	return scale+tic;
}

double _do_left_search(MKP *mkp, int *assigned, int *var){
	int i, n;
	int n_std_zeros;
	int n_zeros;
	int n_zeros_found;
	int n_fracs_found;
	int greater_l_var;
	double greater_val;
	double *x;
	double tic;
	double scale;
	LP *lp;

	n = mkp->n;

	/* solve std lp problem */
	lp = mkp2lp(mkp, 1.0);
	x = lp_simplex(lp);
	lp_free(lp);

	/* counting 'std' zeros assigned variables */
	n_std_zeros = 0;
	for( i = 0 ; i < n ; i++ )
		if( !assigned[i] && x[i] == 0.0 )
			n_std_zeros++;

	scale = 1.0;
	tic = 2./(double)n;
	/* searching for not assigned fracional variables */
	do{
		/* solving lp problem */
		lp = mkp2lp(mkp, scale-tic);
		x = lp_simplex(lp);
		lp_free(lp);

		fprintf(debugout, "scale is %.3f ", scale-tic);
		double_array_fprint(debugout, x, n);
		fprintf(debugout, "\n");

		greater_val = 0.0;
		n_fracs_found = n_zeros_found = n_zeros = 0;
		/* counting n_fracs_found not assigned */
		for( i = 0 ; i < n ; i++ ){
			if( x[i] == 0.0 )
				n_zeros++;
			if( !assigned[i] ){
				if( x[i] == 0.0 ){
					n_zeros_found++;
				} else if( x[i] < 1.0 ){
					n_fracs_found++;
					if( x[i] > greater_val ){
						greater_val = x[i];
						greater_l_var = i;
					}
				}
			}
		}

		/* adjust scale */
		if( n_zeros_found > n_std_zeros ) tic /= 2; /* if tic was too large */
		else if( !n_fracs_found ) {scale -= tic; tic *= 2; }     /* if tic was too small */
	/* while: no frac was found
	 *   AND some non-zero variable turned zero
	 *   AND problem is not 'unfeasible' yet */
	fprintf(debugout, "n_frac_found=%d, n_zeros_found=%d, n_std_zeros=%d, n_zeros=%d\n",
		n_fracs_found, n_zeros_found, n_std_zeros, n_zeros);
	/* FIXME: check while expression */
	}while( !n_fracs_found && (n_zeros < n) );

	/* lesser nonassigned frac is 'lesser_r_var' and its scale point is 'scale+tic' */
	if( n_zeros == n ) *var = -1;
	else *var = greater_l_var;

	return scale-tic;
}

/*
 * Second version of function, searching from "center".
 * */
double *mkp_my_core_vals2(MKP *mkp){
	int i, n, nfracs, nassigned, *assigned;
	double *x, r_scale, l_scale, *d_assigned, lesser_val;
	int greater_l_var, lesser_r_var;      /* index of the greater fractional variable */
	LP *lp;

	/* initializing */
	n = mkp->n;
	assigned = int_array_init(NULL, n, 0);
	nassigned = 0;

	/* solving lp */
	lp = mkp2lp(mkp, 1.0);
	x = lp_simplex(lp);
	lp_free(lp);

	double_array_fprint(stdout, x, n);
	fprintf(debugout, "\n");

	/* assigning fracs */
	do{
		nfracs = 0;
		lesser_val = 1.0;
		for( i = 0 ; i < n ; i++ ){
			/* if variable is frac */
			if( x[i] > 0.0 && x[i] < 1.0 ){
				/* if is not assigned yet */
				if( !assigned[i] ){
					nfracs++;
					/* if is lesser */
					if( x[i] < lesser_val ){
						lesser_r_var = i;
						lesser_val = x[i];
					}
				}
			}
		}

		/* assign lesser frac variable */
		if( nfracs ){
			assigned[lesser_r_var] = n+nassigned;
			nassigned++;
		}
	}while( nfracs );
	printf(" fracs for 1.0 done\n");

	
	r_scale = l_scale = 1.0;
	r_scale = _do_right_search(mkp, assigned, &lesser_r_var);
	fprintf(debugout, "first r_scale: var %d with scale=%.3f\n", lesser_r_var, r_scale);
	l_scale = _do_left_search(mkp, assigned, &greater_l_var);
	fprintf(debugout, "first l_scale: var %d with scale=%.3f\n", greater_l_var, l_scale);
	while( nassigned < n ){
		/* which side is less scalabled? */
		if( r_scale-1.0 < 1.0-l_scale ){ /* right is less */
			assigned[lesser_r_var] = n-nassigned;
			nassigned++;
			fprintf(debugout, "\n right is less scalabled with r_scale=%.3f\n", r_scale);
			r_scale = _do_right_search(mkp, assigned, &lesser_r_var);
			fprintf(debugout, " r_scale: var %d with scale=%.3f\n", lesser_r_var, r_scale);
		}else{  /* left is less */
			assigned[greater_l_var] = n+nassigned;
			nassigned++;
			fprintf(debugout, "\n left is less scalabled with l_scale=%.3f\n", l_scale);
			l_scale = _do_left_search(mkp, assigned, &greater_l_var);
			fprintf(debugout, " l_scale: var %d with scale=%.3f\n", greater_l_var, l_scale);
		}
		printf("-%d\n", nassigned); fflush(stdout);
	}

	/* convert assigned to double */
	d_assigned = double_array_alloc(n);
	for( i = 0 ; i < n ; i++ )
		d_assigned[i] = (double)assigned[i];

	free(assigned);

	return d_assigned;
}

/* mkp_reduced
 * generates a reduced MKP, with a subset of variables.
 *  mkp: the original problem
 *  var_vals: values of i-th variables:
 *    - 1: fixed on 1
 *    - 0: fixed on 0
 *    - other: free
 ***********************************************************/
MKP *mkp_reduced(MKP *mkp, int *var_vals){
	MKP *nmkp;
	int i, var, j, n, m;
	int *ones, *zeros, *frees;
	int nones, nzeros, nfrees;
	long long *p, **w, *b;

	/* problem parameters */
	p = mkp->p;
	w = mkp->w;
	b = mkp->b;
	n = mkp->n;
	m = mkp->m;

	/* allocing auxiliary arrays */
	ones = (int*)malloc(n*sizeof(int));
	zeros = (int*)malloc(n*sizeof(int));
	frees = (int*)malloc(n*sizeof(int));
	nones = nzeros = nfrees = 0;

	/* separating variables */
	for( i = 0 ; i < n ; i++ ){
		if( var_vals[i] == 1 )
			ones[nones++] = i;
		else if( var_vals[i] == 0 )
			zeros[nzeros++] = i;
		else frees[nfrees++] = i;
	}

	/*** setting new problem ***/
	nmkp = mkp_alloc(nfrees, m);

	/*copying weights and profits */
	for( i = 0 ; i < nfrees ; i++ ){ /* each free variable */
		var = frees[i];
		nmkp->p[i] = mkp->p[var];     /* copying profit */
		for( j = 0 ; j < m ; j++ )   /* each dimension */
			nmkp->w[j][i] = mkp->w[j][var]; /* copying weights */
	}

	/* copying capacities */
	long_long_array_copy(nmkp->b, mkp->b, m);

	/* reducing capacities spent by "ones" variables */
	for( i = 0 ; i < nones ; i++ ){
		var = ones[i];
		for( j = 0 ; j < m ; j++ )
			nmkp->b[j] -= mkp->w[j][var];
	}

	/* freeing auxiliary arrays */
	free(ones);
	free(zeros);
	free(frees);

	return nmkp;
}

/* Returns an array with the indexs of variables, sorted by increasing order
 * of efficiency.
 * */
int *mkp_core_val(MKP *mkp, char type){
	double *x, *vals, sum, sum2, *r;
	long long *p, *b, **w;
	int i, j, k, n, m;
	int *idxs;

	n = mkp->n;
	m = mkp->m;
	p = mkp->p;
	w = mkp->w;
	b = mkp->b;
	vals = (double*)malloc(n*sizeof(double));
	r = (double*)malloc(m*sizeof(double));

	switch(type){
		/* Simple Efficiency */
		case MKP_CORE_SIMPLE:
		for( i = 0 ; i < n ; i++ ){
			sum = 0.0;
			for( j = 0 ; j < m ; j++ )
				sum += w[j][i];
			vals[i] = p[i]/sum;
		}
		break;

		/* Scaled Efficiency */
		case MKP_CORE_SCALED:
		for( i = 0 ; i < n ; i++ ){
			sum = 0.0;
			for( j = 0 ; j < m ; j++ )
				sum += w[j][i]/b[j];
			vals[i] = p[i]/sum;
		}
		break;

		/* Senju and Toyoda */
		case MKP_CORE_ST:
		for( i = 0 ; i < n ; i++ ){
			sum = 0.0;
			for( j = 0 ; j < m ; j++ )
				for( k = 0 ; k < n ; k++ )
					sum += w[j][i]*(w[j][k]-b[j]);
			vals[i] = p[i]/sum;
		}
		break;

		/* Freville-Plateau efficiency */
		case MKP_CORE_FP:
		for( j = 0 ; j < m ; j++ ){ /* relevance values */
			sum = sum2 = 0;
			for( k = 0 ; k < n ; k++ ){
				sum += w[j][k] - b[j];
				sum2 += w[j][k];
			}
			r[j] = sum/sum2;
		}
		for( i = 0 ; i < n ; i++ ){
			sum = 0;
			for( j = 0 ; j < m ; j++ )
				sum += r[j]*w[j][i];
			vals[i] = p[i]/sum;
		}
		break;

		/* Duals efficiency */
		case MKP_CORE_DUALS:
		x = mkp_solve_dual_with_scip(mkp);
		for( j = 0 ; j < n ; j++ ){
			sum = 0.0;
			for( i = 0 ; i < m ; i++ )
				sum += w[i][j]*x[i];
			vals[j] = p[j]/sum;
		}
		free(x);
		break;

		/* proposed efficiency */
		case MKP_CORE_LP:
		free(vals);
		vals = mkp_my_core_vals(mkp);
		break;

		/* proposed efficiency, second method */
		case MKP_CORE_LP2:
		free(vals);
		vals = mkp_my_core_vals2(mkp);
		break;
	}
	idxs = double_index_sort(vals, n);

	free(r);
	free(vals);

	return idxs;
}

/* Returns a core problem for a MKP problem.
 * - core_size number of variables on core problem;
 *   Will record fix of variables on vars_fix array:
 *     1 - fixed on 1;
 *     0 - fixed on 0;
 *     other - free (in of core problem).
 * */
MKP *mkp_core_problem(MKP *mkp, int core_size, int **vars_fix){
	MKP *mkp_core;
	int i, j, idx, n, ncore, m, n_fst_fixed;
	int *efficieny_ordering;

	n = mkp->n;
	m = mkp->m;
	ncore = 0;

	/* computing efficiency measure */
	if( !(*vars_fix) )
		(*vars_fix) = (int*)malloc(n*sizeof(int));
	efficieny_ordering = mkp_core_val(mkp, MKP_CORE_DUALS);
	mkp_core = mkp_alloc(core_size, m);

	/* initializing capacities */
	for( j = 0 ; j < m ; j++ )
		mkp_core->b[j] = mkp->b[j];

	/* defining fixed variables */
	n_fst_fixed = (n-core_size)/2;
	for( i = 0 ; i < n_fst_fixed ; i++ ){                /* '1' fixed */
		idx = efficieny_ordering[i];
		(*vars_fix)[idx] = 1;
		for( j = 0 ; j < m ; j++ )  /* (consumed resources) */
			mkp_core->b[j] -= mkp->w[j][idx];
	}for( i = 0 ; i < core_size ; i++ ){                 /* free variables */
		idx = efficieny_ordering[n_fst_fixed+i];
		(*vars_fix)[idx] = -1;
		mkp_core->p[i] = mkp->p[idx];  /* coping profits */
		for( j = 0 ; j < m ; j++ )     /* coping weights */
			mkp_core->w[j][i] = mkp->w[j][idx];
	} for( i = core_size+n_fst_fixed ; i < n ; i++ )    /* '0' fixed */
		(*vars_fix)[efficieny_ordering[i]] = 0;
	
	printf("efficiency ordering:\n");
	int_array_fprint(stdout, efficieny_ordering, n);
	printf("\nvariable fixing:\n");
	int_array_fprint(stdout, (*vars_fix), n);

	free(efficieny_ordering);

	return mkp_core;
}

/*******************************************************************************
 ***                               MKPSOL                                    ***
*******************************************************************************/
MKPSol *mkpsol_new(MKP *mkp){
	int i;
	MKPSol *mkpsol;

	mkpsol = (MKPSol*)malloc(sizeof(MKPSol));
	mkpsol->x = int_array_init(NULL, mkp->n, 0);
	mkpsol->b_left = long_long_array_copy(NULL, mkp->b, mkp->m);
	mkpsol->obj = 0;
	mkpsol->feasible = 1;
	mkpsol->mkp = mkp;

	return mkpsol;
}

MKPSol *mkpsol_new_random(MKP *mkp){
	MKPSol *sol = mkpsol_new(mkp);
	int i, n;

	n = mkp->n;

	mkp->idxs = int_array_shuffle(mkp->idxs, n);

	for( i = 0 ; i < n ; i++ ){
		sol = mkpsol_add_item(sol, mkp->idxs[i]);
		if(!sol->feasible)
			mkpsol_rm_item(sol, mkp->idxs[i]);
	}

	return sol;
}

MKPSol *mkpsol_new_random_search(MKP *mkp){
	MKPSol *sol, *new;
	int i, n;

	sol = mkpsol_new_random(mkp);
	new = mkpsol_local_search(sol, sol->mkp->n/25);
	mkpsol_free(sol);

	return new;
}

MKPSol *mkpsol_new_lpls(MKP *mkp){
	MKPSol *mkpsol;

	mkpsol = mkpsol_local_search(mkp_get_lp_trunc(mkp), 4);

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
		mkpsol->b_left[i] -= mkp->w[i][a];
		if( mkpsol->b_left[i] < 0LL )
			mkpsol->feasible = 0;
	}

	return mkpsol;
}

MKPSol *mkpsol_rm_item(MKPSol *mkpsol, int a){
	MKP *mkp;
	int i, m;

	mkp = mkpsol->mkp;
	m = mkp->m;

	if( !mkpsol->x[a] ) {
		fprintf(stderr, "%s error: item %d-th item not in knapsak.\n",
			__PRETTY_FUNCTION__, a+1);
		return mkpsol;
	}

	mkpsol->x[a] = 0;
	mkpsol->obj -= mkp->p[a];
	mkpsol->feasible = 1;
	for( i = 0 ; i < m ; i++ ){
		mkpsol->b_left[i] += mkp->w[i][a];
		if( mkpsol->b_left[i] < 0LL )
			mkpsol->feasible = 0;
	}

	return mkpsol;
}

MKPSol *mkpsol_flip_item(MKPSol *mkpsol, int a){
	if( mkpsol->x[a] ) mkpsol = mkpsol_rm_item(mkpsol, a);
	else mkpsol = mkpsol_add_item(mkpsol, a);
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
	mkpsol_new->x = int_array_copy(NULL, mkpsol->x, mkpsol->mkp->n);
	mkpsol_new->b_left = long_long_array_copy(NULL, mkpsol->b_left, mkpsol->mkp->m);
	mkpsol_new->obj = mkpsol->obj;
	mkpsol_new->feasible = mkpsol->feasible;
	mkpsol_new->mkp = mkpsol->mkp;

	return mkpsol_new;
}

int mkpsol_dominated_by(MKPSol *ms1, MKPSol *ms2){
	int i, m;

	m = ms1->mkp->m;
	for( i = 0 ; i < m ; i++ )
		if( ms2->b_left[i] < ms1->b_left[i] )
			return 0;
	return( ms2->obj >= ms1->obj );
}

int mkpsol_dominates(MKPSol *ms1, MKPSol *ms2){
	int i, m;

	m = ms1->mkp->m;
	if( ms1->obj <= ms2->obj ) return 0;
	for( i = 0 ; i < m ; i++ )
		if( ms1->b_left[i] < ms2->b_left[i] )
			return 0;
	return 1;
}

int mkpsol_get_core_size(MKPSol *mkpsol, int *first_0p, int *last_1p){
	int first_0, last_1, i, n, core_size;
	int *x;

	n = mkpsol->mkp->n;
	x = mkpsol->x;
	i = 0;
	/* finding first 0 */
	while( i < n && x[i] == 1)
		i++;
	first_0 = i;
	/* finding last 1 */
	while( ++i < n )
		if( x[i] == 1)
			last_1 = i;

	core_size = last_1 - first_0 + 1;
	if(first_0p) *first_0p = first_0;
	if(last_1p) *last_1p = last_1;

	return core_size;
}

/*
 * prt_sol:
 *   0 - no solution
 *   1 - with solution
 *   2 - solution only
 * */
void mkpsol_fprint(FILE *fout, MKPSol *mkpsol, char ptr_sol){
	long long *b;
	int i, n, m;

	b = mkpsol->mkp->b;
	m = mkpsol->mkp->m;
	n = mkpsol->mkp->n;

	if( ptr_sol != 2 ){
		fprintf(fout, "%lld;[", mkpsol->obj);
		for( i = 0 ; i < m ; i++)
			fprintf(fout, "%lld%s", mkpsol->b_left[i], i+1==m?"]":",");
	}
	if(ptr_sol){
		if( ptr_sol == 1)
			printf(";");
		for( i = 0 ; i < n ; i++)
			fprintf(fout, "%d", mkpsol->x[i]);
	}
	fprintf(fout, "\n");
	return;
}

void mkpsol_free(MKPSol *mkpsol){
	free(mkpsol->x);
	free(mkpsol->b_left);
	free(mkpsol);
	return;
}

double *mkp_solve_with_scip(MKP *mkp, double maxtime, double capacity_scale, char linear){
	double *x, val;
	int a, n, nread;
	char tempf[200], buff[400];
	FILE *pip, *out;

	n = mkp->n;
	x = double_array_init(NULL, n, 0.0);  /* solution array */

	/* temp file for mkp model */
	pip = popen("tempfile", "r");
	fscanf(pip, "%s", tempf);
	fclose(pip);

	/* write mkp model on temp file*/
	out = fopen(tempf, "w");
	mkp_to_zimpl(out, mkp, 0.0, capacity_scale, linear);
	fclose(out);

	/* solve model */
	sprintf(buff, "zpl2lp %s | runscip %lf | scip2summary -s ", tempf, maxtime);
	pip = popen(buff, "r");

	/* read solution */
	fscanf(pip, "%s", buff);
	nread = fscanf(pip, "%d %lf", &a, &val);
	while( nread == 2 ){
		x[a-1] = val;
		nread = fscanf(pip, "%d %lf", &a, &val);
	}
	fclose(pip);

	return x;
}

MKPSol *mkpsol_solve_with_scip(MKP *mkp, double maxtime, double capacity_scale, char linear){
	MKPSol *sol;
	int i, n;
	double *x;

	n = mkp->n;
	sol = mkpsol_new(mkp);

	/* Solve MKP */
	x = mkp_solve_with_scip(mkp, maxtime, capacity_scale, linear);

	/* Set solution */
	for( i = 0 ; i < n ; i++ )
		if( x[i] == 1.0 )
			mkpsol_add_item(sol, i);

	free(x);

	return sol;
}

MKPSol *mkpsol_from_lp(MKP *mkp){
	return mkpsol_solve_with_scip(mkp, 60.0, 1.0, 1);
}

MKPSol *greedy_mkp(MKP *mkp){
	unimplemented();
}

MKPSol *mkpsol_local_search(MKPSol *mkpsol, int niter){
	MKPSol *current, *best;
	long long current_profit;
	int best_item, item, i, j, n, q, *idxs;

	/* initialization */
	idxs = mkpsol->mkp->idxs;
	current = mkpsol_copy(mkpsol);
	best = mkpsol_copy(mkpsol);
	current_profit = current->obj;
	n = mkpsol->mkp->n;

	/* iteration loop */
	q = 1+n/2;
	for( i = 0 ; i < niter ; i++ ){
		idxs = int_array_shuffle(idxs, n);
		best_item = 0;
		current_profit = 0;
		/* sort moves */
		for( j = 0 ; j < q ; j++ ){
			item = idxs[j];
			current = mkpsol_flip_item(current, item);
			/* best move yet? */
			if( current->obj > current_profit && current->feasible )
				{ best_item = item; current_profit = current->obj; }
			current = mkpsol_flip_item(current, item);
		}
		current = mkpsol_flip_item(current, best_item);
		/* is best global? */
		if( current->obj > best->obj )
			{ mkpsol_free(best); best = mkpsol_copy(current); }
	}

	mkpsol_free(current);

	return best;
}

/* repair a solution, removing at first itens that minimum reduces profit */
MKPSol *mkpsol_greedy_repair(MKPSol *mkpsol){
	int i, a, n;
	long long profit;
	n = mkpsol->mkp->n;

	/* removing item */
	for( i = 0 ; i < n && !mkpsol->feasible ; i++ )
		if( mkpsol->x[n-i-1] )
			mkpsol_rm_item(mkpsol, n-i-1);
	
	return mkpsol;
}

/*  */
MKPSol *mkpsol_cross(MKPSol *child, MKPSol *father, int c){
	int i, n, *idxs, idx;
	int a, b;

	n = child->mkp->n;
	idxs = child->mkp->idxs;
	int_array_shuffle(idxs, n);

	/* copy 'c' variables (chosen reandomly) from father */
	for( i = 0 ; i < c ; i++ ){
		idx = idxs[i];
		a = father->x[idx];
		b = child->x[idx];

		/* flip if not equal */
		if( a != b )
			mkpsol_flip_item(child, idx);
	}
	
	/* repair, if not feasibble */
	if( !child->feasible ){
		child = mkpsol_greedy_repair(child);
	}

	return child;
}

/* n/2 */
MKPSol *mkpsol_cross1(MKPSol *child, MKPSol *father)
	{ return mkpsol_cross(child, father, (child->mkp->n/2)); }
/* n/5 */
MKPSol *mkpsol_cross2(MKPSol *child, MKPSol *father)
	{ return mkpsol_cross(child, father, (child->mkp->n/5)); }
/* n/10 */
MKPSol *mkpsol_cross3(MKPSol *child, MKPSol *father)
	{ return mkpsol_cross(child, father, (child->mkp->n/10)); }


/*
 * Nemhauser-Ullman Algorithm for MKP.
 * */
Array *mkp_nemull(MKP *mkp){
	Array *dom_sets, *merged_sets;
	MKPSol *new_sol, *old_sol;
	int n, m, i, j, k, n_dom_sets, n_merged_sets, not_dominated_by;
	long long **w, *p, *b;
	n = mkp->n;

	/* initializing sets */
	dom_sets = array_new();
	merged_sets = array_new();

	for( i = 0 ; i < n ; i++ ){
		/* generating new sets from old ones */
		n_dom_sets = array_get_size(dom_sets);
		for( j = 0 ; j < n_dom_sets ; j++ ){
			/* merging sets */
			old_sol = array_get(dom_sets, j);   /* inserting old solution */
			array_insert(merged_sets, old_sol);

			new_sol = mkpsol_copy(old_sol);     /* inserting new solution */
			mkpsol_add_item(new_sol, i);
			array_insert(merged_sets, new_sol);
		}
		/* the i-th item alone is a candidate set */
		new_sol = mkpsol_new(mkp);
		mkpsol_add_item(new_sol, i);
		array_insert(merged_sets, new_sol);

		/* now emptying the dom sets */
		dom_sets = array_empty(dom_sets);

		/* filtering: checking dominance of each set */
		for( j = 0 ; j < array_get_size(merged_sets); j++ ){
			new_sol = array_get(merged_sets, j);
			not_dominated_by = 1;
			/* scan all current sets */
			for( k = 0 ; k < array_get_size(merged_sets) && not_dominated_by ; k++ ){
				old_sol = array_get(merged_sets, k);
				if(new_sol != old_sol )
					not_dominated_by &= !mkpsol_dominated_by(new_sol, old_sol);
				not_dominated_by &= new_sol->feasible; /* only feasible sets (optimization) */
			}
			/* the solution is not dominated by no one? */
			if( not_dominated_by )
				array_insert(dom_sets, new_sol);                 /* add */
			else{        /* free set */
				mkpsol_free(new_sol);
				array_remove(merged_sets, j--);
			}
		}
		/* TODO: Report current sets... */
		/* empty the merged set struct */
		merged_sets = array_empty(merged_sets);
	}

	array_free(merged_sets);

	return dom_sets;
}

/*******************************************************************************
*       DES interface implementation
*******************************************************************************/
/* activate */
double mkp_des_activate(double trust, int val, double step){
	double prob;

	/*step *= step;*/
	prob = val ? 1.0-trust : trust;
	prob = prob*step + 0.5*(1.0-step);

	return prob;
}

/* set */
MKPSol *mkpsol_set(MKPSol *mkpsol, int a, int val){
	if( mkpsol->x[a] != val ){
		if(val) mkpsol_rm_item(mkpsol, a);
		else mkpsol_add_item(mkpsol, a);
	}

	return mkpsol;
}

/* get */
int mkpsol_get(MKPSol *mkpsol, int a){
	return mkpsol->x[a];
}
/* cmp-profit */
int mkpsol_cmp_profit(MKPSol *ms1, MKPSol *ms2){
	if( ms1->obj > ms2->obj) return 1;
	else if(ms1->obj < ms2->obj ) return (-1);
	return 0;
}
/* cmp-j-th-weight */
int mkpsol_cmp_weight(MKPSol *ms1, MKPSol *ms2, int *j){
	long long b_left1, b_left2;
	b_left1 = ms1->b_left[*j];
	b_left2 = ms2->b_left[*j];
	if( b_left1 < b_left2 ) return 1;
	else if( b_left1 > b_left2 ) return (-1);
	return 0;
}
/* obj */
double mkpsol_fitness(MKPSol *mkpsol){
	return (double)mkpsol->obj;
}
/* feasible */
int mkpsol_feasible(MKPSol *mkpsol){
	return mkpsol->feasible;
}
/* new solution */
MKPSol *mkp_des_new_solution(MKP *mkp){
	return mkpsol_new_random(mkp);
}

/* random repair a solution, removing random itens until feasibility */
MKPSol *mkpsol_repair(MKPSol *mkpsol){
	int i, n, *idxs;
	n = mkpsol->mkp->n;

	/* shuffling itens */
	idxs = mkpsol->mkp->idxs;
	idxs = int_array_shuffle(idxs, n);

	/* removing item */
	for( i = 0 ; i < n && !mkpsol->feasible ; i++ )
		if( mkpsol->x[idxs[i]] )
			mkpsol_rm_item(mkpsol, idxs[i]);
	
	return mkpsol;
}

/* Returns a original solution, extracted from a MKP core problem solution. */
MKPSol *mkpsol_from_mkp_core(MKPSol *core_sol, MKP *orig_mkp, int *vars_fix){
	MKPSol *sol;
	int i, j, n, m;

	n = orig_mkp->n;
	m = orig_mkp->m;

	sol = mkpsol_new(orig_mkp);

	j = 0;
	for( i = 0 ; i < n ; i++ ){
		/* fixed in '1' */
		if( vars_fix[i] == 1 ){
			mkpsol_add_item(sol, i);
		/* variable in core problem */
		}else if( vars_fix[i] ){
			if( core_sol->x[j++] ){
				mkpsol_add_item(sol, i);
			}
		}
	}

	return sol;
}

DES_Interface *mkp_des_interface(){
	DES_Interface *desi;

	desi = (DES_Interface*)malloc(sizeof(DES_Interface));
	desi->activate = NULL;
	desi->set = (des_set_f)mkpsol_set;
	desi->get = (des_get_f)mkpsol_get;
	desi->fitness = (des_fitness_f)mkpsol_fitness;
	desi->feasible = (des_feasible_f)mkpsol_feasible;
	desi->repair = (des_repair_f)mkpsol_repair;
	desi->new_solution = (des_new_solution_f)mkpsol_new_random;
	desi->copy_solution = (des_copy_solution_f)mkpsol_copy;
	desi->free_solution = (des_free_solution_f)mkpsol_free;

	return desi;
}

SFL_Interface *mkp_sfl_interface(int cross, int newsol){
	SFL_Interface *sfli;

	sfli = (SFL_Interface*)malloc(sizeof(SFL_Interface));
	/* crossing rule */
	switch(cross){
		case 1: sfli->cross = (sfl_cross_f)mkpsol_cross1; break;
		case 2: sfli->cross = (sfl_cross_f)mkpsol_cross2; break;
		case 3: sfli->cross = (sfl_cross_f)mkpsol_cross3; break;
		default: sfli->cross = (sfl_cross_f)mkpsol_cross3; break;
	}
	switch(newsol){
		case 1: sfli->new_solution = (sfl_new_solution_f)mkpsol_new_random; break;
		case 2: sfli->new_solution = (sfl_new_solution_f)mkpsol_new_random_search; break;
		default: sfli->new_solution = (sfl_new_solution_f)mkpsol_new_random; break;
	}
	sfli->fitness = (sfl_fitness_f)mkpsol_fitness;
	sfli->copy_solution = (sfl_copy_solution_f)mkpsol_copy;
	sfli->free_solution = (sfl_free_solution_f)mkpsol_free;

	return sfli;
}

