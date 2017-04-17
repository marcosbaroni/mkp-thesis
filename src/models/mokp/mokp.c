#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#include "../../utils/util.h"
#include "../../utils/kdtree.h"

#include "../mkp/mkp.h"
//#include "mokp.h"
#include "order.h"

/*******************************************************************************
*       MOKPNUM
*******************************************************************************/
int mokpnode_sscanf(char *str, mokpnum *a){
    return sscanf(str, "%d", a);
}
int mokpnum_fscanf(FILE *in, mokpnum *a){
    return fscanf(in, "%d", a);
}

void mokpnum_fprintf(FILE *out, mokpnum x){
    fprintf(out, "%d ", x);
}
void mokpnum_printf(mokpnum x){
    return mokpnum_fprintf(stdout, x);
}

void mokpnum_array_write(FILE *out, mokpnum *array, int n){
    int i;
    for( i = 0 ; i < n ; i++ )
        mokpnum_fprintf(out, array[i]);
}


/*******************************************************************************
*       MOKP
*******************************************************************************/
MOKP *mokp_alloc(int n, int np){
    MOKP *mokp;
    int i;

    mokp = (MOKP*)malloc(sizeof(MOKP));

    mokp->n = n;
    mokp->np = np;
    mokp->p = (mokpnum**)malloc(np*sizeof(mokpnum*));
    for( i = 0 ; i < np ; i++ )
        mokp->p[i] = (mokpnum*)malloc(n*sizeof(mokpnum));
    mokp->w = (mokpnum*)malloc(n*sizeof(mokpnum));

    return mokp;
}

mokpnum mokpnum_rand(mokpnum max){ return (rand() % max) + 1; }
mokpnum min(mokpnum a, mokpnum b){ return a < b ? a : b; }
mokpnum max(mokpnum a, mokpnum b){ return a > b ? a : b; }

/*
 *   Type A
 * */
MOKP *_mokp_random_uniform(int n, int np){
    MOKP *mokp;
    int i, j;
    mokpnum maxcoef = 1000;
    mokpnum b;

    mokp = mokp_alloc(n, np);

    mokp->b = 0;
    for( i = 0 ; i < n ; i++ ){
        for( j = 0 ; j < np ; j++ )
            mokp->p[j][i] = mokpnum_rand(maxcoef);
        mokp->w[i] = mokpnum_rand(maxcoef);
        mokp->b += mokp->w[i];
    }
    mokp->b = (mokp->b / 2);

    return mokp;
}

/*
 *   Type B
 * */
MOKP *_mokp_random_unconflict(int n, int np){
    MOKP *mokp;
    int i, j;
    mokpnum maxcoef = 1000;
    mokpnum b;

    mokp = mokp_alloc(n, np);

    mokp->b = 0;
    for( i = 0 ; i < n ; i++ ){
        mokp->p[0][i] = 111 + mokpnum_rand(889);
        for( j = 1 ; j < np ; j++ )
            mokp->p[j][i] = mokp->p[0][i] - mokpnum_rand(200) + 100;
        mokp->w[i] = mokpnum_rand(1000);
        mokp->b += mokp->w[i];
    }
    mokp->b = (mokp->b / 2);

    return mokp;
}

/*
 *   Type C
 * */
MOKP *_mokp_random_conflict(int n, int np){
    MOKP *mokp;
    int i, j;
    mokpnum lower, upper;
    mokpnum **p;

    mokp = mokp_alloc(n, np);
    p = mokp->p;

    mokp->b = 0;
    for( i = 0 ; i < n ; i++ ){
        /* first profit */
        p[0][i] = mokpnum_rand(1000);

        /* case m = 2 */
        if( np == 2 ){
            lower = max( 900 - p[0][i],   1);
            upper = min(1100 - p[0][i], 100);
            p[1][i] = lower + mokpnum_rand((upper - lower));
        }
        /* case m = 3,4,5,... */
        if( np > 2 ){
            p[1][i] = mokpnum_rand(1001 - p[0][i]);
            lower = max( 900 - p[0][i] - p[1][i], 1);
            upper = min(1100 - p[0][i] - p[1][i], 1001 - p[0][i]);
            p[2][i] = lower + mokpnum_rand((upper - lower));
        }
        /* further profits... */
        for( j = 3 ; j < np ; j++ )
            mokp->p[j][i] = mokpnum_rand(1000);

        /* wieght */
        mokp->w[i] = mokpnum_rand(1000);
        mokp->b += mokp->w[i];
    }
    mokp->b = mokp->b / 2;

    return mokp;
}

