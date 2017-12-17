/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: december, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
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
	int ndim,           /* dimension to be used on solution indexing */
	double *secs        /* execution time (seconds) */
);

MOKPSolIndexer *mofpa(
	MOKP *mokp,
	int niter,
	int popsize
);

#endif


