#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/ssum.h"
#include "mmkp/util.h"

void print_usage(int argc, char **argv){
	printf("Generates a random instance of Subset-sum (n).\n");
	printf(" usage %s <n> [beta=0.5] [seed=<msecs>] [max_coeficient=%d]\n", argv[0], MAX_COEFFICIENT);
	printf("   n: number of itens;\n");
	printf("   beta: tightness of sum (total itens weights capacity);\n");
	printf("   seed: seed for random generation. (0 for current time);\n");
	printf("   max_coeficient: the larger weight coeficient allowed.\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	int n, m, o, max_w, *vec;
	double beta;
	long seed = 0;
	SSum *ssum;

	FILE *f;

	/* checking min number of arguments */
	if(argc < 2)
		print_usage(argc, argv);

	/* parsing arguments */
	max_w = MAX_COEFFICIENT;
	n = atol(argv[1]);
	if(argc > 2) beta = atof(argv[2]);
	else beta = 0.5;
	if( argc > 3 ) seed = atol(argv[3]);
	if( !seed ) seed = getmillis();
	if( argc > 4 ) max_w = atol(argv[4]);
	if( ! max_w ) error("MAX_COEFFICIENT must be positive.\n");

	/* check non-zero argmunets */
	if(!n)
		print_usage(argc, argv);

	/* initializing random seed */
	srand(seed);

	/* Generating random instance */
	ssum = ssum_new_random(n, max_w, beta);

	/* Printing random instance */
	ssum_write(stdout, ssum);

	ssum_free(ssum);
	
	return 0;
}

