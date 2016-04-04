# include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "mmkp/mkp/mkp.h"
#include "mmkp/util.h"
#include "mmkp/sfl.h"

#define CBPATH "/home/mbaroni/Computacao/dmkp/insts/chubeas/"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s <input file> <n of memeplex> <size of memeplex> <size of submemeplex> <n of iterations> <n of subiterations> [seed=time(null)]\n", argv[0]);
	fprintf(out, " Shuffled Frog Leaping Algorithm for MKP.\n");
	fprintf(out, " - input file: a MKP instance. If no file is given, instance is read from stdin.\n");
	fprintf(out, " - Program outputs \"<profit of solution>;<best iter>;<time (s)>\"\n");

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
	if( argc > 7 ) seed = atoll(argv[7]);
	srand(seed);

	/* reading problem instance */
	mkp = mkp_read_from_file(input);
	fclose(input);
	sfli = mkp_sfl_interface();

	/* solving problem */
	c0 = clock();
	sol = sfl(sfli, mkp, nmeme, meme_size, q, niter, subniter, &best_iter);
	cf = clock();

	/* output */
	mkpnum_fprintf(stdout, sol->obj);
	fprintf(stdout, ";%d;%f\n", best_iter, ((cf-c0)/(float)CLOCKS_PER_SEC));

	/* freeing */
	mkpsol_free(sol);
	mkp_free(mkp);
	sfli_free(sfli);

	return 0;
}

