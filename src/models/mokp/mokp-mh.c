#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "mokp-mh.h"
#include "../../metahrs/sfl.h"
#include "../../utils/kdtree.h"
#include "../../utils/list.h"

KDTree **population_rank(MOKPSol **pop, int n, int ndim, int *nranks){
	KDTree *kdt, *front, **fronts;
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
		kdt = mokpsol_new_kdtree(ndim);
		for( i = 0 ; i < nunrankeds ; i++ )
			kdtree_insert(kdt, unrankeds[i]);

		/* find i-ths rankeds */
		front = mokpsol_new_kdtree(ndim);
		for( i = 0 ; i < nunrankeds ; i++ )
			if( !mokpsol_find_dominant_kdtree(unrankeds[i], kdt) ){
				unrankeds[i]->rank = *nranks;
				kdtree_insert(front, unrankeds[i]);
			}else{
				unrankeds_[nunrankeds_++] = unrankeds[i];
			}

		/* swap unrankeds */
		uaux = unrankeds;
		unrankeds = unrankeds_;
		unrankeds_ = uaux;
		nunrankeds = nunrankeds_;

		kdtree_free(kdt);
		list_insert(front_list, front);
		(*nranks)++;
	}
	fronts = (KDTree**)list_get_all(front_list);

	/* freeing */
	free(unrankeds);
	free(unrankeds_);
	list_free(front_list);

	return fronts;
}

/* Shuffled Complex Evolution for MOKP */
MOKPSolIndexer *mokp_sce(
	MOKP *mokp,			 /* MOKP instance */
	int nmeme,           /* number of memeplex */
	int meme_size,       /* size of memeplex */
	int q,               /* size of submemeplex */
	int niter,           /* number of iterations */
	int subniter        /* number of iterations for each memeplex opt */
){
	MOKPSol **pop;
	KDTree **ranks;
	MOKPSolIndexer *msi;
	int i, nranks, npop;

	npop = nmeme*meme_size;
	msi = msi_new(0);
	for( i = 0 ; i < npop ; i++ )
		msi_insert(msi, mokpsol_new_random(mokp));

	return msi;
}

