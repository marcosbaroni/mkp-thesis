#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "balev.h"

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
 * number of iteration of enumeration algorithm
 */
MKPSol *balev_enum(MKPSol *mkpsol, int k){
    return mkpsol;
}

void mkp_balev(MKPSol *mkpsol){
    mkpnum *uppers;
    MKPSol *best_from_enum;
    int i, n, m, k;
    int *ord;

    n = mkpsol->mkp->n;
    m = mkpsol->mkp->m;

    /* compute upper-bounds */
    uppers = compute_opposite_uppers(mkpsol);
    ord = double_index_sort(uppers, n);
    for( i = 0 ; i < n ; i++ )  /* flooring uppers */
        uppers[i] = floor(uppers[i]);

    for( i = 0 ; i < n ; i++ )
        printf("%03d: %.2lf\n", ord[i]+1, uppers[ord[i]]);

    k = 18 - floor(log2(m+2));
    best_from_enum = balev_enum(mkpsol, k);

    free(ord);
    free(uppers);

    return;
}

