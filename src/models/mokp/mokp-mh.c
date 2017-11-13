#include <stdlib.h>
#include <stdio.h>
#include "mokp-mh.h"
#include "../metahrs/sfl.h"
#include "../../utils/kdtree.h"

/* new random sce solution */
SceSol *ssol_new_random(MOKP *mokp){
	SceSol *ssol;

	ssol = (SceSol*)malloc(sizeof(SceSol));
	ssol->sol = mokpsol_new_random(mokp);
	ssol->rank = -1;

	return ssol;
}
/* Free SCE solution*/
void ssol_free(SceSol ssol{
	mokpsol_free(ssol->sol);
	free(ssol);
}
double ssol_axis_get(SceSol *ssol, int dim){
	return (double)ssol->sol->profit[dim];
}

void population_rank(SceSol **pop, int n, int ndim){
	KDTree *kdt;
	int i;

	kdt = kdtree_new(ndim, (kdtree_eval_f)ssol_axis_get);
	for( i = 0 ; i < n ; i++ )
		kdtree_insert(kdt, pop[i]);
	/* TODO: keep ranking algorithm... */
}

/* Shuffled Complex Evolution for MOKP */
MOKPSol *mokp_sce(
	MOKP *mokp,			 /* MOKP instance */
	int nmeme,           /* number of memeplex */
	int meme_size,       /* size of memeplex */
	int q,               /* size of submemeplex */
	int niter,           /* number of iterations */
	int subniter,        /* number of iterations for each memeplex opt */
	int *best_iter       /* to record the iteration that found the best */
){
	SceSol **pop;
	int i, npop;

	npop = nmeme*meme_size;
	pop = (SceSol**)malloc(sizeof(SceSol*));
	for( i = 0 ; i < npop ; i++ )
		pop[i] = scesol_new_random(mokp);

}

