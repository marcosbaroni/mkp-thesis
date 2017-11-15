#ifndef MOKPMH
#define MOKPMH 1

#include "mokp.h"

MOKPSolIndexer *mokp_sce(
	MOKP *mokp,
	int ncomp,           /* number of memeplex */
	int compsize,       /* size of memeplex */
	int nsubcomp,               /* size of submemeplex */
	int niter,           /* number of iterations */
	int nsubniter        /* number of iterations for each memeplex opt */
);

#endif


