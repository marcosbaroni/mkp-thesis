#include <stdio.h>
#include <stdlib.h>
#include "mmkp/mkp.h"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, "\nDESCRIPTION:\n");
	fprintf(out, "     Study of Nemhuauser-Ullmann algorithm for MKP.\n");
	fprintf(out, "\nARGUMENTS:\n");
	fprintf(out, "     input file\ta MKP instance. If '-' is given, instance is read from stdin.\n");

	return 1;
}

void plot_domsets(FILE *out, Array *sols){
	MKPSol *mkpsol;
	int i, nsols;

	return;
}

int execute_domset_search(int argc, char **argv){
	MKP *mkp;
	Array *sols;
	int i, dim, nsols;
	FILE *input;

	input = stdin;

	/* reading arguments */
	if(argc < 2 )
		return print_usage(argc, argv);
	if(strcmp("-", argv[1]))
		input = fopen(argv[1], "r");
	
	/* reading instance */
	mkp = mkp_read_from_file(input);
	fclose(input);

	/* searching domsets */
	sols = mkp_nemull(mkp);
	nsols = array_get_size(sols);

	/* output */
	dim = 0;
	array_sort_r( /*sorting by 1st weigth */
		sols, (int(*)(void*, void*, void*))mkpsol_cmp_weight, &dim);
	for( i = 0 ; i < nsols ; i++ ){
		printf("%03d - ", i);
		mkpsol_fprint(stdout, array_get(sols, i), 1);
	}

	/* frees */
	mkp_free(mkp);
	array_apply(sols, (array_apply_f)mkpsol_free);
	array_free(sols);
	
	return 0;
}

int main(int argc, char **argv){
	return execute_domset_search(argc, argv);
}
