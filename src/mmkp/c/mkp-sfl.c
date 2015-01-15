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
	fprintf(out, " usage: %s <input file> <n of memeplex> <size of memeplex> <size of submemeplex> <n of iterations> <n of subiterations> [cross type=(1|2|3)] [new sol type=(1|2)] [seed=time(null)]\n", argv[0]);
	fprintf(out, " Shuffled Frog Leaping Algorithm for MKP.\n");
	fprintf(out, "   input file: a MKP instance. If no file is given, instance is read from stdin.\n");
	fprintf(out, "   Program outputs \"<profit of solution>;<best iter>\"\n");

	return 1;
}

int execute_sfl_mkp(int argc, char **argv){
	MKP *mkp;
	FILE *input;
	MKPSol *best_sol, *sol;
	SFL_Interface *sfli;
	int i, seed, nmeme, meme_size, q, niter, subniter, best_iter, cross, newsol;
	clock_t c0, cf;         /* c0 = clock(); */

	/* checking input */
	cross = 1;
	newsol = 1;
	seed = time(NULL);
	input = stdin;
	if(strcmp(argv[1], "-")) input = fopen(argv[1], "r");
	nmeme = atoll(argv[2]);
	meme_size = atoll(argv[3]);
	q = atoll(argv[4]);
	niter = atoll(argv[5]);
	subniter = atoll(argv[6]);
	if( argc > 7 ) cross = atoll(argv[7]);
	if( argc > 8 ) newsol = atoll(argv[8]);
	if( argc > 9 ) seed = atoll(argv[9]);

	srand(seed);

	/* reading problem instance */
	mkp = mkp_read_from_file(input);
	fclose(input);
	sfli = mkp_sfl_interface(cross, newsol);

	/* solving problem */
	c0 = clock();
	sol = sfl(sfli, mkp, nmeme, meme_size, q, niter, subniter, &best_iter);
	cf = clock();

	/* output */
	printf("%lld;%d;%f\n", sol->obj, best_iter, ((cf-c0)/(float)CLOCKS_PER_SEC));

	/* freeing */
	mkpsol_free(sol);
	mkp_free(mkp);
	sfli_free(sfli);

	return 0;
}

int main(int argc, char **argv){
	if( argc < 7 )
		return print_usage(argc, argv);
	return execute_sfl_mkp(argc, argv);
}

