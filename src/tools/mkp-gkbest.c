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
#include <string.h>
#include "mmkp/mkp/mkp.h"


int main(int argc, char **argv){
    int best[11] = {3766, 3958, 5656, 5767, 7558, 7678, 19218, 18799, 58986, 57285, 95219};
    int ns[11] = { 100, 100, 150, 150, 200, 200, 500, 500, 1500, 1500, 2500};
    int ms[11] = { 15, 25, 25, 50, 25, 50, 25, 50, 25, 50, 100};

    int i;

    if (argc < 2 ){
        printf("usage: %s <i>\n", argv[0]);
        printf("Returns the best known value for the respective GK mkp instance and its dimenstion (n, m).\n");
        printf("   format \"<best>;<n>;<m>\"\n");
        return 1;
    }
    i = atoll(argv[1]);
    i--;

    if ( i < 0 || i > 10){
        fprintf(stderr, "Wrong value of 'i'. Please enter between 1 and 11.\n");
        return 1;
    }

    /* Printing. */
    fprintf(stdout, "%d;%d;%d", best[i], ns[i], ms[i]);

    return 0;
}

