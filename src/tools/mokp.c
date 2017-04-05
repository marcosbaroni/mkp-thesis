#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "../utils/util.h"
//#include "../models/mkp/mkp.h"
#include "../models/mokp/mokp.h"
#include "../models/mokp/order.h"
#include "../models/mokp/bazgan.h"

#define DYNPROG_OPT "dynprog"
#define RAND_OPT "rand"
#define MKP2_OPT "mkp2"
#define BAZGAN_OPT "bazgan"
#define BATCH_OPT "batch"

int print_usage_batch(int argc, char **argv){
    printf("Batch test for the Bazgan MOKP algorithm using KDTree.\n\n");
    printf("   Uses random instance generation.\n\n");
    printf("  usage: %s %s <n> <np> [seed] [times=10] [DKdim=3] [print_summary=0] [ordering=0]\n", argv[0], BAZGAN_OPT);
    printf("\n  Output: list of results\n");
    printf("    <n nodes>;<n comparison>;<time (s)>\n\n");

    return 1;
}

int execute_batch(int argc, char **argv){
    int n, np, ndim, i, seed, times, print_summary;
    int *inds;
    MOKP *mokp;
    struct timeval timeval_seeder;
    Bazgan *bazgan;
    char ordering_type;

    if( argc < 4 )
        return print_usage_batch(argc, argv);

    n = atoll(argv[2]);
    np = atoll(argv[3]);

    /* setting random seed */
    seed = 0;
    if( argc > 4 )
        seed = atoll(argv[4]);
    if(!seed){
        gettimeofday(&timeval_seeder, NULL);
        seed = timeval_seeder.tv_sec*1000;
        seed += timeval_seeder.tv_usec;
    }
    srand(seed);

    /* setting times */
    times = 10;
    if( argc > 5 )
        times =  atoll(argv[5]);
    if( !times )
        times = 10;

    /* setting kdtree dimension */
    ndim = 3;
    if( argc > 6 )
        ndim = atoll(argv[6]);

    /* setting print avg */
    print_summary = 0;
    if( argc > 7 )
        print_summary = 1 && atoll(argv[7]);

    ordering_type = 0;
    if( argc > 8 )
        ordering_type = argv[8][0];
    switch( ordering_type ){
        case 'm':
        case 'M':
        case 's':
            break;

        default:
            ordering_type = 0;
    }

    for( i = 0 ; i < times ; i++ ){
        mokp = mokp_random(n, np, 1);

        /* Reordering MOKP indexes */
        if( ordering_type )
            mokp = mokp_reord_by_type(mokp, ordering_type);

        /* Execute */
        bazgan = bazgan_exec(mokp, mokp->n, ndim);

        /* Outputing */
        if( !print_summary )
            printf("%d;%lld;%.3lf;%d\n",
                bazgan->avl_lex->n,
                bazgan->_ncomparison,
                bazgan_get_seconds(bazgan),
                bazgan->max_nd);

        /* Free */
        bazgan_free(bazgan);
        mokp_free(mokp);
    }

    return 0;
}

int print_usage_bazgan(int argc, char **argv){
    printf("Solve Multiobjective Knapsack Problem using Bazgan 2009 Algorithm.\n\n");
    printf("  usage: %s %s <ndim> [input file] [n iterations] [order option]\n", argv[0], BAZGAN_OPT);
    printf("  ...\n");
    printf("\n  Output:\n");
    printf("    <n nodes>;<n comparison>;<time (s)>\n\n");

    return 1;
}

int execute_bazgan(int argc, char **argv){
    int ndim, kmax, use_kdtree;
    FILE *finput;
    char order_opt;
    MOKP *mokp;
    List *list;
    Bazgan *bazgan;

    finput = stdin;
    order_opt = 's';
    if( argc < 3 )
        return print_usage_bazgan(argc, argv);

    ndim = atoll(argv[2]);
    if( argc > 3 )
        finput = fopen(argv[3], "r");

    if( argc > 5 )
        order_opt = argv[5][0];

    mokp = mokp_read(finput);
    kmax = mokp->n;
    if( argc > 4 )
        kmax = atoll(argv[4]);

    if( kmax < 0 || kmax > mokp->n ){
        kmax = mokp->n;
        fprintf(stderr, "Error: inconsistent kmax value. kmax=%d will be considered.\n", mokp->n);
    }

    use_kdtree = 0;

    /* Execute Bazgan */
    bazgan = bazgan_exec(mokp, kmax, ndim);
    //bazgan_exec_simple(mokp, kmax);

    /* Outputing */
    printf("%d;%lld;%.3lf;%d\n",
        bazgan->avl_lex->n,
        bazgan->_ncomparison,
        bazgan_get_seconds(bazgan),
        bazgan->max_nd);

    /* Free */
    bazgan_free(bazgan);
    mokp_free(mokp);
    fclose(finput);

    return 0;
}

void print_usage_mkp2(int argc, char **argv){
    printf("Convert an Multidimensional Knaspack Problema Instance to a MOKP\n");
    printf("usage: %s %s [input file] [output file]\n", argv[0], MKP2_OPT);
}

