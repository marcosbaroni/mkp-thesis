#include <stdio.h>
#include <stdlib.h>
#include "sfl.h"
#include "util.h"

void sfl_swap(void **pop, int a, int b){
	void *p;
	p = pop[a]; pop[a] = pop[b]; pop[b] = p;
	return;
}

int sfl_compar(void **pop, int a, int b, double (*fitness)(void*)){
	double fa, fb;
	fa = fitness(pop[a]);
	fb = fitness(pop[b]);

	/* descending order of qsort */
	if( fa > fb ) return -1;
	else if( fa < fb ) return 1;
	return 0;
}

void *sfl(
	SFL_Interface *sfli, /* the SFL Interface */
	void *problem,       /* the problem */
	int nmeme,           /* number of memeplex */
	int meme_size,       /* size of memeplex */
	int q,               /* size of submemeplex */
	int niter,           /* number of iterations */
	int subniter,        /* number of iterations for each memeplex opt */
	int *best_iter)      /* to record the iteration that found the best */
{
	int i, j, k, f, iter, idx;
	int widx;          /* worst (idx) */
	void **population;
	void *individual;
	void **memeplex;
	void ***memeplexes;
	void **memeplexes_best;
	void *global_best, *meme_best, *submeme_best, *submeme_worst;
	double fitness;

	f = nmeme*meme_size;
	*best_iter = 0;

	/* allocing space for population */
	population = (void**)malloc(f*sizeof(void*));
	memeplexes = (void***)malloc(nmeme*sizeof(void**));
	memeplexes_best = (void**)malloc(nmeme*sizeof(void*));
	for( i = 0 ; i < nmeme ; i++ )
		memeplexes[i] = (void**)malloc(meme_size*sizeof(void*));
	
	/* initializing population */
	for( i = 0 ; i < f ; i++ )
		population[i] = sfli->new_solution(problem);
	global_best = sfli->copy_solution(population[0]);

	/* each iteration */
	for( iter = 0 ; iter < niter ; iter++ ){
		/* sorting population */
		mp_qsort_r(population, f, 
			(mp_cmp_r_f)sfl_compar, 
			(mp_swap_f)sfl_swap, 
			sfli->fitness, 0);
		
		/* checking if its best */
		if( sfli->fitness(population[0]) > sfli->fitness(global_best) ){
			sfli->free_solution(global_best);
			global_best = sfli->copy_solution(population[0]);
			*best_iter = iter;
		}

		/* shuffling */
		for( i = 0 ; i < nmeme ; i++ ){      /* best of each */
			memeplexes[i][0] = memeplexes_best[i] = population[i];
		}
		for( i = 1 ; i < meme_size; i++ )      /* populating memeplexes */
			for( j = 0 ; j < nmeme ; j++ )
				memeplexes[j][i] = population[i*nmeme+j];

		/* IMPROVING */
		/* for each memeplex */
		for( i = 0 ; i < nmeme ; i++ ){
			memeplex = memeplexes[i];
			meme_best = memeplexes_best[i];

			/* throught some steps... */
			for( j = 0 ; j < subniter ; j++ ){

				/* selecting submemeplex */
				widx = idx = meme_size-triang_raffle(meme_size-1)-1;
				submeme_best = submeme_worst = memeplex[idx];
				for( k = 1 ; k < q || (idx == widx) ; k++ ){
					idx = meme_size-triang_raffle(meme_size-1)-1;
					individual = memeplex[idx];
					if( sfli->fitness(individual) > sfli->fitness(submeme_best) )
						submeme_best = individual;
					else if( sfli->fitness(individual) < sfli->fitness(submeme_worst) ){
						submeme_worst = individual;
						widx = idx;
					}
				}

				/* evolving worst with local best */
				fitness = sfli->fitness(submeme_worst);
				submeme_worst = sfli->cross(submeme_worst, submeme_best);
				/* evolving worst with memeplex best */
				if( sfli->fitness(submeme_worst) < fitness ){
					submeme_worst = sfli->cross(submeme_worst, meme_best);
				}
				/* evolving worst with global best */
				if( sfli->fitness(submeme_worst) < fitness ){
					submeme_worst = sfli->cross(submeme_worst, global_best);
				}
				/* replacing worst */
				if( sfli->fitness(submeme_worst) < fitness ){
					sfli->free_solution(submeme_worst);
					submeme_worst = sfli->new_solution(problem);
				}
				memeplexes[i][widx] = population[widx*nmeme+i] = submeme_worst;
			}
		}
	}

	/* getting global best solution */
	for( i = 0 ; i < f ; i++ ){
		if( sfli->fitness(population[i]) > sfli->fitness(global_best) ){
			sfli->free_solution(global_best);
			global_best = sfli->copy_solution(population[i]);
		}
	}
	
	/* freeing solutions */
	for( i = 0 ; i < f ; i++ )
		sfli->free_solution(population[i]);
	for( i = 0 ; i < nmeme ; i++ )
		free(memeplexes[i]);
	free(memeplexes);
	free(memeplexes_best);
	free(population);

	/* returning best*/
	return global_best;
}

void sfli_free(SFL_Interface *sfli){
	free(sfli);
	return;
}

