
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/ssum.h"
#include "mmkp/util.h"

int main(int argc, char **argv){
	SSum *ssum;
	FILE *input;

	input = stdin;
	if( argc > 1) 
		input = fopen(argv[1], "r");

	ssum = ssum_read(input);
	ssum_to_zimpl(stdout, ssum);
	ssum_free(ssum);

	return 0;
}

