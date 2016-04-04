#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmkp/mkp/mkp.h"

int main(int argc, char **argv){
	int n, m, i, ialpha;
	float alpha;

	if (argc < 5 ){
		printf("usage: %s <n> <m> <alpha> <i>\n", argv[0]);
		printf("Returns the best known value for the respective chubeasly instance.\n");
		printf("  n: number of itens\n");
		printf("  m: number of dimensions\n");
		printf("  alpha: tighness of knapsack (0.25, 0.5, 0.75)\n");
		printf("  i: sample ([1-10])\n");
		return 1;
	}
	n = atoll(argv[1]);
	m = atoll(argv[2]);
	alpha = atof(argv[3]);
	i = atoll(argv[4]);
	ialpha = 0;

	/* Getting n */
	switch(n){
		case 100: n = 0; break;
		case 250: n = 1; break;
		case 500: n = 2; break;
		default:
		fprintf(stderr, "Bad value for n. Assuming n = 100.\n");
		n = 0;
		break;
	}

	/* Getting m */
	switch(m){
		case 5: m = 0; break;
		case 10: m = 1; break;
		case 30: m = 2; break;
		default:
		fprintf(stderr, "Bad value for m. Assuming m = 5.\n");
		m = 0;
		break;
	}

	/* Getting alpha */
	if(alpha == 0.25){
		ialpha = 0;
	}else if(alpha == 0.5){
		ialpha = 1;
	}else if(alpha == 0.75){
		ialpha = 2;
	}else{
		fprintf(stderr, "Bad value for alpha. Assuming alpha = 0.5.\n");
		ialpha = 0;
	}

	/* Getting i */
	i--;
	if( i < 0 || i > 10){
		fprintf(stderr, "Bad value for i. Assuming i = 1.\n");
		i = 0;
	}

	/* Printing. */
	mkpnum_fprintf(stdout, chubeas_best[n][m][ialpha][i]);

	return 0;
}

