#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "mokp-mh.h"
#include "../../metahrs/sfl.h"
#include "../../utils/kdtree.h"


int mokpsol_dominates_(MOKPSol *a, MOKPSol *b){
	return (mokpsol_dom_cmp(a, b) == SOL_DOMINATES);
}

MOKPSol *mokpsol_find_dominant_kdtree(MOKPSol *sol, KDTree *kdt){
	MOKPSol *dominant = NULL;
	double *bounds;
	int i, ndim;

	ndim = kdt->ndim;
	printf("  finding odminet for\n");
	mokpsol_printf(sol);fflush(stdout);
	bounds = (double*)malloc(2*ndim*sizeof(double));
	for( i = 0 ; i < ndim ; i++ ){
		bounds[2*i] = sol->profit[i];
		bounds[2*i+1] = INFINITY;
	}

	dominant = kdtree_range_search_r(
		kdt,
		bounds,
		(property_f_r)mokpsol_dominates_,
		sol);

	free(bounds);

	return dominant;
}

double sol_axis_get(MOKPSol *sol, int dim){
	return (double)sol->profit[dim];
}

KDTree **population_rank(MOKPSol **pop, int n, int ndim, int *nranks){
	KDTree *kdt, **fronts;
	MOKPSol *sol, *dominant;
	MOKPSol **unrankeds, **unrankeds_, **uaux; /* dominateds*/
	int nunrankeds, nunrankeds_;
	int i, maxranks;

	maxranks = 10;

	fronts = (KDTree**)malloc(maxranks*sizeof(KDTree*));
	unrankeds = (MOKPSol**)malloc(n*sizeof(MOKPSol*));
	unrankeds_ = (MOKPSol**)malloc(n*sizeof(MOKPSol*));
	memcpy(unrankeds, pop, n*sizeof(MOKPSol*));
	nunrankeds = n;

	/* while exists solutions to be ranked */
	*nranks = 0;
	while( nunrankeds ){
		printf("Setting up rank: %d\n", *nranks);
		nunrankeds_ = 0;

		/* create kdtree of unranked */
		kdt = kdtree_new(ndim, (kdtree_eval_f)sol_axis_get);
		for( i = 0 ; i < nunrankeds ; i++ )
			kdtree_insert(kdt, unrankeds[i]);

		/* find i-ths rankeds */
		fronts[*nranks] = kdtree_new(ndim, (kdtree_eval_f)sol_axis_get);
		for( i = 0 ; i < nunrankeds ; i++ )
			if( !mokpsol_find_dominant_kdtree(unrankeds[i], kdt) )
				kdtree_insert(fronts[*nranks], unrankeds[i]);
			else
				unrankeds_[nunrankeds_++] = unrankeds[i];

		/* swap unrankeds */
		uaux = unrankeds;
		unrankeds = unrankeds_;
		unrankeds_ = uaux;
		nunrankeds = nunrankeds_;

		/* free kdftree of unranked */
		kdtree_free(kdt);

		(*nranks)++;
	}
	free(unrankeds);
	free(unrankeds_);
	return fronts;
}

/* Shuffled Complex Evolution for MOKP */
MOKPSol **mokp_sce(
	MOKP *mokp,			 /* MOKP instance */
	int nmeme,           /* number of memeplex */
	int meme_size,       /* size of memeplex */
	int q,               /* size of submemeplex */
	int niter,           /* number of iterations */
	int subniter,        /* number of iterations for each memeplex opt */
	int *best_iter       /* to record the iteration that found the best */
){
	MOKPSol **pop;
	KDTree **ranks;
	int i, nranks, npop;

	npop = nmeme*meme_size;
	pop = (MOKPSol**)malloc(npop*sizeof(MOKPSol*));
	for( i = 0 ; i < npop ; i++ )
		pop[i] = mokpsol_new_random(mokp);

	printf("ALL POPULATION\n");
	for( i = 0 ; i < npop ; i++ )
		mokpsol_printf(pop[i]);

	ranks = population_rank(pop, npop, 3, &nranks);

	printf("\n");
	for( i = 0 ; i < nranks ; i++ ){
		printf("RANK %d:\n", i);
		kdtree_apply_to_all(ranks[i], (void(*)(void*))mokpsol_printf);
		kdtree_apply_to_all(ranks[i], (void(*)(void*))mokpsol_free);
		kdtree_free(ranks[i]);
	}
	free(ranks);
	free(pop);

	return NULL;
}

