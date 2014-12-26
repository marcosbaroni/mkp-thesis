#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"
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
	act_degree = sigmoid((steps-0.5)*6);

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
	double prob, best_fitness, fitness;
	int i, j, k, val;

	/* checking if activate solution is set */
	if(!desi->des_activate) desi->des_activate = des_std_activate;

	/* initialize solutions */
	sols = (void**)malloc(popsize*sizeof(void*));
	for( i = 0 ; i < popsize ; i++ )
		sols[i] = desi->des_new_solution(problem);

	/* assume best solution */
	best_fitness = desi->des_fitness(sols[0]);
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
					val ? trust[k] : 1. - trust[k],
					i/(float)niter,
					nvars);
				if( drand() < prob )
					desi->des_set(sol, k, 1-val);
			}
			/* repair, if needed */
			if(!desi->des_feasible(sol))
				sol = desi->des_repair(sol);
			/* check if is best */
			fitness = desi->des_fitness(sol);
			//printf("fitness:%lf best:%lf\n", fitness, best_fitness);
			if( fitness > best_fitness ){
				desi->des_free_solution(best_sol);
				best_sol = desi->des_copy_solution(sol);
				best_fitness = fitness;
			}
			//kpsol_fprint(stdout, sol);
		}
	}

	/* free */
	for( i = 0 ; i < popsize ; i++ )
		desi->des_free_solution(sols[i]);
	free(sols);
	
	return best_sol;
}
