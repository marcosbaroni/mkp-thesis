/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: november, 2016
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
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
#include "../models/mokp/mokp-mh.h"
#include "../metahrs/sfl.h"

#ifdef COUNT_COMPARISON
unsigned long long ncomp_ = 0;
#endif

#define DYNPROG_OPT "dynprog"
#define RAND_OPT    "rand"
#define MKP2_OPT    "mkp2"
#define BAZGAN_OPT  "bazgan"
#define BATCH_OPT   "batch"
#define CONVERT_OPT "convert"
#define SCE_OPT     "sce"
#define MOFPA_OPT   "mofpa"
#define HVOL_OPT    "hvol"

void auto_seed(unsigned int seed){
    struct timeval timeval_seeder;
    if( !seed ){
        gettimeofday(&timeval_seeder, NULL);
        seed = timeval_seeder.tv_sec*1000;
        seed += timeval_seeder.tv_usec;
    }
    srand(seed);
	return;
}

int print_usage_convert(int argc, char **argv){
    printf("Convert a MOKP instance.\n\n");
    printf("  usage: %s %s <in format> <out format> <input-file> [output-file]\n", argv[0], CONVERT_OPT);
    printf("  - in/out formats are:\n");
    printf("      d: default\n");
    printf("      b: bazgan\n");
    printf("      z: zitzler\n");
    printf("  - input-file: '-' for stdin\n");
    printf("  - output-file: if not given, uses stdout\n");

    return 1;
}

int execute_convert(int argc, char **argv){
    MOKP *mokp = NULL;
	char in_format, out_format;
    FILE *fin, *fout;

    if( argc < 5 )
        return print_usage_convert(argc, argv);

	/* Reading inputs */
	in_format = argv[2][0];
	out_format = argv[3][0];

    fin = stdin;
    if( strcmp(argv[4], "-") )
        fin = fopen(argv[4], "r");

    fout = stdout;
    if( argc > 5 )
        fout = fopen(argv[3], "w");

	/* Reading instance */
	switch( in_format ){
		case 'd':
			mokp = mokp_read(fin);
			break;
		case 'b':
			mokp = mokp_read_bazgan(fin);
			break;
		case 'z':
			/* TODO: todo? */
		default:
			fprintf(stderr, "input format not implemented.\n");
			exit(1);
	}

	/* Writing instance */
	switch( out_format ){
		case 'd':
			mokp_write(fout, mokp);
			break;
		case 'z':
			mokp_write_zitzler(fout, mokp);
			break;
		case 'b':
		default:
			mokp_write_bazgan(fout, mokp);
			exit(1);
	}
    fclose(fin);
    fclose(fout);

    mokp_free(mokp);

    return 0;
}

int print_usage_batch(int argc, char **argv){
    printf("Batch test for the Bazgan MOKP algorithm using KDTree.\n\n");
    printf("   Uses random instance generation.\n\n");
    printf("  usage: %s %s <n> <np> [seed] [times=10] [DKdim=3] [print_summary=0] [ordering=0] [algs=[0,3,10]]\n", argv[0], BAZGAN_OPT);
    printf("\n  Output: list of results\n");
    printf("    <n nodes>;<n comparison>;<time (s)>\n\n");

    return 1;
}

struct BatchSummary{
    double time, max_nd, nd, comps;
    int n;
};
void bsummary_init(struct BatchSummary *bsum){
    bsum->time = bsum->max_nd = bsum->nd = bsum->comps = 0.0;
    bsum->n = 0;
}
void bsummary_accum(struct BatchSummary *bsum, Bazgan *bazgan){
    bsum->time += bazgan_get_seconds(bazgan);
    bsum->max_nd += (double)bazgan->max_nd;
    bsum->nd += (double)bazgan->avl_lex->n;
    bsum->comps += (double)bazgan->_ncomparison;
    bsum->n++;
}
void bsummary_print(struct BatchSummary *bsum){
    int n;
    printf("%.3lf;%.3lf;%.3lf;%.3lf",
        bsum->nd / (double)bsum->n,
        bsum->comps / (double)bsum->n,
        bsum->time / (double)bsum->n,
        bsum->max_nd / (double)bsum->n);
}

