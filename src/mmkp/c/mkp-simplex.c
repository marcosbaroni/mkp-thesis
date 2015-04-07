#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mmkp/mkp/mkp.h"
#include "mmkp/util.h"
#include "mmkp/lp.h"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s <input file>\n", argv[0]);
	fprintf(out, " Use Simplex for solving LP relaxation version of MKP instance given.\n");
	fprintf(out, "   input file: a MKP instance. \"-\" to read input from stdin.\n");
	fprintf(out, "   Program outputs \"<profit>;<solution>\"\n");

	return 1;
}

int execute_mkp_simplex(int argc, char **argv){
	MKP *mkp;
	LP *lp;
	FILE *input;
	double profit, *x;
	clock_t c0, cf;
	int i, n;

	verbose = 0;

	/* checing input */
	input = stdin;
	if(strcmp(argv[1], "-")) input = fopen(argv[1], "r");

	/* reading MKP instance */
	mkp = mkp_read_from_file(input);
	fclose(input);
	n = mkp->n;

	/* MKP -> LP */
	lp = mkp2lp(mkp, 1.0);

	//mkp_fprint(stdout, mkp);
	//lp_fprint(stdout, lp);

	/* solving LP */
	c0 = clock();
	x = lp_simplex(lp);
	cf = clock();

	profit = 0.0;
	for( i = 0 ; i < n ; i++ )
		profit += ((double)mkp->p[i])*x[i];
	printf("%.2lf;\n", profit);
	for( i = 0 ; i < n ; i++ )
		printf("%.2lf\n", x[i]);

	free(x);
	lp_free(lp);
	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
	if( argc < 2 ) return print_usage(argc, argv);
	return execute_mkp_simplex(argc, argv);
}

