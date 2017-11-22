#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "mokp-mh.h"
#include "../../metahrs/sfl.h"
#include "../../utils/kdtree.h"
#include "../../utils/list.h"

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

void _ranks_printf(List *ranks){
	ListIter *ranksiter;
	MSIIter *msiiter;
	MOKPSolIndexer *rank;
	MOKPSol *sol;
	int nrank;

	nrank = 0;
	ranksiter = listiter_new(ranks);
	rank = listiter_get(ranksiter);
	msiiter = msiiter_new(rank);
	printf("PRINTING RANKS\n");
	printf(" RANK %d\n", nrank);
	sol = msiiter_get(msiiter);
	while( sol ){
		mokpsol_printf(sol);
		sol = msiiter_forward(msiiter);
		if( !sol ){
			msiiter_free(msiiter);
			rank = listiter_forward(ranksiter);
			nrank++;
			if( rank ){
				printf(" RANK %d\n", nrank);
				msiiter = msiiter_new(rank);
				sol = msiiter_get(msiiter);
			}
		}
	}
	listiter_free(ranksiter);
}

List *rank_population(
	MOKPSol **pop,
	int n,
	int ndim
){
	MOKPSolIndexer *msi, *front;
	List *front_list;
	MOKPSol *sol, *dominant;
	MOKPSol **unrankeds, **unrankeds_, **uaux; /* dominateds*/
	int nrank, nunrankeds, nunrankeds_;
	int i;

	front_list = list_new();
	unrankeds = (MOKPSol**)malloc(n*sizeof(MOKPSol*));
	unrankeds_ = (MOKPSol**)malloc(n*sizeof(MOKPSol*));
	memcpy(unrankeds, pop, n*sizeof(MOKPSol*));
	nunrankeds = n;

	/* while exists solutions to be ranked */
	nrank = 0;
	while( nunrankeds ){
		nunrankeds_ = 0;

		/* create kdtree of unranked */
		msi = msi_new(ndim);
		for( i = 0 ; i < nunrankeds ; i++ )
			msi_insert(msi, unrankeds[i]);

		/* find i-ths rankeds */
		front = msi_new(ndim);
		for( i = 0 ; i < nunrankeds ; i++ )
			if( !msi_find_dominant(msi, unrankeds[i]) ){
				unrankeds[i]->rank = nrank;
				msi_insert(front, unrankeds[i]);
			}else{
				unrankeds_[nunrankeds_++] = unrankeds[i];
			}

		/* swap unrankeds */
		uaux = unrankeds;
		unrankeds = unrankeds_;
		unrankeds_ = uaux;
		nunrankeds = nunrankeds_;

		msi_free(msi);
		list_insert(front_list, front);
		nrank++;
	}

	/* freeing */
	free(unrankeds);
	free(unrankeds_);

	return front_list;
}

MOKPSol **shuffle_memeplexes(
	List *ranks,
	int nranks,
	int ncomp,
	int compsize
){
	MOKPSol **pop;
	MOKPSol *sol;
	ListIter *rankiter;
	MOKPSolIndexer *rank;
	MSIIter *soliter;
	int i, idx, npop;

	npop = ncomp*compsize;
	pop = (MOKPSol**)malloc(npop*sizeof(MOKPSol*));

	rankiter = listiter_new(ranks);
	rank = listiter_get(rankiter);
	soliter = msiiter_new(rank);
	sol = msiiter_get(soliter);
	for( i = 0 ; i < npop ; i++ ){
		if( !sol ){
			msiiter_free(soliter);
			rank = listiter_forward(rankiter);
			soliter = msiiter_new(rank);
			sol = msiiter_get(soliter);
		}
		idx = (i/compsize) + (i%compsize)*ncomp;
		pop[idx] = sol;
		sol = msiiter_forward(soliter);
	}
	msiiter_free(soliter);
	listiter_free(rankiter);
	return pop;
}