/*******************************************************************************
*    Conversor de instâncias MKP para MOKP
*******************************************************************************/
int execute_mkp2(int argc, char **argv){
    FILE *input, *output;
    //MKP *mkp;
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

    //mkp = mkp_read_from_file(input);
    //mokp = mokp_from_mkp(mkp);
    //mokp_write(output, mokp);

    //mkp_free(mkp);
    mokp_free(mokp);

    return 0;
}


/*******************************************************************************
*    Gerador de instâncias MOKP sintéticas (aleatórias).
*******************************************************************************/
int execute_rand(int argc, char **argv){
    int n, np, classe;
    unsigned int seed;
    MOKP *mokp;
    struct timeval timeval_seeder;

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
    if(!seed){
        gettimeofday(&timeval_seeder, NULL);
        seed = timeval_seeder.tv_sec*1000;
        seed += timeval_seeder.tv_usec;
    }
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
    printf("  usage: %s %s [input file] [n iterations]\n", argv[0], DYNPROG_OPT);
    printf("    input file: - to read from stdin\n");
    printf("\n  Output:\n");
    printf("    <n nodes>;<n comparison>;<time (s)>\n\n");
}

/*******************************************************************************
*    Algoritmo de programação dinâmica para solução do MOKP.
*    Implementado com base no Bazgan2009
*******************************************************************************/
int execute_dynprog(int argc, char **argv){
    FILE *input;
    Bazgan *bazgan;
    MOKP *mokp;
	clock_t c0, cf;
    int k, i, n, n_nodes;
    long long n_comps;
    double exec_time;

    input = stdout;

    if( argc < 3 ){
        print_usage_dynprog(argc, argv);
        return 1;
    }

    /**********************************************************
    *    READING INPUTS
    **********************************************************/
    /* Opening input file */
    input = stdin;
    if( argc > 2 ){
        if( strcmp(argv[2], "-") )
            input = fopen(argv[2], "r");
        if(!input){
            fprintf(stderr, "could not open file %s.\n", argv[3]);
            return 1;
        }
    }

    /* Reading instance */
    mokp = mokp_read(input);
    fclose(input);
    n = mokp->n;

    /* Checking k (n. of iterations) consistency */
    k = n;
    if( argc > 3 ){
        k = atoll(argv[3]);
        if( k < 2 || k > n){
            k = n;
            fprintf(stderr, "invalid number of iterations. %d will be used.\n", k);
        }
    }

    /**********************************************************
    *   EXECUTING ALGORITHM
    **********************************************************/
	c0 = clock();
    bazgan = bazgan_brute(mokp, k);
    exec_time = (clock()-c0)*1./CLOCKS_PER_SEC;

    /* Output result */
    n_comps = 0;
    printf("%d;%lld;%.3f;%d\n",
        bazgan->avl_lex->n, n_comps, exec_time, bazgan->max_nd);

    /* Freeing variables */
    bazgan_fprint_nodes(stdout, bazgan);
    bazgan_free(bazgan);
    //mokp_free(mokp);

    return 0;
}


/*******************************************************************************
*  Imprime uso da ferramenta MOKP
*******************************************************************************/
void print_usage(int argc, char **argv){
    printf("usage: %s <option> [args...]\n", argv[0]);
    printf("Multiobjective Knapsack Problem analysis tool.\n\n");
    printf("Please check option list below:\n");
    printf("  %s\tGenerate a random MOKP instance\n", RAND_OPT);
    printf("  %s\tSolve a MOKP using simple dynamic programming\n", DYNPROG_OPT);
    printf("  %s\tSolve a MOKP using Bazgan2009 algorithm\n", BAZGAN_OPT);
    printf("  %s\tBatch test of bazgan MOKP algorithm.\n", BATCH_OPT);
    printf("  %s\tConvert a MKP instance in MOKP instance\n", MKP2_OPT);

    return;
}

/* Função principal da utilidade MOKP. */
int main(int argc, char **argv){
    char *option;
    int ret, emptyret;

    ret = emptyret = -9989;

    /* ao menos 2 argumentos */
    if(argc < 2){
        print_usage(argc, argv);
        exit(1);
    }

    /* Opcao desejada */
    option = argv[1];

    /***************************************************************
    *        EXECUÇÃO DOS ALGORITMOS
    ***************************************************************/
    /*** Geração de instâncias   ***/
    if(!strcmp(option, RAND_OPT))
        ret = execute_rand(argc, argv);

    /***   Algoritmo de Prog. Dinamica   ***/
    if(!strcmp(option, DYNPROG_OPT))
        ret = execute_dynprog(argc, argv);

    /***   Conversão de instâncias MKP para MOKP   ***/
    if(!strcmp(option, MKP2_OPT))
        ret = execute_mkp2(argc, argv);

    /***   Execução do algoritmo de Bazgan   ***/
    if(!strcmp(option, BAZGAN_OPT))
        ret = execute_bazgan(argc, argv);

    /***   Execução Batch do algoritmo Bazgan ***/
    if(!strcmp(option, BATCH_OPT))
        ret = execute_batch(argc, argv);

    /***************************************************************
    *    IMPRESSÃO DA FORMA DE USO
    ***************************************************************/
    if( ret == emptyret )
        print_usage(argc, argv);

    return ret;
}

