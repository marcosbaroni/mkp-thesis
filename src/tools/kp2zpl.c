/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: december, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmkp/kp.h"
#include "mmkp/util.h"

int main(int argc, char **argv){
	KP *kp;
	FILE *input;

	input = stdin;
	if( argc > 1) 
		input = fopen(argv[1], "r");

	kp = kp_read(input);
	fclose(input);
	kp_to_zimpl(stdout, kp);
	kp_free(kp);

	return 0;
}

