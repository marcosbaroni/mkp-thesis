#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmkp/util.h"
#include "mmkp/mkp/mkp.h"

/* 
 * This funcion prints the maximum number of selectable itens, extracted from
 * single contrained surrogate relaxations of an MKP.
 * Two types of surrogate relaxations are used (all resulting a single
 * contrained MKP):
 *  - pairs of contraints (i-th and i+1-th) being surrogated, with multiplier = '1'
 *  - all constraints begin surrogated, with multiplier = '1'
 * */
int execute_surrogate_research(int argc, char **argv){
	MKP *mkp, *mkp2;
	FILE *input;
	int i, m, nmax, *multps;
	clock_t c0, cf;

	/* default arguments */
	input = stdin;

	/* checking number of inputs */
	if( argc < 2 ){
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		fprintf(stderr, " - filename : name of MKP instance file. \"-\" for stdin.\n");
		return 1;
	}

	/* opening input file */
	if(strcmp(argv[1], "-"))
		input = fopen(argv[1], "r");
	
	/* reading instance */
	mkp = mkp_read_from_file(input);
	m = mkp->m;
	fclose(input);

	multps = (int*)malloc(m*sizeof(int));

	/* surrogating pairs of constraints */
	for( i = 0 ; i < m-1 ; i++ ){
		multps = int_array_init(multps, m, 0);
		multps[i] = multps[i+1] = 1;
		mkp2 = mkp_surrogate(mkp, multps);
		nmax = mkp_max_cardinality(mkp2);

		printf("(%d, %d): %d\n", i+1, i+2, nmax);

		mkp_free(mkp2);
	}

	/* surrogating all constraints */
	multps = int_array_init(multps, m, 1);
	mkp2 = mkp_surrogate(mkp, multps);
	nmax = mkp_max_cardinality(mkp2);
	printf("(all): %d\n", nmax);
	mkp_free(mkp2);

	/* freeing */
	free(multps);

	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
	return execute_surrogate_research(argc, argv);
}

