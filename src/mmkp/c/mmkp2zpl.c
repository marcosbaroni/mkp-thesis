#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/mmkp.h"
#include "mmkp/util.h"

int main(){
	MMKP *mmkp;
	mmkp = mmkp_read_from_file(stdin);
	mmkp_to_zimpl(stdout, mmkp);
	mmkp_free(mmkp);

	return 1;
}

