#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mkp.h"

void batch1(){
	int i, j, n, m, s, nrange, mrange, ns;
	char dirname[300];
	MKP *mkp;

	nrange = 6;
	mrange = 5;
	ns = 1000;
	
	sprintf(dirname, "/tmp/mkp");
	mkdir(dirname, 0777);
	for( i = 0 ; i < nrange ; i++ ){
		n = pow(3, i+1);
		sprintf(dirname, "/tmp/mkp/%04d", n);
		mkdir(dirname, 0777);
		for( j = 0 ; j < mrange ; j++ ){
			m = pow(3, j+1);
			sprintf(dirname, "/tmp/mkp/%04d/%04d", n, m);
			mkdir(dirname, 0777);
			for( s = 0 ; s < ns ; s++ ){
				mkp = mkp_random(n, m, time(NULL));
				sprintf(dirname, "/tmp/mkp/%04d/%04d/%03d.gz", n, m, s);
				mkp_write_to_gzip(mkp, dirname);
				mkp_free(mkp);
			}
		}
	}

	return;
}

void print_usage(int argc, char **argv){
	printf("usage ./%s <n> <m>\n", argv[0]);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	int n, m;
	long seed;
	MKP *mkp;

	/* checking min number of arguments */
	if(argc < 3)
		print_usage(argc, argv);

	/* parsing arguments */
	n = atol(argv[1]);
	m = atol(argv[2]);
	if(argc > 3) seed = atol(argv[3]);
	else seed = time(NULL);

	/* check non-zero argmunets */
	if(!(n*m))
		print_usage(argc, argv);

	batch1();
	
	return 0;
}

