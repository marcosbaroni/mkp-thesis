/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: december, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#include "../../utils/util.h"
#include "../../utils/kdtree.h"
#include "../../utils/avl.h"
#include "../../utils/list.h"
#include "../../hv-2.0/hv.h"


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
    fprintf(out, "%d", x);
}
void mokpnum_printf(mokpnum x){
    return mokpnum_fprintf(stdout, x);
}

void mokpnum_array_write(FILE *out, mokpnum *array, int n){
    int i;
    for( i = 0 ; i < n ; i++ ){
        mokpnum_fprintf(out, array[i]);
		fprintf(out, " ");
	}
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
	mokp->idxs = (int*)malloc(n*sizeof(int));
	for( i = 0 ; i < n ; i++ )
		mokp->idxs[i] = i;
    mokp->p = (mokpnum**)malloc(np*sizeof(mokpnum*));
    for( i = 0 ; i < np ; i++ )
        mokp->p[i] = (mokpnum*)malloc(n*sizeof(mokpnum));
    mokp->w = (mokpnum*)malloc(n*sizeof(mokpnum));

    return mokp;
}

mokpnum mokpnum_rand(mokpnum max){ return (!max ? 0 : (rand() % max) + 1); }
mokpnum min(mokpnum a, mokpnum b){ return a < b ? a : b; }
mokpnum max(mokpnum a, mokpnum b){ return a > b ? a : b; }

/*
 *   Type A
 * */
