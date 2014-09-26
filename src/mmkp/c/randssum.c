#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/ssum.h"
#include "mmkp/util.h"

void print_usage(int argc, char **argv){
	printf("Generates a random instance of Subset-sum (n).\n");
	printf(" usage %s <n> [beta=0.5] [seed=<msecs>] [max_coeficient=%ld]\n", argv[0], MAX_COEFFICIENT);
	printf("   n: number of itens;\n");
	printf("   beta: tightness of sum (total itens weights capacity);\n");
	printf("   seed: seed for random generation. (0 for current time);\n");
	printf("   max_coeficient: the larger weight coeficient allowed.\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	int n, m, o, *vec;
	double beta;
	long seed = 0;
	SSum *ssum;

	FILE *f;

	/* checking min number of arguments */
	if(argc < 2)
		print_usage(argc, argv);

	/* parsing arguments */
	n = atol(argv[1]);
	m = atol(argv[2]);
	if(argc > 3) beta = atof(argv[3]);
	else beta = 0.5;
	if( argc > 4 ) seed = atol(argv[4]);
	if( !seed ) seed = getmillis();
	if( argc > 5 ) MAX_COEFFICIENT = atol(argv[5]);
	if( ! MAX_COEFFICIENT ) error("MAX_COEFFICIENT must be positive.\n");

	/* check non-zero argmunets */
	if(!n)
		print_usage(argc, argv);

	/* initializing random seed */
	srand(seed);

	/* Generating random instance */
	ssum = ssum_new_random(n, MAX_COEFFICIENT, beta);

	/* Printing random instance */
	ssum_write(stdout, ssum);

	ssum_free(ssum);
	
	return 0;
}