MOKPSol **memeplex_new_population(
	List *ranks,
	MOKPSol **pop,
	int ncomp,
	int compsize,
	int nsubcomp,
	int nsubiter
){
	MOKP *mokp;
	MOKPSolIndexer *rank, **ranks_;
	ListIter *liter;
	MOKPSol *dominant, *new, *worst, **news;
	int i, isub, isubworst, isubbest;
	int c, npop, nnew_pop, icomp, subiter;
	float cross_rate;

	mokp = pop[0]->mokp;
	cross_rate = 0.2;
	c = ceil(mokp->n*cross_rate);
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
				isub = (icomp*ncomp) + compsize - triang_raffle(compsize-1) - 1;
				if( isub > isubworst )
					isubworst = isub;
				if( isub < isubbest )
					isubbest = isub;
			}
			worst = pop[isubworst];
			rank = ranks_[worst->rank];

			/* cross with best from submemeplex */
			mokp_shuffle_idxs(mokp);
			new = mokpsol_cross(mokpsol_copy(worst), pop[isubbest], c);
			dominant = msi_find_dominant(rank, new);
			/* cross with best from memeplex */
			if( dominant ){
				mokpsol_free(new);
				mokp_shuffle_idxs(mokp);
				new = mokpsol_cross(mokpsol_copy(worst), pop[icomp*compsize], c);
				dominant = msi_find_dominant(rank, new);
			}
			/* cross with best from population */
			if( dominant ){
				mokpsol_free(new);
				mokp_shuffle_idxs(mokp);
				new = mokpsol_cross(mokpsol_copy(worst), pop[0], c);
				dominant = msi_find_dominant(rank, new);
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
	memcpy(&(new_pop[nnew_pop+1]), pop, npop*sizeof(MOKPSol*));
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
		else mokpsol_free(pop[i]);
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

/* Shuffled Complex Evolution for MOKP */
MOKPSolIndexer *mokp_sce(
	MOKP *mokp,			 /* MOKP instance */
	int ncomp,           /* number of memeplex */
	int compsize,        /* size of memeplex */
	int nsubcomp,        /* size of submemeplex */
	int niter,           /* number of iterations */
	int nsubiter,        /* number of iterations for each memeplex opt */
	int archsize,
	int ndim
){
	MOKPSol *sol, **new_pop, **pop, **memeplexes;
	MOKPSolIndexer **new_ranks;
	MOKPSolIndexer *msi;
	List *ranks;
	MSIIter *msiiter;
	int i, k, irank, nranks, nnew_ranks, npop, nnew_pop;
	Archive *arch;

	arch = archive_new(mokp, 100, 0);
	npop = ncomp*compsize;
	nnew_pop = ncomp*nsubiter;
	/* Initialize population */
	pop = (MOKPSol**)malloc(npop*sizeof(MOKPSol*));
	//printf("POP INICIALIZATION:\n");
	for( i = 0 ; i < npop ; i++ ){
		pop[i] = mokpsol_new_random(mokp);
		//mokpsol_printf(pop[i]);
	}
	
	/* Iterate */
	for( k = 0 ; k < niter ; k++ ){
		printf(" ITERATION %d ", k+1);
		/* Rank population */
		ranks = rank_population(pop, npop, ndim);
		//_ranks_printf(ranks);
		free(pop);

		/* Evolving */
		pop = shuffle_memeplexes(ranks, nranks, ncomp, compsize);
		new_pop = memeplex_new_population(ranks, pop, ncomp, compsize,
														nsubcomp, nsubiter);
		/* Updating archive */
		for( i = 0 ; i < nnew_pop ; i++ )
			archive_propose_sol(arch, new_pop[i]);
		printf("%d sols\n", msi_get_n(arch->pareto));

		/* freeing old ranks */
		list_apply(ranks, (void(*)(void*))msi_free);
		list_free(ranks);
	}
	for( i = 0 ; i < npop ; i++ )
		mokpsol_free(pop[i]);
	free(pop);
	msi = archive_free(arch);
	printf("\nApproximate pareto:\n");
	msi_apply_all(msi, (void(*)(void*))mokpsol_printf);

	return msi;
}

