/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: december, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "mmkp/mkp/mkp.h"
#include "mmkp/des.h"

void print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, " Discrete Stochastic Evolution Strategy for MKP.\n");
	fprintf(out, "   input file: a MKP instance. If no file is given, instance is read from stdin.\n");

	return;
}

double *get_probs(MKP *mkp){
	int n;
	double probs;

	n = mkp->n;
	probs = (double*)malloc(n*sizeof(double));
	for( i = 0 ; i < n ; i++ )
		probs[i] = 0;

	return probs;
}

int execute_mkp_des(int argc, char **argv){
	MKP *mkp;
	FILE *input;
	MKPSol *sol;
	double *probs, time;
	int i, n;
	clock_t c0, cf;

	input = stdin;
	if(argc > 1)
		input = fopen(argv[1], "r");

	mkp = mkp_read_from_file(input);
	fclose(input);

	probs = get_probs(mkp);

	c0 = clock();
	dom_sets = mkp_nemull(mkp);
	cf = clock();
	time = ((cf-c0)*1./CLOCKS_PER_SEC);

	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
	return execute_mkp_des(argc, argv);
}

