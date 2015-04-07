#include <stdio.h>
#include <stdlib.h>
#include "domset.h"

DomSetTree *dstree_new(MKPSol *mkpsol){
	DomSetTree *dstree;

	dstree = (DomSetTree*)malloc(sizeof(DomSetTree));
	dstree->mkpsol = mkpsol;
	dstree->mkp = mkpsol->mkp;
	dstree->n = 0;
	dstree->root = NULL;

	return dstree;
}

MKPSol *mkp_fast_domsets(MKP *mkp){
	DomSetTree *dstree;
	MKPSol *mkpsol;
	MKP *mkp;
	int i, j, n, m;

	n = mkp->n;
	m = mkp->m;
	mkp = mkpsol->mkp;
	dstree = dstree_new(mkpsol);

	/* TODO: iniciar enumeração das soluções... 
	 * 	    - iniciar de uma solução vazia (por enquanto);
	 * 	    - criar novo nó apenas no caso de inserção de item. */

	for( i = 0 ; i

	return mkpsol;
}

