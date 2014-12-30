#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/kp.h"
#include "mmkp/util.h"

int main(int argc, char **argv){
	KP *kp;
	FILE *input;

	input = stdin;
	if( argc > 1) 
		input = fopen(argv[1], "r");

	kp = kp_read(input);
	fclose(input);
	kp = kp_qsort_by_density(kp);
	kp_fprintf(stdout, kp);
	kp_free(kp);

	return 0;
}


