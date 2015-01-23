#include <stdio.h>
#include <stdlib.h>
#include "mmkp/mkp.h"

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
				core_size = int_array_sum(sol->x, n);
				printf("%d;%d;%.2lf;%d\n", n, m, t, core_size);

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

int main(int argc, char **argv){
	//return execute_core_search(argc, argv);
	return execute_fractional_search(argc, argv);
}

