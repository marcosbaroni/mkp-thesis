#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../utils/util.h"
#include "../utils/avl.h"
#include "../utils/kdtree.h"
#include "../utils/list.h"
#include "../models/mokp/mokp.h"

#define MAX_COORD 1000

long long ncomp = 0;
long long ninside = 0;

void print_usage(int argc, char **argv){
	printf("%s <dim> <tad type> <n points> <n querys> <query hvol ratio>\n", argv[0]);
	printf("  - dim     : number of point dimension\n");
	printf("  - tad type  : 0: list / 1: avl / >1: kdtree\n");
	printf("  - n points  : n. of holded points\n");
	printf("  - n querys  : n. of query to be executed\n");
	printf("  - query hvol: size of query (\?\?) \n");
	printf("\n");
	return;
}

typedef struct Point{
	double *x;
	int pdim;
}Point;

Point *pnt_new( int pdim ){
	Point *pt;
	pt = (Point*)malloc(sizeof(Point));
	pt->x = (double*)malloc(pdim*sizeof(double));
	pt->pdim = pdim;
	return pt;
}

Point *pnt_new_random( int pdim ){
	Point *pt;
	int i;

	pt = pnt_new(pdim);
	for( i = 0 ; i < pdim ; i++ ){
		pt->x[i] = MAX_COORD*(rand()/(double)RAND_MAX);
		printf("%lf\n", pt->x[i]);
	}
	
	return pt;
}

void pnt_free(Point *pt){
	free(pt->x);
	free(pt);
}

void pnt_print(Point *pt){
	int i;
	printf("(");
	for( i = 0 ; i < pt->pdim ; i++ )
		printf("%s%lf", pt->x[i], i ? ", ": "");
	printf(")\n");
}

int pnt_is_inside(Point *pt, Point *l1, Point *l2){
	int i, pdim;
	pdim = pt->pdim;

	ncomp++;

	for( i = 0 ; i < pdim ; i++ )
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
	if( p1->x[0] > p2->x[0] ){
		return 1;
	}else if( p1->x[0] < p2->x[0] ){
		return -1;
	}
	return 0;
}

typedef struct PointIndexer{
	int ndim;
	int pdim;
	union {
		AVLTree *avl;
		KDTree *kdt;
		List *list;
	}tad;
}PointIndexer;

PointIndexer *pidx_new(int pdim, int ndim){
	PointIndexer *pidx;
	pidx = (PointIndexer*)malloc(sizeof(PointIndexer));
	pidx->pdim = pdim;
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
	Point *pt, *found;
	int i, ndim, fits;

	ListIter *liter = list_get_first(list);
	pt = (Point*)listiter_get(liter);
	found = NULL;

	while( pt && !found ){
		ndim = pt->pdim;
		if( pnt_is_inside(pt, l1, l2) )
			found = pt;
		listiter_forward(liter);
		pt = (Point*)listiter_get(liter);
	}
	listiter_free(liter);
	return found;
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

void pidx_apply_all(PointIndexer *pidx, void(*f)(void*)){
	if( pidx->ndim == 0 )
		list_apply(pidx->tad.list, f);
	if( pidx->ndim == 1 )
		avl_apply_to_all(pidx->tad.avl, f);
	if( pidx->ndim  > 1 )
		kdtree_apply_to_all(pidx->tad.kdt, f);
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

int pidx_get_n(PointIndexer *pidx){
	if( pidx->ndim == 0 )
		return pidx->tad.list->n;
	if( pidx->ndim == 1 )
		return pidx->tad.avl->n;
	return pidx->tad.kdt->n;
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

PointIndexer *pidx_new_populated(int n, int pdim, int ndim){
	Point *pt;
	PointIndexer *pidx = pidx_new(pdim, ndim);
	while( n-- ){
		pt = pnt_new_random(pdim);
		pidx_insert(pidx, pt);
	}
	return pidx;
}

void pidx_free(PointIndexer *pidx){
	if( pidx->ndim == 0 )
		list_free(pidx->tad.list);
	if( pidx->ndim == 1 )
		avl_free(pidx->tad.avl);
	if( pidx->ndim  > 1 )
		kdtree_free(pidx->tad.kdt);

	free(pidx);
}

void pidx_batch_random_searchs(PointIndexer *pidx, int nquery){
	Point *l1, *l2, *found;
	int n, pdim, i;
	double avghvol, sides_hvol;
	double sides[pidx->pdim];
	pdim = pidx->pdim;

	l1 = pnt_new(pdim);
	l2 = pnt_new(pdim);

	n = pidx_get_n(pidx);

	avghvol = .5;
	avghvol *= pow(MAX_COORD, pidx->pdim)/n;

	while( nquery-- ){
		sides_hvol = 1.;
		for( i = 0 ; i < pdim ; i++ ){
			sides[i] = rand()/(double)RAND_MAX;
			sides_hvol *= sides[i];
		}
		for( i = 0 ; i < pdim ; i++ ){
			sides[i] /= pow(sides_hvol/avghvol, 1.0/(double)pdim);
		}
		for( i = 0 ; i < pdim ; i++ ){
			l1->x[i] = (MAX_COORD-sides[i])*(rand()/(double)RAND_MAX);
			l2->x[i] = l1->x[i] + sides[i];
		}
	
		found = pidx_range_search(pidx, l1, l2);
		if( found )
			ninside++;
	}

	pnt_free(l1);
	pnt_free(l2);
}

int main(int argc, char **argv){
	PointIndexer *pidx;

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
	/* seed */
	int seed = time(NULL);
	if( argc > 6 )
		seed = atoi(argv[6]);
	auto_seed(seed);

	pidx = pidx_new_populated(npts, pdim, ndim);
	pidx_apply_all(pidx, (void(*)(void*))pnt_print);
	pidx_batch_random_searchs(pidx, nquery);
	pidx_apply_all(pidx, (void(*)(void*))pnt_free);
	pidx_free(pidx);

	printf("%lld;%.2f\n", ncomp, ninside/(float)nquery);
	
	return 0;
}
