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

int execute_domset_search(int argc, char **argv){
	MKP *mkp;
	Array *sols;
	int i, nsols;
	FILE *input;

	input = stdin;

	/* reading arguments */
	if(argc < 2 )
		return print_usage(argc, argv);
	if(strcmp("-", argv[1]))
		input = fopen(argv[1], "r");
	
	/* reading instance */
	mkp = mkp_read_from_file(input);

	/* searching domsets */
	sols = mkp_nemull(mkp);

	/* output */
	array_sort(sols, (int(*)(void*, void*))mkpsol_profit_cmp);
	nsols = array_get_size(sols);
	for( i = 0 ; i < nsols ; i++ )
		mkpsol_fprint(stdout, array_get(sols, i), 1);

	/* frees */
	fclose(input);
	mkp_free(mkp);
	array_free(sols);
	
	return 0;
}

int main(int argc, char **argv){
	return execute_domset_search(argc, argv);
}
