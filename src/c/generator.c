#include <stdio.h>
#include <stdlib.h>
#include "pcope.h"



int main(int argc, char **argv){
	PCOPE *p;
	RandConf *rc;


	rc = randconf_default(0, 5, 4, 3, 2, 0.15, 1.0);
	p = pcope_random(rc);
	//pcope_to_plain(p, stdout);
	pcope_to_scip(p, stdout);

	pcope_free(p);
	randconf_free(rc);

	return 0;
}