/*
 *   Type D
 * */
MOKP *_mokp_random_conflict_correl(int n, int np){
    MOKP *mokp;
    int i, j;
    mokpnum upper, lower;

    mokp = mokp_alloc(n, np);

    mokp-> b = 0;
    for( i = 0 ; i < n ; i++ ){
            mokp->p[0][i] = mokpnum_rand(1000);
            /* second profit */
            if( np > 1 ){
                lower = max( 900 - mokp->p[0][i], 1);
                upper = min(1100 - mokp->p[0][i], 1000);
                mokp->p[1][i] = lower + mokpnum_rand(upper - lower);
            }
            /* further profits... */
            for( j = 2 ; j < np ; j++ )
                mokp->p[j][i] = mokpnum_rand(1000);

        /* wieght */
        mokp->w[i] = mokp->p[0][i] + mokp->p[1][i] - 200 + mokpnum_rand(400);
        mokp->b += mokp->w[i];
    }
    mokp->b = mokp->b / 2;

    return mokp;
}

/*  Sintethic MOKP instance */
MOKP *mokp_random(int n, int np, int option){
    MOKP *mokp;
    
    switch( option ){
        case 1: mokp = _mokp_random_uniform(n, np); break;
        case 2: mokp = _mokp_random_unconflict(n, np); break;
        case 3: mokp = _mokp_random_conflict(n, np); break;
        case 4: mokp = _mokp_random_conflict_correl(n, np); break;
    }

    return mokp;
}

/*  Converts a MKP instace into a MOKP instance */
/*
MOKP *mokp_from_mkp(MKP *mkp){
    MOKP *mokp;
    int i, j, n, m, np;

    n = mkp->n;
    m = mkp->m;

    mokp = (MOKP*)malloc(sizeof(MOKP));
    mokp->n = n;
    mokp->np = m;
    mokp->p = (double**)malloc(m*sizeof(double*));
    mokp->p[0] = double_array_copy(mkp->p, n);
    for( i = 1 ; i < m ; i++ ){
        mokp->p[i] = (double*)malloc(n*sizeof(double));
        for( j = 0 ; j < n ; j++ )
            mokp->p[i][j] = -mkp->w[i-1][j];
    }
    mokp->w = double_array_copy(mkp->w[m-1], n);
    mokp->b = mkp->b[m-1];

    return mokp;
}
*/

