#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "balev.h"
#include "mkp.h"
#include "domset.h"
#include "lbucket.h"

/*
 * Finds the upper bound of a variable fixed on opposite size of a solution
 */
mkpnum *compute_opposite_uppers(MKPSol *mkpsol){
    MKP *mkp;
    int i, n;
    int *fixs;
    mkpnum* uppers;

    mkp = mkpsol->mkp;
    n = mkp->n;
    uppers = (mkpnum*)malloc(n*sizeof(mkpnum));
    fixs = (int*)malloc(n*sizeof(int));

    /* all variables are free, by default */
    for( i = 0 ; i < n ; i++ )
        fixs[i] = -1;

    /* for each variable, check upper bound */
    for( i = 0 ; i < n ; i++ ){
        fixs[i] = 1 - mkpsol->x[i]; /* oposite value */
        uppers[i] = mkp_get_lp_ub(mkp->p, mkp->w, mkp->b, mkp->n, mkp->m, fixs); /* LP upper bound*/
        fixs[i] = -1;
    }

    free(fixs);

    return uppers;
}

/*
 * k: max number of iteration (items inserted)
 */
MKPSol *balev_enum(MKPSol *mkpsol, int *idxs, int k, LinkedBucket *lbucket){
    MKP *mkp;
    DomSetTree *dstree;
    DomSetNode *dsnode;
    DomSetNode *best_node;
    MKPSol *mkpsol_new;
    mkpnum best_half_profit;
    mkpnum *b_left_k; /* space remaining without items [k+1, ..., n] */
    int i, j, n, m, n_nodes;
    int feasible;

    mkp = mkpsol->mkp;
    n = mkp->n;
    m = mkp->m;

    /* computing b_left needed to fit items "before" the (k+1)-th item */
    b_left_k = (mkpnum*)malloc(m*sizeof(mkpnum));
    best_half_profit = mkpsol->obj;
    for( j = 0 ; j < m ; j++ )
        b_left_k[j] = mkp->b[j];
    for( i = k ; i < n ; i++ ){
        if( mkpsol->x[i] ){
            for( j = 0 ; j < m ; j++ )
                b_left_k[j] -= mkp->w[j][i];
            best_half_profit -= mkp->p[i];
        }
    }

    /* executing nemhauser-Ullman */
    dstree = dstree_new(mkpsol->mkp);
    for( i = 0 ; i < k ; i++ ){
        printf(" iter %d / %d\n", i, k);
        dstree = lbucket_dstree_dynprog(dstree, idxs[i], lbucket);
    }

    best_node = NULL;

    /* finding best "half-feasible" node */
    n_nodes = dstree->n;
    dsnode = dstree->root;
    for( i = 0 ; i < n_nodes ; i++ ){
        if( best_half_profit < dsnode->profit ){
            for( j = 0 ; j < m ; j++ ){
                if( b_left_k[j] < dsnode->b_left[j] )
                    { break; }
                best_half_profit = dsnode->profit;
                best_node = dsnode;
            }
        }
        dsnode = dsnode->next;
    }

    /* Extracting solution found (if better) */
    mkpsol_new = NULL;
    if( best_node ){
        mkpsol_new = dsnode_get_mkpsol( best_node );
        for( i = k ; i < n ; i++ )
            if( mkpsol->x[idxs[i]] )
                mkpsol_add_item(mkpsol_new, idxs[i]);
    }

    free(b_left_k);
    dstree_free(dstree);
    if(lbucket)
        lbucket_free(lbucket);

    return mkpsol_new;
}

void mkp_balev(MKPSol *mkpsol, int use_lbucket){
    mkpnum *uppers;
    MKPSol *best_from_enum;
    LinkedBucket *lbucket;
	mkpnum **max_b_lefts, sum;

    int i, n, m, k;
    int *ord;

    int ndim, nsub; // for linked bucket
    char type;

    n = mkpsol->mkp->n;
    m = mkpsol->mkp->m;
    /* linked bucket setup*/
    lbucket = NULL;
    ndim = 2;
    nsub = 10;
    type = 'l';

    /* compute upper-bounds */
    uppers = compute_opposite_uppers(mkpsol);
    ord = double_index_sort(uppers, n);
    for( i = 0 ; i < n ; i++ )  /* flooring uppers */
        uppers[i] = floor(uppers[i]);

    /* Prepare Linked Buckets */
	max_b_lefts = lbucket_prepare_max_b_left(mkpsol->mkp, ndim, nsub, type);
    if( use_lbucket )
	    lbucket = lbucket_new(max_b_lefts, nsub, ndim);
	for( i = 0 ; i < ndim ; i++ ) /* freeing max_b_lefts */
		free(max_b_lefts[i]);
	free(max_b_lefts);

    /* execute enumeration */
    k = 18 - floor(log2(m+2));
    if( k > (int)(n*0.6) ) k = (int)(n*0.76); /* FIXME: k cant be > n */
    best_from_enum = balev_enum(mkpsol, ord, k, lbucket);

    /* print solution */
    printf("Solution given:\n");
    mkpsol_fprint(stdout, mkpsol, 0);
    printf("Solution found:\n");
    if( best_from_enum ) mkpsol_fprint(stdout, best_from_enum, 0);
    else printf("<none>\n");

    /* free memory */
    if(best_from_enum)
        free(best_from_enum);
    free(ord);
    free(uppers);

    return;
}

