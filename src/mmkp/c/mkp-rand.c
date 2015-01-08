#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/mkp.h"
#include "mmkp/util.h"

void print_usage(int argc, char **argv){
	printf("%s <n> <m> [beta=0.5] [seed=<msecs>] [max_coeficient=%d]\n", argv[0], MAX_COEFFICIENT);
	printf(" Generates a random instance of MKP (n, m).\n");
	printf("   n: number of itens;\n");
	printf("   m: number of dimensions;\n");
	printf("   beta: tightness of knapsack (total itens weight/knapsack capacity);\n");
	printf("   seed: seed for random generation. (0 for current time);\n");
	printf("   max_coeficient: the larger profit and weight coeficient allowed.\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	int n, m, o, max_coefs, *vec;
	double beta;
	long seed = 0;
	MKP *mkp;

	FILE *f;

	/* checking min number of arguments */
	if(argc < 2)
		print_usage(argc, argv);

	max_coefs = MAX_COEFFICIENT;
	/* parsing arguments */
	n = atol(argv[1]);
	m = atol(argv[2]);
	if(argc > 3) beta = atof(argv[3]);
	else beta = 0.5;
	if( argc > 4 ) seed = atol(argv[4]);
	if( !seed ) seed = getmillis();
	if( argc > 5 ) max_coefs = atol(argv[5]);
	if( ! MAX_COEFFICIENT ) error("MAX_COEFFICIENT must be positive.\n");

	/* check non-zero argmunets */
	if(!(n*m))
		print_usage(argc, argv);

	/* initializing random seed */
	srand(seed);

	/* Generating random instance */
	mkp = mkp_random(n, m, beta, max_coefs);

	/* Printing random instance */
	mkp_write_to_file(mkp, stdout);
	//mkp_fprint(stdout, mkp);

	mkp_free(mkp);
	
	return 0;
}