int *mokp_get_item_order(MOKP *mokp, char ordering_type){
    int **ranks, *sort, *idxs, n, np, i, j;
    mokpnum *w, **p;
    double *ratios, *measure, *sum;

    n = mokp->n;
    np = mokp->np;
    w = mokp->w;
    p = mokp->p;

    ranks = int_matrix_alloc(np, n);
    ratios = (double*)malloc(n*sizeof(double));
    sum = double_array_init(NULL, n, 0.0);

    for( i = 0 ; i < np ; i++ ){
        for( j = 0 ; j < n ; j++ )
            ratios[j] = p[i][j]/(double)w[j];
        sort = double_index_sort(ratios, n);
        for( j = 0 ; j < n ; j++ )
            ranks[i][sort[j]] = j;
        free(sort);
    }

    measure = (double*)malloc(n*sizeof(double));
    for( i = 0 ; i < np ; i++ )
        for( j = 0 ; j < n ; j++ )
            sum[j] += ranks[i][j];

    switch( ordering_type ){
        /***   MIN   ***/
        case 'm':
            for( i = 0 ; i < n ; i++ ){
                measure[i] = ranks[0][i];
                for( j = 0 ; j < np ; j++ )
                    if( ranks[j][i] < measure[i] )
                        measure[i] = ranks[j][i];
                measure[i] += sum[i]/(double)(np*n);
            }
            break;

        /***   MAX   ***/
        case 'M':
            for( i = 0 ; i < n ; i++ ){
                measure[i] = ranks[0][i];
                for( j = 0 ; j < np ; j++ )
                    if( ranks[j][i] > measure[i] )
                        measure[i] = ranks[j][i];
                measure[i] += sum[i]/(double)(np*n);
            }
            break;

        /***   SUM   ***/
        case 's':
            for( i = 0 ; i < n ; i++ )
                measure[i] = sum[i];
            break;
    }

    idxs = double_index_sort(measure, n);
    idxs = int_array_reverse(idxs, n);

    free(ratios);
    free(measure);
    free(sum);
    int_matrix_free(ranks, np);

    return idxs;
}

MOKP *mokp_new_reordered(MOKP *mokp, int *new_idx_order){
    MOKP *new_mokp;
    int i, j, idx, n, np;

    new_mokp = mokp_alloc(mokp->n, mokp->np);
    n = mokp->n;
    np = mokp->np;

    for( i = 0 ; i < n ; i++ ){
        idx = new_idx_order[i];
        for( j = 0 ; j < np ; j++ )    /* Coping profits */
            new_mokp->p[j][i] = mokp->p[j][idx];
        new_mokp->w[i] = mokp->w[idx]; /* Coping Weight */
    }
    new_mokp->b = mokp->b;             /* Coping Capacity */

    return new_mokp;
}

MOKP *mokp_reord_by_type(MOKP *mokp, char ordering_type){
    int *idxs;
    MOKP *mokp2;

    idxs = mokp_get_item_order(mokp, ordering_type);
    mokp2 = mokp_new_reordered(mokp, idxs);

    free(idxs);
    
    return mokp2;
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
        mokpnum_array_write(out, mokp->p[j], n);
        fprintf(out, "\n");
    }
    /* writing weight of items */
    mokpnum_array_write(out, mokp->w, n);
    /* writing capacity */
    mokpnum_fprintf(out, mokp->b);
    fprintf(out, "\n");

    return;
}

/*  Reads a MOKP instance from a given file */
MOKP *mokp_read(FILE *fin){
    int n, np;
    int i, j;
    int nerr;

    nerr = fscanf(fin, "%d", &n);
    nerr = fscanf(fin, "%d", &np);

    MOKP *mokp = mokp_alloc(n, np);
    for( i = 0 ; i < np ; i++ )
        for( j = 0 ; j < n ; j++ )
            mokpnum_fscanf(fin, &(mokp->p[i][j]));
    for( j = 0 ; j < n ; j++ )
        mokpnum_fscanf(fin, &(mokp->w[j]));
    mokpnum_fscanf(fin, &(mokp->b));

    return mokp;
}

int _get_np_from_bazgan_line(char *line){
    int i, np;

    i = np = 0;
    while( line[i] ){
        if( ISNUM(line[i]) ){
            np++;
            while( ISNUM(line[i]) )
                i++;
        }
        i++;
    }

    return np-1;
}

void _read_new_item_from_bazgan_line(char *cbuffer, MOKP *mokp, int _n){
    int i;

    i = 0;
    /* read weight */
    while( !ISNUM(*cbuffer) )
        cbuffer++;
    mokpnode_sscanf(cbuffer, &(mokp->w[_n]));
    while( ISNUM(*cbuffer) )
        cbuffer++;
    
    /* read profits */
    while( *cbuffer ){
        while( !ISNUM(*cbuffer) && *cbuffer )
            cbuffer++;
        if( *cbuffer )
            mokpnode_sscanf(cbuffer, &(mokp->p[i++][_n]));

        while( ISNUM(*cbuffer) && *cbuffer )
            cbuffer++;
    }
}

