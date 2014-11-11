#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/kp.h"
#include "mmkp/util.h"

void print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s <n> <m> [beta=0.5] [seed=<msecs>] [max_coeficient=%d]\n", argv[0], MAX_COEFFICIENT);
	fprintf(out, "usage: %s [input] \n", argv[0]);
	fprintf(out, "kpbt (a Knapsack Problem BackTrack procedure)\n");
	fprintf(out, "   If no input file is given, problem is read from standard input.");
	fprintf(out, "   Solution is printed on standard output as a list of the indexes of selected itens on solution.\n");
	fprintf(out, "   Ex.: 1\n");
	fprintf(out, "        2\n");
	fprintf(out, "        3\n");
	fprintf(out, "        4\n");
	fprintf(out, "        6\n");
	fprintf(out, "\n");

	return;
}

int main(int argc, char **argv){
	KP *kp;
	KPSol *kpsol;
	FILE *input;
	Array *sols;
	int i, n, nx, *x, last, first, split;
	long long b_left;

	if(argc > 1) input = fopen(argv[1], "r");
	else input = stdin;

	kp = kp_read(input);
	fclose(input);
	sols = kp_backtrack(kp, 0);
	kpsol = array_get(sols, 0);

	n = kpsol->kp->n;
	x = kpsol->x;
	nx = kpsol->nx;
	/* TODO: following lines "not that safe"... */
	for( split = 0, b_left = kpsol->kp->b ;
		b_left > 0 ;
		b_left -= kpsol->kp->w[split++] ){}
	for( i = 0 ; x[i] ; i++ ){} first = i;
	for( i = n ; !(x[i]) ; i-- ){} last = i;

	/* printing results */
	printf("%d;", kpsol->kp->n);          /* number of items*/
	printf("%lld;", kpsol->find_steps);   /* steps to find */
	printf("%lld;", kpsol->proof_steps);  /* steps to proof */
	printf("%lld;", kpsol->profit);       /* profit */
	printf("%d;", split+1);               /* split */
	printf("%d;", first+1);               /* first variable setted to 1 */
	printf("%d;", last+1);                /* last variable setted to 0 */
	printf("%d\n", nx);                   /* selected */
	

	array_apply(sols, (array_apply_f)kpsol_free);
	array_free(sols);
	kp_free(kp);

	return 0;
}

