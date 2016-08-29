#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "mmkp/mkp/mkp.h"
#include "mmkp/util.h"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, " Nemhauser-Ullman Algorithm Adapted for MKP.\n");
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
	dom_sets = mkp_nemull(mkp);
	cf = clock();

	n = array_get_size(dom_sets);

	/* find best set */
	best_sol = array_get(dom_sets, 0);
	for( i = 1 ; i < n ; i++ ){
		sol = array_get(dom_sets, i);
		if( sol->feasible && sol->obj > best_sol->obj)
			best_sol = sol;
	}

	/* output solution */
	printf("%lld;%d;%.3lf\n", best_sol->obj, array_get_size(dom_sets), ((cf-c0)*1./CLOCKS_PER_SEC));

	/* frees */
	array_apply(dom_sets, (void(*)(void *))mkpsol_free);
	array_free(dom_sets);
	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
	return execute_nemullman_mkp(argc, argv);
}

