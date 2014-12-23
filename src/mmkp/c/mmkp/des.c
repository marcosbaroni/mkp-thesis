#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "des.h"

/* Returns the probability of changing the variable value, based on trust of that
 *   variable of been the current and the current step
 *   of algorithm execution.
 *
 * trust: trust degree of the given variable value be "1" on "good" solutions. [0,1]
 * val: current value of variable {0,1}
 * steps: current position on algorithm execution (current iteration/max. iterations)
 *   - 0 for begining of execution;
 *   - 1 for end of execution.
 * n: number of variable on solution
 * */
double des_std_activate(double trust, double steps, int n){
	double act_degree, min_chance, prob;

	min_chance = 0.5;
	act_degree = steps;

	prob = act_degree*(1. - trust) + min_chance*(1. - act_degree);
	prob = (prob/(float)n)*(log((float)n)/log(2));

	return prob;
}

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

	/* checking if activate solution is set */
	if(!desi->des_activate) desi->des_activate = des_std_activate;

	/* initialize solutions */
	sols = (void**)malloc(popsize*sizeof(void*));
	for( i = 0 ; i < popsize ; i++ )
		sols[i] = desi->des_new_solution(problem);
	printf("initated\n"); fflush(stdout);

	/* assume best solution */
	best_obj = desi->des_obj(sols[0]);
	best_sol = desi->des_copy_solution(sols[0]);
	printf("best recorded\n"); fflush(stdout);
	
	for( i = 0 ; i < niter ; i++ ){
		printf("iter %d...\n", i); fflush(stdout);
		/* update each individual */
		for( j = 0 ; j < popsize ; j++ ){
			printf(" indiv %d...\n", j);
			sol = sols[j];
			/* for each variable */
			for( k = 0 ; k < nvars ; k++ ){
				printf("  var %d...\n", k);
				/* probability for changing value */
				val = desi->des_get(sol, k);
				prob = desi->des_activate(
					val ? trust[k] : 1. - trust[k],
					i/(float)niter,
					nvars);
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
	
	return best_sol;
}
