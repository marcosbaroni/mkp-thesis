#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mmkp/mkp.h"
#include "mmkp/util.h"

void print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, " Nemhauser-Ullman Algorithm Adapted for MKP.\n");
	fprintf(out, "   input file: a MKP instance. If no file is given, instance is read from stdin.\n");
	fprintf(out, "   Program outputs \"<n. of dom. subsets>;<profit of solution>\"\n");

	return;
}

int execute_nemullman_mkp(int argc, char **argv){
	MKP *mkp;
	FILE *input;
	Array *dom_sets;
	MKPSol *best_sol, *sol;
	int i, n;
	clock_t c0, cf;

	input = stdin;
	if(argc > 1) input = fopen(argv[1], "r");

	mkp = mkp_read_from_file(input);
	fclose(input);

	c0 = clock();
	dom_sets = mkp_nemull(mkp);
	cf = clock();

	n = array_get_size(dom_sets);

	best_sol = array_get(dom_sets, 0);
	for( i = 1 ; i < n ; i++ ){
		sol = array_get(dom_sets, i);
		if( sol->feasible && sol->obj > best_sol->obj)
			best_sol = sol;
	}

	printf("%lld;%d;%lf\n", best_sol->obj, array_get_size(dom_sets), ((cf-c0)*1./CLOCKS_PER_SEC));

	array_apply(dom_sets, (void(*)(void *))mkpsol_free);
	array_free(dom_sets);
	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
	return execute_nemullman_mkp(argc, argv);
}

