#include <stdio.h>
#include <stdlib.h>
#include "mmkp/mkp.h"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [input file]\n", argv[0]);
	fprintf(out, "\nDESCRIPTION:\n");
	fprintf(out, "     Study of Nemhuauser-Ullmann algorithm for MKP.\n");
	fprintf(out, "\nARGUMENTS:\n");
	fprintf(out, "     input file\ta MKP instance. If '-' is given, instance is read from stdin.\n");

	return 1;
}

void plot_domsets(FILE *out, Array *sols){
	MKPSol *mkpsol;
	int i, nsols;

	nsols = array_get_size(sols);

	for( i = 0 ; i < nsols ; i++ ){
		mkpsol = (MKPSol*)array_get(sols, i);
		fprintf(out, " %lld %lld %lld\n",
			mkpsol->mkp->b[0] - mkpsol->b_left[0],
			mkpsol->mkp->b[1] - mkpsol->b_left[1],
			mkpsol->obj);
	}

	return;
}

void find_uppers(MKP *mkp){
	MKP *mkp2;
	MKPSol *mkpsol;
	LP *lp;
	double *x;
	int i, j, n, m;

	n = mkp->n;
	m = mkp->m;

	/* DELETE CONSTRAINT */
	/* for each dimension */
	for( j = 0 ; j < m ; j++ ){
		/* relax problem (delete constraint) */
		mkp2 = mkp_select_contraints(mkp, &j, 1);

		/* solve a (single) KP */
		x = mkp_solve_with_scip(mkp2, 600, 1.0, 0);
		
		/* extract solution */
		mkpsol = mkpsol_new(mkp2);
		for( i = 0 ; i < n ; i++ )
			if( x[i] >= 1.0 )
				mkpsol_add_item(mkpsol, i);
		printf("%d dim KP: %lld\n", j+1, mkpsol->obj);

		mkpsol_free(mkpsol);
		mkp_free(mkp2);
		free(x);
		/* TODO: test using surrogate relax here. */
	}

	/* LP */
	printf("LP relax: %lf\n", mkp_get_lp_obj(mkp));

	return;
}

void output_sols(FILE *out, Array *sols){
	int dim, i, nsols;

	nsols = array_get_size(sols);
	dim = 0;

	/* sorting solutions */
	array_sort_r(
		sols, (int(*)(void*, void*, void*))mkpsol_cmp_profit, &dim);

	/* outputing */
	for( i = 0 ; i < nsols ; i++ ){
		printf("%03d - ", i);
		mkpsol_fprint(stdout, array_get(sols, i), 1);
	}

	return;
}

int execute_domset_search(int argc, char **argv){
	MKP *mkp;
	Array *sols;
	int i, dim, nsols;
	FILE *input;

	input = stdin;

	/* reading arguments */
	if(argc < 2 )
		return print_usage(argc, argv);
	if(strcmp("-", argv[1]))
		input = fopen(argv[1], "r");
	
	/* reading instance */
	mkp = mkp_read_from_file(input);
	fclose(input);

	/* max items...*/
	//mkp_max_items(mkp);
	//find_uppers(mkp);

	/* searching domsets */
	sols = mkp_nemull(mkp);
	nsols = array_get_size(sols);

	/* output */
	//output_sols(sols);
	plot_domsets(stdout, sols);

	/* frees */
	mkp_free(mkp);
	array_apply(sols, (array_apply_f)mkpsol_free);
	array_free(sols);
	
	return 0;
}

int main(int argc, char **argv){
	return execute_domset_search(argc, argv);
}
