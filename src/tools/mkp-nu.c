#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "../models/mkp/mkp.h"
#include "../models/mkp/balev.h"
#include "../models/mkp/domset.h"
#include "../utils/util.h"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [alg] [input file]\n", argv[0]);
	fprintf(out, " Nemhauser-Ullman Algorithm for MKP.\n");
	fprintf(out, "   input file: a MKP instance. If '-' is given, instance is read from stdin.\n");
	fprintf(out, "   alg: the algorithm which should be used.\n");
	fprintf(out, "      1 - Nemhauser-Ullman plain (only cutting unfeaseble)\n");
	fprintf(out, "      2 - Nemhauser-Ullman with linked buckets\n");
	fprintf(out, "      3 - Balev plain\n");
	fprintf(out, "      4 - Balev using linked buckets\n");
	fprintf(out, "\n");
	fprintf(out, "   Program outputs \"<n. of dom. subsets>;<profit of solution>\"\n");

	return 1;
}

int execute_nemullman(int argc, char **argv){
	MKP *mkp;           /* the proglem */
	FILE *input;        /* input stream */
	Array *dom_sets;
	MKPSol *best_sol;
    MKPSol *sol;
	int i, n;

	clock_t c0, cf;

	input = stdin;
	/* checking inputs */
	if(strcmp(argv[2], "-")){ /* not '-' */
		input = fopen(argv[2], "r");
        if( !input ){
            fprintf(stderr, "Error: file %s could not be opened.\n", argv[0]);
            return 1;
        }
    }

	/* reading instance */
	mkp = mkp_read_from_file(input);
	fclose(input);

	/* enumerate sets */
	c0 = clock();
    best_sol = mkp_dynprog(mkp, NULL);
	cf = clock();

	/* output solution */
    mkpnum_fprintf(stdout, best_sol->obj);
	printf(";%.3lf\n", ((cf-c0)*1./CLOCKS_PER_SEC));

	/* frees */
    mkpsol_free(best_sol);
	mkp_free(mkp);

	return 0;
}

int execute_balev(int argc, char **argv, int use_lb){
	MKP *mkp;           /* the problem */
	MKP *mkp2;          /* the reduced problem */
    MKPSol *mkpsol;     /* feasible solution given */
	FILE *input;        /* input stream */
	Array *dom_sets;
	MKPSol *best_sol;
    MKPSol *sol;
	int i, n;

	clock_t c0, cf;

	input = stdin;
	/* checking inputs */
	if(strcmp(argv[2], "-")){ /* not '-' */
		input = fopen(argv[2], "r");
        if( !input ){
            fprintf(stderr, "Error: file %s could not be opened.\n", argv[0]);
            return 1;
        }
    }

	/* reading instance */
	mkp = mkp_read_from_file(input);
    if( input != stdin )
	    fclose(input);

    mkpsol = mkpsol_read_from_file(stdin, mkp);

	/* enumerate sets */
	c0 = clock();
    mkp_balev(mkpsol, use_lb);
	cf = clock();

	/* frees */
    mkpsol_free(mkpsol);
	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
    int alg;
	if( argc < 3 ){
		return print_usage(argc, argv);
    }
    alg = atol(argv[1]);

    switch( alg ){
	    case 1: return execute_nemullman(argc, argv); break;
        case 2: fprintf(stderr, "Not implemented yet.\n"); break;
	    case 3: return execute_balev(argc, argv, 0); break;
	    case 4: return execute_balev(argc, argv, 1); break;
    }
    return 0;
}

