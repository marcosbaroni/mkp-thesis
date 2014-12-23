#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mmkp/kp.h"
#include "mmkp/des.h"

void print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s <input file> <size of population> <#iterations> [seed=time()]\n\n", argv[0]);
	fprintf(out, " Discrete Stochastic Evolution Strategy for KP.\n");
	fprintf(out, "   input file: a KP instance. If \"-\" is given, instance is read from stdin.\n");

	return;
}

/*
 * Get probability of each item of knapsack, based on profit density.
 * */
double *get_probs(KP *kp){
	int i, n;
	double *probs;

	n = kp->n;
	probs = (double*)malloc(n*sizeof(double));
	for( i = 0 ; i < n ; i++ )
		probs[i] = 0.5;

	return probs;
}

int execute_kp_des(int argc, char **argv){
	int i, n, popsize, niter;
	KP *kp;
	FILE *input;
	KPSol *sol;
	DES_Interface *desi;
	double *probs, secs;
	clock_t c0, cf;

	if(argc < 4) return 1;

	/* read arguments */
	if(strcmp("-", argv[1])) input = fopen(argv[1], "r");
	else input = stdin;
	popsize = atoll(argv[2]);
	niter = atoll(argv[3]);
	if( argc > 5) srand(atoll(argv[4]));
	else srand(time(NULL));

	/* read input problem */
	kp = kp_read(input);
	fclose(input);

	/* load probabilities and interface */
	probs = get_probs(kp);
	desi = kp_des_interface();

	/* execute DES */
	c0 = clock();
	sol = (KPSol*)des(desi, probs, kp, kp->n, popsize, niter);
	cf = clock();
	secs = ((cf-c0)*1./CLOCKS_PER_SEC);

	/* print result */
	printf("%lld\n", sol->profit);

	/* frees */
	kpsol_free(sol);
	kp_free(kp);
	free(desi);
	free(probs);

	return 0;
}

int main(int argc, char **argv){
	if(argc < 4) print_usage(argc, argv);
	return execute_kp_des(argc, argv);
}