MOKP *mokp_read_bazgan_format(FILE *fin){
    MOKP *mokp;
    int i, j, it, n, _n, np, b;
    char *cbuffer, cbuffer_[1000], c;

    it = 0;
    n = np = _n = 0;
    mokp = NULL;

    /* Reading Instance */
    while( !feof(fin) ){
        fgets(cbuffer_, 1000, fin);
        cbuffer = cbuffer_;
        switch( cbuffer[0] ){
            /* Comment */
            case 'c': break;

            /* N of itens */
            case 'n':
            sscanf(cbuffer, "%c %d", &c, &n);
            break;

            /* Capacity */
            case 'w':
            case 'W':
            while( !ISNUM(*cbuffer) )
                cbuffer++;
            mokpnode_sscanf(cbuffer, &(mokp->b) );
            break;

            /* Itens */
            case 'i':
            case 'I':
            if( !np ){
                np = _get_np_from_bazgan_line(cbuffer);
                mokp = mokp_alloc(n, np);
            }
            _read_new_item_from_bazgan_line(cbuffer, mokp, _n);
            _n++;
            break;
        }
    }

    return mokp;
}

/*  Writes a MOKP instance into a file */
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

/*  Reads a MOKP instance from a file (given its name) */
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

/*  returns a new MOKP Node (for Dynamic Programming), given its father and the
 *  item to be inserted. */
MOKPNode *mokpnode_new(MOKPNode *father, int idx){
    MOKPNode *node;
    MOKP *mokp;
    int np, i;

    mokp = father->tree->mokp;
    np = mokp->np;

    node = (MOKPNode*)malloc(sizeof(MOKPNode));
    /* initing new node */
    node->tree = father->tree;
    node->idx = idx;
    node->tree = father->tree;
    node->profit = int_array_copy(NULL, father->profit, np);
    node->b_left = father->b_left;
    node->father = father;
    node->prev = node->next = NULL;
    /* updating weight and profits */
    node->b_left -= mokp->w[idx];
    for( i = 0 ; i < np ; i++ )
        node->profit[i] += mokp->p[i][idx];

    return node;
}