void print_usage_core(int argc, char **argv){
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
	int *vars_fix = NULL; /* value of each variable on core problem generation */

	/* checking input */
	if( argc < 2 ){
		print_usage_core(argc, argv);
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
	srand(seed);

	/* reading problem instance */
	mkp = mkp_read_from_file(input);
	fclose(input);
	//printf("MKP problem\n");
	//mkp_fprint(stdout, mkp); fflush(stdout);

	n = mkp->n;
	m = mkp->m;
	if(!core_size)
		core_size = m + n/10;

	/* generation MKP core problem */
	mkp_core = mkp_core_problem(mkp, core_size, &vars_fix);
	//printf("MKP Core problem\n");
	//mkp_fprint(stdout, mkp_core); fflush(stdout);

	sfli = mkp_sfl_interface();
	c0 = clock();
	//printf("\nMKP CORE problem\n"); fflush(stdout);
	//mkp_fprint(stdout, mkp_core); fflush(stdout);
	/* solving core problem */
	core_sol = (MKPSol*)sfl(sfli, mkp_core, nmeme, meme_size, submeme_size, niter, subniter, &best_iter);
	/* extracting solution from original problem */

	//printf("core solution\n");
	//mkpsol_fprint(stdout, core_sol, 1);

	sol = mkpsol_from_mkp_core(core_sol, mkp, vars_fix);
	//printf("solution\n");
	//mkpsol_fprint(stdout, sol, 1);
	cf = clock();

	/* TODO: test/validate this code... */
	mkpsol_free(core_sol);
	mkp_free(mkp_core);
	sfli_free(sfli);

	/* output */
	mkpnum_fprintf(stdout, sol->obj);
	printf(";%d;%f\n", best_iter, ((cf-c0)/(float)CLOCKS_PER_SEC));

	/* freeing */
	mkpsol_free(sol);
	mkp_free(mkp);
	free(vars_fix);

	return 0;
}

int print_usage_core_batch(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s <type> <n of memeplex> <size of memeplex> <size of submemeplex> <n of iterations> <n of subiterations> [core_size] [seed=time(null)]\n", argv[0]);
	fprintf(out, " Shuffled Frog Leaping Algorithm for MKP.\n");
	fprintf(out, " - type: 1: chu beasly instances.\n");
	fprintf(out, "         2: random instances.\n");
	fprintf(out, " - Program outputs \"<profit of solution>;[<best known (if chubealy)>;]<best iter>;<time>\"\n");
	fprintf(out, " - cross types:\n");
	fprintf(out, "    1 - 50%% from father\n");
	fprintf(out, "    2 - 20%% from father\n");
	fprintf(out, "    3 - 10%% from father\n");

	return 1;
}

int execute_sfl_mkp_core_batch(int argc, char **argv){
	MKP *mkp, *mkp_core;
	FILE *input;
	SFL_Interface *sfli;
	MKPSol *sol, *core_sol;
	mkpnum best;
	double *x;
	char filename[300];
	int i, j, k, l, s;
	int n, m;
	double t;
	int type;
	int ns[3] = {100, 250, 500};
	int ms[3] = {5, 10, 30};
	double ts[3] = {0.25, 0.50, 0.75};
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
	int *vars_fix = NULL; /* value of each variable on core problem generation */

	/* checking input */
	if( argc < 2 ){
		print_usage_core_batch(argc, argv);
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
	type = atoll(argv[1]);
	if( argc > 2 ) nmeme = atoll(argv[2]);
	if( argc > 3 ) meme_size = atoll(argv[3]);
	if( argc > 4 ) submeme_size = atoll(argv[4]);
	if( argc > 5 ) niter = atoll(argv[5]);
	if( argc > 6 ) subniter = atoll(argv[6]);
	if( argc > 7 ) core_size = atoll(argv[7]);
	if( argc > 8 ) seed = atoll(argv[8]);
	srand(seed);

	/* reading problem instance */
	sfli = mkp_sfl_interface();
	for( i = 0 ; i < 3 ; i++ ){
		n = ns[i];
		for( j = 0 ; j < 3 ; j++ ){
			m = ms[j];
			for( k = 2 ; k < 3 ; k++ ){
				t = ts[k];
				for( l = 0 ; l < 10 ; l++ ){
					/* reading instance */
					if( type == 1 ){
						sprintf(filename, "%sOR%dx%d-%.2f_%d.dat", CBPATH, m, n, t, l+1);
						mkp = mkp_read_from_filename(filename);
						best = chubeas_best[i][j][k][l];
						fclose(input);
					}else{
						mkp = mkp_random(n, m, t, 0.5, 1000);
						x = mkp_solve_with_scip(mkp, 180, 1.0, 0);
						best = 0;
						for( i = 0 ; i < n ; i++ )
							if( feq(x[i], 1.0) )
								best += mkp->p[i];
						free(x);
					}

					n = mkp->n;
					m = mkp->m;
					core_size = m + n/5;

					/* generation MKP core problem */
					mkp_core = mkp_core_problem(mkp, core_size, &vars_fix);
					//mkp_fprint(stdout, mkp_core);

					for( s = 0 ; s < 10 ; s++ ){
						/* solving core problem */
						c0 = clock();
						core_sol = (MKPSol*)sfl(
							sfli,
							mkp_core,
							nmeme,
							meme_size,
							submeme_size,
							niter,
							subniter,
							&best_iter);
						//mkpsol_fprint(stdout, core_sol, 1);
						/* extracting solution from original problem */
						sol = mkpsol_from_mkp_core(core_sol, mkp, vars_fix);
						cf = clock();
						mkpsol_free(core_sol);

						/* output */
						printf("%d;%d;%.2lf;%d;%d;", n, m, t, l+1, s);
						mkpnum_fprintf(stdout, sol->obj);
						printf(";");
						mkpnum_fprintf(stdout, best);
						printf(";%d;%f\n", best_iter, ((cf-c0)/(float)CLOCKS_PER_SEC));
	
						/* freeing sols */
						mkpsol_free(sol);
					}

					/* freeing instances */
					free(vars_fix); vars_fix = NULL;
					mkp_free(mkp_core);
					mkp_free(mkp);
					fflush(stdout);
				}
			}
		}
	}
	sfli_free(sfli);

	return 0;
}

int main(int argc, char **argv){
	return execute_sfl_mkp(argc, argv);
	//return execute_sfl_mkp_core_batch(argc, argv);
}

