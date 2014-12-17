#include <stdio.h>
#include "des.h"

/*
 * Populational or pure evolutionary?
 * Fitness of individual impacts number of children?
 * How to normalize mutation rate (des_active)? 
 *  - (1/n^2)?
 *  - (1/n)?
 *  - no normalization?
 *
 * */


void *des( DES_Interface *desi, double *trust, void *problem, int nvars, int popsize, int niter){
	void **sols;
	void *best_sol, *sol;
	double prob, best_obj, obj;
	int i, j, k, val;

	/* initialize solutions */
	sols = (void**)malloc(popsize*sizeof(void*));
	sols[0] = desi->des_new_solution(problem);
	for( i = 1 ; i < popsize ; i++ )
		sols[i] = desi->des_new_solution(problem);

	/* assume best solution */
	best_obj = desi->des_obj(sols[0]);
	best_sol = desi->des_copy_solution(sols[0]);
	
	for( i = 0 ; i < niter ; i++ ){
		/* update each individual */
		for( j = 0 ; j < popsize ; j++ ){
			sol = sols[j];
			/* for each variable */
			for( k = 0 ; k < nvars ; k++ ){
				/* probability for changing value */
				val = desi->des_get(sol, k);
				prob = desi->des_activate(
					trust[k],
					desi->des_get(sols[j]),
					i/(float)niter);
				if( drand() < prob )
					desi->des_set(sol, k, 1-val);
			}
			/* check if is best */
			obj = desi->des_obj(sol);
			if( obj > best_obj ){
				desi->des_free_solution(best_sol);
				best_sol = desi->des_copy_solution(sol);
				best_obj = obj;
			}
		}
	}

	/* free */
	for( i = 0 ; i < popsize ; i++ )
		desi->des_free_solution(sols[i]);
	free(sols);
	
	return best;
}
