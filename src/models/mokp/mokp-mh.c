/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: december, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "mokp-mh.h"
#include "../../metahrs/sfl.h"
#include "../../utils/kdtree.h"
#include "../../utils/list.h"


/************************************
 *    SHUFFLED COMPLEX EVOLUTION    *
 ************************************/
void _memeplexes_printf(
	MOKPSol **pop,
	int ncomp,
	int compsize
){
	int i, j;
	printf("PRINTING COMPLEXES\n");
	for( i = 0 ; i < ncomp ; i++ ){
		printf(" COMPLEX %d:\n", i+1);
		for( j = 0 ; j < compsize ; j++ )
			mokpsol_printf(pop[i*ncomp+j]);
	}
	return;
}

void pop_printf(MOKPSol **pop, int npop){
	int i, j, np;
	unsigned long long prod;

	np = pop[0]->mokp->np;

	for( i = 0 ; i < npop ; i++ ){
		printf("%d: [%d] (", i+1, pop[i]->rank);
		prod = 1;
		for( j = 0 ; j < np ; j++ ){
			prod *= pop[i]->profit[j];
			printf("%s%d", !j ? "" : ",", pop[i]->profit[j]);
		}
		printf(") %lld\n", prod);
	}
	return;
}

List *rank_population(
	MOKPSol **_pop,
	int n,
	int ndim
){
	MOKPSolIndexer *front, *pop;
	MSIIter *pop_iter, *front_iter;
	List *ranks;
	MOKPSol *sol, *dominant;
	int nranked, *isranked;
	int i;

	ranks = list_new();
	pop = msi_new(ndim);
	for( i = 0 ; i < n ; i++ )
		msi_insert(pop, _pop[i]);
	nranked = 0;

	/* while exists solutions to be ranked */
	while( msi_get_n(pop) ){
		/* open a new front */
		front = msi_new(ndim);

		/* insert non-dominated in opened front */
		pop_iter = msiiter_new(pop);
		while( sol = (MOKPSol*)msiiter_get(pop_iter) ){
			if( !(dominant = msi_find_dominant(pop, sol, 0)) ){
				sol->rank = ranks->n;
				msi_insert(front, sol);
			}
			msiiter_forward(pop_iter);
		}
		msiiter_free(pop_iter);
		
		/* remove non-dominated from pop */
		front_iter = msiiter_new(front);
		while( sol = (MOKPSol*)msiiter_get(front_iter) ){
			msi_remove(pop, sol);
			msiiter_forward(front_iter);
		}
		msiiter_free(front_iter);

		list_insert(ranks, front);
	}

	/* freeing */
	msi_free(pop);

	return ranks;
}

int _sort_ranks_pop_cmp( const void *a, const void *b){
	MOKPSol *sol_a, *sol_b;
	int rank_res, profit_res;
	unsigned long long prod_a, prod_b, prod_res;
	int j, np;

	sol_a = *((MOKPSol**)a);
	sol_b = *((MOKPSol**)b);
	np = sol_a->mokp->np;

	rank_res = sol_a->rank - sol_b->rank;
	if( rank_res )
		return rank_res;

	prod_a = 1;
	prod_b = 1;
	for( j = 0 ; j < np ; j++ ){
		prod_a *= sol_a->profit[j];
		prod_b *= sol_b->profit[j];
	}
	prod_res = prod_a - prod_b;
	if( prod_res )
		return -prod_res;

	for( j = 0 ; j < np ; j++ )
		if( profit_res = sol_a->profit[j] - sol_b->profit[j] )
			return -profit_res;

	return 0;
}

MOKPSol **sort_ranks_pop(List *ranks, int npop){
	MOKPSol **pop;
	ListIter *rankiter;
	MOKPSolIndexer *rank;
	MSIIter *msiiter;
	MOKPSol *sol;
	int i, j;

	pop = (MOKPSol**)malloc(npop*sizeof(MOKPSol*));

	rankiter = listiter_new(ranks);
	rank = listiter_get(rankiter);
	msiiter = msiiter_new(rank);
	sol = msiiter_get(msiiter);

	for( i = 0 ; i < npop ; i++ ){
		if( !sol ){
			msiiter_free(msiiter);
			rank = listiter_forward(rankiter);
			msiiter = msiiter_new(rank);
			sol = msiiter_get(msiiter);
		}
		//pop[i] = sol;
		pop[i] = sol;
		msiiter_forward(msiiter);
		sol = msiiter_get(msiiter);
	}

	msiiter_free(msiiter);
	listiter_free(rankiter);
	qsort(pop, npop, sizeof(MOKPSol*), _sort_ranks_pop_cmp);

	return pop;
}

