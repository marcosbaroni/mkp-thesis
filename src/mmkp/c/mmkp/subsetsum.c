#include <stdio.h>
#include <stdlib.h>
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

void ssum_write(FILE *in, SSum *ssum){
	return;
}

void ssum_free(SSum *ssum){
	return;
}

void mmkp_fprint(FILE *out, SSum *sum){
	return;
}

