#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmkp/util.h"
#include "mmkp/avl.h"
#include "mmkp/mkp/mkp.h"

int int_cmp(int *a, int *b){
	return *a > *b ? 1 : ( *a < *b ? -1 : 0 );
}
void int_ptr(FILE *out, int *a){
	fprintf(out, "%d", *a);
}

/*
 * */
int execute_avl_teste(int argc, char **argv){
	AVLTree *avl;
	int i, n, *v;

	if( argc < 2 ){
		fprintf(stderr, "usage: %s <n numbers>\n", argv[0]);
		return 1;
	}

	n = atoi(argv[1]);
	avl = new_avltree((avl_cmp_f)int_cmp);

	v = (int*)malloc(sizeof(int));

	for( i = 0 ; i < n ; i++ ){
		v[i] = rand() % 1000;
		avl_insert(avl, &(v[i]));
	}

	avl_fprint_dot(stdout, avl, (avl_prt_f)int_ptr);

	free_avltree(avl);
	free(v);

	return 0;
}


/* 
 * This funcion prints the maximum number of selectable itens, extracted from
 * single contrained surrogate relaxations of an MKP.
 * Two types of surrogate relaxations are used (all resulting a single
 * contrained MKP):
 *  - pairs of contraints (i-th and i+1-th) being surrogated, with multiplier = '1'
 *  - all constraints begin surrogated, with multiplier = '1'
 * */
int execute_surrogate_research(int argc, char **argv){
	MKP *mkp, *mkp2;
	FILE *input;
	int i, n, m, nmax, *multps;
	clock_t c0, cf;

	double *x, sum;

	/* default arguments */
	input = stdin;

	/* checking number of inputs */
	if( argc < 2 ){
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		fprintf(stderr, " - filename : name of MKP instance file. \"-\" for stdin.\n");
		return 1;
	}

	/* opening input file */
	if(strcmp(argv[1], "-"))
		input = fopen(argv[1], "r");
	
	/* reading instance */
	mkp = mkp_read_from_file(input);
	m = mkp->m;
	n = mkp->n;
	fclose(input);

	multps = (int*)malloc(m*sizeof(int));

	/* surrogating pairs of constraints */
	for( i = 0 ; i < m-1 ; i++ ){
		multps = int_array_init(multps, m, 0);
		multps[i] = multps[i+1] = 1;
		mkp2 = mkp_surrogate(mkp, multps);
		nmax = mkp_max_cardinality(mkp2);

		printf("d;%d;%d;%d\n", n, m, nmax);

		mkp_free(mkp2);
	}

	/* surrogating all constraints */
	multps = int_array_init(multps, m, 1);
	mkp2 = mkp_surrogate(mkp, multps);
	nmax = mkp_max_cardinality(mkp2);
	printf("a;%d;%d;%d\n", n, m, nmax);
	mkp_free(mkp2);

	/* checking LP */
	x = mkp_get_lp_sol(mkp);;
	sum = 0;
	for( i = 0 ;  i < n ; i++ )
		sum += x[i];
	printf("l;%d;%d;%.2lf\n", n, m, sum);
	free(x);

	/* checking exact */
	x = mkp_solve_with_scip(mkp, 600, 1.0, 0);
	nmax = 0;
	for( i = 0 ; i < n ; i++ )
		if( x[i] >= 1.0 )
			nmax++;
	printf("s;%d;%d;%d\n", n, m, nmax);

	/* freeing */
	free(multps);

	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
	//return execute_surrogate_research(argc, argv);
	return execute_avl_teste(argc, argv);
}

