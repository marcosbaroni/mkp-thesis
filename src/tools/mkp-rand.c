#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../models/mkp/mkp.h"
#include "../utils/util.h"

void print_usage(int argc, char **argv){
	printf("%s [OPTIONS]... <n> <m>\n\n", argv[0]);
	printf("DESCRIPTION:\n");
	printf(" Generates a random instance of MKP (n, m).\n\n");
	printf("   w[ij] = U(0, max_coeficient)\n");
	printf("   b[j] = alpha*sum(w[j])\n");
	printf("   p[i] = 2*beta*sum(w[i]) + (1-beta)*max_coeficient*U(0, 1)\n\n");
	printf("ARGUMENTS:\n");
	printf("   n\tnumber of itens;\n");
	printf("   m\tnumber of dimensions;\n\n");
	printf("OPTIONS:\n");
	printf("   -s <seed>\tset seed for random generation (current time is default);\n");
	printf("   -[e|r] \tOutputs the instance with itens sorted by [non-descreasing|non-increasing] \"dual\" efficiency measure. Otherwise their sorted by profit;\n");
	printf("   -a <alpha ratio>\tset tightness of knapsack, i.e., total itens weight/knapsack capacity (0.5 is default);\n");
	printf("   -b <beta ratio>\tcorrelation between items profit-weight (0.5 is default);\n");
	printf("   -m <max_coeficient>\tset larger profit and weight coeficient allowed (%d is default).\n", MAX_COEFFICIENT);
	exit(EXIT_FAILURE);
}

void process_arguments(int argc, char **argv, int *n, int *m, double *alpha, double *beta, long *seed, mkpnum *max_coeficient, int *sort){
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

				/* sort by efficiency measure */
				case 'e':
				*sort = 1;
				break;

				/* sort by reversed efficiency measure */
				case 'r':
				*sort = 2;
				break;

				/* custom alpha */
				case 'a':
				*alpha = atof(argv[++i]);
				break;

				/* custom beta */
				case 'b':
				*beta = atof(argv[++i]);
				break;

				/* custom max_coeficient */
				case 'm':
				mkpnum_scanf(argv[++i], max_coeficient);
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
	int n, m, o, sort;
	mkpnum max_coefs;
	double beta, alpha;
	long seed;
	MKP *mkp;
	FILE *f;

	/* checking min number of arguments */
	if(argc < 3)
		print_usage(argc, argv);

	/* default arguments */
	alpha = beta = 0.5;
	seed = getmillis();
	max_coefs = MAX_COEFFICIENT;
	sort = 0;
	/* parsing arguments */
	process_arguments(argc, argv, &n, &m, &alpha, &beta, &seed, &max_coefs, &sort);

	/* check non-zero arguments */
	if(!(n*m)){
		fprintf(stderr, "error: n*m cannot be 0.\n");
		return 1;
	}

	/* initializing random seed */
	srand(seed);

	/* Generating random instance */
	mkp = mkp_random(n, m, alpha, beta, max_coefs);
	
	/* sorting items */
	if(sort) mkp_sort_by_em(mkp, sort-1);
	else mkp_sort_by_profit(mkp);

	/* Printing random instance */
	mkp_write_to_file(mkp, stdout);
	//mkp_fprint(stdout, mkp);

	mkp_free(mkp);
	
	return 0;
}

