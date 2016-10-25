#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define DYNPROG_OPT "dp"
#define RAND_OPT "rand"

#include "../utils/util.h"
#include "../models/mokp/mokp.h"

int execute_rand(int argc, char **argv){
    int n, np;
    unsigned int seed;
    MOKP *mokp;

    if( argc < 4 ){
        printf("usage: %s %s <n> <np> [seed] [outfile]\n", argv[1], RAND_OPT);
        return 1;
    }

    n = atoll(argv[2]);
    np = atoll(argv[3]);

    /* setting random seed */
    seed = 0;
    if( argc > 4 )
        seed = atoll(argv[4]);
    if(!seed)
    seed = time(NULL);
    srand(seed);

    /* setting output file */
    mokp = mokp_random(n, np);
    if( argc > 5 ){
        if(!strcmp(argv[5], "-"))
            mokp_write(stdout, mokp);
        else
            mokp_save(argv[5], mokp);
    }else{
        mokp_write(stdout, mokp);
    }

    mokp_free(mokp);

    return 0;
}

void print_usage_dynprog(int argc, char **argv){
    printf("usage: %s %s <option> [input file] [n iterations]\n", argv[0], DYNPROG_OPT);
    printf("Solve Multiobjective Knapsack Problem using Dynamic Programming.\n\n");
    printf("Please check option list below:\n");
    printf("  1\tusing plain list\n");
    printf("  2\tusing KD-tree\n");
}

int execute_dynprog(int argc, char **argv){
    FILE *input;
    MOKP *mokp;
    int option, use_kdtree, *idxs;
	clock_t c0, cf;
    int k, i, n, n_nodes;
    long long n_comps;
    double exec_time;

    input = stdout;

    if( argc < 3 ){
        print_usage_dynprog(argc, argv);
        return 1;
    }

    /* algorithm */
    option = atoll(argv[2]);
    use_kdtree = 0;
    switch(option){
        case 1:
            break;
        case 2:
            use_kdtree = 1;
            break;
    }

    /* opening input file */
    input = stdin;
    if( argc > 3 ){
        if(strcmp(argv[3], "-") != 0 )
            input = fopen(argv[3], "r");
        if(!input){
            fprintf(stderr, "could not open file %s.\n", argv[3]);
            return 1;
        }
    }

    /* reading instance */
    mokp = mokp_read(input);
    fclose(input);
    n = mokp->n;

    /* getting k (n. of iterations) */
    k = n;
    if( argc > 4 ){
        k = atoll(argv[4]);
        if( k < 2 || k > n){
            k = n;
            fprintf(stderr, "invalid number of iterations. %d will be used.\n", k);
        }
    }

    /* build indexs array */
    idxs = (int*)malloc(n*sizeof(int));
    for( i = 0 ; i < n ; i++ )
        idxs[i] = i;

    /* executing algorithm */
	c0 = clock();
    n_nodes = mokp_dynprog(mokp, use_kdtree, k, idxs, &n_comps);
    exec_time = (clock()-c0)*1./CLOCKS_PER_SEC;

    /* output result */
    printf("%d:%.1e:%.3f\n", n_nodes, (double)n_comps, exec_time);

    /* freeing variables */
    free(idxs);
    mokp_free(mokp);

    return 0;
}

void print_usage(int argc, char **argv){
    printf("usage: %s <option> [args...]\n", argv[0]);
    printf("Multiobjective Knapsack Problem analysis tool.\n\n");
    printf("Please check option list below:\n");
    printf("  %s\tGenerate a random MOKP instance\n", RAND_OPT);
    printf("  %s\tSolve a MOKP using dynamic programming\n", DYNPROG_OPT);

    return;
}

int main(int argc, char **argv){
    char *option;
    if(argc < 2){
        print_usage(argc, argv);
        exit(1);
    }

    option = argv[1];

    if(!strcmp(option, RAND_OPT))
        return execute_rand(argc, argv);
    if(!strcmp(option, DYNPROG_OPT))
        return execute_dynprog(argc, argv);

    print_usage(argc, argv);

    return 0;
}

