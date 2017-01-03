#include "mokp.h"
#include "order.h"
#include "../../utils/util.h"

/********************************************************************
 * Returns an item oredinr (Check Bazgan 2009, page 8)
 *   mokp: the MOKP instance
 *   opt:
 *      'M' for max
 *      'm' for min
 *      's' (or 'S') for sum
 *******************************************************************/
int *mokp_get_order(MOKP *mokp, char opt){
    int n, np, i, j, _max, _min;
    double **p, *w;
    double **ratios;
    int **rank;
    double *sums;
    double *metric;
    int **ordering;
    int *idxs;

    n = mokp->n;
    np = mokp->np;
    p = mokp->p;
    w = mokp->w;

    ratios = double_matrix_alloc(np, n);
    sums = (double*)malloc(n*sizeof(double));
    metric = (double*)malloc(n*sizeof(double));

    printf("ORDER OPT:%c\n", opt);

    /* computing p/w ratios */
    for( i = 0 ; i < np ; i++ )
        for( j = 0 ; j < n ; j++ )
            ratios[i][j] = p[i][j] / w[j];

    printf("ratios:\n");
    double_matrix_fprintf(stdout, ratios, np, n);

    /* ordering of items, given its rank */
    ordering = (int**)malloc(np*sizeof(int*));
    for( i = 0 ; i < np ; i++ )
        ordering[i] = double_index_sort(ratios[i], n);

    printf("ordering:\n");
    int_matrix_fprintf(stdout, ordering, np, n);

    /* ranking items on each dim */
    rank = (int**)malloc(np*sizeof(int*));
    for( i = 0 ; i < np ; i++ ){
        rank[i] = (int*)malloc(n*sizeof(int));
        for( j = 0 ; j < n ; j++ ){
            /* obs.: the first in rank (rank = 1) is the item <ordering[i][0]>
             * , i.e., the rank[i] of item <ordering[i][0]> is 1. */
            rank[i][ordering[i][j]] = j+1;
        }
    }

    printf("rank:\n");
    int_matrix_fprintf(stdout, rank, np, n);

    /* sum the ranks */
    for( j = 0 ; j < n ; j++ ){
        sums[j] = 0;
        for( i = 0 ; i < np ; i++ )
            sums[j] += rank[i][j];
    }

    /* decide metric */
    if( opt == 's' ){
        /* Sum */
        for( i = 0 ; i < n ; i++ )
            metric[i] = sums[i];
    }else if( opt == 'M' ){
        /* Max */
        for( i = 0 ; i < n ; i++ ){
            _max = rank[0][i];
            for( j = 0 ; j < np ; j++ )
                if( _max < rank[j][i] )
                    _max = rank[j][i];
            metric[i] = _max + (sums[i]/(double)(np*n));
        }
    }else if( opt == 'm' ){
        /* Min */
        for( i = 0 ; i < n ; i++ ){
            _min = rank[0][i];
            for( j = 0 ; j < np ; j++ )
                if( _min > rank[j][i] )
                    _min = rank[j][i];
            metric[i] = _min + (sums[i]/(double)(np*n));
        }
    }else if( opt == 'a' ){
        /* TODO: Testar normalizar cada valor de profit, dividindo-o pelo
         * total daquele profit em todos os itens e depois, para cada item,
         * dividir a soma dos profits normalizados pelo peso do item. */
    }

    /* computing order */
    idxs = double_index_sort(metric, n);

    /* DEBUGGING */
    printf("Rank:\n");
    int_matrix_fprintf(stdout, rank, np, n);
    printf("metric:\n");
    double_array_fprint(stdout, metric, n);
    printf("\n");
    printf("idxs:\n");
    int_array_fprint(stdout, idxs, n);
    printf("\n");

    /* freeing variables */
    for( i = 0 ; i < np ; i++ ){
        free(ordering[i]);
        free(ratios[i]);
        free(rank[i]);
    }

    free(ordering);
    free(ratios);
    free(rank);
    free(sums);
    free(metric);

    return idxs;
}

