#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mmkp/mkp/mkp.h"
#include "mmkp/mkp/domset.h"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, "\nDESCRIPTION:\n");
	fprintf(out, "     Fast Nemhauser-Ullman algorithm for MKP.\n");
	fprintf(out, "\nARGUMENTS:\n");
	fprintf(out, "     input file\ta MKP instance. If '-' is given, instance is read from stdin.\n");

	return 1;
}

int execute_fast_nu(int argc, char **argv){
	MKP *mkp;
	FILE *input;
	int i;
	clock_t c0, cf;

	/* default arguments */
	input = stdin;

	/* checking inputs */
	if( argc < 2 )
		return print_usage(argc, argv);
	if(strcmp(argv[1], "-"))
		input = fopen(argv[1], "r");
	
	/* reading instance */
	mkp = mkp_read_from_file(input);
	fclose(input);

	/* enumearting solutions */
	c0 = clock();
	mkp_fast_domsets_enum(mkp);
	cf = clock();

	/* outputing */
	printf("%.3lf secs\n", ((cf-c0)*1./CLOCKS_PER_SEC));

	/* frees */
	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
	return execute_fast_nu(argc, argv);
}

