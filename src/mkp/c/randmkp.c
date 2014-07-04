#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mkp.h"
#include "util.h"

void print_usage(int argc, char **argv){
	printf("Generates a random instance of MKP (n,m).\n");
	printf(" usage %s <n> <m> [seed]\n", argv[0]);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	int n, m, i, *vec;
	long seed;
	MKP *mkp;

	FILE *f;

	/* checking min number of arguments */
	if(argc < 3)
		print_usage(argc, argv);

	/* parsing arguments */
	n = atol(argv[1]);
	m = atol(argv[2]);
	if(argc > 3) seed = atol(argv[3]);
	else seed = getmillis();
	srand(seed);

	/* check non-zero argmunets */
	if(!(n*m))
		print_usage(argc, argv);

	mkp = mkp_random(n, m);
	mkp_write_to_file(mkp, stdout);
	mkp_free(mkp);
	
	return 0;
}

