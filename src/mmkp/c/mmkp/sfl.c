#include <stdio.h>
#include <stdlib.h>
#include "sfl.h"

int sfl_compar(void *a, void *b, void *fitness_f){
	double fa, fb;
	fa = fitness_f(a);
	fb = fitness_f(b);

	/* descending order of qsort */
	if( fa > fb ) return -1;
	else if( fa < fb ) return 1;
	return 0;
}

void *sfl(
	SFL_Interface *sfli; /* the SFL Interface */
	void *problem,       /* the problem */
	int nvars,           /* size of solution */
	int m,               /* number of memeplex */
	int n,               /* size of memeplex */
	int q,               /* size of submemeplex */
	int niter)           /* number of iterations */
{
	int i, f;
	void **population;
	void ***memeplexes;
	void **memeplexes_best;
	double *memeplexes_best_fitness;
	void *best_solution;
	double best_fitness;
	double aux_fitness;

	f = m*n;
	/* allocing space for population */
	population = (void**)malloc(f*sizeof(void*));
	memeplexes = (void***)malloc(m*sizeof(void**));
	memeplexes_best = (void**)malloc(m*sizeof(void*));
	memeplexes_best_fitness = (double*)malloc(m*sizeof(double));
	for( i = 0 ; i < m ; i++ ){
		memeplexes = (void**)malloc(n*sizeof(void*);
		memeplexes_best_fitness[i] = 0.;
	}
	
	/* initializing population */
	for( i = 0 ; i < f ; i++ )
		population[i] = sfli->new_solution(problem);
	
	/* sorting */
	qsort_r(population, f, sizeof(void*), sfl_compar, sfli->fitness);
	if(sfli->fitness(population[0]) > best_fitness){
		best_solution = population[0];
		best_fitness = sfli->fitness(best_solution);
	}

	/* shuffling */
	for( i = 0 ; i < n ; i++ ){
		for( j = 0 ; j < m ; j++ ){
			memeplexes[j][i] = population[i*m+j];
			fitness = sfli->fitness(memeplexes[j][i]);
			/* best of memeplex? */
			if(fitness > memeplexes_best_fitness[j]){
				memeplexes_best[j] = memeplexes[j][i];
				memeplexes_best_fitness[j] = fitness;
			}
		}
	}

	/* improving... */
	
	/* freeing solutions */
	for( i = 0 ; i < f ; i++ )
		sfli->free_solution(population[i]);
	for( i = 0 ; i < m ; i++ )
		free(memeplexes[i]);
	free(memeplexes);
	free(memeplexes_best);
	free(memeplexes_best_fitness);
	free(population);

	/* returning best*/
	return best_solution;
}

void sfli_fee(SFL_Interface *sfli){
	free(sfli);
	return;
}

