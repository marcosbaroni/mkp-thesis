#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mmkp/kp.h"
#include "mmkp/des.h"

void print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, " Discrete Stochastic Evolution Strategy for KP.\n");
	fprintf(out, "   input file: a KP instance. If no file is given, instance is read from stdin.\n");

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
		probs[i] = 0;

	return probs;
}

int execute_kp_des(int argc, char **argv){
	KP *kp;
	FILE *input;
	KPSol *sol;
	DES_Interface *desi;
	double *probs, time;
	int i, n;
	clock_t c0, cf;

	input = stdin;
	if(argc > 1) input = fopen(argv[1], "r");

	kp = kp_read(input);
	fclose(input);

	probs = get_probs(kp);
	desi = kp_des_interface();

	c0 = clock();
	sol = (KPSol*)des(desi, probs, kp, kp->n, 10, 10);
	cf = clock();
	time = ((cf-c0)*1./CLOCKS_PER_SEC);

	printf("%lld\n", sol->profit);

	kpsol_free(sol);
	kp_free(kp);
	free(desi);

	return 0;
}

int main(int argc, char **argv){
	return execute_kp_des(argc, argv);
}
