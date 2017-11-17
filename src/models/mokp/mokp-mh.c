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
	npop = ncomp*ncompsize;
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

/* Shuffled Complex Evolution for MOKP */
MOKPSolIndexer *mokp_sce(
	MOKP *mokp,			 /* MOKP instance */
	int nmeme,           /* number of memeplex */
	int meme_size,       /* size of memeplex */
	int q,               /* size of submemeplex */
	int niter,           /* number of iterations */
	int subniter,        /* number of iterations for each memeplex opt */
	int archsize,
	int ndim
){
	MOKPSol **pop, **memeplexes;
	MOKPSolIndexer **ranks;
	MOKPSolIndexer *msi;
	int i, nranks, npop;

	npop = nmeme*meme_size;
	/* Initialize population */
	pop = (MOKPSol**)malloc(npop*sizeof(MOKPSol*));
	for( i = 0 ; i < npop ; i++ )
		pop[i] = mokpsol_new_random(mokp);
	
	/* Rank population */
	ranks = rank_population(pop, npop, ndim, &nranks);
	free(pop);

	pop = shuffle_memeplexes(ranks, nranks, ncomp, compsize);

	/* Free */
	for( i = 0 ; i < nranks ; i++ ){
		msi_apply_all(ranks[i], (void(*)(void*))mokpsol_free);
		msi_free(ranks[i]);
	}
	free(ranks);

	msi = msi_new(0);
	return msi;
}

