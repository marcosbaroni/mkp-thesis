#include <stdio.h>
#include <stdlib.h>
#include "sfl.h"

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
	void *best_solution;

	f = m*n;

	/* allocing space for population */
	population = (void**)malloc(f*sizeof(void*));
	memeplexes = (void***)malloc(m*sizeof(void**));
	for( i = 0 ; i < m ; i++ )
		memeplexes = (void**)malloc(n*sizeof(void*);
	
	/* initializing population */
	for( i = 0 ; i < f ; i++ )
		population[i] = sfli->new_solution(problem);
	
	/* sorting... */
	/* shuffling... */
	/* improving... */
	
	/* freeing solutions */
	for( i = 0 ; i < f ; i++ )
		sfli->free_solution(population[i]);
	for( i = 0 ; i < m ; i++ )
		free(memeplexes[i]);
	free(memeplexes);
	free(population);

	/* returning best*/
	return best_solution;
}

