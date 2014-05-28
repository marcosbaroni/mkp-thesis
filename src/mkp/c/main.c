#include <stdio.h>
#include "mkp.h"

int main(int argc, char **argv){
	int n, m;
	MKP *mkp;

	if(argc < 3){
		printf("usage ./mkp <n> <m>\n");
		return 1;
	}

	n = atol(argv[1]);
	m = atol(argv[2]);

	if(!(n*m)){
		printf("usage ./mkp <n> <m>\n");
		return 1;
	}

	mkp = mkp_random(n, m, time(NULL));
	mkp_write_to_file(mkp, stdout);
	mkp_free(mkp);
	
	return 0;
}

