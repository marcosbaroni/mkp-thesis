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
#include "../models/mkp/mkp.h"
#include "../utils/util.h"

void print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [options] <input file>\n\n", argv[0]);
	fprintf(out, " Convert a MKP instance to ZIMPL format.\n");
	fprintf(out, " INPUT: a MKP instance file.\n\tIf \"-\" is given, instance is read from stdin.\n");
	fprintf(out, " OPTIONS:\n");
	fprintf(out, " \t-l: convert to LP relaxation.\n");
	fprintf(out, " \t-d: convert to LP relaxation for dual problem.\n");
	fprintf(out, " \t-s <ratio>: scale capacities of original problem.\n");
	fprintf(out, " \t-m <opt>: max objective function allowed.\n");

	return;
}

int main(int argc, char **argv){
	MKP *mkp;
	FILE *input;
	int i;
	char linear;
    char dual;
	double scale, max_opt;

	linear = 0;
    dual = 0;
	scale = 1.0;
	input = stdin;
	max_opt = 1.e100;
	if( argc < 2){
		print_usage(argc, argv);
		return(1);
	}

	for( i = 1 ; i < argc ; i++ ){
		if(argv[i][0] == '-'){
			switch(argv[i][1]){
				case 'l': linear = 1; break;
				case 'd': linear = 1; dual = 1; break;
				case 's': scale = atof(argv[++i]); break;
				case 'm': max_opt = atof(argv[++i]); break;
				case '\0': input = stdin; i = argc; break;
				default: print_usage(argc, argv); return 1;
			}
		}else{
			input = fopen(argv[i], "r");
			i = argc;
		}
	}

	mkp = mkp_read_from_file(input);
    if(!dual)
	    mkp_to_zimpl(stdout, mkp, max_opt, scale, linear);
    else
        mkp_dual_to_zimpl(stdout, mkp, 1);

	mkp_free(mkp);

	return 0;
}

