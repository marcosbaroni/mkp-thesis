#ifndef BAZGAN_H
#define BAZGAN_H 1

#include <time.h>

#include "../../utils/avl.h"
#include "../../utils/list.h"

#include "mokp.h"

typedef unsigned long long ulonglong;
typedef AVLNode* BNodeIter;

typedef struct BazganNode{
    struct Bazgan *bazgan;
	int idx;	      /* the index of item which was last fixed */
    double *profit;
    double b_left;
    ulonglong *sol; /* Solution Representation (bits) */
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
double *bnode_get_dominant_bounds(BazganNode *bnode, int ndim);
double *bnode_get_dominated_bounds(BazganNode *bnode, int ndim);
BazganNode *bnode_get_upper_bound(BazganNode*, int, int**);
BazganNode *bnode_get_lower_bound(BazganNode*, int, int*);

typedef struct Bazgan{
    MOKP *mokp;
    int solsize;
    clock_t ping, pong;
    int **best_profit_cost_order;

    long long _ncomparison;

    AVLTree *avl_lex;
}Bazgan;

Bazgan *bazgan_new(MOKP *mokp);
void bazgan_free(Bazgan *bazgan);
void bazgan_ping(Bazgan *bazgan);
void bazgan_pong(Bazgan *bazgan);
double bazgan_get_seconds(Bazgan *bazgan);
void bazgan_fprint_nodes(FILE *out, Bazgan *bazgan);

List *bazgan_exec_simple(MOKP *mokp, int k);
Bazgan *bazgan_exec(MOKP *mokp, char ordering_type, int kmax, int ndim);

#endif