MOKPSol **shuffle_memeplexes(
	List *ranks,
	int nranks,
	int ncomp,
	int compsize
){
	MOKPSol **pop_shuffled, **pop_sorted;
	int i, idx, npop;

	npop = ncomp*compsize;
	pop_sorted = sort_ranks_pop(ranks, npop);
	pop_shuffled = (MOKPSol**)malloc(npop*sizeof(MOKPSol*));

	for( i = 0 ; i < npop ; i++ ){
		idx = ( i % ncomp)*compsize + ( i / ncomp);
		pop_shuffled[( i % ncomp)*compsize + ( i / ncomp)] = pop_sorted[i];
	}
	free(pop_sorted);

	return pop_shuffled;
}

MOKPSol **memeplex_new_population(
	List *ranks,
	MOKPSol **pop,
	int ncomp,
	int compsize,
	int nsubcomp,
	int nsubiter,
	int ncross
){
	MOKP *mokp;
	MOKPSolIndexer *rank, **ranks_;
	ListIter *liter;
	MOKPSol *dominant, *new, *worst, **news;
	int i, isub, isubworst, isubbest;
	int npop, nnew_pop, icomp, subiter;

	mokp = pop[0]->mokp;
	npop = ncomp*compsize;
	nnew_pop = ncomp*nsubiter;
	news = (MOKPSol**)malloc(nnew_pop*sizeof(MOKPSol*));
	ranks_ = (MOKPSolIndexer**)list_get_all(ranks);
	//_memeplexes_printf(pop, ncomp, compsize);

	for( icomp = 0 ; icomp < ncomp ; icomp++ ){
		for( subiter = 0 ; subiter < nsubiter ; subiter++ ){
			isubworst = 0;
			isubbest = 99999999;

			/* sorting sub memeplext (select a best and a worst among nsubcomp) */
			for( i = 0 ; i < nsubcomp || isubworst == isubbest ; i++ ){
				//isub = lrand(nsubcomp-1);
				isub = (icomp*compsize) + compsize - triang_raffle(compsize-1) - 1;
				if( isub > isubworst )
					isubworst = isub;
				if( isub < isubbest )
					isubbest = isub;
			}
			worst = pop[isubworst];
			rank = ranks_[worst->rank];

			/* cross with best from submemeplex */
			mokp_shuffle_idxs(mokp);
			new = mokpsol_cross(mokpsol_copy(worst), pop[isubbest], ncross);
			dominant = msi_find_dominant(rank, new, 1);
			/* cross with best from memeplex */
			if( dominant ){
				mokpsol_free(new);
				mokp_shuffle_idxs(mokp);
				new = mokpsol_cross(mokpsol_copy(worst), pop[icomp*compsize], ncross);
				dominant = msi_find_dominant(rank, new, 1);
			}
			/* cross with best from population */
			if( dominant ){
				mokpsol_free(new);
				mokp_shuffle_idxs(mokp);
				new = mokpsol_cross(mokpsol_copy(worst), pop[0], ncross);
				dominant = msi_find_dominant(rank, new, 1);
			}
			/* generate new random */
			if( dominant ){
				mokpsol_free(new);
				new = mokpsol_new_random(mokp);
			}
			news[icomp*nsubiter+subiter] = new;
		}
	}
	free(ranks_);
	return news;
}

MOKPSol **select_population(
	MOKPSol **pop,      /* old population */
	MOKPSol **new_pop,  /* new generated population */
	int npop,
	int nnew_pop,
	int ndim
){
	int i, ntot, nranks;
	List *ranks;
	ListIter *rankiter;
	MOKPSolIndexer *rank;
	MSIIter *soliter;
	MOKPSol *sol;

	/* re-ranking population */
	new_pop = realloc(new_pop, (nnew_pop + npop)*sizeof(MOKPSol*));
	memcpy(&(new_pop[nnew_pop]), pop, npop*sizeof(MOKPSol*));
	ranks = rank_population(new_pop, (nnew_pop+npop), ndim);

	/* selecting population */
	rankiter = listiter_new(ranks);
	rank = listiter_get(rankiter);
	soliter = msiiter_new(rank);
	sol = msiiter_get(soliter);
	for( i = 0 ; i < npop+nnew_pop ; i++ ){
		if( !sol ){
			msiiter_free(soliter);
			rank = listiter_forward(rankiter);
			soliter = msiiter_new(rank);
			sol = msiiter_get(soliter);
		}
		if( i < npop ) pop[i] = sol;
		else mokpsol_free(sol);
		sol = msiiter_forward(soliter);
	}
	msiiter_free(soliter);
	listiter_free(rankiter);
	list_apply(ranks, (void(*)(void*))msi_free);
	list_free(ranks);
	free(new_pop);

	return pop;
}

void archive_propose_sol_(MOKPSol *sol, Archive *arch){
	archive_propose_sol(arch, sol);
}

