#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

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
MOKPNode *mokpnode_new(MOKP *mokp, MOKPNode *father, int idx){
    MOKPNode *node;
    int np, i;

    np = mokp->np;

    node = (MOKPNode*)malloc(sizeof(MOKPNode));
    node->father = node->next = node->prev = NULL;
    node->mokp = mokp;

    if( !father ){   /* empty solution (the first one) */
        node->idx = -1;
        node->b_left = mokp->b;
        node->profit = double_array_init(NULL, np, 0.0);
    }else{
        node->idx = idx;
        node->profit = double_array_copy(father->profit, np);
        node->b_left = father->b_left;
        node->father = father;

        node->b_left -= mokp->w[idx];
        for( i = 0 ; i < np ; i++ )
            node->profit[i] += mokp->p[i][idx];
    }

    return node;
}

void mokpnode_fprintf(FILE *out, MOKPNode *node){
    int i, np;
    np = node->mokp->np;
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
    if( h <= node->mokp->np )
        return node->profit[h];
    return node->b_left;
}

int mokpnode_dominates(MOKPNode *dominant, MOKPNode *node){
    int np, dominates;
    np = dominant->mokp->np;

    dominates = 1;
    switch(np){
        case 6: dominates &= (dominant->profit[5] > node->profit[5]);
        case 5: dominates &= (dominant->profit[4] > node->profit[4]);
        case 4: dominates &= (dominant->profit[3] > node->profit[3]);
        case 3: dominates &= (dominant->profit[2] > node->profit[2]);
        case 2: dominates &= (dominant->profit[1] > node->profit[1]);
        case 1: dominates &= (dominant->profit[0] > node->profit[0]);
    }
    dominates &= dominant->b_left >= node->b_left;

    printf("  ** ");
    mokpnode_fprintf(stdout, dominant);
    printf(" dominates ");
    mokpnode_fprintf(stdout, node);
    printf(" ? %s\n", dominates ? "YES" : "NO");

    return dominates;
}

/* Solving */
void _mokp_dynprog(MOKP *mokp, MOKPNode *root, int idx, KDTree *kdtree, MOKPNode **tail, int *n_nodes){
    MOKPNode *current, *current2;
    MOKPNode *new;
    MOKPNode *dominant;
    MOKPNode *init_tail;
    double bounds[10];
    int ndim, np;
    int last_node, last_node2;

#ifdef MOKP_DEBUG
    current = root;
    int i = 1;
    printf("fixing item %d\n", idx);
    printf(" current nodes:\n");
    do{
        printf(" - %d ", i);
        mokpnode_fprintf(stdout, current);
        printf("\n");
        i++;
    }while( current = current->next );
#endif

    init_tail = *tail;
    current = root;
    np = mokp->np;
    if( kdtree )
        ndim = kdtree->ndim;

    /* iterate for each existant node */
    last_node = 0;
    do{
        if( current == init_tail )
            last_node = 1;
        /* create new node, using index */
        new = mokpnode_new(mokp, current, idx);
#ifdef MOKP_DEBUG
        printf("   - new: ");
        mokpnode_fprintf(stdout, new);
        printf("  (father: ");
        mokpnode_fprintf(stdout, current);
        printf(")\n");
#endif
        /* check if dominant exists */
        dominant = NULL;
        if( kdtree ){
            /* using kdtree */
            switch( ndim ){
                /* configuring bounds */
                case 5: bounds[8] = new->profit[4]; bounds[9] = INFINITY;
                case 4: bounds[6] = new->profit[3]; bounds[7] = INFINITY;
                case 3: bounds[4] = new->profit[2]; bounds[5] = INFINITY;
                case 2: bounds[2] = new->profit[1]; bounds[3] = INFINITY;
                case 1: bounds[0] = new->profit[0]; bounds[1] = INFINITY;
            }
            /* check dominance */
            dominant =
                (MOKPNode*)kdtree_range_search_r(
                    kdtree,
                    bounds,
                    (property_f_r)mokpnode_dominates,
                    new);
        }else{
            current2 = root; /* using plain list */
            last_node2 = 0;
            do{
                last_node2 = ( current2 == *tail );
                if( mokpnode_dominates(current2, new) )
                    dominant = current;
                current2 = current2->next;
            }while( !last_node2 && !dominant );
        }

        /* inserint if new node is not dominated */
        if( !dominant ){
#ifdef MOKP_DEBUG
            printf("     - no dominant found\n");
#endif
            /* inserting node in list */
            new->prev = *tail;
            (*tail)->next = new;
            *tail = new;

            (*n_nodes)++;
            if( kdtree )
                kdtree_insert(kdtree, new);
        }else{
#ifdef MOKP_DEBUG
            printf("     - has dominant: ");
            mokpnode_fprintf(stdout, dominant );
            printf("\n");
#endif
            mokpnode_free(new);
        }

        current = current->next;
    }while( !last_node );

#ifdef MOKP_DEBUG
    printf("\n");
#endif

    return;
}

/*
 *       mokp: the problem instance
 * use_kdtree: if want to use kdtree
 *          k: number of iterations
 *       idxs: custom ordering of variables
 * */
int mokp_dynprog(MOKP *mokp, int use_kdtree, int k, int *idxs){
    KDTree *kdtree = NULL;
    MOKPNode *root;
    MOKPNode *current_node;
    MOKPNode *next_node;
    MOKPNode *tail;
    int i, n_nodes;

    n_nodes = 1;
    tail = root = mokpnode_new(mokp, NULL, -1);

    /* config kdtree */
    if( use_kdtree ){
        kdtree = kdtree_new(3, (kdtree_eval_f)mokpnode_axis_val);
        kdtree_insert(kdtree, root);
    }

    /* iterate */
    for( i = 0 ; i < k ; i++ ){
        _mokp_dynprog(mokp, root, idxs[i], kdtree, &tail, &n_nodes);
    }

    /* freeing variables */
    if( kdtree )
        kdtree_free(kdtree);
    /* freeing mokp nodes */
    next_node = root;
    printf("PARETO:\n");
    while( next_node ){
        mokpnode_fprintf(stdout, next_node); printf("\n");
        current_node = next_node;
        next_node = next_node->next;
        mokpnode_free(current_node);
    }

    return n_nodes;
}

