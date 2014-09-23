#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"
#include "subsetsum.h"


/* SubSet-Sum Problem */
SSum *ssum_read_file(char *filename){
	SSum *ssum;
	FILE *fin;

	fin = fopen(filename, "r");
	ssum = ssum_read(fin);
	fclose(fin);

	return ssum;
}

SSum *ssum_read(FILE *in){
	SSum *ssum;
	ssum = (SSum*)malloc(sizeof(SSum));

	fscanf(in, "%d", &(ssum->n));
	fscanf(in, "%ld", &(ssum->b));
	ssum->w = read_long_array(in, NULL, ssum->n);

	return ssum;
}

SSum *ssum_new_random(int n, long bound, double b_ratio){
	SSum *ssum;
	long sum;
	int i;

	ssum = (SSum*)malloc(sizeof(SSum));
	ssum->n = n;
	ssum->w = (long*)malloc(n*sizeof(long));
	for( i = 0 ; i < n ; i++ )
		{ ssum->w[i] = lrand(bound); sum+= ssum->w[i]; }
	
	ssum->b = (long)(floor(sum*b_ratio));

	return ssum;
}

void ssum_write(FILE *out, SSum *ssum){
	int i;
	fprintf(out, "%d\n", ssum->n);
	for( i = 0 ; i < ssum->n ; i++ )
		fprintf(out, "%ld ", ssum->w[i]);
	fprintf(out, "%ld\n", ssum->b);
	return ;
}

void ssum_free(SSum *ssum){
	free(ssum->w);
	free(ssum);
	return;
}

void ssumsol_fprint(FILE *out, SSumSol *sumsol){
	return;
}