/* for debug */
void _print_ranks(List *ranks){
	ListIter *iter;
	MOKPSolIndexer *msi;
	int r = 1;


	iter = list_get_first(ranks);
	msi = listiter_get(iter);
	printf(" RANKED POPULATION\n");
	while( msi = listiter_get(iter) ){
		printf(" Rank %d (%d)\n", r++, msi_get_n(msi));
		msi_apply_all(msi, mokpsol_printf);
		listiter_forward(iter);
	}
	listiter_free(iter);
}

int msi_pareto_update_(MOKPSol *sol, MOKPSolIndexer *msi){
	return msi_pareto_update(msi, sol);
}

/***   Shuffled Complex Evolution for MOKP   ***/
MOKPSolIndexer *mokp_sce(
	MOKP *mokp,			 /* MOKP instance */
	int ncomp,           /* number of memeplex */
	int compsize,        /* size of memeplex */
	int nsubcomp,        /* size of submemeplex */
	int niter,           /* number of iterations */
	int nsubiter,        /* number of iterations for each memeplex opt */
	int ndim,
	int ncross,          /* number of genes crossed */
	double *secs
){
	MOKPSol *sol, **new_pop, **pop, **memeplexes;
	MOKPSolIndexer **new_ranks;
	MOKPSolIndexer *msi;
	List *ranks;
	MSIIter *msiiter;
	int i, j, k, irank, nranks, nnew_ranks, npop, nnew_pop;
	time_t ping, pong;

	ping = clock();

	npop = ncomp*compsize;
	nnew_pop = ncomp*nsubiter;
	/* Initialize population */
	pop = (MOKPSol**)malloc(npop*sizeof(MOKPSol*));
	for( i = 0 ; i < npop ; i++ )
		pop[i] = mokpsol_new_random(mokp);
	/* ranking initial population */
	ranks = rank_population(pop, npop, ndim);
	/* initializing initial pareto */
	msi = msi_new(ndim);
	//printf(" Current pareto (initial)\n");
	//msi_apply_all(msi, mokpsol_printf);
	msi_apply_all_r(
		list_get_head(ranks),
		(void(*)(MOKPSol*,void*))msi_pareto_update_,
		msi
	);
	
	/* Iterate */
	for( k = 0 ; k < niter ; k++ ){
		//printf("%d/%d\n", k+1, niter); fflush(stdout);

		/* Evolving */
		free(pop);
		pop = shuffle_memeplexes(ranks, nranks, ncomp, compsize);
		new_pop = memeplex_new_population(
			ranks, pop, ncomp, compsize, nsubcomp, nsubiter, ncross);
		/* Updating archive */
		// TODO: just try to update individuals from first front
		//printf(" Current pareto (%d/%d)\n", k+1, niter);
		//msi_apply_all(msi, mokpsol_printf);
		for( i = 0 ; i < nnew_pop ; i++ )
			msi_pareto_update(msi, new_pop[i]);

		pop = select_population(pop, new_pop, npop, nnew_pop, ndim);

		/* freeing old ranks */
		list_apply(ranks, (void(*)(void*))msi_free);
		list_free(ranks);

		/* Rank population */
		ranks = rank_population(pop, npop, ndim);
	}
	//printf("\n");
	list_apply(ranks, (void(*)(void*))msi_free);
	list_free(ranks);
	for( i = 0 ; i < npop ; i++ )
		mokpsol_free(pop[i]);
	free(pop);

	pong = clock();
	if(secs)
		*secs = (pong - ping)/(double)CLOCKS_PER_SEC;

	//msi_apply_all(msi, mokpsol_printf);

	return msi;
}


/*********************************
*    FIREFLY PARTICLE SWARM     *
********************************/
void safeguard_non_dominated(MOKPSolIndexer *archive, MOKPSol **pop, int popsize){
	int i, j;
	MOKPSol *candidate, *sol, *dominant;

	for( i = 0 ; i < popsize ; i++ ){
		candidate = pop[i];
		dominant = NULL;
		for( j = 0 ; !dominant && j < popsize ; i++ )
			if( mokpsol_dom_cmp(pop[j], candidate) == SOL_DOMINATES )
				dominant = pop[j];
		if( !dominant )
			msi_insert(archive, candidate);
	}

	return;
}

/*  Firefly Particle Swarm  */
MOKPSolIndexer *mofpa(
	MOKP *mokp,
	int niter,
	int popsize
){
	MOKPSolIndexer *msi, *archive;
	MOKPSol **pop;
	int i, j, k;

	/* Inicialization */
	archive = msi_new(0);
	pop = (MOKPSol**)malloc(popsize*sizeof(MOKPSol*));
	for( i = 0 ; i < popsize ; i++ )
		pop[i] = mokpsol_new_random(mokp);

	/* Frees */

	return msi;
}

