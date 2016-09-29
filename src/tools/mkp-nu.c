#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "../models/mkp/mkp.h"
#include "../models/mkp/balev.h"
#include "../models/mkp/domset.h"
#include "../utils/util.h"
#include "../utils/ppm.h"

int print_usage(int argc, char **argv){
	FILE *out;

	out = stdout;
	fprintf(out, " usage: %s [alg] [input file] <k>\n", argv[0]);
	fprintf(out, " Nemhauser-Ullman Algorithm for MKP.\n");
	fprintf(out, "   input file: a MKP instance. If '-' is given, instance is read from stdin.\n");
	fprintf(out, "   alg: the algorithm which should be used.\n");
	fprintf(out, "      1 - Nemhauser-Ullman plain (only cutting unfeaseble)\n");
	fprintf(out, "      2 - Nemhauser-Ullman with Linked Buckets\n");
	fprintf(out, "      3 - Nemhauser-Ullman with KD-Tree \n");
	fprintf(out, "   k: number of DP iterations. If not given all iterations is executed.\n");
	fprintf(out, "\n");
	fprintf(out, "   Program outputs \"<n. of dom. subsets>;<n comparison>;<enumeration time (s)>;<best sol profit>\"\n");

	return 1;
}

void _find_median(
    DomSetNode **ord_profit,
    DomSetNode **ord_bleft0,
    DomSetNode **ord_bleft1,
    DomSetNode **inserting_ord,
    int a,
    int b,
    int h)
{
}

void teste_dstree_profile(DomSetTree *dstree){
    DomSetNode **ord_profit;
    DomSetNode **ord_bleft0;
    DomSetNode **ord_bleft1;
    DomSetNode **inserting_ord;
    DomSetNode *dsnode;
    int i, n_nodes;

    /* allocing orderes */
    n_nodes = dstree->n;
    ord_profit = (DomSetNode**)malloc(n_nodes*sizeof(DomSetNode*));
    ord_bleft0 = (DomSetNode**)malloc(n_nodes*sizeof(DomSetNode*));
    ord_bleft1 = (DomSetNode**)malloc(n_nodes*sizeof(DomSetNode*));
    inserting_ord = (DomSetNode**)malloc(n_nodes*sizeof(DomSetNode*));

    /* collection the nodes */
    dsnode = dstree->root;
    i = 0;
    do{
        ord_profit[i] =
            ord_bleft0[i] =
            ord_bleft1[i] = dsnode;
        i++;
    }while( dsnode = dsnode->next );

    /* sorting */
    void_sort((void**)ord_profit, n_nodes, (int(*)(void*,void*))dsnode_cmp_by_profit);
    void_sort((void**)ord_bleft0, n_nodes, (int(*)(void*,void*))dsnode_cmp_by_b_left0);
    void_sort((void**)ord_bleft1, n_nodes, (int(*)(void*,void*))dsnode_cmp_by_b_left1);

    /* finding medians... */
    _find_median(ord_profit, ord_bleft0, ord_bleft1, inserting_ord, 0, n_nodes-1, 0);

    /* print kdtree profile */
    dskdtree_fprintf_balance_profile(stdout, dstree->kdtree);

    /* building balanced tree */

    /* print kdtree profile */
    dskdtree_fprintf_balance_profile(stdout, dstree->kdtree);

    free(ord_profit);
    free(ord_bleft0);
    free(ord_bleft1);
    free(inserting_ord);
    
    return;
}

int execute_nemullman(int argc, char **argv){
	MKP *mkp;           /* the problem instance */
	FILE *input;        /* input stream */
	MKPSol *best_sol;
    DomSetTree *dstree;

    int alg;
	int i, n;
    int k;
    int *idxs;

	clock_t c0, cf;

    alg = 1;
    alg = atoi(argv[1]);
    k = 0;

	input = stdin;
	/* CHECKING INPUTS */
	if(strcmp(argv[2], "-")){ /* not '-' */
		input = fopen(argv[2], "r");
        if( !input ){
            fprintf(stderr, "Error: file %s could not be opened.\n", argv[0]);
            return 1;
        }
    }

	/* READING INSTANCE */
	mkp = mkp_read_from_file(input);
	fclose(input);
    n = mkp->n;

    /* SETTING K */
    if( argc > 3 )
        k = atoi(argv[3]);
    else
        k = n;
    idxs = (int*)malloc(n*sizeof(int));
    for( i = 0 ; i < n ; i++ )
        idxs[i] = i;

    dstree = dstree_new(mkp);
    switch( alg ){
        case 1:
        break;

        case 2:
        dstree_set_lbucket(dstree, lbucket_new(mkp, 10, 2, 'l'));
        break;

        case 3:
        dstree_set_kdtree(dstree, dskdtree_new(3));
        break;
    }
    
	/* ENUMERATE SETS */
	c0 = clock();
    for( i = 0 ; i < k ; i++ ){
        dstree_dp_iter(dstree, idxs[i]);
	    cf = clock();
        //fprintf(stderr, "iter: %d/%d (%d, %.3e) %.3lfs\n", i+1, k, dstree->n, (double)dstree->n_comparison, ((cf-c0)*1./CLOCKS_PER_SEC));
    }
	cf = clock();
    best_sol = dsnode_get_mkpsol(dstree->best);

    //teste_dstree_profile(dstree);

	/* OUTPUT SOLUTION */
	printf("%d;%llu;%.3lf;", dstree->n, dstree->n_comparison, ((cf-c0)*1./CLOCKS_PER_SEC));
    mkpnum_fprintf(stdout, best_sol->obj);
	printf("\n");

	/* FREES */
    mkpsol_free(best_sol);
    dstree_free(dstree);
	mkp_free(mkp);
    free(idxs);

	return 0;
}

/*
 * use_lb: Use LinkedBuckets
 */
int execute_balev(int argc, char **argv, int use_lb){
	MKP *mkp;           /* the problem */
	MKP *mkp2;          /* the reduced problem */
    MKPSol *mkpsol;     /* feasible solution given */
	FILE *input;        /* input stream */
	Array *dom_sets;
	MKPSol *best_sol;
    MKPSol *sol;
	int i, n;

	clock_t c0, cf;

	input = stdin;
	/* checking inputs */
	if(strcmp(argv[2], "-")){ /* not '-' */
		input = fopen(argv[2], "r");
        if( !input ){
            fprintf(stderr, "Error: file %s could not be opened.\n", argv[0]);
            return 1;
        }
    }

	/* reading instance */
	mkp = mkp_read_from_file(input);
    if( input != stdin )
	    fclose(input);

    /* reading first solution given pro enumeration method */
    if( argc > 3 )
        mkpsol = mkpsol_read_from_filename(argv[3], mkp);
    else{
        mkpsol = mkpsol_new(mkp);
        mkpsol = mkpsol_greedy_fill(mkpsol);
    }

	/* enumerate sets */
	c0 = clock();
    mkp_balev(mkpsol, use_lb);
	cf = clock();

	/* frees */
    mkpsol_free(mkpsol);
	mkp_free(mkp);

	return 0;
}

int main(int argc, char **argv){
    int alg;
	if( argc < 3 ){
		return print_usage(argc, argv);
    }

#ifndef DEBUG_LVL
    setdebug_lvl(0);
#else
    setdebug_lvl(DEBUG_LVL);
#endif

    execute_nemullman(argc, argv);

    return 0;
}

