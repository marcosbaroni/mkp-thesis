#ifndef MOKPMH
#define MOKPMH 1

#include "mokp.h"

MOKPSolIndexer *mokp_sce(
	MOKP *mokp,
	int ncomp,          /* number of memeplex */
	int compsize,       /* size of memeplex */
	int nsubcomp,       /* size of submemeplex */
	int niter,          /* number of iterations */
	int nsubiter,       /* number of iterations for each memeplex opt */
	int archsize,       /* size of archive */
	int ndim,           /* dimension to be used on solution indexing */
	double *secs         /* execution time (seconds) */
);

#endif


