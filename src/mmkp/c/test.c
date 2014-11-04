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
	int n;

	srand(time(NULL));
	if( argc > 2) n = atol(argv[1]);
	else n = 10;

	kp = kp_new_random(n, 0.6, 1000);
	kp = kp_qsort_by_density(kp);
	kp_fprintf(stdout, kp);

	kp_free(kp);

	return;
}

void test_kp_backtrack(int argc, char** argv){
	KP *kp;
	FILE *kpout;
	KPSol *kpsol;
	Array *array;
	int n;

	if( argc > 1) n = atol(argv[1]);
	else n = 10;

	/* random seed */
	srand(time(NULL));

	/* random KP */
	kp = kp_new_random(n, 0.5, 1000);
	kp = kp_qsort_by_density(kp);
	kp_fprintf(stdout, kp);

	kpout = fopen("/tmp/kp/kp.zpl", "w");
	kp_to_zimpl(kpout, kp);
	fclose(kpout);

	/* solve KP */
	array = kp_backtrack(kp, 0);
	kpsol = array_get(array, 0);
	kpsol_fprint(stdout, kpsol);

	/* free */
	kpsol_free(kpsol);
	kp_free(kp);

	return;
}

int main(int argc, char **argv){
	test_kp_backtrack(argc, argv);

	return 0;
}

