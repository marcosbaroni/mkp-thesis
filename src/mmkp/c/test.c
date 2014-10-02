#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmkp/util.h"
#include "mmkp/ssum.h"

void teste_qsort(){
	long *a;
	int i, n;

	n = 20;
	a = (long*)malloc(n*sizeof(long));

	for( i = 0 ; i < n ; i++)
		a[i] = lrand(100);
	
	long_array_fprint(stdout, a, 20);
	long_array_qsort(a, n);
	long_array_fprint(stdout, a, 20);
	free(a);
	return;
}

void teste_ssum_backtrack(int n){
	int i, nsol;
	SSum *ssum;
	SSumSol *sol;
	Array *a;

	a = array_new();
	ssum = ssum_new_random(n, 100, 0.5);

	a = ssum_backtrack(ssum);
	nsol = array_get_size(a);
	for( i = 0 ; i < nsol ; i++ ){
		sol = (SSumSol*)array_get(a, i);
		ssumsol_fprint(stdout, sol);
		ssumsol_free(sol);
	}

	array_free(a);

	return;
}

int main(int argv, char **argc){
	teste_ssum_backtrack(20);
	return 0;
}

