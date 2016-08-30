#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mmkp/util.h"
#include "mmkp/avl.h"
#include "mmkp/mkp/mkp.h"
#include "mmkp/mkp/domset.h"

int int_cmp(int *a, int *b){
	return *a > *b ? 1 : ( *a < *b ? -1 : 0 );
}
void int_ptr(FILE *out, int *a){
	fprintf(out, "%d", *a);
}

/*
 * Test AVL Tree implementation.
 * */
//int execute_avl_teste(int argc, char **argv){
//	AVLTree *avl;
//	int i, n, *v;
//	char filename[100];
//	FILE *out;
//
//	if( argc < 2 ){
//		fprintf(stderr, "usage: %s <n numbers>\n", argv[0]);
//		return 1;
//	}
//
//	n = atoi(argv[1]);
//	avl = new_avltree((avl_cmp_f)int_cmp);
//	//srand(time(NULL));
//
//	v = (int*)malloc(n*sizeof(int));
//
//	for( i = 0 ; i < n ; i++ ){
//		v[i] = rand() % 1000;
//		avl_insert(avl, &(v[i]));
//
//		/* outputing */
//		sprintf(filename, "/tmp/avl/out%03d.dot", i);
//		out = fopen(filename, "w");
//		//avl_fprint_dot(out, avl, (avl_prt_f)int_ptr);
//		fclose(out);
//	}
//	//system("cd /tmp/avl/; for i in `ls *.dot`; do dot -Tpng -o $i.png $i; done");
//
//	free_avltree(avl);
//	free(v);
//
//	return 0;
//}


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
	int i, n, m, nmax;
	clock_t c0, cf;
	mkpnum profit, *multps;

	double *x, sum, profit_d;

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

	multps = (mkpnum*)malloc(m*sizeof(mkpnum));

	/* FIXME: Algo errado (provavelmente) com mkp_surrogate */
	/* surrogating pairs of constraints */
	for( i = 0 ; i < m-1 ; i++ ){
		multps = mkpnum_array_init(multps, m, 0);
		multps[i] = multps[i+1] = 1;
		mkp2 = mkp_surrogate(mkp, multps);
		profit_d = mkp_get_lp_obj(mkp2);
		nmax = mkp_max_cardinality(mkp2);

		printf("d;%d;%.2lf\n", nmax, profit_d);

		mkp_free(mkp2);
	}

	/* surrogating all constraints equaly */
	multps = mkpnum_array_init(multps, m, 1);
	mkp2 = mkp_surrogate(mkp, multps);
	profit_d = mkp_get_lp_obj(mkp2);
	nmax = mkp_max_cardinality(mkp2);
	printf("a;%d;%.2lf\n", nmax, profit_d);
	mkp_free(mkp2);

	/* surrogating all constraints using duals multipliers */
	x = mkp_solve_dual_with_scip(mkp);
	mkp2 = mkp_surrogate(mkp, x);
	profit_d = mkp_get_lp_obj(mkp2);
	nmax = mkp_max_cardinality(mkp2);
	printf("u;%d;%.2lf\n", nmax, profit_d);
	mkp_free(mkp2);

	/* checking LP */
	x = mkp_get_lp_sol(mkp);
	sum = 0;
	profit_d = mkp_get_lp_obj(mkp);
	for( i = 0 ;  i < n ; i++ )
		sum += x[i];
	printf("l;%.2lf;%.2lf\n", sum, profit_d);
	free(x);

	/* checking LP-maximizing cardinality */
	mkp2 = mkp_card_mkp(mkp);
	profit_d = mkp_get_lp_obj(mkp2);
	printf("m;%.2lf;%.2lf\n", profit_d, profit_d);
	mkp_free(mkp2);

	/* checking exact */
	x = mkp_solve_with_scip(mkp, 600, 1.0, 0);
	nmax = 0;
	profit = 0;
	for( i = 0 ; i < n ; i++ ){
		if( x[i] >= 1.0 ){
			nmax++;
			profit += mkp->p[i];
		}
	}
	printf("s;%d;", nmax);
	mkpnum_fprintf(stdout, profit);
	printf("\n");

	/* freeing */
	free(multps);

	mkp_free(mkp);

	return 0;
}

int ncomp;

/*
 * Analyse distribution of dominating solution throught the different dimensions.
 * */
int execute_domset_search(int argc, char **argv){
	FILE *input;
	MKP *mkp;
	MKPSol *mkpsol;
	int n, m, ndim, nsub;
	char type;
	clock_t c0, c1;
	float t0, t1;

	/* default arguments */
	input = stdin;

	/* checking number of inputs */
	if( argc < 2 ){
		fprintf(stderr, "MKP dominating set search test\n");
		fprintf(stderr, "usage: %s <filename> [ndim=2] [nsub=5] [max_b type='l']\n", argv[0]);
		fprintf(stderr, " - filename : name of MKP instance file. \"-\" for stdin.\n");
		fprintf(stderr, " - max_b types:");
		fprintf(stderr, "    'l' : linear\n");
		fprintf(stderr, "    'q' : quadratic\n");
		fprintf(stderr, "    's' : square root\n");
		return 1;
	}

	ndim = 2;
	nsub = 5;
	type = 'l';
	/* opening input file */
	if(strcmp(argv[1], "-"))
		input = fopen(argv[1], "r");
	/* reading arguments */
	if( argc > 2 )
		ndim = atoll(argv[2]);
	if( argc > 3 )
		nsub = atoll(argv[3]);
	if( argc > 4 )
		nsub = argv[3][0];
	
	/* reading instance */
	mkp = mkp_read_from_file(input);
	m = mkp->m;
	n = mkp->n;
	fclose(input);

	//ncomp = 0;
	///* enumerating */
	//c0 = clock();
	//mkpsol = mkp_fast_domsets_enum(mkp);
	//c1 = clock();
	//t0 = (c1-c0)/(float)CLOCKS_PER_SEC, ncomp;
	//mkpnum_fprintf(stdout, mkpsol->obj);
	//printf(";%3.3f;%.2e\n", t0, (double)ncomp);
	//mkpsol_free(mkpsol);

	ncomp = 0;
	/* enumerating (with linked buckets) */
	c0 = clock();
	mkpsol = mkp_fast_domsets_enum_lbucket(mkp, ndim, nsub, type);
	c1 = clock();
	t1 = (c1-c0)/(float)CLOCKS_PER_SEC, ncomp;
	mkpnum_fprintf(stdout, mkpsol->obj);
	printf(";%3.3f;%.2e\n", t1, (double)ncomp);
	mkpsol_free(mkpsol);

	/* frees */
	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
	//return execute_surrogate_research(argc, argv);
	//return execute_avl_teste(argc, argv);
	return execute_domset_search(argc, argv);
}

