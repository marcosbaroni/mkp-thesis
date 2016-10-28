#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#include "../../utils/util.h"
#include "mokp.h"

MOKP *mokp_alloc(int n, int np){
    MOKP *mokp;
    int i;

    mokp = (MOKP*)malloc(sizeof(MOKP));

    mokp->n = n;
    mokp->np = np;
    mokp->p = (double**)malloc(np*sizeof(double*));
    for( i = 0 ; i < np ; i++ )
        mokp->p[i] = (double*)malloc(n*sizeof(double));
    mokp->w = (double*)malloc(n*sizeof(double));

    return mokp;
}

MOKP *mokp_random(int n, int np){
    MOKP *mokp;
    int i, j;
    double maxcoef = 1000.0;
    double b;

    mokp = mokp_alloc(n, np);
    mokp->b = 0;
    for( i = 0 ; i < n ; i++ ){
        for( j = 0 ; j < np ; j++ )
            mokp->p[j][i] = (double)llrand(1000);
        mokp->w[i] = (double)llrand(1000);
        mokp->b += mokp->w[i];
    }
    mokp->b = (mokp->b / 2);

    return mokp;
}

void mokp_free(MOKP *mokp){
    int i, np;

    np = mokp->np;
    for( i = 0 ; i < np ; i++ )
        free(mokp->p[i]);
    free(mokp->p);
    free(mokp->w);
    free(mokp);

    return;
}

void mokp_write(FILE *out, MOKP *mokp){
    int i, j;
    int n, np;

    n = mokp->n;
    np = mokp->np;
    /* size of instance */
    fprintf(out, "%d %d\n", mokp->n, mokp->np);
    /* writing profits of items */
    for( j = 0 ; j < np ; j++ ){
        double_array_write(out, mokp->p[j], n);
        fprintf(out, "\n");
    }
    /* writing weight of items */
    double_array_write(out, mokp->w, n);
    /* writing capacity */
    fprintf(out, "\n%.3lf\n", mokp->b);

    return;
}

MOKP *mokp_read(FILE *fin){
    int n, np;
    int i, j;
    int nerr;

    nerr = fscanf(fin, "%d", &n);
    nerr = fscanf(fin, "%d", &np);

    MOKP *mokp = mokp_alloc(n, np);
    for( i = 0 ; i < np ; i++ )
        mokp->p[i] = double_array_read(fin, mokp->p[i], n);
    double_array_read(fin, mokp->w, n);
    nerr = fscanf(fin, "%lf", &(mokp->b));

    return mokp;
}

void mokp_save(char *filename, MOKP *mokp){
    FILE *fout;

    fout = fopen(filename, "w");
    if( !fout ){
        fprintf(stderr, "%s: error trying to open file \"%s\".\n", "mokp_save", filename);
        return;
    }

    mokp_write(fout, mokp);
    fclose(fout);

    return;
}

MOKP *mokp_open(char *filename){
    FILE *fin;
    MOKP *mokp;

    fin = fopen(filename, "r");
    if( !fin){
        fprintf(stderr, "%s: error trying to open file \"%s\".\n", "mokp_load", filename);
        return NULL;
    }

    mokp = mokp_read(fin);
    fclose(fin);

    return mokp;
}

/* MOKP Node (for Dynamic Programming) */
MOKPNode *mokpnode_new(MOKPNode *father, int idx){
    MOKPNode *node;
    MOKP *mokp;
    int np, i;

    mokp = father->tree->mokp;
    np = mokp->np;

    node = (MOKPNode*)malloc(sizeof(MOKPNode));
    node->father = node->next = node->prev = NULL;
    node->tree = father->tree;

    node->idx = idx;
    node->profit = double_array_copy(father->profit, np);
    node->b_left = father->b_left;
    node->father = father;
    node->prev = node->next = NULL;

    node->b_left -= mokp->w[idx];
    for( i = 0 ; i < np ; i++ )
        node->profit[i] += mokp->p[i][idx];

    return node;
}

