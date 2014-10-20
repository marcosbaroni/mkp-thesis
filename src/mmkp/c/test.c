#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmkp/util.h"
#include "mmkp/ssum.h"
#include "mmkp/kp.h"

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

void teste_ssum_backtrack(int argc, char **argv){
	int i, n, nsol;
	SSum *ssum;
	SSumSol *sol;
	Array *a;
	long long wmax;

	if( argc < 2 ){
		printf("usage.: %s <n> [wmax=1000] [seed=time]\n", argv[0]);
		return;
	}

	n = 10;
	srand(time(0));
	wmax = 1000;
	if( argc > 1 ) n = atoll(argv[1]);       /* number of itens */
	if( argc > 2 ) wmax = atoll(argv[2]);    /* number of itens */
	if( argc > 3 ) srand(atoll(argv[3]));    /* random seed     */

	ssum = ssum_new_random(n, wmax , 0.5);

	a = ssum_backtrack(ssum, 1);
	nsol = array_get_size(a);
	printf("%d solutions.\n", nsol);
	for( i = 0 ; i < nsol ; i++ ){
		sol = (SSumSol*)array_get(a, i);
		//ssumsol_fprint(stdout, sol);
		ssumsol_free(sol);
	}

	array_free(a);
	ssum_free(ssum);

	return;
}

void test_kp_fprintf(int argc, char** argv){
	KP *kp;

	srand(time(NULL));

	kp = kp_new_random(10, 0.6, 1000);
	kp = kp_qsort_by_density(kp);
	kp_fprintf(stdout, kp);

	kp_free(kp);

	return;
}

int main(int argc, char **argv){
	//teste_ssum_backtrack(argc, argv);
	test_kp_fprintf(argc, argv);

	return 0;
}

