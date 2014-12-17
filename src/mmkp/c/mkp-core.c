#include <stdio.h>
#include <stdlib.h>

void print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, "   input file: a MKP instance. If no file is given, instance is read from stdin.\n");

	return;
}

int execute_nemullman_mkp(int argc, char **argv){
	MKP *mkp;
	FILE *input;
	MKPSol *best_sol, *sol;
	int i, n;

	input = stdin;
	if(argc > 1) input = fopen(argv[1], "r");
	mkp = mkp_read_from_file(input);
	fclose(input);

	return 0;
}

int main(int argc, char **argv){
	return execute_core_search(argc, argv);
}

