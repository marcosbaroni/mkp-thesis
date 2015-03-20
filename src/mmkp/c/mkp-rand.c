#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/mkp.h"
#include "mmkp/util.h"

void print_usage(int argc, char **argv){
	printf("%s [OPTIONS]... <n> <m>\n\n", argv[0]);
	printf("DESCRIPTION:\n");
	printf(" Generates a random instance of MKP (n, m).\n\n");
	printf("ARGUMENTS:\n");
	printf("   n\tnumber of itens;\n");
	printf("   m\tnumber of dimensions;\n\n");
	printf("OPTIONS:\n");
	printf("   -s <seed>\tset seed for random generation. Current time is default.;\n");
	printf("   -s <seed>\tset tightness of knapsack, i.e., total itens weight/knapsack capacity. 0.5 is default.;\n");
	printf("   -m <max_coeficient>\tset larger profit and weight coeficient allowed. %d is default.\n", MAX_COEFFICIENT);
	exit(EXIT_FAILURE);
}

void process_arguments(int argc, char **argv, int *n, int *m, double *beta, long *seed, int *max_coeficient){
	int i;
	*n = atol(argv[1]); /* n. of items */
	*m = atol(argv[2]); /* n. of dimensions */

	*n = *m = 0;
	for( i = 1 ; i < argc ; i++ ){
		if( argv[i][0] == '-' ){
			switch(argv[i][1]){
				/* custom seed */
				case 's':
				*seed = atol(argv[++i]);
				break;

				/* custom beta */
				case 'b':
				*beta = atof(argv[++i]);
				break;

				/* custom max_coeficient */
				case 'm':
				*max_coeficient = atol(argv[++i]);
				break;
			}
		}else{
			/* reading n (or m) */
			if(!*n)
				*n = atol(argv[i]);
			else if(!*m)
				*m = atol(argv[i]);
			else{
				fprintf(stderr, "WARNING: known argument \"%s\" ignored.\n", argv[i]);
			}
		}
	}
	
	return;
}

int main(int argc, char **argv){
	int n, m, o, max_coefs, *vec;
	double beta;
	long seed;
	MKP *mkp;
	FILE *f;

	/* checking min number of arguments */
	if(argc < 3)
		print_usage(argc, argv);

	/* default arguments */
	beta = 0.5;
	seed = getmillis();
	max_coefs = MAX_COEFFICIENT;
	/* parsing arguments */
	process_arguments(argc, argv, &n, &m, &beta, &seed, &max_coefs);

	/* check non-zero arguments */
	if(!(n*m)){
		fprintf(stderr, "error: n*m cannot be 0.\n");
		return 1;
	}

	/* initializing random seed */
	srand(seed);

	/* Generating random instance */
	mkp = mkp_random(n, m, beta, max_coefs);
	mkp_sort_by_profit(mkp);

	/* Printing random instance */
	mkp_write_to_file(mkp, stdout);
	//mkp_fprint(stdout, mkp);

	mkp_free(mkp);
	
	return 0;
}

