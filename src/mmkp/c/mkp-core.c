#include <stdio.h>
#include <stdlib.h>

void print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, " Tool for investigating core concepts for MKP.\n");
	fprintf(out, "   input file: a MKP instance. If no file is given, instance is read from stdin.\n");

	return;
}

int execute_core_search(int argc, char **argv){
	MKP *mkp;
	FILE *input;
	MKPSol *sol;
	double t;
	int i, n, t, m, nmax, mmax, niter;

	/* read input problem */
	input = stdin;
	if(argc > 1) input = fopen(argv[1], "r");
	mkp = mkp_read_from_file(input);
	fclose(input);

	mmax = 30
	nmax = 300;
	t = 0.5;
	for( m = 1 ; m <= mmax ; m += 10 ){
		for( n = 50 ; n <= nmax ; n += 25 ){
			for( t = 0.25 ; t < 1.0 ; t += 0.25 ){
				/* generate problem */
				mkp = mkp_random(n, m, t, 1000);

				/* solve on scip */
				sol = mkpsol_solve_with_scip(mkp, 300.0, 0);

				/* core size*/
				core_size = mkpsol_get_core_size(sol);

				/* "center" */

				mkpsol_free(core_size);
				mkp_free(mkp);
			}
		}
	}

	return 0;
}

int main(int argc, char **argv){
	return execute_core_search(argc, argv);
}