MOKPNode *mokpnode_new_empty(MOKPTree *tree){
    MOKPNode *node;
    MOKP *mokp;
    int np, i;

    mokp = tree->mokp;
    np = mokp->np;

    node = (MOKPNode*)malloc(sizeof(MOKPNode));
    node->profit = double_array_init(NULL, np, 0);

    node->tree = tree;
    node->idx = -1;
    node->b_left = mokp->b;
    node->father = node->next = node->prev = NULL;

    return node;
}

void mokpnode_fprintf(FILE *out, MOKPNode *node){
    int i, np;
    np = node->tree->mokp->np;
    fprintf(out, "%x: ", node);
    for( i = 0 ; i < np ; i++ )
        fprintf(out, "%.0lf ", node->profit[i]);
    fprintf(out, "(%.0lf)", node->b_left);

    return;
}

void mokpnode_free(MOKPNode *node){
    free(node->profit);
    free(node);

    return;
}

double mokpnode_axis_val(MOKPNode *node, int h){
    if( h <= node->tree->mokp->np )
        return node->profit[h];
    return node->b_left;
}

int mokpnode_dominates(MOKPNode *dominant, MOKPNode *node){
    int np, dominates;

    np = dominant->tree->mokp->np;
    dominant->tree->n_comparisons++;
    dominates = 1;
    switch(np){
        case 10: dominates &= (dominant->profit[9] >= node->profit[9]);
        case 9: dominates &= (dominant->profit[8] >= node->profit[8]);
        case 8: dominates &= (dominant->profit[7] >= node->profit[7]);
        case 7: dominates &= (dominant->profit[6] >= node->profit[6]);
        case 6: dominates &= (dominant->profit[5] >= node->profit[5]);
        case 5: dominates &= (dominant->profit[4] >= node->profit[4]);
        case 4: dominates &= (dominant->profit[3] >= node->profit[3]);
        case 3: dominates &= (dominant->profit[2] >= node->profit[2]);
        case 2: dominates &= (dominant->profit[1] >= node->profit[1]);
        case 1: dominates &= (dominant->profit[0] >= node->profit[0]);
    }
    dominates &= dominant->b_left >= node->b_left;

#ifdef MOKP_DEBUG
    printf("  ** ");
    mokpnode_fprintf(stdout, dominant);
    printf(" dominates ");
    mokpnode_fprintf(stdout, node);
    printf(" ? %s\n", dominates ? "YES" : "NO");
#endif

    return dominates;
}

double *mokpnode_dominant_bounds(MOKPNode *node, int ndim, double *bounds){
    int np;

    np = node->tree->mokp->np;
    if( !bounds )
        bounds = (double*)malloc(2*ndim*sizeof(double));

    /* configuring bounds */
    switch( ndim ){
        case 5: bounds[8] = node->profit[4]; bounds[9] = INFINITY;
        case 4: bounds[6] = node->profit[3]; bounds[7] = INFINITY;
        case 3: bounds[4] = node->profit[2]; bounds[5] = INFINITY;
        case 2: bounds[2] = node->profit[1]; bounds[3] = INFINITY;
        case 1: bounds[0] = node->profit[0]; bounds[1] = INFINITY;
    }

    return bounds;
}

MOKPTree *mokptree_new(MOKP *mokp){
    MOKPTree *tree;

    tree = (MOKPTree*)malloc(sizeof(MOKPTree));
    tree->mokp = mokp;
    tree->n_nodes = 1;
    tree->root = tree->tail = mokpnode_new_empty(tree);
    tree->n_comparisons = 0;

    tree->kdtree = NULL;

    return tree;
}

void mokptree_fprintf(FILE *out, MOKPTree *tree){
    MOKPNode *node;
    MOKPNode *current_node;

    node = tree->root;
    printf(" root2: %x\n", node);
    do{
        mokpnode_fprintf(out, node);
        printf("\n");
    }while( node = node->next);

    return;
}

