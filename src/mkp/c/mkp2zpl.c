#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp.h"
#include "util.h"

int main(){
	MMKP *mmkp;
	mmkp = mmkp_read_from_file(stdin);
	//mmkp_to_zimpl(mmkp, stdout);
	fprintf(stderr, "not working yet\n");
	mmkp_free(mmkp);

	return 1;
}

