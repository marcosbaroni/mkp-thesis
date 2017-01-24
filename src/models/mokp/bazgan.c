#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../../utils/avl.h"

#include "mokp.h"
#include "bazgan.h"


void ulonglongs_bits_fprintf(FILE *fout, ulonglong *sol, int nbits){
    int i, j, ullsize, n, pos;
    ulonglong ull;

    ullsize = 8*sizeof(ulonglong);
    for( i = 0 ; i < nbits ; i++ ){
        j = i/ullsize;
        ull = sol[j];
        pos = i - (j*ullsize);
        fprintf(fout, "%c", ((1ULL << pos) & ull) ? '1' : '0');
    }
}

/*******************************************************************************
 *      BAZGAN NODE
*******************************************************************************/
BazganNode *bnode_alloc(Bazgan *baz){
    BazganNode *bnode;

    bnode = (BazganNode*)malloc(sizeof(BazganNode));
    bnode->bazgan = baz;
    bnode->profit = (double*)malloc(baz->mokp->np*sizeof(double));
    bnode->sol = (ulonglong*)malloc(baz->solsize*sizeof(ulonglong));

    return bnode;
}

void bnode_free(BazganNode *bnode){
    free(bnode->profit);
    free(bnode->sol);
    free(bnode);
    
    return;
}

BazganNode *bnode_new_empty(Bazgan *baz){
    BazganNode *bnode;
    int i, np, solsize;

    solsize = baz->solsize;

    bnode = bnode_alloc(baz);
    bnode->idx = -1;
    for( i = 0 ; i < baz->mokp->np ; i++ )
        bnode->profit[i] = 0.0;
    for( i = 0 ; i < solsize ; i++ )
        bnode->sol[i] = 0ULL;
    bnode->b_left = baz->mokp->b;

    return bnode;
}

BazganNode *bnode_new_children(BazganNode *bnode, int idx){
    BazganNode *new_bnode;
    MOKP *mokp;
    int i, np, pos;

    mokp = bnode->bazgan->mokp;
    np = mokp->np;

    new_bnode = bnode_alloc(bnode->bazgan);
    new_bnode->idx = idx;
    for( i = 0 ; i < np ; i++ )
        new_bnode->profit[i] = bnode->profit[i] + mokp->p[i][idx];
    new_bnode->b_left = bnode->b_left - mokp->w[idx];
    memcpy(new_bnode->sol, bnode->sol, bnode->bazgan->solsize*sizeof(ulonglong));
    i = idx / (8*sizeof(ulonglong));
    pos = (idx - i*(8*sizeof(ulonglong)));
    new_bnode->sol[i] |= (1ULL << pos);

    return new_bnode;
}

int bnode_lex_cmp(BazganNode *n1, BazganNode *n2){
    int res, solsize, i;

    res = n1->b_left - n2->b_left;

    if( res )
        return res;

    solsize = n1->bazgan->solsize;
    for( i = 0 ; i < solsize ; i++ )
        if( res = (n1->sol[i] > n2->sol[i]) )
            return res;

    return 0;
}

double bnode_axis_val(BazganNode *n1, int axis){
}

int *get_mokp_new_ordering(MOKP *mokp, char ordering_type){
    int *idxs;
    int i, n, np;

    n = mokp->n;
    np = mokp->np;

    idxs = (int*)malloc(n*sizeof(int));
    for( i = 0 ; i < n ; i++ )
        idxs[i] = i;

    return idxs;
}

/*******************************************************************************
 *      BAZGAN INSTANCE
*******************************************************************************/
Bazgan *bazgan_new(MOKP *mokp){
    int n, np;
    Bazgan *baz;

    n = mokp->n;
    baz = (Bazgan*)malloc(sizeof(Bazgan));

    baz->mokp = mokp;
    baz->solsize = ((n-1)/8)+1;
    baz->avl_lex = new_avltree((avl_cmp_f)bnode_lex_cmp);

    return baz;
}

void bazgan_free(Bazgan *bazgan){
    free(bazgan);
}

void bazgan_exec(MOKP *mokp, char ordering_type, int kmax){
    Bazgan *bazgan;
    MOKP *reord_mokp;
    int *idxs;

    BazganNode *n1, *n2, *n3;

    /* reordering indexes */
    idxs = get_mokp_new_ordering(mokp, ordering_type);
    reord_mokp = mokp_reorder(mokp, idxs);
    free(idxs);
    mokp_write(stdout, reord_mokp);

    /* creating bazgan execution instance */
    bazgan = bazgan_new(reord_mokp);

    /* freeing structures */
    bazgan_free(bazgan);
    mokp_free(reord_mokp);

    return;
}

