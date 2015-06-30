#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mmkp/mkp/mkp.h"
#include "mmkp/util.h"
#include "mmkp/sfl.h"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s <input file> <n of memeplex> <size of memeplex> <size of submemeplex> <n of iterations> <n of subiterations> [cross type=(1|2|3)] [new sol type=(1|2)] [seed=time(null)]\n", argv[0]);
	fprintf(out, " Shuffled Frog Leaping Algorithm for MKP.\n");
	fprintf(out, " - input file: a MKP instance. If no file is given, instance is read from stdin.\n");
	fprintf(out, " - Program outputs \"<profit of solution>;<best iter>\"\n");
	fprintf(out, " - cross types:\n");
	fprintf(out, "    1 - 50%% from father\n");
	fprintf(out, "    2 - 20%% from father\n");
	fprintf(out, "    3 - 10%% from father\n");
	fprintf(out, " - new solutions types:\n");
	fprintf(out, "    1 - random solution\n");
	fprintf(out, "    2 - random solution + local search (experimental)\n");

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
	if( argc < 7 ){
		print_usage(argc, argv);
		return 1;
	}

	/* reading input */
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
	printf(";");
	mkpnum_fprintf(stdout, sol->obj);
	mkpnum_fprintf(";%d;%f\n", best_iter, ((cf-c0)/(float)CLOCKS_PER_SEC));

	/* freeing */
	mkpsol_free(sol);
	mkp_free(mkp);
	sfli_free(sfli);

	return 0;
}

int print_usage_core(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s <input file> <n of memeplex> <size of memeplex> <size of submemeplex> <n of iterations> <n of subiterations> [cross type=(1|2|3)] [new sol type=(1|2)] [seed=time(null)]\n", argv[0]);
	fprintf(out, " Shuffled Frog Leaping Algorithm for MKP.\n");
	fprintf(out, " - Input file: a MKP instance. If no file is given, instance is read from stdin.\n");
	fprintf(out, " - Program outputs \"<profit of solution>;<best iter>\"\n");
	fprintf(out, " - Cross types:\n");
	fprintf(out, "    1 - 50%% from father\n");
	fprintf(out, "    2 - 20%% from father\n");
	fprintf(out, "    3 - 10%% from father\n");
	fprintf(out, " - New solutions types:\n");
	fprintf(out, "    1 - random solution\n");
	fprintf(out, "    2 - random solution + local search (experimental)\n");

	return 1;
}

void print_core_usage(int argc, char **argv){
	FILE *out;

	out = stderr;
	fprintf(out, " usage: %s <input file> [n of memeplex=20] [size of memeplex=20] [size of submemeplex=5] [n of iterations=300] [n of subiterations=20] [core_size=m+n/10] [seed=time(null)]\n", argv[0]);
	fprintf(out, " Shuffled Frog Leaping Algorithm for MKP.\n");
	fprintf(out, " - Input file: a MKP instance. If no file is given, instance is read from stdin.\n");
	fprintf(out, " - Program outputs \"<profit of solution>;<best iter>\"\n");
	fprintf(out, " - Cross types:\n");
	fprintf(out, "    1 - 50%% from father\n");
	fprintf(out, "    2 - 20%% from father\n");
	fprintf(out, "    3 - 10%% from father\n");
	fprintf(out, " - New solutions types:\n");
	fprintf(out, "    1 - random solution\n");
	fprintf(out, "    2 - random solution + local search (experimental)\n");
	return;
}

int execute_sfl_mkp_core(int argc, char **argv){
	MKP *mkp, *mkp_core;
	FILE *input;
	SFL_Interface *sfli;
	MKPSol *sol, *core_sol;
	int n, m;
	int seed;         /* seed for random generator */
	int nmeme;        /* n of memeplexs */
	int meme_size;    /* size of each memeplex */
	int submeme_size; /* size of each submemeplex */
	int niter;        /* n iterations */
	int subniter;     /* n subiterations */
	int best_iter;    /* best iteration */
	int id_cross;     /* id of crossing function */
	int id_newsol;    /* id of new solution function */
	int core_size;    /* core size */
	clock_t c0, cf;   /* c0 = clock(); for couting time spent */
	int *vars_fix = NULL;

	/* checking input */
	if( argc < 2 ){
		print_core_usage(argc, argv);
		return 1;
	}

	/* default values of arguments */
	nmeme = 20;
	meme_size = 20;
	submeme_size = 5;
	niter = 300;
	subniter = 20;
	seed = time(NULL);
	input = stdin;
	core_size = 0;

	/* reading arguments */
	if(strcmp(argv[1], "-"))
		input = fopen(argv[1], "r");
	if( argc > 2 ) nmeme = atoll(argv[2]);
	if( argc > 3 ) meme_size = atoll(argv[3]);
	if( argc > 4 ) submeme_size = atoll(argv[4]);
	if( argc > 5 ) niter = atoll(argv[5]);
	if( argc > 6 ) subniter = atoll(argv[6]);
	if( argc > 7 ) core_size = atoll(argv[7]);
	if( argc > 8 ) seed = atoll(argv[8]);

	/* inicializing random generation */
	srand(seed);

	/* reading problem instance */
	mkp = mkp_read_from_file(input);
	fclose(input);

	printf("MKP problem\n");
	mkp_fprint(stdout, mkp); fflush(stdout);

	n = mkp->n;
	m = mkp->m;
	if(!core_size)
		core_size = m + n/10;
	
	sfli = mkp_sfl_interface();

	c0 = clock();
	/* generation MKP core problem */
	mkp_core = mkp_core_problem(mkp, core_size, &vars_fix);
	printf("\nMKP CORE problem\n"); fflush(stdout);
	mkp_fprint(stdout, mkp_core); fflush(stdout);
	/* solving core problem */
	core_sol = (MKPSol*)sfl(sfli, mkp_core, nmeme, meme_size, submeme_size, niter, subniter, &best_iter);
	/* extracting solution from original problem */
	sol = mkpsol_from_mkp_core(core_sol, mkp, vars_fix);
	cf = clock();

	/* TODO: test/validate this code... */
	mkpsol_free(core_sol);
	mkp_free(mkp_core);
	sfli_free(sfli);

	/* output */
	printf("- ");
	mkpnum_fprintf(stdout, sol->obj);
	printf("%d;%f\n", best_iter, ((cf-c0)/(float)CLOCKS_PER_SEC));

	/* freeing */
	mkpsol_free(sol);
	mkp_free(mkp);
	free(vars_fix);

	return 0;
}

int main(int argc, char **argv){
	return execute_sfl_mkp_core(argc, argv);
}

