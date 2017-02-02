#ifndef BAZGAN_H
#define BAZGAN_H 1

#include "../../utils/avl.h"
#include "../../utils/list.h"

#include "mokp.h"

typedef unsigned long long ulonglong;

typedef struct BazganNode{
    struct Bazgan *bazgan;
	int idx;	      /* the index of item which was fixed */
    double *profit;
    double b_left;
    ulonglong *sol; /* Solution Representation (bits) */
}BazganNode;

BazganNode *bnode_new_empty(struct Bazgan *baz);
BazganNode *bnode_new_children(BazganNode *bnode, int idx);
void bnode_free(BazganNode *bnode);
int bnode_lex_cmp(BazganNode *n1, BazganNode *n2);
int bnode_dominates(BazganNode *b1, BazganNode *b2);
int bnode_is_dominated_by(BazganNode *b1, BazganNode *b2);
double bnode_axis_val(BazganNode *n1, int axis);
void bnode_fprintf(FILE *fout, BazganNode *node);
double *bnode_dominant_bounds(BazganNode *bnode);

typedef struct Bazgan{
    MOKP *mokp;
    int solsize;

    AVLTree *avl_lex;
}Bazgan;

Bazgan *bazgan_new(MOKP *mokp);
void bazgan_free(Bazgan *bazgan);

List *bazgan_exec_simple(MOKP *mokp, int k);
void bazgan_exec(MOKP *mokp, char ordering_type, int kmax);

#endif

