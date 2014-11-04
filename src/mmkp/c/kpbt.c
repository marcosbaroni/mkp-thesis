#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/kp.h"
#include "mmkp/util.h"

void print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s <n> <m> [beta=0.5] [seed=<msecs>] [max_coeficient=%d]\n", argv[0], MAX_COEFFICIENT);
	fprintf(out, "usage: %s [input] \n", argv[0]);
	fprintf(out, "kpbt (a Knapsack Problem BackTrack procedure)\n");
	fprintf(out, "   If no input file is given, problem is read from standard input.");
	fprintf(out, "   Solution is printed on standard output as a list of the indexes of selected itens on solution.\n");
	fprintf(out, "   Ex.: 1\n");
	fprintf(out, "        2\n");
	fprintf(out, "        3\n");
	fprintf(out, "        4\n");
	fprintf(out, "        6\n");
	fprintf(out, "\n");

	return;
}

int main(int argc, char **argv){
	KP *kp;
	KPSol *kpsol;
	FILE *input;
	Array *sols;

	if(argv > 1) input = fopen(argv[1], "r");
	else input = stdin;

	kp = kp_read(input);
	fclose(input);
	sols = kp_backtrack(kp, 0);
	kpsol = array_get(sols, 0);
	kpsol_fprint(stdout, kpsol);

	array_apply(sols, (array_apply_f)kpsol_free);
	array_free(sols);
	kp_free(kp);

	return 0;
}

