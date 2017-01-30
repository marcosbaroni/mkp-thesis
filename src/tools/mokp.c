#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "../utils/util.h"
#include "../models/mkp/mkp.h"
#include "../models/mokp/mokp.h"
#include "../models/mokp/order.h"
#include "../models/mokp/bazgan.h"

#define DYNPROG_OPT "dynprog"
#define RAND_OPT "rand"
#define MKP2_OPT "mkp2"
#define BAZGAN_OPT "bazgan"

int print_usage_bazgan(int argc, char **argv){
    printf("Solve Multiobjective Knapsack Problem using Bazgan 2009 Algorithm.\n\n");
    printf("  usage: %s %s <ndim> [input file] [n iterations] [order option]\n", argv[0], DYNPROG_OPT);
    printf("  ...\n");
    printf("\n  Output:\n");
    printf("    <n nodes>;<n comparison>;<time (s)>\n\n");

    return 1;
}

int execute_bazgan(int argc, char **argv){
    int ndim, kmax;
    FILE *finput;
    char order_opt;
    MOKP *mokp;
    List *list;

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

    /* Execute Bazgan */
    //bazgan_exec(mokp, order_opt, kmax);
    bazgan_exec_simple(mokp);

    /* free */
    mokp_free(mokp);

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


/*******************************************************************************
*    Gerador de instâncias MOKP sintéticas (aleatórias).
*******************************************************************************/
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
    printf("  usage: %s %s <ndim> [input file] [n iterations] [order option]\n", argv[0], DYNPROG_OPT);
    printf("    ndim: number of dimensions indexed by kdtree (0 will not use it)\n");
    printf("    input file: - to read from stdin\n");
    printf("    order options:\n");
    printf("      0: given item order is used (default)\n");
    printf("      M: ordering according to max item rank\n");
    printf("      m: ordering according to min item rank\n");
    printf("      s: ordering according to total sum of item rank\n");
    printf("\n  Output:\n");
    printf("    <n nodes>;<n comparison>;<time (s)>\n\n");
}

/*******************************************************************************
*    Algoritmo de programação dinâmica para solução do MOKP.
*    Implementado com base no Bazgan2009
*******************************************************************************/
int execute_dynprog(int argc, char **argv){
    FILE *input;
    MOKP *mokp;
    int ndim, use_kdtree, *idxs;
    char order_opt;
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
    /* Número de dimensões utilizadas na KD-tree. */
    ndim = atoll(argv[2]);
    /* ordenação default */
    order_opt = '0';
    /* Opening input file */
    input = stdin;
    if( argc > 3 ){
        if(strcmp(argv[3], "-") != 0 )
            input = fopen(argv[3], "r");
        if(!input){
            fprintf(stderr, "could not open file %s.\n", argv[3]);
            return 1;
        }
    }

    /* Reading instance */
    mokp = mokp_read(input);
    fclose(input);
    n = mokp->n;

    /* Checking indexed dimensions consistency */
    if( ndim > mokp->np + 1 ){
        fprintf(stderr, "Number od indexing dimensions exceeds problems dimensions. ndim = %d will be used.\n", mokp->np+1);
        ndim = mokp->np + 1;
    }

    /* Checking k (n. of iterations) consistency */
    k = n;
    if( argc > 4 ){
        k = atoll(argv[4]);
        if( k < 2 || k > n){
            k = n;
            fprintf(stderr, "invalid number of iterations. %d will be used.\n", k);
        }
    }

    /* opção de ordenação de itens */
    if( argc > 5 ){
        if( strlen(argv[5]) > 1 ){
            fprintf(stderr, "invalid argument \"%s\" as ordering option. \"%c\" (default) will be used.", order_opt);
        }else{
            order_opt = argv[5][0];
            switch( order_opt ){
                case 'm':
                case 'M':
                case 's':
                break;
                default:
                order_opt = 's';
                fprintf(stderr, "invalid argument \"%s\" as ordering option. \"%c\" (default) will be used.", argv[5][0], order_opt);
            }
        }
    }

    /**********************************************************
    *    PRE-PROCESSING
    **********************************************************/
    // TODO: ordenar indices de inserção segundo proposta do Bazgan
    idxs = mokp_get_order(mokp, order_opt);

    /**********************************************************
    *   EXECUTING ALGORITHM
    **********************************************************/
	c0 = clock();
    n_nodes = mokp_dynprog(mokp, ndim, k, idxs, &n_comps);
    exec_time = (clock()-c0)*1./CLOCKS_PER_SEC;

    /* Output result */
    printf("%d;%lld;%.3f\n", n_nodes, n_comps, exec_time);

    /* Freeing variables */
    free(idxs);
    mokp_free(mokp);

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
    printf("  %s\tConvert a MKP instance in MOKP instance\n", MKP2_OPT);

    return;
}

/* Função principal da utilidade MOKP. */
int main(int argc, char **argv){
    char *option;

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
        return execute_rand(argc, argv);

    /***   Algoritmo de Prog. Dinamica   ***/
    if(!strcmp(option, DYNPROG_OPT))
        return execute_dynprog(argc, argv);

    /***   Conversão de instâncias MKP para MOKP   ***/
    if(!strcmp(option, MKP2_OPT))
        return execute_mkp2(argc, argv);

    /***   Execução do algoritmo de Bazgan   ***/
    if(!strcmp(option, BAZGAN_OPT))
        return execute_bazgan(argc, argv);

    /***************************************************************
    *    IMPRESSÃO DA FORMA DE USO
    ***************************************************************/
    print_usage(argc, argv);

    return 0;
}

