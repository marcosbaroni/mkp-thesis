#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

int str_cmp(char *a, char *b){
	return strcmp(a, b);
}


void test_avl(){
	AVLTree *avl;
	avl = avl_new(str_cmp);
}

void teste_qsort(){
	long *a;
	int i, n;

	n = 20;
	a = (long*)malloc(n*sizeof(long));

	for( i = 0 ; i < n ; i++)
		a[i] = lrand(100);
	
	fprint_long_array(stdout, a, 20);
	qsort_long_array(a, n);
	fprint_long_array(stdout, a, 20);
	free(a);
	return;
}

int main(int argv, char **argc){
	teste_qsort();
	return 0;
}

