#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/kp.h"
#include "mmkp/util.h"

void print_usage(int argc, char **argv){
	printf("%s <n> [beta=0.5] [seed=<msecs>] [max_coeficient=%d]\n", argv[0], MAX_COEFFICIENT);
	printf("  Generates a random instance of KP.\n");
	printf("   n: number of itens;\n");
	printf("   beta: tightness of knapsack (<total itens weight>/<knapsack capacity>);\n");
	printf("   seed: seed for random generation. (0 for current time);\n");
	printf("   max_coeficient: the larger profit and weight coeficient allowed.\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	int n, o, max_coefs, *vec;
	double beta;
	long seed = 0;
	KP *kp;

	FILE *f;

	/* checking min number of arguments */
	if(argc < 2)
		print_usage(argc, argv);

	max_coefs = MAX_COEFFICIENT;
	/* parsing arguments */
	n = atol(argv[1]);
	if(argc > 2) beta = atof(argv[2]);
	else beta = 0.5;
	if( argc > 3 ) seed = atol(argv[3]);
	if( !seed ) seed = getmillis();
	if( argc > 4 ) max_coefs = atol(argv[4]);
	if( ! MAX_COEFFICIENT ) error("MAX_COEFFICIENT must be positive.\n");

	/* check non-zero argmunets */
	if(!n)
		print_usage(argc, argv);

	/* initializing random seed */
	srand(seed);

	/* Generating random instance */
	kp = kp_new_random(n, beta, MAX_COEFFICIENT);
	kp = kp_qsort_by_density(kp);

	/* Printing random instance */
	kp_write(stdout, kp);

	kp_free(kp);
	
	return 0;
}

