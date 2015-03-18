#include <stdio.h>
#include <stdlib.h>
#include "mmkp/mkp.h"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, " Study of Nemhuauser-Ullmann algorithm for MKP.\n");
	fprintf(out, "   input file: a MKP instance. If no file is given, instance is read from stdin.\n");

	return 1;
}

int *execute_domset_search(int argc, char **argv){
	MKP *mkp;
	Array *sols;

	if(argc < 2 )
		return print_usage(argc, argv);
	
	mkp = mkp_read_from_file(input);

	sols = mkp_nemull(mkp);
	/* TODO: stopped here... */

	mkp_free(mkp);
	
	return 0;
}

int main(int argc, char **argv){
	return execute_domset_search(argc, argv);
}
