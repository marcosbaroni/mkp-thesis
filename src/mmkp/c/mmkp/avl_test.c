#include <stdio.h>
#include <stdlib.h>
#include "avl.h"

int intcmp(int *a, int *b){
	if( *a > *b ) return 1;
	else if( *a < *b ) return -1;
	return 0;
}

int intptr(FILE *out, int *a){
	fprintf(out, "%d", *a);
	return;
}

int main(int argc, char** argv){
	AVLTree *avl;
	int i, a[200];
	FILE *out;

	avl = new_avltree((avl_cmp_f)intcmp);

	for( i = 0 ; i < 200 ; i++ ){
		a[i] = rand()%10000;
		avl_insert(avl, &(a[i]));
	}
	fprintf(stdout, "ok\n"); fflush(stdout);

	out = fopen(argv[1], "w");
	fprint_avlt_dot(out, avl, (avl_prt_f)intptr);

	free_avltree(avl);

	return 0;
}

