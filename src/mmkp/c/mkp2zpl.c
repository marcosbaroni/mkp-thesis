#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/mkp.h"
#include "mmkp/util.h"

int main(int argc, char **argv){
	MKP *mkp;
	FILE *input;

	input = stdin;
	if( argc > 1) {
		input = fopen(argv[1], "r");
		printf("file %s\n", argv[1]);
	}

	mkp = mkp_read_from_file(input);
	mkp_to_zimpl(stdout, mkp);
	mkp_free(mkp);

	return 0;
}

