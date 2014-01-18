#include <stdio.h>
#include <stdlib.h>
#include "pcope.h"



int main(int argc, char **argv){
	PCOPE *p;
	RandConf *rc;

	srand(time(NULL));

	rc = randconf_default();
	p = pcope_random(5, 4, 3, 2, 0.15, rc);
	//pcope_to_plain(p, stdout);

	pcope_free(p);
	randconf_free(rc);

	return 0;
}

