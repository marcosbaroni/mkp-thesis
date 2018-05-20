/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: december, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#ifndef BAZGAN_H
#define BAZGAN_H 1

#include <time.h>

#include "../../utils/avl.h"
#include "../../utils/list.h"

#include "mokp.h"

#ifndef SOL_ARRAY_ENABLED
#define SOL_ARRAY_ENABLED 1
#endif

typedef unsigned long long ulonglong;
typedef AVLNode* BNodeIter;

typedef struct BazganNode{
    struct Bazgan *bazgan;
	int idx;	      /* the index of item which was last fixed */
    mokpnum profit[6];
    mokpnum b_left;
#if SOL_ARRAY_ENABLED
    ulonglong *sol; /* Solution Representation (bits) */
#endif
}BazganNode;

BazganNode *bnode_new_empty(struct Bazgan *baz);
BazganNode *bnode_copy(BazganNode *bnode);
BazganNode *bnode_new_children(BazganNode *bnode, int idx);
void bnode_free(BazganNode *bnode);
int bnode_lex_cmp(BazganNode *n1, BazganNode *n2);
int bnode_lex_cmp_inv(BazganNode *n1, BazganNode *n2);
int bnode_dom_cmp(BazganNode *n1, BazganNode *n2);
int bnode_dominates(BazganNode *b1, BazganNode *b2);
int bnode_is_dominated_by(BazganNode *b1, BazganNode *b2);
double bnode_axis_val(BazganNode *n1, int axis);
void bnode_fprintf(FILE *fout, BazganNode *node);
double *bnode_get_dominant_bounds(BazganNode *bnode, int ndim, int just_profits);
double *bnode_get_dominated_bounds(BazganNode *bnode, int ndim, int just_profits);
BazganNode *bnode_get_upper_bound(BazganNode*, int, int**);
BazganNode *bnode_get_lower_bound(BazganNode*, int*, int);

typedef struct Bazgan{
    MOKP *mokp;
    int solsize;
    clock_t ping, pong;
    int **best_profit_cost_order;
    int *max_order;
    int *sum_order;
    int max_nd;

    long long _ncomparison;
    int just_profits;

    AVLTree *avl_lex;
}Bazgan;

Bazgan *bazgan_new(MOKP *mokp);
void bazgan_free(Bazgan *bazgan);
void bazgan_ping(Bazgan *bazgan);
void bazgan_pong(Bazgan *bazgan);
double bazgan_get_seconds(Bazgan *bazgan);
void bazgan_fprint_nodes(FILE *out, Bazgan *bazgan);
void bazgan_print_nodes( Bazgan *bazgan);
void bazgan_fprint_nodes_lex(FILE *out, Bazgan *bazgan);
void bazgan_print_nodes_lex(Bazgan *bazgan);
void bazgan_print_pareto(Bazgan *bazgan);

List *bazgan_exec_simple(MOKP *mokp, int k);
Bazgan *bazgan_exec(MOKP *mokp, int kmax, int ndim);
Bazgan *bazgan_brute(MOKP *mokp, int k);
void bazgan_fprint_summary(FILE *out, Bazgan *bazgan);
void bazgan_print_summary(Bazgan *bazgan);

#endif