MOKP *_mokp_random_A(int n, int np){
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
MOKP *_mokp_random_B(int n, int np){
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
MOKP *_mokp_random_C(int n, int np){
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
            upper = min(1100 - p[0][i], 1000);
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
MOKP *_mokp_random_D(int n, int np){
    MOKP *mokp;
    int i, j;
    mokpnum upper, lower, psum;
    mokpnum **p;

    mokp = mokp_alloc(n, np);
	p = mokp->p;

    mokp->b = 0;
    for( i = 0 ; i < n ; i++ ){
        psum = p[0][i] = mokpnum_rand(1000);
        if( np == 2 ){
            /* second profit */
            lower = max( 900 - p[0][i], 1);
            upper = min(1100 - p[0][i], 1000);
            p[1][i] = lower + mokpnum_rand(upper - lower);
			psum += p[1][i];
        }
		if( np == 3 ){
            /* second profit */
            p[1][i] = mokpnum_rand(1001 - p[0][i]);
			psum += p[1][i];

            /* third profit */
            lower = max( 900 - p[0][i] - p[1][i], 1);
            upper = min(1100 - p[0][i] - p[1][i], 1001 - p[0][i]);
            p[2][i] = lower + mokpnum_rand((upper - lower));
			psum += p[2][i];
		}
        /* further profits... */
		if( np > 3 ){
            for( j = 2 ; j < np ; j++ ){
                p[j][i] = mokpnum_rand(1000);
			}
		}
        mokp->w[i] = psum - 200 + mokpnum_rand(400);

        /* wieght */
        mokp->b += mokp->w[i];
    }
    mokp->b = mokp->b / 2;

    return mokp;
}

/*  Sintethic MOKP instance */
MOKP *mokp_random(int n, int np, char option){
    MOKP *mokp;
    
    switch( option ){
        case 'a':
        case 'A':
		mokp = _mokp_random_A(n, np); break;

        case 'b':
        case 'B':
		mokp = _mokp_random_B(n, np); break;

        case 'c':
        case 'C':
		mokp = _mokp_random_C(n, np); break;

        case 'd':
        case 'D':
		mokp = _mokp_random_D(n, np); break;
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
    mokp_free(mokp);
    
    return mokp2;
}

void mokp_free(MOKP *mokp){
    int i, np;

    np = mokp->np;
    for( i = 0 ; i < np ; i++ )
        free(mokp->p[i]);
    free(mokp->p);
    free(mokp->w);
	free(mokp->idxs);
    free(mokp);

    return;
}

MOKP *mokp_shuffle_idxs(MOKP *mokp){
	mokp->idxs = int_array_shuffle(mokp->idxs, mokp->n);
	return mokp;
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

/* Writes an instance on Zitzler format */
void mokp_write_zitzler(FILE *fout, MOKP *mokp){
	int i, j, n, np;
	n = mokp->n;
	np = mokp->np;

	/* first knapsack */
	for( j = 0 ; j < np ; j++ ){
		if( j > 0 ) fprintf(fout, "=\nknapsack %d:\n", j+1);
		fprintf(fout, " capacity: ");
		if( j == 0 ) mokpnum_fprintf(fout, mokp->b);
		else fprintf(fout, "%d", n);
		fprintf(fout, "\n");
		for( i = 0 ; i < n ; i++ ){
			fprintf(fout, " item %d:\n", i+1);
			fprintf(fout, "  weight: ");
			if( j == 0 ) mokpnum_fprintf(fout, mokp->w[i]);
			else fprintf(fout, "1");
			fprintf(fout, "\n");
			fprintf(fout, "  profit: ");
			mokpnum_fprintf(fout, mokp->p[j][i]);
			fprintf(fout, "\n");
		}
	}

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


/*************************
 *     MOKP Solution     *
 ************************/
/* New MOKP empty solution */
MOKPSol *mokpsol_new_empty(MOKP *mokp){
	MOKPSol *sol;
	int i, n, np;

	n = mokp->n;
	np = mokp->np;

	sol = (MOKPSol*)malloc(sizeof(MOKPSol));
	sol->mokp = mokp;
	sol->x = (mokpval*)malloc(n*sizeof(mokpval));
	for( i = 0 ; i < n ; i++ )
		sol->x[i] = 0;
	sol->profit = (mokpnum*)malloc(np*sizeof(mokpnum));
	for( i = 0 ; i < np ; i++ )
		sol->profit[i] = 0;
	sol->b_left = mokp->b;
	sol->rank = -1;
	sol->msi = NULL;
	
	return sol;
}
/* Free a MOKP solution */
void mokpsol_free(MOKPSol *sol){
	free(sol->x);
	free(sol->profit);
	free(sol);

	return;
}

void mokpsol_fprintf(FILE* out, MOKPSol* sol){
	int i;
	for( i = 0 ; i < sol->mokp->n ; i++ )
		fprintf(out, "%d", sol->x[i]);
	fprintf(out, " (");
	mokpnum_fprintf(out, sol->profit[0]);
	for( i = 1 ; i < sol->mokp->np ; i++ ){
		fprintf(out, ",");
		mokpnum_fprintf(out, sol->profit[i]);
	}
	fprintf(out, ") [");
	mokpnum_fprintf(out, sol->b_left);
	fprintf(out, "]\n");
	return;
}
void mokpsol_printf(MOKPSol* sol){
	return mokpsol_fprintf(stdout, sol);
}
void mokpsol_profit_write(MOKPSol* sol){
	int i, np;
	np = sol->mokp->np;
	mokpnum_fprintf(stdout, sol->profit[0]);
	for( i = 1 ; i < np ; i++ ){
		printf(" ");
		mokpnum_fprintf(stdout, sol->profit[i]);
	}
	printf("\n");
}


/* Dominance comparison between two solutions */
int mokpsol_dom_cmp(MOKPSol *a, MOKPSol *b){
	int i, np;
	int hasBetter = 0;
	int hasWorse = 0;

#ifdef COUNT_MSI_COMP
	if( a->msi ) a->msi->ncomp_++;
	else if( b->msi ) b->msi->ncomp_++;
#endif

	np = a->mokp->np;

	for( i = 0 ; i < np ; i++ )
		if( a->profit[i] < b->profit[i] )
			hasWorse = 1;
		else if( a->profit[i] > b->profit[i] )
			hasBetter = 1;
	if( hasBetter )
		if( hasWorse )
			return SOL_NOTHING;
		else
			return SOL_DOMINATES;
	else
		if( hasWorse )
			return SOL_DOMINATED;
		else
			return SOL_EQUAL;
}
double mokpsol_spacing(MOKPSol *a, MOKPSol *b){
	double spc = 0.0;
	int i, np = a->mokp->np;
	if( a == b )
		return INFINITY;

	for( i = 0 ; i < np ; i++ )
		spc += fabs((double)(a->profit[i] - b->profit[i]));

	return spc;
}
int mokpsol_dominates_(MOKPSol *a, MOKPSol *b){
	return (mokpsol_dom_cmp(a, b) == SOL_DOMINATES);
}
int mokpsol_dominated_by_(MOKPSol *a, MOKPSol *b){
	return (mokpsol_dom_cmp(a, b) == SOL_DOMINATED);
}
double mokpsol_axis_get(MOKPSol *sol, int dim){
#ifdef COUNT_MSI_COMP
	/* TODO: FIXME: aqui, se ambas solucoes estiverem em um MSI, será contado
	 *   valor dobrado no caso de utilizacao da KDTree. */
	if( sol->msi ) sol->msi->ncomp_++;
#endif
	return (double)sol->profit[dim];
}
int mokpsol_profit1_cmp(MOKPSol *a, MOKPSol *b){
#ifdef COUNT_MSI_COMP
	if( a->msi ) a->msi->ncomp_++;
	else if( b->msi ) b->msi->ncomp_++;
#endif
	return ( a->profit[0] - b->profit[0] );
}
KDTree *mokpsol_new_kdtree(int ndim){
	return kdtree_new(ndim, (kdtree_eval_f)mokpsol_axis_get);
}
MOKPSol *mokpsol_find_dominant_kdtree(MOKPSol *sol, KDTree *kdt){
	MOKPSol *dominant = NULL;
	double *bounds;
	int i, ndim;

	ndim = kdt->ndim;
	bounds = (double*)malloc(2*ndim*sizeof(double));
	for( i = 0 ; i < ndim ; i++ ){
		bounds[2*i] = sol->profit[i];
		bounds[2*i+1] = INFINITY;
	}

	dominant = kdtree_range_search_r(
		kdt,
		bounds,
		(property_f_r)mokpsol_dominates_,
		sol);

	free(bounds);

	return dominant;
}
MOKPSol *mokpsol_find_dominant_avl(MOKPSol *sol, AVLTree *avlt){
	MOKPSol *dominant = NULL;
	MOKPSol *sol2;
	AVLIter *iter;

    iter = avl_get_higher_lower_than(avlt, sol);
	while( (sol2 = avliter_forward(iter)) && !dominant )
		if( mokpsol_dominates_(sol2, sol) )
			dominant = sol2;

	avliter_free(iter);

	return dominant;
}
MOKPSol *mokpsol_find_dominant_list(MOKPSol *sol, List *list){
	ListIter *iter;
	MOKPSol *sol2, *dominant = NULL;

	iter = list_get_first(list);
	while( (sol2 = (MOKPSol*)listiter_forward(iter)) && !dominant )
		if( mokpsol_dominates_(sol2, sol) )
			dominant = sol2;

	listiter_free(iter);

	return dominant;
}
MOKPSol *msi_find_dominant(MOKPSolIndexer *msi, MOKPSol *sol){
	if( msi->ndim == 0 )
		return mokpsol_find_dominant_list(sol, msi->tad.list);
	if( msi->ndim == 1 )
		return mokpsol_find_dominant_avl(sol, msi->tad.avl);
	return mokpsol_find_dominant_kdtree(sol, msi->tad.kdt);
}

MOKPSol *mokpsol_find_dominanted_list(MOKPSol *sol, List *list){
	ListIter *iter;
	MOKPSol *sol2, *dominated = NULL;

	iter = list_get_first(list);
	while( (sol2 = (MOKPSol*)listiter_forward(iter)) && !dominated )
		if( mokpsol_dominated_by_(sol2, sol) )
			dominated = sol2;

	listiter_free(iter);

	return dominated;
}
MOKPSol *mokpsol_find_dominanted_avl(MOKPSol *sol, AVLTree *avl){
	MOKPSol *dominated = NULL;
	MOKPSol *sol2;
	AVLIter *iter;

    iter = avl_get_lower_higher_than(avl, sol);
	while( (sol2 = avliter_backward(iter)) && !dominated )
		if( mokpsol_dominates_(sol2, sol) )
			dominated = sol2;

	avliter_free(iter);

	return dominated;
}
MOKPSol *mokpsol_find_dominanted_kdtree(MOKPSol *sol, KDTree *kdt){
	MOKPSol *dominated = NULL;
	double *bounds;
	int i, ndim;

	ndim = kdt->ndim;
	bounds = (double*)malloc(2*ndim*sizeof(double));
	for( i = 0 ; i < ndim ; i++ ){
		bounds[2*i] = 0;
		bounds[2*i+1] = sol->profit[i];
	}

	dominated = kdtree_range_search_r(
		kdt,
		bounds,
		(property_f_r)mokpsol_dominated_by_,
		sol);

	free(bounds);

	return dominated;
}
MOKPSol *msi_find_dominated(MOKPSolIndexer *msi, MOKPSol *sol){
	if( msi->ndim == 0 )
		return mokpsol_find_dominanted_list(sol, msi->tad.list);
	if( msi->ndim == 1 )
		return mokpsol_find_dominanted_avl(sol, msi->tad.avl);
	return mokpsol_find_dominanted_kdtree(sol, msi->tad.kdt);
}

MOKPSolIndexer *msi_new(int ndim){
	MOKPSolIndexer *msi;
	msi = (MOKPSolIndexer*)malloc(sizeof(MOKPSolIndexer));
	msi->ndim = ndim;
	if( ndim == 0)
		msi->tad.list = list_new();
	if( ndim == 1 )
		msi->tad.avl = new_avltree( (avl_cmp_f)mokpsol_profit1_cmp );
	if( ndim  > 1 )
		msi->tad.kdt = kdtree_new(ndim, (kdtree_eval_f)mokpsol_axis_get);
	msi->ncomp_ = 0;
	return msi;
}
MOKPSolIndexer *msi_insert(MOKPSolIndexer *msi, MOKPSol *sol){
	if( msi->ndim == 0)
		list_insert(msi->tad.list, sol);
	if( msi->ndim == 1 )
		avl_insert(msi->tad.avl, sol);
	if( msi->ndim  > 1 )
		kdtree_insert(msi->tad.kdt, sol);
	sol->msi = msi;
		
	return msi;
}
void msi_free(MOKPSolIndexer* msi){
	if( msi->ndim == 0 )
		list_free(msi->tad.list);
	if( msi->ndim == 1 )
		avl_free(msi->tad.avl);
	if( msi->ndim  > 1 )
		kdtree_free(msi->tad.kdt);

	free(msi);

	return;
}
int msi_get_n(MOKPSolIndexer *msi){
	if( msi->ndim == 0 )
		return msi->tad.list->n;
	if( msi->ndim == 1 )
		return msi->tad.avl->n;
	return msi->tad.kdt->n;
}
MOKPSol **msi_get_all(MOKPSolIndexer *msi){
	MOKPSol **all;
	int n;

	n = msi_get_n(msi);
	if( msi->ndim == 0 )
		all = (MOKPSol**)list_get_all(msi->tad.list);
	if( msi->ndim == 1 )
		all = (MOKPSol**)avl_to_array(msi->tad.avl);
	if( msi->ndim  > 1 )
		all = (MOKPSol**)kdtree_get_all(msi->tad.kdt);
	return all;
}
void msi_apply_all(MOKPSolIndexer *msi, void(*f)(void*)){
	if( msi->ndim == 0 )
		list_apply(msi->tad.list, f);
	if( msi->ndim == 1 )
		avl_apply_to_all(msi->tad.avl, f);
	if( msi->ndim  > 1 )
		kdtree_apply_to_all(msi->tad.kdt, f);
}
void msi_apply_all_r(MOKPSolIndexer *msi, void(*f)(void*, void*), void *arg){
	if( msi->ndim == 0 )
		list_apply_r(msi->tad.list, f, arg);
	if( msi->ndim == 1 )
		avl_apply_to_all_r(msi->tad.avl, f, arg);
	if( msi->ndim  > 1 )
		kdtree_apply_to_all_r(msi->tad.kdt, f, arg);
}
MOKPSol *msi_remove(MOKPSolIndexer *msi, void *x){
	if( msi->ndim == 0 )
		list_remove(msi->tad.list, x);
	if( msi->ndim == 1 )
		avl_remove(msi->tad.avl, x);
	if( msi->ndim  > 1 )
		kdtree_remove(msi->tad.kdt, x);
}
int msi_set_coverage(MOKPSolIndexer *msi_a, MOKPSolIndexer *msi_b){
	int ndominated_a;
	MSIIter *iter;
	MOKPSol *sol;

	ndominated_a = 0;
	iter = msiiter_new(msi_a);
	while( sol = msiiter_forward(iter) )
		if( msi_find_dominant(msi_b, sol) )
			ndominated_a++;
	msiiter_free(iter);
	return ndominated_a;
}
double msi_spacing(MOKPSolIndexer *msi){
	MSIIter *iter1, *iter2;
	MOKPSol *sol1, *sol2, *closest;
	double closest_dist, sol2_dist;
	int i, n, np;
	double avg_dist;
	double sum, res;
	double *dists;

	fprintf(stderr, "warning: %s implementation not performant yet.\n",
		__PRETTY_FUNCTION__);
	/* TODO: improve performance by using the TADs. */

	n = msi_get_n(msi);
	if( !n )
		return INFINITY;


	np = sol1->mokp->np;
	dists = (double*)malloc(n*sizeof(double));
	avg_dist = 0.;
	i = 0;

	iter1 = msiiter_new(msi);
	sol1 = msiiter_get(iter1);
	while( sol1 ){
		closest_dist = INFINITY;
		iter2 = msiiter_new(msi);
		sol2 = msiiter_get(iter2);
		while( sol2 ){
			sol2_dist = mokpsol_spacing(sol1, sol2);
			if( sol2_dist < closest_dist )
				closest_dist = sol2_dist;
			sol2 = msiiter_forward(iter2);
		}
		msiiter_free(iter2);
		dists[i++] = closest_dist;
		avg_dist += closest_dist;
		sol1 = msiiter_forward(iter1);
	}
	msiiter_free(iter1);

	avg_dist /= (double)n;
	sum = 0.;
	for( i = 0 ; i < n ; i++ )
		sum += (avg_dist - dists[i])*(avg_dist - dists[i]);
	res = sqrt(sum/(double)(n-1));
	free(dists);

	return res;
}
double msi_hvolume(MOKPSolIndexer *msi){
	MOKPSol **sols;
	int i, j, n, np;
	double *data, *ref, hvol;

	sols = msi_get_all(msi);
	np = sols[0]->mokp->np;
	n = msi_get_n(msi);
	data = (double*)malloc((n*np)*sizeof(double));
	ref = (double*)malloc(np*sizeof(double));

	/* reference */
	for( j = 0 ; j < np ; j++ )
		ref[j] = 0.0;

	/* data */
	for( i = 0 ; i < n ; i++ )
		for( j = 0 ; j < np ; j++ )
			data[i*np + j] = -sols[i]->profit[j];

	hvol = fpli_hv(data, np, n, ref);

	free(sols);
	free(data);
	free(ref);

	return hvol;
}

/* MOKP Solution Indexed Iterator */
MSIIter *msiiter_new(MOKPSolIndexer *msi){
	MSIIter *iter;
	iter = (MSIIter*)malloc(sizeof(MSIIter));
	iter->msi = msi;
	if( iter->msi->ndim == 0 )
		iter->tad.listi = listiter_new(msi->tad.list);
	if( iter->msi->ndim == 1 )
		iter->tad.avli = avl_get_first(msi->tad.avl);
	if( iter->msi->ndim  > 1 )
		iter->tad.kdti = kdtiter_new(msi->tad.kdt);
	return iter;
}
MOKPSol *msiiter_get(MSIIter *iter){
	if( iter->msi->ndim == 0 )
		return listiter_get(iter->tad.listi);
	if( iter->msi->ndim == 1 )
		return avliter_get(iter->tad.avli);
	if( iter->msi->ndim  > 1 )
		return kdtiter_get(iter->tad.kdti);
}
MOKPSol *msiiter_forward(MSIIter *iter){
	if( iter->msi->ndim == 0 )
		return listiter_forward(iter->tad.listi);
	if( iter->msi->ndim == 1 )
		return avliter_forward(iter->tad.avli);
	if( iter->msi->ndim  > 1 )
		return kdtiter_forward(iter->tad.kdti);
}
void msiiter_free(MSIIter *iter){
	if( iter->msi->ndim == 0 )
		listiter_free(iter->tad.listi);
	if( iter->msi->ndim == 1 )
		avliter_free(iter->tad.avli);
	if( iter->msi->ndim  > 1 )
		kdtiter_free(iter->tad.kdti);
	free(iter);
}
void msiiter_remove(MSIIter *iter){
	if( iter->msi->ndim == 0 )
		listiter_remove(iter->tad.listi);
	if( iter->msi->ndim == 1 )
		avliter_remove(iter->tad.avli);
	if( iter->msi->ndim  > 1 )
		kdtiter_remove(iter->tad.kdti);
}

Archive *archive_new(MOKP* mokp, int nmax, int ndim){
	Archive *arch;
	arch = (Archive*)malloc(sizeof(Archive));
	arch->mokp = mokp;
	arch->nmax = nmax;
	arch->pareto = msi_new(ndim);
}
int archive_get_n(Archive *arch){
	return msi_get_n(arch->pareto);
}

void archive_propose_sol(Archive *arch, MOKPSol *sol){
	MSIIter *iter;
	MOKPSol *dominant, *dominated;
	int cmp_res;

	dominant = msi_find_dominant( arch->pareto, sol );
	if( dominant )
		return;
	while( dominated = msi_find_dominated( arch->pareto, sol ) ){
		msi_remove(arch->pareto, dominated );
		mokpsol_free(dominated);
	}

	msi_insert(arch->pareto, mokpsol_copy(sol));

	return;
}
MOKPSolIndexer *archive_free(Archive *archive){
	MOKPSolIndexer *msi;
	msi = archive->pareto;
	free(archive);
	return msi;
}

/* Insert item (if not) */
MOKPSol *mokpsol_insert_item(MOKPSol *sol, int idx){
	int i;

	MOKP *mokp;
	if(sol->x[idx])
		return sol;

	mokp = sol->mokp;
	sol->x[idx] = 1;
	sol->b_left -= mokp->w[idx];
	for( i = 0 ; i < mokp->np ; i++ )
		sol->profit[i] += mokp->p[i][idx];

	return sol;
}
/* Remove an item from solution */
MOKPSol *mokpsol_rm_item(MOKPSol *sol, int idx){
	int i;
	if( !sol->x[idx] )
		return sol;

	sol->x[idx] = 0;
	sol->b_left += sol->mokp->w[idx];
	for( i = 0 ; i < sol->mokp->np ; i++ )
		sol->profit[i] -= sol->mokp->p[i][idx];

	return sol;
}
/* New random solution */
MOKPSol *mokpsol_new_random(MOKP *mokp){
	int i, n, *idxs;
	MOKPSol *sol;

	n = mokp->n;
	sol = mokpsol_new_empty(mokp);
	idxs = (int*)malloc(n*sizeof(int));
	for( i = 0 ; i < n ; i++ )
		idxs[i] = i;
	idxs = int_array_shuffle(idxs, n);
	for( i = 0 ; i < n ; i++ )
		if( sol->b_left >= mokp->w[idxs[i]] )
			sol = mokpsol_insert_item(sol, idxs[i]);

	free(idxs);

	return sol;
}
/* Copy a MOKP solution */
MOKPSol *mokpsol_copy(MOKPSol *sol){
	MOKPSol *new;
	int i, n, np;

	n = sol->mokp->n;
	np = sol->mokp->np;

	new = (MOKPSol*)malloc(sizeof(MOKPSol));
	new->mokp = sol->mokp;
	new->x = (mokpval*)malloc(n*sizeof(mokpval));
	new->profit = (mokpnum*)malloc(np*sizeof(mokpnum));
	new->b_left = sol->b_left;
	memcpy(new->x, sol->x, n*sizeof(mokpval));
	memcpy(new->profit, sol->profit, np*sizeof(mokpnum));
	new->msi = NULL;
	
	return new;
}
MOKPSol *mokpsol_flip_item(MOKPSol *sol, int idx){
	if( sol->x[idx] ) mokpsol_rm_item(sol, idx);
	else mokpsol_insert_item(sol, idx);
}
MOKPSol *mokpsol_repair(MOKPSol *sol){
	int i, idx, *idxs;
	i = 0;
	idxs = sol->mokp->idxs;
	idx = idxs[i];
	while( sol->b_left < 0 ){
		while( sol->x[idx] == 0)
			idx = idxs[++i];
		sol = mokpsol_rm_item(sol, idx);
	}
	return sol;
}

/* cross two solutions */
MOKPSol *mokpsol_cross(MOKPSol *child, MOKPSol *father, int c){
	int *idxs;
	int a, b, i, n, idx;
	idxs = child->mokp->idxs;

	for( i = 0 ; i < c ; i++ ){
		idx = idxs[i];
		a = father->x[idx];
		b = child->x[idx];
		if( a != b )
			child = mokpsol_flip_item(child, idx);
	}

	/* repair, if not feasiable? */
	if( child->b_left < 0 ){
		mokp_shuffle_idxs(child->mokp);
		child = mokpsol_repair(child);
	}
	return child;
}

/*******************************
 *     MOKPNode (DynProg)      *
 ******************************/
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

