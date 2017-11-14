#ifndef MOKPMH
#define MOKPMH 1

#include "mokp.h"

MOKPSol **mokp_sce(
	MOKP *mokp,
	int ncomp,           /* number of memeplex */
	int compsize,       /* size of memeplex */
	int nsubcomp,               /* size of submemeplex */
	int niter,           /* number of iterations */
	int nsubniter,        /* number of iterations for each memeplex opt */
	int *best_iter       /* to record the iteration that found the best */
);

#endif


