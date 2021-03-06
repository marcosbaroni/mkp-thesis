/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: june, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "mmkp/mkp/mkp.h"

void print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [seed=time(NULL)]\n", argv[0]);
	fprintf(out, " Tool for investigating core concepts for MKP.\n");

	return;
}

int execute_core_search(int argc, char **argv){
	MKP *mkp;
	FILE *input;
	MKPSol *sol;
	double t;
	int i, n, m, nmax, mmax, niter, first0, last1, core_size;

	/* read input problem */
	if( argc > 1 ) srand(atoll(argv[1]));
	else srand(time(NULL));

	mmax = 30;
	nmax = 300;
	t = 0.5;
	/* number of dimentions */
	for( m = 0 ; m <= mmax ; m += 5 ){
		if(!m) m = 1;
		/* number of items */
		for( n = 50 ; n <= nmax ; n += 25 ){
			/* tightness ratio */
			for( t = 0.25 ; t < 1.0 ; t += 0.25 ){
				/* generate problem */
				mkp = mkp_random(n, m, t, 1000);

				/* solve on scip */
				sol = mkpsol_solve_with_scip(mkp, 300.0, 1.0, 0);

				/* core size*/
				core_size = mkpsol_get_core_size(sol, &first0, &last1);
				printf("%d;%d;%.2lf;%d;%d;%d;", n, m, t, first0, last1, core_size);
				mkpsol_fprint(stdout, sol, 2);

				/* free */
				mkpsol_free(sol);
				mkp_free(mkp);
			}
		}
	}

	return 0;
}

int execute_fractional_search(int argc, char **argv){
	FILE *input;
	MKP *mkp;
	MKPSol *sol;
	int i, n, m;
	double scale;

	if( argc < 2 ){
		printf(" usage: %s <input problem>\n", argv[0]);
		printf(" Tool for investigating core concepts for MKP.\n");
		printf("  <input problem>: file name of input problem, '-' for stdin.\n");
		return 1;
	}

	/* reading input problem */
	if(strcmp(argv[1], "-"))
		input = fopen(argv[1], "r");
	else input = stdin;
	mkp = mkp_read_from_file(input);
	fclose(input);

	n = mkp->n;
	m = mkp->m;

	/* solving scaled problems. */
	for( i = 1 ; i <= 2*n ; i++ ){
		scale = (i/(double)n);
		sol = mkpsol_solve_with_scip(mkp, 30.0, scale, 1);
		printf("%.2lf:", scale);
		mkpsol_fprint(stdout, sol, 2);
		mkpsol_free(sol);
	}

	/* opt */
	sol = mkpsol_solve_with_scip(mkp, 30.0, 1.0, 0);
	printf("-OPT:");
	mkpsol_fprint(stdout, sol, 2);

	mkpsol_free(sol);
	mkp_free(mkp);

	return 0;
}

int execute_core_test(int argc, char **argv){
	double *lp_sol;
	int i, j, n, m, a, *var_vals;
	int em[6], nem;   /* em: efficiency measure */
	int *em_ordering, *em_position;
	int core_size, core0, coref, core_center, fst_frac, last_frac;
	int *fracs, nfracs;
	MKP *mkp, *core_mkp;
	MKPSol *sol, *opt;
	FILE *input;
	long long total_profit;

	setdebug(1);

	if(argc < 2 ){
		printf("usage %s <mkp instance>\n", argv[0]);
		printf(" <mkp instance>: mkp instance file. '-' for stdin.\n");
		return 1;
	}

	/* set efficiency measures types */
	nem = 2;
	em[0] =  MKP_CORE_LP2;
	em[1] =  MKP_CORE_DUALS;
	em[2] =  MKP_CORE_LP;
	em[3] =  MKP_CORE_SIMPLE;
	em[4] =  MKP_CORE_SCALED;
	em[5] =  MKP_CORE_ST;
	em[6] =  MKP_CORE_FP;

	/* read input problem */
	if( strcmp(argv[1], "-") != 0 )
		input = fopen(argv[1], "r");
	else input = stdin;
	mkp = mkp_read_from_file(input);
	fclose(input);

	n = mkp->n;
	m = mkp->m;
	fprintf(debugout, "problem read n=%d, m=%d\n", n, m);
	fracs = (int*)malloc(n*sizeof(int));
	var_vals = (int*)malloc(n*sizeof(int));
	em_position = (int*)malloc(n*sizeof(int));
	core_size = m+(n/10);

	/* solve opt */
	fprintf(debugout, "finding optimal with scip\n");
	opt = mkpsol_solve_with_scip(mkp, 10.0, 1.0, 0);

	/* find the fractional variables (lp solution) */
	fprintf(debugout, "finding fractionals\n");
	lp_sol = mkp_get_lp_sol(mkp);
	nfracs = 0;
	for( i = 0 ; i < n ; i++ )
		if( lp_sol[i] > 0 && lp_sol[i] < 1.0 ) /* is fractional? */
			fracs[nfracs++] = i;

	fprintf(debugout, "fracs: ");
	int_array_fprint(debugout, fracs, nfracs);
	fprintf(debugout, "\n");
	for( i = 0 ; i < nem ; i++ ){
		fprintf(debugout, "\nexecuting core %d\n", em[i]);
		/* get "core ordering" of each heuristics, i.e.,
		**  an array of variables index, sorted by descending "efficienct measure". */
		em_ordering = mkp_core_val(mkp, em[i]);
		for( j = 0 ; j < n ; j++ )
			em_position[em_ordering[j]] = j;
		int_array_fprint(debugout, em_ordering, n);
		fprintf(debugout, "\n");
		int_array_fprint(debugout, em_position, n);
		fprintf(debugout, "\n");
	
		fprintf(debugout, "\nfinding core center: ");
		/* find center of each efficiency measure */
		fst_frac = n;
		last_frac = 0;
		for( j = 0 ; j < nfracs ; j++ ){
			if( em_position[fracs[j]] > last_frac )
				last_frac = em_position[fracs[j]];
			if( em_position[fracs[j]] < fst_frac )
				fst_frac = em_position[fracs[j]];
		}
		core_center = (fst_frac+last_frac)/2;
		fprintf(debugout, "%d\n", core_center);

		fprintf(debugout, "configuring core:\n");
		core0 = core_center - (core_size/2) + 1;
		coref = core0+core_size;   /* core final position */
		/* configuring core */
		// FIXME
		total_profit = 0;
		for( j = 0 ; j < core0 ; j++ )
			{ a = em_ordering[j]; var_vals[a] = 1; total_profit += mkp->p[a]; }
		for( ; j < coref ; j++ )
			{ a = em_ordering[j]; var_vals[a] = -1; fprintf(debugout, "%d ", a); }
		for( ; j < n ; j++ )
			{ a = em_ordering[j]; var_vals[a] = 0; }
		fprintf(debugout, "\n");
		core_mkp = mkp_reduced(mkp, var_vals);

		/* solving core problem */
		fprintf(debugout, "solving core problem\n");
		sol = mkpsol_solve_with_scip(core_mkp, 600.0, 1.0, 0);
		fprintf(stdout, "%d;%lld;", em[i], total_profit+sol->obj);
		fflush(stdout);

		mkpsol_free(sol);
		mkp_free(core_mkp);
		free(em_ordering);
	}
	fprintf(stdout, "\n");

	mkpsol_free(opt);
	free(em_position);
	free(var_vals);
	free(fracs);
	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
	return execute_core_test(argc, argv);
	//return execute_core_search(argc, argv);
	//return execute_fractional_search(argc, argv);
}

