#ifndef MOKPMH
#define MOKPMH 1

#include "mokp.h"

typedef struct SceSol{
	MOKPSol *sol;
	int rank;
}SceSol;
SceSol *ssol_new(MOKP*);
void ssol_free(SceSol*);

MOKPSol *mokp_sce(MOKP *mokp);

#endif


