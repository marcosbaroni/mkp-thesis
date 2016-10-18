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

void print_usage_dynprog(int argc, char **argv){
    printf("usage: %s dynprog <option> [input file]\n", argv[0]);
    printf("Solve Multiobjective Knapsack Problem using Dynamic Programming.\n\n");
    printf("Please check option list below:\n");
    printf("  1\tusing plain list\n");
    printf("  2\tusing KD-tree\n");
}

int execute_dynprog(int argc, char **argv){
    FILE *input;
    int option, use_kdtree;
	clock_t c0, cf;
    double exec_time;
    MOKP *mokp;

    input = stdout;

    if( argc < 3 ){
        print_usage_dynprog(argc, argv);
        return 1;
    }

    option = atoll(argv[2]);

    switch(option){
        case 1:
            break;
        case 2:
            use_kdtree = 1;
            break;
    }

	c0 = clock();
    //mkp_balev(mkpsol, use_lb);
    exec_time = (clock()-c0)*1./CLOCKS_PER_SEC;

    return 0;
}

void print_usage(int argc, char **argv){
    printf("usage: %s <option> [args...]\n", argv[0]);
    printf("Multiobjective Knapsack Problem analysis tool.\n\n");
    printf("Please check option list below:\n");
    printf("  rand\tGenerate a random MOKP instance\n");
    printf("  rand\tSolve a MOKP using dynamic programming\n");

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
    if(!strcmp(option, "dynprog"))
        return execute_dynprog(argc, argv);

    print_usage(argc, argv);

    return 0;
}

