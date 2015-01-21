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
	for( m = 0 ; m <= mmax ; m += 5 ){
		if(!m) m = 1;
		for( n = 50 ; n <= nmax ; n += 25 ){
			for( t = 0.25 ; t < 1.0 ; t += 0.25 ){
				/* generate problem */
				mkp = mkp_random(n, m, t, 1000);

				/* solve on scip */
				sol = mkpsol_solve_with_scip(mkp, 300.0, 0);

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

int main(int argc, char **argv){
	return execute_core_search(argc, argv);
}