int execute_batch(int argc, char **argv){
    int n, np, ndim, i, seed, times, print_summary;
    int *inds;
    MOKP *mokp;
    Bazgan *bazgan;
    char ordering_type;
    struct BatchSummary bsummaries[3];
    char exec_list, exec_brute, exec_kd;
    int *execs, nexecs;

    bsummary_init(&(bsummaries[0]));
    bsummary_init(&(bsummaries[1]));
    bsummary_init(&(bsummaries[2]));

    if( argc < 4 )
        return print_usage_batch(argc, argv);

    n = atoll(argv[2]);
    np = atoll(argv[3]);

    /* setting random seed */
    seed = 0;
    if( argc > 4 )
        seed = atoll(argv[4]);
	auto_seed(seed);

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

    ordering_type = 'M';
    if( argc > 8 )
        ordering_type = argv[8][0];
    switch( ordering_type ){
        case 'm':
        case 'M':
        case 's':
            break;

        default:
            ordering_type = 'M';
    }

    exec_list = exec_brute = exec_kd = 1;
    if( argc > 9 ){
        exec_list = exec_brute = exec_kd = 0;
        execs = parse_int_list(argv[9], &nexecs);
        for( i = 0 ; i < nexecs ; i++ ){
            if( execs[i] > 9 )
                exec_brute = 1;
            else if( execs[i] == 0 )
                exec_list = 1;
            else if( execs[i] > 0 )
                exec_kd = execs[i];
        }
        free(execs);
    }

    for( i = 0 ; i < times ; i++ ){
        mokp = mokp_random(n, np, 1);

        /* Reordering MOKP indexes */
        if( ordering_type )
            mokp = mokp_reord_by_type(mokp, ordering_type);

        /*** KDTREE ***/
        if( exec_kd ){
            bazgan = bazgan_exec(mokp, mokp->n, exec_kd);
            if( !print_summary ){
                printf("%d;KD;", i+1);
                bazgan_print_summary(bazgan);
                printf("\n");
                bsummary_accum(&(bsummaries[0]), bazgan);
            }
            bazgan_free(bazgan);
        }

        /*** LIST ***/
        if( exec_list ){
            bazgan = bazgan_exec(mokp, mokp->n, 0);
            if( !print_summary ){
                printf("%d;LS;", i+1);
                bazgan_print_summary(bazgan);
                printf("\n");
                bsummary_accum(&(bsummaries[1]), bazgan);
            }
            bazgan_free(bazgan);
        }

        /*** BRUTE ***/
        if( exec_brute ){
            bazgan = bazgan_brute(mokp, mokp->n);
            if( !print_summary ){
                printf("%d;BR;", i+1);
                bazgan_print_summary(bazgan);
                printf("\n");
                bsummary_accum(&(bsummaries[2]), bazgan);
            }
            bazgan_free(bazgan);
        }
        
        /* Free */
        mokp_free(mokp);
        fflush(stdout);
    }

    /* OUTPUT TOTAL */
    if( exec_kd ){
        printf("KD;");
        bsummary_print(&(bsummaries[0]));
        printf("\n");
    }
    if( exec_list ){
        printf("LS;");
        bsummary_print(&(bsummaries[1]));
        printf("\n");
    }
    if( exec_brute ){
        printf("BR;");
        bsummary_print(&(bsummaries[2]));
        printf("\n");
    }

    return 0;
}

/*********************************************************
 *  Bazgan's algorithm
 ********************************************************/
