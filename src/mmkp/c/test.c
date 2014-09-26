#include <stdio.h>
#include <stdlib.h>
#include "mmkp/mmkp.h"
#include "mmkp/util.h"

/*
 * Sums a bunch of random double numbers and retuns its results.
 *   n : number of numbers to be summed
 *   returns : the sum
 *
 * */
void doublesum(int n){
	int i, ml;
	double x, v[n];

	printf(" Double sum\n");

	x = 0.0;
	for( i = 0 ; i < n ; i++ )
		v[i] = drand()*((i%2) ? 1 : -1);

	ml = getmillis();
	x = 0;
	for( i = 0 ; i < n ; i++ )
		x += v[i];
	ml = getmillis() - ml;

	printf("Sum = %.3lf\n", x);
	printf("time = %d\n\n", ml);

	return;
}

/*
 * Sums a bunch of random int numbers and retuns its results.
 *   n : number of numbers to be summed
 *   returns : the sum
 *
 * */
void intsum(int n){
	int i, ml;
	long x, v[n];

	printf(" Int sum\n");

	x = 0;
	for( i = 0 ; i < n ; i++ )
		v[i] = lrand(1000)*((i%2) ? 1 : -1);

	ml = getmillis();
	x = 0;
	for( i = 0 ; i < n ; i++ )
		x += v[i];
	ml = getmillis() - ml;

	printf("Sum = %ld\n", x);
	printf("time = %d\n\n", ml);

	return;
}

void in_double_benchmark(int n){
	srand(time(NULL));

	intsum(n);
	doublesum(n);
}

void teste_mmkp(){
	MMKP *mmkp;
	srand(time(NULL));

	mmkp = mmkp_random(10, 3, 2, 0.5);

	mmkp_fprint(stdout, mmkp);

	mmkp_free(mmkp);
}

void teste_qsort(int n){
	long *a;
	int i;

	a = (long*)malloc(n*sizeof(long));

	srand(time(0));

	for( i = 0 ; i < n ; i++)
		a[i] = lrand(10*n);
	
	fprint_long_array(stdout, a, n);
	qsort_long_array(a, n);
	fprint_long_array(stdout, a, n);

	printf("Sorted? %s!\n", is_sorted_long_array(a, n) ? "YES!" : "NO!");

	free(a);
	return;
}

/*
 * Tests the speed of int and double sum.
 * */
int main(int argc, char **argv){
	teste_qsort( argc > 1 ? atoi(argv[1]) : 20 );
	return 0;
}

