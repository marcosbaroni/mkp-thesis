#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "mokp-mh.h"
#include "../../metahrs/sfl.h"
#include "../../utils/kdtree.h"
#include "../../utils/list.h"

MOKPSolIndexer **rank_population(MOKPSol **pop, int n, int ndim, int *nranks){
	MOKPSolIndexer *msi, *front, **fronts;
	List *front_list;
	MOKPSol *sol, *dominant;
	MOKPSol **unrankeds, **unrankeds_, **uaux; /* dominateds*/
	int nunrankeds, nunrankeds_;
	int i;


	front_list = list_new();
	unrankeds = (MOKPSol**)malloc(n*sizeof(MOKPSol*));
	unrankeds_ = (MOKPSol**)malloc(n*sizeof(MOKPSol*));
	memcpy(unrankeds, pop, n*sizeof(MOKPSol*));
	nunrankeds = n;

	/* while exists solutions to be ranked */
	*nranks = 0;
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
				unrankeds[i]->rank = *nranks;
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
		(*nranks)++;
	}
	fronts = (MOKPSolIndexer**)list_get_all(front_list);

	/* freeing */
	free(unrankeds);
	free(unrankeds_);
	list_free(front_list);

	return fronts;
}

MOKPSol **shuffle_memeplexes(
	MOKPSolIndexer **ranks,
	int nranks,
	int ncomp,
	int compsize
){
	MOKPSol **pop;
	MOKPSol *sol;
	MSIIter *iter;
	int i, idx, rank, npop;

	rank = 0;
	npop = ncomp*compsize;
	pop = (MOKPSol**)malloc(npop*sizeof(MOKPSol*));
	iter = msiiter_new(ranks[0]);
	for( i = 0 ; i < npop ; i++ ){
		/* next best solution */
		sol = msiiter_next(iter);
		if( !sol ){
			msiiter_free(iter);
			iter = msiiter_new(ranks[++rank]);
			sol = msiiter_next(iter);
		}
		idx = (i*compsize) + (i%compsize);
		pop[idx] = sol;
	}

}

MOKPSol **memeplex_new_population(
	MOKPSolIndexer **ranks,
	MOKPSol **pop,
	int ncomp,
	int compsize,
	int nsubcomp,
	int nsubiter
){
	MOKP *mokp;
	MOKPSolIndexer *rank;
	MOKPSol *dominant, *new, *worst, **news;
	int i, isub, isubworst, isubbest;
	int c, npop, icomp, subiter;
	float cross_rate;

	mokp = pop[0]->mokp;
	c = floor(mokp->n*cross_rate);
	npop = ncomp*compsize;
	news = (MOKPSol**)malloc(npop*sizeof(MOKPSol*));

	for( icomp = 0 ; icomp < ncomp ; icomp++ ){
		for( subiter = 0 ; subiter < nsubiter ; subiter++ ){
			isubworst = 0;
			isubbest = 99999999;

			/* sorting sub memeplext (select a best and a worst among nsubcomp) */
			for( i = 0 ; i < nsubcomp ; i++ ){
				isub = lrand(nsubcomp-1);
				if( isub > isubworst )
					isubworst = isub;
				if( isub < isubbest )
					isubbest = isub;
			}
			worst = pop[isubworst];
			rank = ranks[worst->rank];

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
			if( dominant )
				new = mokpsol_new_random(mokp);
			news[icomp*nsubiter+subiter] = new;
		}
	}
	return news;
}

MOKPSol *select_population(
	MOKPSol *pop,
	MOKPSol *new_pop,
	int npop,
	int nnew_pop
){
	List *rank_list;
	int i, nranks;
	MOKPSolIndexer *rank;
	MOKPSolIndexer *msi;
	/* TODO: stopped here... filtering the best npop solutions from both arrays,
	 * putting it in pop and freeinf all others. */

	new_pop = realloc(new_pop, (npop+nnew_pop)*sizeof(MOKPSol*));
	memcpy(&(new_pop[nnew_pop+1]), pop, npop*sizeof(MOKPSol*));

	return pop;
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
	MOKPSol **new_pop, **pop, **memeplexes;
	MOKPSolIndexer **ranks, **new_ranks;
	MOKPSolIndexer *msi;
	int i, k, irank, nranks, nnew_ranks, npop;
	Archive *arch;

	arch = archive_new(mokp, 100, 0);
	npop = ncomp*compsize;
	/* Initialize population */
	pop = (MOKPSol**)malloc(npop*sizeof(MOKPSol*));
	for( i = 0 ; i < npop ; i++ )
		pop[i] = mokpsol_new_random(mokp);
	
	/* iterate */
	for( k = 0 ; k < niter ; k++ ){
		/* Rank population */
		ranks = rank_population(pop, npop, ndim, &nranks);
		free(pop);

		/* Evolving */
		pop = shuffle_memeplexes(ranks, nranks, ncomp, compsize);
		new_pop = memeplex_new_population(ranks, pop, ncomp, compsize,
														nsubcomp, nsubiter);
		/* Updating archive */
		for( i = 0 ; i < ncomp*nsubcomp ; i++ )
			archive_propose_sol(arch, new_pop[i]);
		/* select from population */
	}

	/* Free */
	for( i = 0 ; i < nranks ; i++ ){
		msi_apply_all(ranks[i], (void(*)(void*))mokpsol_free);
		msi_free(ranks[i]);
	}
	free(ranks);

	msi = msi_new(0);
	return msi;
}

