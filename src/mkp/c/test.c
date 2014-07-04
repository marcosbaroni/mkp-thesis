#include <stdio.h>
#include <stdlib.h>
#include "mmkp.h"
#include "util.h"

int main(int argc, char **argv){
	NUMBER **mat;
	mat = read_NUMBER_matrix(2, 2);
	fprintf_NUMBER_matrix(stdout, mat, 2, 2);
	free_NUMBER_matrix(mat, 2);
	return 0;
}

