#include <stdio.h>
#include <stdlib.h>

#include "../utils/util.h"
#include "../utils/avl.h"
#include "../utils/kdtree.h"
#include "../utils/list.h"
#include "../models/mokp/mokp.h"

void print_usage(int argc, char **argv){
	printf("%s <dim> <tad type> <n points> <n querys> <query hvol ratio>\n", argv[0]);
	printf("  - dim     : number of point distances \n");
	printf("  - tad type  : 0: list / 1: avl / >1: kdtree\n");
	printf("  - n points  : n. of holded points\n");
	printf("  - n querys  : n. of query to be executed\n");
	printf("  - query hvol: size of query (\?\?) \n");
	printf("\n");
	return;
}

typedef struct PointIndexer{
	int ndim;
	union {
		AVLTree *avl;
		KDTree *kdt;
		List *list;
	}tad;
}PointIndexer;

PointIndexer *pidx_new(int ndim){
	PointIndexer *pidx;
	pidx = (PointIndexer*)malloc(sizeof(PointIndexer));
	pidx->ndim = ndim;
	if( !ndim )
		pidx->tad.list = list_new();
	else if( ndim == 1 )
		pidx->tad.avl = new_avltree();
	else
		pidx->tad.kdt = kdtree_new();
	return pidx;
}

int main(int argc, char **argv){
	MOKPSolIndexer *msi;

	if( argc < 6 ){
		print_usage(argc, argv);
		return 1;
	}
	int max_range = 1000000;

	/* points dim */
	int pdim = atol(argv[1]);
	/* tad type */
	int ndim = atol(argv[2]);
	/* number of points */
	int npts = atol(argv[3]);
	/* number of query */
	int nquery = atol(argv[4]);
	/* query vol */
	double qratio = atof(argv[5]);

	
}
