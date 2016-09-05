#include <stdio.h>
#include <stdlib.h>
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

void mkp_balev(MKPSol *mkpsol){
    mkpnum *uppers;
    int i, n;

    n = mkpsol->mkp->n;
    uppers = compute_opposite_uppers(mkpsol);

    for( i = 0 ; i < n ; i++ ){
        printf("%03d: %.2lf\n", i+1, uppers[i]);
    }

    free(uppers);

    return;
}