void mokptree_insert(MOKPTree *tree, MOKPNode *node){
    tree->tail->next = node;
    node->prev = tree->tail;
    tree->tail = node;
    tree->n_nodes++;

    if( tree->kdtree )
        kdtree_insert(tree->kdtree, node);

    return;
}
MOKPNode *mokptree_find_dominantor(MOKPTree *tree, MOKPNode *node){
    MOKPNode *dominant;
    MOKPNode *current;
    MOKPNode *last_node;
    KDTree *kdtree;
    double bounds[10];
    int ndim;

    kdtree = tree->kdtree;
    if( kdtree )
        ndim = kdtree->ndim;
    dominant = NULL;

    /* using kdtree */
    if( kdtree ){
        mokpnode_dominant_bounds(node, ndim, bounds);
        /* check dominance */
        dominant = (MOKPNode*)kdtree_range_search_r(kdtree, bounds,
            (property_f_r)mokpnode_dominates, node);
    /* using the plain list */
    }else{
        current = tree->root; /* using plain list */
        do if( mokpnode_dominates(current, node) )
                dominant = current;
        while( (current = current->next) && !dominant );
    }

    return dominant;
}

void mokptree_free(MOKPTree *tree){
    MOKPNode *node;
    MOKPNode *next_node;
    MOKPNode *current_node;

    if( tree->kdtree )
        kdtree_free( tree->kdtree );

    next_node = tree->root;
    while( next_node ){
        node = next_node;
        next_node = node->next;
        mokpnode_free(node);
    }

    free(tree);

    return;
}

/* Solving */
void _mokp_dynprog_iter(MOKPTree *tree, int idx){
    MOKPNode *current, *current2;
    MOKPNode *new;
    MOKPNode *dominant;
    MOKPNode *init_tail;
    MOKPNode *tail;
    KDTree *kdtree;
    double bounds[10];
    int ndim, np;
    int last_node, last_node2;
    int n_discharged, n_added;
    n_discharged = n_added = 0;

    current = tree->root;
    tail = tree->tail;
    init_tail = tail;
    np = tree->mokp->np;
    kdtree = tree->kdtree;

    /* iterate for each existant node */
    last_node = 0;
    do{
        if( current == init_tail )
            last_node = 1;
        /* create new node, using index */
        new = mokpnode_new(current, idx);
        /* check if dominant exists */
        dominant = mokptree_find_dominantor(tree, new);

        if( !dominant ) n_added++;
        else n_discharged++;
        /* inserint if new node is not dominated */
        if( !dominant ) mokptree_insert(tree, new);
        else mokpnode_free(new);

        current = current->next;
    }while( !last_node );

    printf(" %d: discharged %d, added %d (discharge ratio: %.1lf)\n",
            idx+1, n_discharged, n_added, 100*(n_discharged/(double)(n_discharged+n_added)));

    return;
}

/*
 *       mokp: the problem instance
 * use_kdtree: if want to use kdtree
 *          k: number of iterations
 *       idxs: custom ordering of variables
 * */
int mokp_dynprog(MOKP *mokp, int use_kdtree, int k, int *idxs, long long *n_comps){
    MOKPNode *current_node;
    MOKPNode *next_node;
    MOKPTree *tree;
    int i, n_nodes;
    double avg_h, bal_time;
	clock_t c0;

    /* new mokp nodes tree */
    tree = mokptree_new(mokp);
    bal_time = 0;

    /* config kdtree */
    if( use_kdtree ){
        tree->kdtree = kdtree_new(3, (kdtree_eval_f)mokpnode_axis_val);
        kdtree_insert(tree->kdtree, tree->root);
    }

    /* iterate */
    for( i = 0 ; i < k ; i++ ){
        _mokp_dynprog_iter(tree, idxs[i]);
        /* balancing */
        c0 = clock();
        if( 0 )
            kdtree_balance(tree->kdtree);
        bal_time += (clock()-c0)*1./CLOCKS_PER_SEC;
    }

    /* wrinting number of comparison */
    n_nodes = tree->n_nodes;
    if( n_comps )
       (*n_comps) = tree->n_comparisons;

#if KDTREE_STATS
    /* output stats */
    if( tree->kdtree )
        kdtree_fprint_stats(stdout, tree->kdtree);
#endif
#ifdef MOKP_DEBUG
    /* output pareto*/
    printf("PARETO:\n");
    mokptree_fprintf(stdout, tree);
#endif
    printf("total time to balance: %.3f\n", bal_time);

    /* free */
    mokptree_free(tree);

    return n_nodes;
}