/*  Returns an empty solution for the MOKP, given its tree */
MOKPNode *mokpnode_new_empty(MOKPTree *tree){
    MOKPNode *node;
    MOKP *mokp;
    int np, i;

    mokp = tree->mokp;
    np = mokp->np;

    node = (MOKPNode*)malloc(sizeof(MOKPNode));
    node->profit = int_array_init(NULL, np, 0);

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
        mokpnum_fprintf(out, node->profit[i]);
    mokpnum_fprintf(out, node->b_left);

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
    int i, np, dominates;

    np = dominant->tree->mokp->np;
    dominant->tree->n_comparisons++;
    for( i = 0 ; i < np && dominant ; i++ )
        if( dominant->profit[i] < node->profit[i] )
            return 0;
    /*
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
    */
    if( dominant->b_left < node->b_left )
        return 0;

    return 1;
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

MOKPTree *mokptree_new(MOKP *mokp, int ndim){
    MOKPTree *tree;

    tree = (MOKPTree*)malloc(sizeof(MOKPTree));
    tree->mokp = mokp;
    tree->n_nodes = 1;
    tree->root = tree->tail = mokpnode_new_empty(tree);
    tree->n_comparisons = 0;
    tree->ndim = ndim;

    return tree;
}

void mokptree_fprintf(FILE *out, MOKPTree *tree){
    MOKPNode *node;
    MOKPNode *current_node;

    node = tree->root;
    do{
        mokpnode_fprintf(out, node);
        printf("\n");
    }while( node = node->next);

    return;
}

void mokptree_insert(MOKPTree *tree, MOKPNode *node){
    /* insering in list */
    tree->tail->next = node;
    node->prev = tree->tail;
    tree->tail = node;
    tree->n_nodes++;

    return;
}

/* Frees a MOKP instance */
void mokptree_free(MOKPTree *tree){
    MOKPNode *node;
    MOKPNode *next_node;
    MOKPNode *current_node;

    next_node = tree->root;
    while( next_node ){
        node = next_node;
        next_node = node->next;
        mokpnode_free(node);
    }

    free(tree);

    return;
}

/*  Finds a dominator (if exists) for a given node */
MOKPNode *mokptree_find_dominator(MOKPTree *tree, MOKPNode *node, double *stime){
    MOKPNode *dominant = NULL;
    MOKPNode *current;
    MOKPNode *last_node;
    KDTree *kdtree;

    /* not using kdtree... */
    current = tree->root; /* using plain list */
    do if( mokpnode_dominates(current, node) )
            dominant = current;
    while( (current = current->next) && !dominant );

    return dominant;
}


/*******************************************************************************
 *    Iteração do método de Prog. Dyn. para o MOKP.
 *
 *    tree:  arvore com soluções
 *    idx:   indice do item a ser adicionado
 *    stime: time searching for dominant (for ktree performance study)
*******************************************************************************/
void _mokp_dynprog_iter(MOKPTree *tree, int idx, double *stime){
    MOKPNode *current, *current2;
    MOKPNode *new;
    MOKPNode *dominant;
    MOKPNode *init_tail;
    MOKPNode *tail;

    int ndim, np;
    int last_node, last_node2;
    int n_discharged, n_added;
    int i;

    clock_t c0;
    c0 = clock();

    n_discharged = n_added = 0;

    current = tree->root;
    tail = tree->tail;
    init_tail = tail;
    np = tree->mokp->np;

    /* iterate for each existant node */
    last_node = 0;
    i = 0;
    do{
        i++;
        if( current == init_tail )
            last_node = 1;
        /* create new node, using index */
        new = mokpnode_new(current, idx);
        /* check if dominant exists */
        dominant = mokptree_find_dominator(tree, new, stime);

        if( !dominant ) n_added++;
        else n_discharged++;
        /* inserint if new node is not dominated */
        if( !dominant ) mokptree_insert(tree, new);
        else mokpnode_free(new);

        current = current->next;
    }while( !last_node );

    //printf(" %d: %d %lld %.3lfs (disc: %d, added: %d, ratio: %.1lf)\n",
    //        idx, tree->n_nodes, tree->n_comparisons,
    //        (clock()-c0)*1./CLOCKS_PER_SEC,
    //        n_discharged, n_added, 100*(n_discharged/(double)(n_discharged+n_added)));

    return;
}


/*******************************************************************************
 *    Algoritmo de programação dinamica para o problema MOKP.
 *
 *    mokp:        the problem instance
 *    ndim:        dimension of kdtree to be used (0 to use list)
 *    k:           number of iterations
 *    idxs:        custom ordering of variables
 *    *n_comp:     returnes (writes) total number of comparison
*******************************************************************************/
int mokp_dynprog(MOKP *mokp, int ndim, int k, int *idxs, long long *n_comps){
    MOKPNode *current_node;
    MOKPNode *next_node;
    MOKPTree *tree;
    int i, n_nodes;
    double avg_h, bal_time;
	clock_t c0;
    double stime = 0;

    /* new mokp nodes tree */
    tree = mokptree_new(mokp, ndim);
    bal_time = 0;

    c0 = clock();
    /* iterate */
    for( i = 0 ; i < k ; i++ ){
        _mokp_dynprog_iter(tree, idxs[i], &stime);
    }

    /* wrinting number of comparison */
    n_nodes = tree->n_nodes;
    if( n_comps )
       (*n_comps) = tree->n_comparisons;

    //printf("total time to balance: %.5lf\n", bal_time);
    //printf("total time searching: %.5lf\n", stime);

    /* free */
    mokptree_free(tree);

    return n_nodes;
}

