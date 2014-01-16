#include <stdio.h>
#include <stdlib.h>
#include "pcope.h"



int main(int argc, char **argv){
	int v[10];
	int i;

	srand(time(NULL));
	int_rand_fill_with_total(v, 10, 100, 0.0);

	for( i = 0 ; i < 10 ; i++ )
		printf("%d, ", v[i]);
	printf("\n");

	return 0;
}

