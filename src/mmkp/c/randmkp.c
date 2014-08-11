#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/mkp.h"
#include "mmkp/util.h"

void print_usage(int argc, char **argv){
	printf("Generates a random instance of MKP (n, m).\n");
	printf(" usage %s <n> <m> [beta=0.5] [seed=<msecs>]\n", argv[0]);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	int n, m, o, *vec;
	double beta;
	long seed;
	MKP *mkp;

	FILE *f;

	/* checking min number of arguments */
	if(argc < 2)
		print_usage(argc, argv);

	/* parsing arguments */
	n = atol(argv[1]);
	m = atol(argv[2]);
	if(argc > 3) beta = atof(argv[3]);
	else beta = 0.5;
	if(argc > 4) seed = atol(argv[4]);
	else seed = getmillis();

	/* check non-zero argmunets */
	if(!(n*m))
		print_usage(argc, argv);

	/* initializing random seed */
	srand(seed);

	/* Generating random instance */
	mkp = mkp_random(n, m, beta);
	/* Printing random instance */
	mkp_write_to_file(mkp, stdout);
	mkp_free(mkp);
	
	return 0;
}

