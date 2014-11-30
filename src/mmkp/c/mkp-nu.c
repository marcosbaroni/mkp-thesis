#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/mkp.h"
#include "mmkp/util.h"

void print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, " Nemhauser-Ullman Algorithm Adapted for MKP.\n");
	fprintf(out, "   input file: a MKP instance. If no file is given, instance is read from stdin.\n");
	fprintf(out, "   Program outputs \"<n. of dom. subsets>;<profit of solution>\"\n");

	return;
}

int execute_nemullman_mkp(int argc, char **argv){
	MKP *mkp;
	FILE *input;

	input = stdin;
	if(argc > 1) input = fopen(argv[1], "r");

	mkp = mkp_read_from_file(input);
	fclose(input);

	mkp_fprint(stdout, mkp);

	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
	return execute_nemullman_mkp(argc, argv);
}

