#include <stdio.h>
#include <stdlib.h>

#include "../utils/util.h"
#include "../utils/avl.h"
#include "../utils/kdtree.h"
#include "../utils/list.h"
#include "../models/mokp/mokp.h"

#define MAXCOORD 1000000;

long long ncomp = 0;

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

typedef struct Point{
	int *x;
	int ndim;
}Point;

Point *pnt_new_random( int ndim ){
	Point *pt;
	int i;

	pt = (Point*)malloc(sizeof(Point));
	pt->ndim = ndim;
	pt->x = (int*)malloc(ndim*sizeof(int));
	for( i = 0 ; i < ndim ; i++ )
		pt->x[i] = rand() % MAXCOORD;
	
	return pt;
}

int pnt_is_inside(Point *pt, Point *l1, Point *l2){
	int i, ndim;
	ndim = pt->ndim;

	ncomp++;

	for( i = 0 ; i < ndim ; i++ )
		if( pt->x[i] < l1->x[i] || pt->x[i] > l2->x[i] )
			return 0;
	return 1;
}

double point_eval(Point *p, int dim){
	ncomp++;
	return (double)p->x[dim];
}

int point_cmp1(Point *p1, Point *p2){
	ncomp++;
	return (p1->x[0] - p2->x[0]);
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
		pidx->tad.avl = new_avltree((avl_cmp_f)point_cmp1);
	else
		pidx->tad.kdt = kdtree_new(ndim, (kdtree_eval_f)point_eval);
	return pidx;
}

PointIndexer *pidx_insert(PointIndexer *pidx, Point *pt){
	if( !pidx->ndim )
		list_insert(pidx->tad.list, pt);
	if( pidx->ndim == 1 )
		avl_insert(pidx->tad.avl, pt);
	if( pidx->ndim > 1 )
		kdtree_insert(pidx->tad.kdt, pt);
}

Point *pidx_range_search_list(List *list, Point *l1, Point *l2){
	Point *pt;
	int i, ndim, fits;

	ListIter *liter = list_get_first(list);
	pt = (Point*)listiter_get(liter);

	while( pt ){
		ndim = pt->ndim;
		if( pnt_is_inside(pt, l1, l2) )
			return pt;
		liter = listiter_forward(liter);
		pt = (Point*)listiter_get(liter);
	}
	return NULL;
}

Point *pidx_range_search_avl(AVLTree *avl, Point *l1, Point *l2){
	AVLIter *aiter;
	Point *pt;

	aiter = avl_get_lower_higher_than(avl, l1);
	pt = avliter_get(aiter);
	while( pt ){
		if( pt->x[0] > l2->x[0] )
			break;
		if( pnt_is_inside(pt, l1, l2) )
			return pt;

		avliter_forward(aiter);
		pt = avliter_get(aiter);
	}
	return NULL;
}

int pnt_is_index_kdt(Point *pt, Point **pts){
	return pnt_is_inside(pt, pts[0], pts[1]);
}

Point *pidx_range_search_kdtree(KDTree *kdtree, Point *l1, Point *l2){
	int i, ndim;
	Point* pts[2];
	Point *pt;
	double *lims;
	ndim = kdtree->ndim;
	
	pts[0] = l1;
	pts[1] = l2;

	lims = (double*)malloc(2*ndim*sizeof(double));
	for( i = 0 ; i < ndim ; i++ ){
		lims[2*i] = (double)l1->x[0];
		lims[2*i+1] = (double)l2->x[0];
	}
	
	pt = kdtree_range_search_r(kdtree, lims, (property_f_r)pnt_is_index_kdt, pts);

	free(lims);

	return pt;
}

Point *pidx_range_search(PointIndexer *pidx, Point *l1, Point *l2){
	if( !pidx->ndim )
		return pidx_range_search_list(pidx->tad.list, l1, l2);
	else if( pidx->ndim == 1 )
		return pidx_range_search_avl(pidx->tad.avl, l1, l2);
	else
		return pidx_range_search_kdtree(pidx->tad.kdt, l1, l2);
	return NULL;
}

PointIndexer *pidx_new_populated(int n, int dim, int ndim){
	Point *pt;
	PointIndexer *pidx = pidx_new(ndim);
	while( n-- ){
		pt = pnt_new_random(dim);
		pidx_insert(pidx, pt);
	}
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
