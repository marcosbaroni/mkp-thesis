#include <stdio.h>
#include <stdlib.h>
#include "pcope.h"



int main(int argc, char **argv){
	PCOPE *p;
	RandConf *rc;

<<<<<<< HEAD
	srand(time(NULL));

	rc = randconf_default();
	p = pcope_random(5, 4, 3, 2, 0.15, rc);
	pcope_to_plain(p, stdout);
=======
	rc = randconf_default(0, 1150, 12, 53, 2, 0.15, 0.0);
	p = pcope_random(rc);
	//pcope_to_plain(p, stdout);
	pcope_to_scip(p, stdout);
>>>>>>> c801b892585fddcfaa6eeb7e2cd8d736f428d238

	pcope_free(p);
	randconf_free(rc);

	return 0;
}

