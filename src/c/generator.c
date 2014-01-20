#include <stdio.h>
#include <stdlib.h>
#include "pcope.h"



int main(int argc, char **argv){
	PCOPE *p;
	RandConf *rc;

	rc = randconf_default(0, 1150, 12, 53, 2, 0.15, 0.0);
	p = pcope_random(rc);
	//pcope_to_plain(p, stdout);
	pcope_to_scip(p, stdout);

	pcope_free(p);
	randconf_free(rc);

	return 0;
}