MOKPSolIndexer *bazgan2msi(Bazgan *bazgan){
	BazganNode **nodes;
	MOKPSolIndexer *msi;
	MOKPSol *sol;
	int n, i, j, np;

	n = bazgan->avl_lex->n;
	np = bazgan->mokp->np;
	nodes = (BazganNode**)avl_to_array(bazgan->avl_lex);

	msi = msi_new(2);
	for( i = 0 ; i < n ; i++ ){
		sol = mokpsol_new_empty(bazgan->mokp);
		for ( j = 0 ; j < np ; j++ )
			sol->profit[j] = nodes[i]->profit[j];
		sol->b_left = nodes[i]->b_left;
		msi_insert(msi, sol);
	}
	free(nodes);
	return msi;
}
int print_usage_bazgan(int argc, char **argv){
    printf("Solve Multiobjective Knapsack Problem using Bazgan 2009 Algorithm.\n\n");
    printf("  usage: %s %s <ndim> [input file] [n iterations] [order option=s] [print pareto=0]\n", argv[0], BAZGAN_OPT);
    printf("  ...\n");
    printf("\n  Output:\n");
    printf("    <n nodes>;<n comparison>;<time (s)>;<max nd>;<hyper volume>\n\n");

    return 1;
}

int execute_bazgan(int argc, char **argv){
    int ndim, kmax, use_kdtree;
    FILE *finput;
    char order_opt;
    MOKP *mokp;
    List *list;
    Bazgan *bazgan;
	char print_pareto = 0;
	MOKPSolIndexer *msi;
	double hvol;

    finput = stdin;
    order_opt = 's';
    if( argc < 3 )
        return print_usage_bazgan(argc, argv);

    ndim = atoll(argv[2]);
    if( argc > 3 )
		if( strcmp("-", argv[3]) )
        	finput = fopen(argv[3], "r");

    if( argc > 5 )
        order_opt = argv[5][0];

    mokp = mokp_read(finput);
    kmax = mokp->n;
    if( argc > 4 )
        kmax = atoll(argv[4]);

    if( argc > 6 )
        print_pareto = atoll(argv[6]);

    if( kmax < 0 || kmax > mokp->n ){
        kmax = mokp->n;
        fprintf(stderr, "Error: inconsistent kmax value. kmax=%d will be considered.\n", mokp->n);
    }

    use_kdtree = 0;

    /* Execute Bazgan */
    bazgan = bazgan_exec(mokp, kmax, ndim);
    //bazgan_exec_simple(mokp, kmax);

    /* Outputing */
    //bazgan_print_nodes_lex(bazgan);
	msi = bazgan2msi(bazgan);
	hvol = msi_hvolume(msi);
	if( print_pareto )
		msi_apply_all(msi, mokpsol_profit_write);
	msi_apply_all(msi, mokpsol_free);
	msi_free(msi);
    bazgan_print_summary(bazgan);
	printf(";%.3e", hvol);
    printf("\n");

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
    int n, np;
	char classe;
    unsigned int seed;
    MOKP *mokp;

    if( argc < 4 ){
        printf("usage: %s %s <n> <np> [classe] [seed] [outfile] \n", argv[0], RAND_OPT);
        printf("\nclasse:\n");
        printf("   A: uniform [1,1000]   (default)\n");
        printf("   B: unconflicting [111,1000], [p-100, p+100]\n");
        printf("   C: profits negatively-correlated, weight uniform\n");
        printf("   D: profits negatively-correlated, weight positively-correlated with profits\n");
        printf("\n");
        return 1;
    }

    classe = 'A';
    n = atoll(argv[2]);
    np = atoll(argv[3]);
    if( argc > 4 )
        classe = argv[4][0];

    /* checking classe consistency */
    if( !((classe > 64 && classe < 69) || (classe > 96 && classe < 101)) ){
        fprintf(stderr, "Invalid classe %c. classe A will be used.\n", classe);
        classe = 'A';
    }

    /* setting random seed */
    seed = 0;
    if( argc > 5 )
        seed = atoll(argv[5]);
	auto_seed(seed);

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
*    Multi-Objective Shuffled Complex Evolution
*******************************************************************************/
int print_usage_sce(int argc, char **argv){
    printf("Solve Multiobjective Knapsack Problem using SCE.\n\n");
    printf("  usage: %s %s <input file> <ndim> [seed=0] [niter=300] [ncomp=20] [compsize=20] [nsubcomp=5] [nsubiter=20] [cross_ratio]\n", argv[0], SCE_OPT);
    printf("    input file: - to read from stdin\n");
    printf("\n  Output:\n");
    printf("    <time (s)>;<n sols>;<hypervolume>");
#ifdef COUNT_COMPARISON
    printf(";<n comparisons>");
#endif
    printf("\n\n");
}
int execute_sce(int argc, char **argv){
	MOKP *mokp;
	FILE *input;
	int i, ndim;
	int niter, ncomp, compsize, nsubcomp, nsubiter, seed, ncross;
	double cross_ratio;
	SFL_Interface *sfli;
	MOKPSol *sol;
	MOKPSolIndexer *sce, *baz;
	double secs;
    Bazgan *bazgan;

    input = stdin;
	niter = 300;
	ncomp = 20;
	compsize = 20;
	nsubcomp = 5;
	nsubiter = 20;
	ndim = 0;
	seed = 0;
	cross_ratio = 0.2;

	if( argc < 4 ){
		print_usage_sce(argc, argv);
		return 1;
	}

    if( argc > 2 ){
        if( strcmp(argv[2], "-") )
            input = fopen(argv[2], "r");
        if(!input){
            fprintf(stderr, "could not open file %s.\n", argv[2]);
            return 1;
        }
    }
	if( argc > 3 )
		ndim = atoll(argv[3]);
	if( argc > 4 )
		seed = atoll(argv[4]);
	if( argc > 5 )
		niter = atoll(argv[5]);
	if( argc > 6 )
		ncomp = atoll(argv[6]);
	if( argc > 7 )
		compsize = atoll(argv[7]);
	if( argc > 8 )
		nsubcomp = atoll(argv[8]);
	if( argc > 9 )
		nsubiter = atoll(argv[9]);
	if( argc > 10 )
		sscanf(argv[10], "%lf", &cross_ratio);

	auto_seed(seed);

	mokp = mokp_read(input);
	ncross = (int)floor(cross_ratio*mokp->n);

	if( ndim > mokp->np )
		ndim = mokp->np;

	sce = mokp_sce(mokp, ncomp, compsize, nsubcomp,
		niter, nsubiter, ndim, ncross, &secs);
	//bazgan = bazgan_exec(mokp, mokp->n, 2);
	//baz = bazgan2msi(bazgan);

	printf("%.3lf;%d;%.3e;", secs, msi_get_n(sce), msi_hvolume(sce));
	//printf("%.3lf;%.0lf;\n", bazgan_get_seconds(bazgan), msi_hvolume(baz));
#ifdef COUNT_COMPARISON
	printf("%lld;", ncomp_);
#endif
	printf("\n");

	msi_apply_all(sce, mokpsol_free);
	msi_free(sce);
	//msi_free(baz);
	//bazgan_free(bazgan);
	mokp_free(mokp);

	return 0;
}

/*******************************************************************************
*    Multi-Objective Firefly+Particle Swarm for MOKP
*    Implementado com base no Zouache2018
*******************************************************************************/
int print_usage_mofpa(int argc, char **argv){
    printf("Solve Multiobjective Knapsack Problem using MOFPA.\n\n");
    printf("  usage: %s %s [input file] [n iterations]\n", argv[0], MOFPA_OPT);
    printf("    input file: - to read from stdin\n");
    printf("\n  Output:\n");
    printf("    <n nodes>;<n comparison>;<time (s)>\n\n");
}

int execute_mofpa(int argc, char **argv){
	if( argc < 3 ){
		print_usage_mofpa(argc, argv);
		fprintf(stderr, "Not implemented yet.\n");
		return 1;
	}
	return 0;
}

/*******************************************************************************
*    Compute Hypervolume of pareto
*******************************************************************************/
int print_usage_hvol(int argc, char **argv){
    printf("Compute Hypervolume of a given pareto.\n\n");
    printf("  usage: %s %s [input file]\n", argv[0], HVOL_OPT);
    printf("  input file: - to read from stdin\n");
    printf("\nOutput:\n");
    printf("    <hypervolume>\n\n");
    printf("  input format:\n");
    printf("    <p1[0]> <p2[0]> ... <pm[0]>\n");
    printf("    <p1[1]> <p2[1]> ... <pm[1]>\n");
    printf("    ...\n");
    printf("    <p1[n]> <p2[n]> ... <pm[n]>\n");
}
int fcount_numbers_first_line(FILE *in){
	char line[200];
	size_t len = 0;
	int i = 0;
	int nint = 0;
	fgets(line, 200, in);
	rewind(in);

    while( line[i] ){
        if( ISNUM(line[i]) ){
            nint++;
            while( ISNUM(line[i]) )
                i++;
        }else{
        	i++;
		}
    }
	return nint;
}
int execute_hvol(int argc, char **argv){
	FILE *input;
	MOKPSolIndexer *msi;
	MOKPSol *sol;
	MOKP *mokp;
	char cbuffer[1000];
	mokpnum profit;
	int i, np;
	double hvol;

	if( argc < 3 ){
		print_usage_hvol(argc, argv);
		return 1;
	}

	input = stdin;
    if( argc > 2 ){
        if( strcmp(argv[2], "-") )
            input = fopen(argv[2], "r");
        if(!input){
            fprintf(stderr, "could not open file %s.\n", argv[2]);
            return 1;
        }
    }

	msi = msi_new(0);
	np = fcount_numbers_first_line(input);
	mokp = mokp_alloc(10, np);
	while( !feof(input) ){
		sol = mokpsol_new_empty(mokp);
		for( i = 0 ; i < np ; i++ ){
			mokpnum_fscanf(input, &profit);
			sol->profit[i] = profit;
		}
		msi_insert(msi, sol);
	}
	if( input != stdin )
		fclose(input);
	hvol = msi_hvolume(msi);
	printf("%.3f\n", hvol);
	msi_apply_all(msi, mokpsol_free);
	msi_free(msi);
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
    printf("  %s\t\tGenerate a random MOKP instance\n", RAND_OPT);
    printf("  %s\tSolve a MOKP using simple dynamic programming\n", DYNPROG_OPT);
    printf("  %s\tSolve a MOKP using Bazgan2009 algorithm\n", BAZGAN_OPT);
    printf("  %s\t\tSolve a MOKP using SCE algorithm\n", MOFPA_OPT);
    printf("  %s\t\tSolve a MOKP using Zouache2018s MOFPA algorithm\n", SCE_OPT);
    printf("  %s\t\tCompute hypervolume of a given pareto\n", HVOL_OPT);
    printf("  %s\t\tBatch test of bazgan MOKP algorithm.\n", BATCH_OPT);
    printf("  %s\t\tConvert a MKP instance in MOKP instance\n", MKP2_OPT);
    printf("  %s\tConvert a MOKP Bazgan instance.\n", CONVERT_OPT);

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
    /*** Geração de instâncias    ***/
    if(!strcmp(option, RAND_OPT))
        ret = execute_rand(argc, argv);

    /*** Algoritmo de Prog. Dinamica   ***/
    if(!strcmp(option, DYNPROG_OPT))
        ret = execute_dynprog(argc, argv);

    /***   Conversão de instâncias MKP para MOKP   ***/
    if(!strcmp(option, MKP2_OPT))
        ret = execute_mkp2(argc, argv);

    /***   Execução do algoritmo de Bazgan   ***/
    if(!strcmp(option, BAZGAN_OPT))
        ret = execute_bazgan(argc, argv);

    /***   Execução Batch do algoritmo Bazgan   ***/
    if(!strcmp(option, BATCH_OPT))
        ret = execute_batch(argc, argv);

    /***   Conversao das instancias    ***/
    if(!strcmp(option, CONVERT_OPT))
        ret = execute_convert(argc, argv);

    /***   Shuffled Complex Evolution    ****/
    if(!strcmp(option, SCE_OPT))
        ret = execute_sce(argc, argv);

    /***   Firefly Particle Swarm    ***/
    if(!strcmp(option, MOFPA_OPT))
        ret = execute_mofpa(argc, argv);

    /***   Compute Hypervolume ()    ***/
    if(!strcmp(option, HVOL_OPT))
        ret = execute_hvol(argc, argv);

    /***************************************************************
    *    IMPRESSÃO DA FORMA DE USO
    ***************************************************************/
    if( ret == emptyret )
        print_usage(argc, argv);

    return ret;
}

