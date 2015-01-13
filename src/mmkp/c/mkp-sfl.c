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
	int i, n, m, q, niter, best_iter;
	clock_t c0, cf;         /* c0 = clock(); */

	srand(time(NULL));

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

	/* solving problem */
	sol = sfl(sfli, mkp, n, m, q, niter, &best_iter);
	mkpsol_fprint(stdout, sol, 0);

	printf("best_iter = %d\n", best_iter);
	sol = mkp_get_lp_trunc(mkp);
	printf("LP:\n");
	mkpsol_fprint(stdout, sol, 0);
	mkpsol_free(sol);

	/* freeing */
	mkpsol_free(sol);
	mkp_free(mkp);
	sfli_free(sfli);

	return 0;
}

/* testing triangular probability */
int test_triang(int k){
	int i, n, v[10];

	n = 4;
	srand(time(NULL));

	for( i = 0 ; i < n ; i++ )
		v[i] = 0;
	
	for( i = 0 ; i < k; i++ )
		v[triang_raffle(n-1)]++;
	
	for( i = 0 ; i < n ; i++ )
		printf("%d - %f - %f\n", i, v[i]/(float)k, (i+1)/(float)(n*(n+1)/2));

	return 0;
}

int main(int argc, char **argv){
	if( argc < 6 )
		return print_usage(argc, argv);
	return execute_sfl_mkp(argc, argv);
}

