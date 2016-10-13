#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "../utils/util.h"
#include "../models/mokp/mokp.h"

int execute_rand(int argc, char **argv){
    int n, np;
    MOKP *mokp;

    if( argc < 4 ){
        printf("usage: %s rand <n> <np> [outfile]\n", argv[1]);
        return 1;
    }

    n = atoll(argv[2]);
    np = atoll(argv[3]);

    mokp = mokp_random(n, np);
    if( argc > 4 ){
        if(!strcmp(argv[4], "-"))
            mokp_write(stdout, mokp);
        else
            mokp_save(argv[4], mokp);
    }
    mokp_write(stdout, mokp);
    mokp_free(mokp);

    return 0;
}

void print_usage(int argc, char **argv){
    printf("Multiobjective Knapsack Problem analysis tool\n\n");
    printf("  usage: %s <option> [args...]\n\n", argv[0]);
    printf("Please check option list below:\n");
    printf("  rand\tGenerate a random MOKP instance");
    printf("\n");

    return;
}

int main(int argc, char **argv){
    char *option;
    if(argc < 2){
        print_usage(argc, argv);
        exit(1);
    }

    option = argv[1];

    if(!strcmp(option, "rand"))
        return execute_rand(argc, argv);

    print_usage(argc, argv);

    return 0;
}

