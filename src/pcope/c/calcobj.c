#include <stdio.h>
#include <stdlib.h>
#include "core/pcope.h"

void print_usage(){
	printf("usage ./calcobj <problem file> <solution file>\n");
	return;
}

int main(int argc, char **argv){
	FILE *fp, *fsol;
	Solution *sol;
	PCOPE *p;

	if( argc < 3 ){
		print_usage();
		return 1;
	}

	fp = fopen(argv[1], "r");
	fsol = fopen(argv[2], "r");

	p = pcope_from_plain(fp);
	sol = sol_from_plain(p, fsol);

	fclose(fp);
	fclose(fsol);

	sol_fprint(stdout, sol);

	sol_free(sol);
	pcope_free(p);

	return 0;
}

