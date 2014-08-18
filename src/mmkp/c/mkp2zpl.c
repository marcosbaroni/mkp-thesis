#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/mkp.h"
#include "mmkp/util.h"

int main(){
	MKP *mkp;
	mkp = mkp_read_from_file(stdin);
	mkp_to_zimpl(stdout, mkp);
	mkp_free(mkp);

	return 1;
}

