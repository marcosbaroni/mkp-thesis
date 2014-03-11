#include <stdio.h>
#include <stdlib.h>
#include "core/pcope.h"

void print_help(){
	printf("   nacts:  # of actions\n");
	printf("   nyears: # of years\n");
	printf("   npers:  # of periods per years\n");
	printf("   nres:   # of resources\n");
	printf("   seed:   seed for random generation\n");
	printf("   irr:    internal rate of return\n");
	printf("   mtir:   med of action tir\n");
	printf("   vtir:   variation of action tir\n");

	return;
}

void print_usage(){
	printf("usage: ./generator -b <directory>\n");
	printf("\tGenerates a batch of instances on <directory>.\n\n");
	printf("usage: ./generator -s <nacts> <nyears> <npers> <nres> <seed> <irr> <mtir> <vtir>\n");
	printf("\tGenerates a single instance to standart output.\n");

	return;
}

void batch(char *dir){
	
	return;
}

int main(int argc, char **argv){
	PCOPE *p;
	RandConf *rc;

	if( argc < 5 ){
		print_usage();
		return 1;
	}

	rc = rc_from_args(argc, argv);
	p = pcope_random(rc);
	pcope_to_plain(p, stdout);

	pcope_free(p);
	randconf_free(rc);

	return 0;
}

