#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mmkp/mkp.h"
#include "mmkp/util.h"
#include "mmkp/sfl.h"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s <input file> <n of memeplex> <size of memeplex> <size of submemeplex> <n of iterations>\n", argv[0]);
	fprintf(out, " Shuffled Frog Leaping Algorithm for MKP.\n");
	fprintf(out, "   input file: a MKP instance. If no file is given, instance is read from stdin.\n");
	fprintf(out, "   Program outputs \"<profit of solution>;<solution>\"\n");

	return 1;
}

int execute_sfl_mkp(int argc, char **argv){
	MKP *mkp;
	FILE *input;
	MKPSol *best_sol, *sol;
	SFL_Interface *sfli;
	int i, n, m, q, niter;
	clock_t c0, cf;

	/* checking input */
	input = stdin;
	if(strcmp(argv[1], "-")) input = fopen(argv[1], "r");
	n = atoll(argv[2]);
	m = atoll(argv[3]);
	q = atoll(argv[4]);
	niter = atoll(argv[5]);

	/* reading problem instance */
	mkp = mkp_read_from_file(input);
	fclose(input);
	sfli = mkp_sfl_interface();

	//c0 = clock();
	//best_sol = sfl(sfli, mkp, mkp->n, m, n, q, niter);
	//cf = clock();

	sol = mkpsol_new_random(mkp);
	printf("random\n");
	mkpsol_fprint(stdout, sol);

	printf("random + local\n");
	best_sol = mkpsol_local_search(sol, niter);
	mkpsol_fprint(stdout, best_sol);

	mkpsol_free(sol);
	mkpsol_free(best_sol);

	sol = mkpsol_from_lp(mkp);
	printf("lp\n");
	mkpsol_fprint(stdout, sol);

	best_sol = mkpsol_local_search(sol, niter);
	printf("lp + local\n");
	mkpsol_fprint(stdout, best_sol);

	mkpsol_free(sol);
	mkpsol_free(best_sol);

	mkp_free(mkp);
	sfli_free(sfli);

	return 0;
}

int main(int argc, char **argv){
	if( argc < 6 )
		return print_usage(argc, argv);
	return execute_sfl_mkp(argc, argv);
}
