#ifndef BAZGAN_H
#define BAZGAN_H 1

#include "../../utils/avl.h"

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

typedef struct Bazgan{
    MOKP *mokp;
    int solsize;

    AVLTree *avl_lex;
}Bazgan;

Bazgan *bazgan_new(MOKP *mokp);
void bazgan_free(Bazgan *bazgan);

void bazgan_exec(MOKP *mokp, char ordering_type, int kmax);

#endif

