#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/mmkp.h"
#include "mmkp/util.h"

void print_usage(int argc, char **argv){
	printf("Generates a random instance of MMKP (n, m, o).\n");
	printf(" usage %s <n> <m> <o> [beta] [seed]\n", argv[0]);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	int n, m, o, *vec;
	double beta;
	long seed;
	MMKP *mmkp;

	FILE *f;

	/* checking min number of arguments */
	if(argc < 3)
		print_usage(argc, argv);

	/* parsing arguments */
	n = atol(argv[1]);
	m = atol(argv[2]);
	o = atol(argv[3]);
	if(argc > 4) beta = atof(argv[4]);
	else beta = 0.5;
	if(argc > 5) seed = atol(argv[5]);
	else seed = getmillis();

	/* check non-zero argmunets */
	if(!(n*m))
		print_usage(argc, argv);

	/* initializing random seed */
	srand(seed);

	/* Generating random instance */
	mmkp = mmkp_random(n, m, o, beta);
	/* Printing random instance */
	mmkp_write_to_file(mmkp, stdout);
	mmkp_free(mmkp);
	
	return 0;
}

