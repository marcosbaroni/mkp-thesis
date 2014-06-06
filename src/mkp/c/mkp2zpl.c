#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mkp.h"
#include "util.h"

int main(){
	MKP *mkp;
	mkp = mkp_read_from_file(stdin);
	mkp_to_zimpl(mkp, stdout);
	mkp_free(mkp);

	return 0;
}

