#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "../models/mkp/mkp.h"
#include "../models/mkp/domset.h"
#include "../utils/util.h"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, " Nemhauser-Ullman Algorithm for MKP.\n");
	fprintf(out, "   input file: a MKP instance. If no file is given, instance is read from stdin.\n");
	fprintf(out, "   Program outputs \"<n. of dom. subsets>;<profit of solution>\"\n");

	return 1;
}

int execute_nemullman_mkp(int argc, char **argv){
	MKP *mkp;           /* the proglem */
	FILE *input;        /* input stream */
	Array *dom_sets;
	MKPSol *best_sol;
    MKPSol *sol;
	int i, n;

	clock_t c0, cf;

	input = stdin;
	/* checking inputs */
	if( argc < 2 )
		return print_usage(argc, argv);
	if(strcmp(argv[1], "-")) /* not '-' */
		input = fopen(argv[1], "r");

	/* reading instance */
	mkp = mkp_read_from_file(input);
	fclose(input);

	/* enumerate sets */
	c0 = clock();
    best_sol = mkp_dynprog(mkp, NULL);
	cf = clock();

	/* output solution */
	printf("%lld;%.3lf\n", best_sol->obj, ((cf-c0)*1./CLOCKS_PER_SEC));

	/* frees */
    mkpsol_free(best_sol);
	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
	return execute_nemullman_mkp(argc, argv);
}

