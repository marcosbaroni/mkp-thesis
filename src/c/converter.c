#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "pcope.h"

int main(int argc, char **argv){
	PCOPE *p;
	FILE *fin;

	fin = fopen(argv[1], "r");
	p = pcope_from_plain(fin);
	fclose(fin);

	//pcope_to_plain(p, stdout);
	pcope_to_scip(p, stdout);
	pcope_free(p);

	return 0;
}

