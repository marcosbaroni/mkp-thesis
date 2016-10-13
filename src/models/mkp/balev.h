#ifndef BALEV_H
#define BALEV_H 1

#include "mkp.h"
#include "domset.h"
#include "mkp-soputils.h"
#include "lbucket.h"
/*
 *  TODO: *Testar completar as soluções usando variaveis livres com maior eficiencia
 */

typedef struct BalevNode{
    DomSetNode domset;
    mkpnum opt;
}BalevNode;

void mkp_balev(MKPSol *mkpsol, int use_lbucket);

#endif
