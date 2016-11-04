#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define DYNPROG_OPT "dynprog"
#define RAND_OPT "rand"
#define MKP2_OPT "mkp2"

#include "../utils/util.h"
#include "../models/mkp/mkp.h"
#include "../models/mokp/mokp.h"

void print_usage_mkp2(int argc, char **argv){
    printf("Convert an Multidimensional Knaspack Problema Instance to a MOKP\n");
    printf("usage: %s %s [input file] [output file]\n", argv[0], MKP2_OPT);
}
int execute_mkp2(int argc, char **argv){
    FILE *input, *output;
    MKP *mkp;
    MOKP *mokp;

    input = stdin;
    output = stdout;

    if( argc > 2 ){
        input = fopen(argv[2], "r");
        if( !input ){
            fprintf(stderr, "Could not open file \"%s\".\n", argv[2]);
            return 1;
        }
    }

    if( argc > 3 ){
        output = fopen(argv[3], "w");
        if( !output ){
            fprintf(stderr, "Could not open file %s.\n", argv[3]);
            return 1;
        }
    }

    mkp = mkp_read_from_file(input);
    mokp = mokp_from_mkp(mkp);
    mokp_write(output, mokp);

    mkp_free(mkp);
    mokp_free(mokp);

    return 0;
}

int execute_rand(int argc, char **argv){
    int n, np, classe;
    unsigned int seed;
    MOKP *mokp;

    if( argc < 4 ){
        printf("usage: %s %s <n> <np> [classe] [seed] [outfile] \n", argv[0], RAND_OPT);
        printf("\nclasse:\n");
        printf("   1: uniform [1,1000]   (default)\n");
        printf("   2: unconflicting [111,1000], [p-100, p+100]\n");
        printf("   3: profits negatively-correlated, weight uniform\n");
        printf("   4: profits negatively-correlated, weight positively-correlated with profits\n");
        printf("\n");
        return 1;
    }

    classe = 1;
    n = atoll(argv[2]);
    np = atoll(argv[3]);
    if( argc > 4 )
        classe = atoll(argv[4]);

    /* checking classe consistency */
    if( !classe || classe > 4 ){
        fprintf(stderr, "Invalid classe %d. classe 1 will be used.\n", classe);
        classe = 1;
    }

    /* setting random seed */
    seed = 0;
    if( argc > 5 )
        seed = atoll(argv[5]);
    if(!seed)
    seed = time(NULL);
    srand(seed);

    /* setting output file */
    mokp = mokp_random(n, np, classe);
    if( argc > 6 ){
        if(!strcmp(argv[6], "-"))
            mokp_write(stdout, mokp);
        else
            mokp_save(argv[6], mokp);
    }else{
        mokp_write(stdout, mokp);
    }

    mokp_free(mokp);

    return 0;
}

void print_usage_dynprog(int argc, char **argv){
    printf("Solve Multiobjective Knapsack Problem using Dynamic Programming.\n\n");
    printf("usage: %s %s <ndim> [input file] [n iterations]\n", argv[0], DYNPROG_OPT);
    printf("  ndim: number of dimensions indexed by kdtree (0 will not use it)\n");
    printf("\nOutput:\n");
    printf("  <n nodes>;<n comparison>;<time (s)>\n");
}

int execute_dynprog(int argc, char **argv){
    FILE *input;
    MOKP *mokp;
    int ndim, use_kdtree, *idxs;
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
    ndim = atoll(argv[2]);

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

    /* checking indexed dimensions consistency */
    if( ndim > mokp->np + 1 ){
        fprintf(stderr, "Number od indexing dimensions exceeds problems dimensions. ndim = %d will be used.\n", mokp->np+1);
        ndim = mokp->np + 1;
    }

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
    n_nodes = mokp_dynprog(mokp, ndim, k, idxs, &n_comps);
    exec_time = (clock()-c0)*1./CLOCKS_PER_SEC;

    /* output result */
    printf("%d;%lld;%.3f\n", n_nodes, n_comps, exec_time);

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
    printf("  %s\tConvert a MKP instnace in MOKP instance\n", MKP2_OPT);

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
    if(!strcmp(option, MKP2_OPT))
        return execute_mkp2(argc, argv);

    print_usage(argc, argv);

    return 0;
}

