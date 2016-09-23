#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "mkp.h"

#include "../../utils/util.h"
#include "../../metahrs/des.h"
#include "../../metahrs/sfl.h"
#include "../../utils/lp.h"
#include "soputils.h"

/* chu beasley instance best known objectives */
mkpnum chubeas_best[3][3][3][10] = {
  { /* n = 100 */
   { /* m = 5 */
    {24381, 24274, 23551, 23534, 23991, 24613, 25591, 23410, 24216, 24411}, /* t = 0.25 */
    {42757, 42545, 41968, 45090, 42218, 42927, 42009, 45020, 43441, 44554}, /* t = 0.50 */
    {59822, 62081, 59802, 60479, 61091, 58959, 61538, 61520, 59453, 59965} /* t = 0.75 */
   },
   { /* m = 10 */
    {23064, 22801, 22131, 22772, 22751, 22777, 21875, 22635, 22511, 22702}, /* 0.25 */
    {41395, 42344, 42401, 45624, 41884, 42995, 43574, 42970, 42212, 41207}, /* 0.50 */
    {57375, 58978, 58391, 61966, 60803, 61437, 56377, 59391, 60205, 60633} /* 0.75 */
   },
   { /* m = 30 */
    {21946, 21716, 20754, 21464, 21844, 22176, 21799, 21397, 22525, 20983}, /* 0.25 */
    {40767, 41308, 41630, 41041, 40889, 41058, 41062, 42719, 42230, 41700}, /* 0.50 */
    {57494, 60027, 58052, 60776, 58884, 60011, 58132, 59064, 58975, 60603} /* 0.75 */
   }
  },
  { /* n = 250 */
   { /* m = 5 */
    {59312, 61472, 62130, 59463, 58951, 60077, 60414, 61472, 61885, 58959}, /* 0.25 */
    {109109, 109841, 108508, 109383, 110720, 110256, 109040, 109042, 109971, 107058}, /* 0.50 */
    {149665, 155944, 149334, 152130, 150353, 150045, 148607, 149782, 155075, 154668} /* 0.75 */
   },
   { /* m = 10 */
    {59187, 58781, 58097, 61000, 58092, 58824, 58704, 58936, 59387, 59208}, /* 0.25 */
    {110913, 108717, 108932, 110086, 108485, 110845, 106077, 106686, 109829, 106723}, /* 0.50 */
    {151809, 148772, 151909, 151324, 151966, 152109, 153131, 153578, 149160, 149704} /* 0.75 */
   },
   { /* m = 30 */
    {56842, 58520, 56614, 56930, 56629, 57205, 56357, 56457, 57474, 56447}, /* 0.25 */
    {107770, 108392, 106442, 106876, 107414, 107271, 106372, 104032, 106856, 105780}, /* 0.50 */
    {150163, 149958, 153007, 153234, 150287, 148574, 147477, 152912, 149570, 149668} /* 0.75 */
   }
  },
  { /* n = 500 */
   { /* m = 5 */
    {120148, 117879, 121131, 120804, 122319, 122024, 119127, 120568, 121586, 120717}, /* 0.25 */
    {218428, 221202, 217542, 223560, 218966, 220530, 219989, 218215, 216976, 219719}, /* 0.50*/
    {295828, 308086, 299796, 306480, 300342, 302571, 301339, 306454, 302828, 299910} /* 0.75 */
   },
   { /* m = 10 */
    {117821, 119249, 119215, 118829, 116530, 119504, 119827, 118344, 117815, 119251}, /* 0.25 */
    {217377, 219077, 217847, 216868, 213873, 215086, 217940, 219990, 214382, 220899}, /* 0.50 */
    {304387, 302379, 302417, 300784, 304374, 301836, 304952, 296478, 301359, 307089} /* 0.75 */
   },
   { /* m = 30 */
    {116056, 114810, 116741, 115354, 116525, 115741, 114181, 114403, 115419, 117116}, /* 0.25 */
    {218104, 214648, 215978, 217910, 215689, 215919, 215907, 216542, 217340, 214739}, /* 0.50 */
    {301675, 300055, 305087, 302032, 304462, 297012, 303364, 307007, 303199, 300572} /* 0.75 */
   }
  }
 };


/* */
mkpnum mkpnum_rand(mkpnum max){
	return round(max*(rand()/(mkpnum)RAND_MAX)); /* double */
}

void mkpnum_fprintf(FILE *out, mkpnum a){
	if( round(a) == a)
		fprintf(out, "%.0lf", a);
	else
		fprintf(out, "%lf", a);
}

void mkpnum_fscanf(FILE *out, mkpnum *a){
	fscanf(out, "%lf", a);
}

mkpnum mkpnum_array_sum(mkpnum *array, int n){
	int i;
	mkpnum sum;

	sum = 0;
	for( i = 0 ; i < n ; i++ )
		sum += array[i];

	return sum;
}

void mkpnum_array_zimpl_print(FILE *fout, mkpnum *array, int n){
	int i;
	for( i = 0 ; i < n-1 ; i++ ){
		fprintf(fout, "<%d> ", i+1);
		mkpnum_fprintf(fout, array[i]);
		fprintf(fout, ",\n");
	}
	fprintf(fout, "<%d> ", i+1);
	mkpnum_fprintf(fout, array[n-1]);
	fprintf(fout, ";\n");

	return;
}

void mkpnum_matrix_zimpl_print(FILE *fout, mkpnum **mat, int nlin, int ncol){
	int i, j;
	
	/* header */
	fprintf(fout, "|1");
	for( i = 1 ; i < ncol ; i++ )
		fprintf(fout, ",%d", i+1);
	fprintf(fout, "|\n");

	/* values */
	for( i = 0 ; i < nlin ; i++ ){
		fprintf(fout, "|%d|", i+1);
		mkpnum_fprintf(fout, mat[i][0]);
		for( j = 1 ; j < ncol ; j++ ){
			fprintf(fout, ",");
			mkpnum_fprintf(fout, mat[i][j]);
		}
		fprintf(fout, "|\n");
	}
	fprintf(fout, ";\n");
	return;
}

mkpnum mkpnum_matrix_max_col(mkpnum **mat, int n, int m, int col){
	int i;
	mkpnum max;

	max = mat[0][col];
	for( i = 1 ; i < n ; i++ )
		if( max < mat[i][col] )
			max = mat[i][col];
	return max;
}

mkpnum mkpnum_array_max(mkpnum *array, int n){
	int i;
	mkpnum max;

	max = array[0];
	for( i = 1 ; i < n ; i++ )
		if( max < array[i] )
			max = array[i];
	return max;
}

void mkpnum_array_write(FILE *fout, mkpnum *array, int n){
	int i;
	for( i = 0 ; i < n ; i++ ){
		mkpnum_fprintf(fout, array[i]);
		fprintf(fout, " ");
	}
	fprintf(fout, "\n");
	return;
}

void mkpnum_matrix_write(FILE *fout, mkpnum **mat, int n, int m){
	int i;
	for( i = 0 ; i < n ; i++ )
		mkpnum_array_write(fout, mat[i], m);
}

mkpnum *mkpnum_array_read(FILE *fin, mkpnum *array, int n){
	int i;
	for( i = 0 ; i < n ; i++ )
		mkpnum_fscanf(fin, &(array[i]));
	return array;
}

mkpnum *mkpnum_array_malloc(int n){
	return (mkpnum*)malloc(n*sizeof(mkpnum));
}

mkpnum **mkpnum_matrix_malloc(int n, int m){
	mkpnum **mat;
	int i;
	
	mat = (mkpnum**)malloc(n*sizeof(mkpnum*));
	for( i = 0 ; i < n ; i++ )
		mat[i] = (mkpnum*)malloc(m*sizeof(mkpnum));

	return mat;
}

mkpnum **mkpnum_matrix_copy( mkpnum **dest, mkpnum **src, int n, int m){
    int i, j;

    if(!dest){
        dest = (mkpnum**)malloc(n*sizeof(mkpnum*));
        for( i = 0 ; i < n ; i++ )
            dest[i] = (mkpnum*)malloc(m*sizeof(mkpnum));
    }
    
    /* coping */
    for( i = 0 ; i < n ; i++ )
        for( j = 0 ; j < m ; j++ )
            dest[i][j] = src[i][j];

    return dest;
}

mkpnum **mkpnum_matrix_read(FILE *fin, mkpnum **mat, int n, int m){
	int i;

	if(!mat)
		mat = mkpnum_matrix_malloc(n, m);
	
	for( i = 0 ; i < n ; i++ )
		mat[i] = mkpnum_array_read(fin, mat[i], m);

	return mat;
}

void mkpnum_matrix_free(mkpnum **mat, int n){
	int i;
	for( i = 0 ; i < n ; i++ )
		free(mat[i]);
	free(mat);
}

mkpnum *mkpnum_array_init(mkpnum *array, int n, mkpnum x){
	int i;
	if(!array)
		array = (mkpnum*)malloc(n*sizeof(mkpnum));
	for( i = 0 ; i < n ; i++ )
		array[i] = x;
	return array;
}

void mkpnum_scanf(const char *str, mkpnum *x){
	sscanf(str, "%lf", x);
}

mkpnum *mkpnum_array_copy(mkpnum *dest, mkpnum *src, int n){
	int i;

	if(!dest)
		dest = mkpnum_array_malloc(n);

	for( i = 0 ; i < n ; i++ )
		dest[i] = src[i];
	return dest;
}

void SWAP_MKPNUM(mkpnum *array, int a, int b){
	mkpnum aux;
	aux = array[a];
	array[a] = array[b];
	array[b] = aux;
	return;
}

int mkpnum_array_partition(mkpnum *array, int a, int b){
	int i, j;
	mkpnum pivot;
	
	i = a; j = b+1;
	pivot = array[a];

	while( 1 ){
		while( array[++i] < pivot ) if( i == b ) break;
		while( pivot < array[--j] ) if( j == a ) break;
		if( i >= j ) break;            /* Crossed? */
		SWAP_MKPNUM(array, i, j);        /* swap */
	}
	SWAP_MKPNUM(array, j, a);     /* place pivot */

	return j;
}

mkpnum *mkpnum_array_sub_qsort(mkpnum *array, int a, int b){
	int m;
	if( b <= a )
		return array;
	m = mkpnum_array_partition(array, a, b);
	mkpnum_array_sub_qsort(array, a, m-1);
	mkpnum_array_sub_qsort(array, m+1, b);
	return array;
}

mkpnum *mkpnum_array_qsort(mkpnum *array, int n){
	mkpnum_array_sub_qsort(array, 0, n-1);
	return array;
}

/*** memory management ***/
MKP *mkp_alloc(int n, int m){
	int i;
	MKP *mkp;

	mkp = (MKP*)malloc(sizeof(MKP));
	mkp->p = mkpnum_array_malloc(n);
	mkp->w = mkpnum_matrix_malloc(m, n);
	mkp->b = mkpnum_array_malloc(n);
	mkp->idxs = (int*)malloc(n*sizeof(int));
	for( i = 0 ; i < n ; i++ )
		mkp->idxs[i] = i;
	mkp->n = n;
	mkp->m = m;
	mkp->em = NULL;
	mkp->lp_sol = NULL;
	mkp->lp_trunc = NULL;
	mkp->lp_obj = 0.0;

	return mkp;
}

/*
 * Returns a random MKP problem with coeficients taken from uniform
 *    distribution.
 * ARGUMENTS:
 *   alpha: correlation of items
 *   beta: tigthness of knapsack
 * GENERATION:
 *   w[i][j] = U(0, max_coefs);
 *   b[j] = alpha*(w[0][j] + w[1][j] + ... + w[n-1][j]);
 *   p[j] = beta*2*(w[i][0] + w[i][1] + ... + w[i][m-1]) +
 * 	+ (1-beta)*max_coefs*U(0, 1)
 */
MKP *mkp_random(int n, int m, double alpha, double beta, mkpnum max_coefs){
	int i, j;
	mkpnum *wsums, wsum, w;
	MKP *mkp;

	mkp = mkp_alloc(n, m);
	wsums = mkpnum_array_init(NULL, m, 0);

	for( i = 0 ; i < n ; i++ ){
		wsum = 0;
		for( j = 0 ; j < m ; j++ ){
			/* weights */
			w = mkpnum_rand(max_coefs);
			mkp->w[j][i] = w;
			wsums[j] += w;
			wsum += w;
		}
		/* profits */
		mkp->p[i] = round(beta*mkpnum_rand(wsum)/m + mkpnum_rand((1-beta)*max_coefs));
	}

	/* capacities */
	for( j = 0 ; j < m ; j++ ){
		mkp->b[j] = (mkpnum)(ceil(wsums[j]*alpha));
	}

	free(wsums);

	return mkp;
}

int mkp_cmp_profit(MKP *mkp, int a, int b){
	if( mkp->p[a] < mkp->p[b] ) return -1;
	else if(mkp->p[a] > mkp->p[b] ) return 1;
	return 0;
}

int mkp_cmp_em(MKP *mkp, int a, int b){
	if( mkp->em[a] < mkp->em[b] ) return -1;
	else if(mkp->em[a] > mkp->em[b] ) return 1;
	return 0;
}

void mkp_swap_itens(MKP *mkp, int a, int b){
	mkpnum w[mkp->m], p;
	int i, m;

	m = mkp->m;
	/* swaping profits */
	p = mkp->p[a]; mkp->p[a] = mkp->p[b]; mkp->p[b] = p;
	/* swaping weights */
	for( i = 0 ; i < m ; i++ ) w[i] = mkp->w[i][a];
	for( i = 0 ; i < m ; i++ ) mkp->w[i][a] = mkp->w[i][b];
	for( i = 0 ; i < m ; i++ ) mkp->w[i][b] = w[i];

	return ;
}

void mkp_sort_by_profit(MKP *mkp){
	mp_qsort(mkp, mkp->n, (mp_cmp_f)mkp_cmp_profit, (mp_swap_f)mkp_swap_itens, 1);
	return;
}

/* 
 * Returns a MKP, copy of original MKP, with p[i] = 1, i = 1, ..., n.
 */
MKP *mkp_card_mkp(MKP *mkp){
	int i, n;
	MKP *new;

	n = mkp->n;
	new = mkp_copy(mkp);
	/* setting profits to 1 */
	for( i = 0 ; i < n ; i++ )
		new->p[i] = 1;
	
	return new;
}

/*
 * Finds an upper bound on number of items on knapsack, based on
 *   max items for each dimension. */
int mkp_max_items(MKP *mkp){
	int nx, n, m, i, j;
	mkpnum *ws, b;

	m = mkp->m;
	n = mkp->n;
	ws = (mkpnum*)malloc(n*sizeof(mkpnum));

	/* for each dimension */
	nx = n;
	for( j = 0 ; j < m ; j++ ){
		mkpnum_array_copy(ws, mkp->w[j], n);
		mkpnum_array_qsort(ws, n);
		b = mkp->b[j];
		for( i = 0 ; i < n && b > 0 ; i++ )
			b -= ws[i];
		printf("max item for dim %d: %d\n", j+1, i);
		if( i < nx )
			nx = i;
	}

	free(ws);

	return nx;
}

/* Returns the dual efficiency measure of each item.
 * check.: "The core concept for the multidimensional knapsack problem",
 *         Evolutionary Computation in Combinatorial Optimization, 2006 */
double *mkp_dual_em(MKP *mkp){
	double *x, *vals, sum;
	int j, i, n, m;
	mkpnum *p, **w;
	LP *lp;

	n = mkp->n;
	m = mkp->m;
	p = mkp->p;
	w = mkp->w;
	vals = (double*)malloc(n*sizeof(double));

	/* solving relaxed dual problem */
	//x = mkp_solve_dual_with_scip(mkp);
	x = mkp_get_dual_real(mkp->p, mkp->w, mkp->b, mkp->n, mkp->m); // using soplex

	for( j = 0 ; j < n ; j++ ){
		sum = 0.0;
		for( i = 0 ; i < m ; i++ )
			sum += w[i][j]*x[i];
		vals[j] = p[j]/sum;
	}
	free(x);

	return vals;
}

void mkp_sort_by_em(MKP *mkp, int reverse){
	if(!mkp->em)
		mkp->em = mkp_dual_em(mkp);
	mp_qsort(mkp, mkp->n, (mp_cmp_f)mkp_cmp_em, (mp_swap_f)mkp_swap_itens, !reverse);
	return;
}

void mkp_free(MKP *mkp){
	mkpnum_matrix_free(mkp->w, mkp->m);
	if(mkp->em)
		free(mkp->em);
	if(mkp->lp_sol)
		free(mkp->lp_sol);
	if(mkp->lp_trunc)
		mkpsol_free(mkp->lp_trunc);
	free(mkp->p);
	free(mkp->b);
	free(mkp->idxs);
	free(mkp);

	return;
}

double *mkp_solve_with_scip(MKP *mkp, double maxtime, double capacity_scale, char linear){
	double *x, val;
	int a, n, nread;
	char tempf[200], buff[400];
	FILE *pip, *out;

	n = mkp->n;
	x = double_array_init(NULL, n, 0.0);  /* solution array */

	/* temp file for mkp model */
	pip = popen("tempfile", "r");
	fscanf(pip, "%s", tempf);
	fclose(pip);

	/* write mkp model on temp file*/
	out = fopen(tempf, "w");
	mkp_to_zimpl(out, mkp, 0.0, capacity_scale, linear);
	fclose(out);

	/* solve model */
	sprintf(buff, "zpl2lp %s | runscip - %lf | scip2summary -s ", tempf, maxtime);
	pip = popen(buff, "r");

	/* read solution */
	fscanf(pip, "%s", buff);
	nread = fscanf(pip, "%d %lf", &a, &val);
	while( nread == 2 ){
		x[a-1] = val;
		nread = fscanf(pip, "%d %lf", &a, &val);
	}
	fclose(pip);

	return x;
}


MKPSol *mkp_get_lp_trunc(MKP *mkp){
	if(!mkp->lp_trunc)
		mkp->lp_trunc = mkpsol_from_lp(mkp);
	return mkp->lp_trunc;
}

LP *mkp2lp(MKP *mkp, double capacity_scale){
	LP *lp;
	int i, j, n, m;

	if( capacity_scale < 0.0 )
		capacity_scale = 0.0;
	n = mkp->n;
	m = mkp->m;

	/* allocs */
	lp = (LP*)malloc(sizeof(LP));
	lp->nvars = n;
	lp->ncs = m+n;
	lp->c = double_array_alloc(n);
	lp->a = double_matrix_alloc(m+n, n);
	lp->b = double_array_alloc(m+n);

	/* the profit */
	for( i = 0 ; i < n ; i++ )
		lp->c[i] = mkp->p[i];
	/* the weights constraints */
	for( i = 0 ; i < m ; i++ )
		for( j = 0 ; j < n ; j++ )
			lp->a[i][j] = mkp->w[i][j];
	/* the "binary" contraint of each variable */
	for( i = 0 ; i < n ; i++ ){
		for( j = 0 ; j < n ; j++ )
			lp->a[m+i][j] = 0.0;
		lp->a[m+i][i] = 1.0;
	}
	/* the capacities */
	for( i = 0 ; i < m ; i++ )
		lp->b[i] = capacity_scale*mkp->b[i];
	for( i = 0 ; i < n ; i++ )
		lp->b[m+i] = 1.0;
	
	return lp;
}

void mkpsol_write(FILE *fout, MKPSol *mkpsol){
	int i, n;

	n = mkpsol->mkp->n;

	for( i = 0 ; i < n ; i++ )
		if(mkpsol->x[i])
			fprintf(fout, "%d ", i);
	fprintf(fout, "\n");

	return;
}

MKP *mkp_read_from_filename(char *filename){
	MKP *mkp;
	FILE *fin;

	assert_faccess(filename, R_OK);
	fin = fopen(filename, "r");

	mkp = mkp_read_from_file(fin);
	fclose(fin);

	return mkp;
}

MKP *mkp_read_from_file(FILE *fin){
	int i, n, m;
	MKP *mkp;
	
	fscanf(fin, "%d", &n);
	fscanf(fin, "%d", &m);

	mkp = mkp_alloc(n, m);
	mkp->n = n;
	mkp->m = m;
	mkpnum_array_read(fin, mkp->p, n);
	mkpnum_matrix_read(fin, mkp->w, m, n);
	mkpnum_array_read(fin, mkp->b, n);
	for( i = 0 ; i < n ; i++ )
		mkp->idxs[i] = i;

	return mkp;
}

void mkp_write_to_filename(MKP *mkp, char *filename){
	FILE *fout;
	assert_faccess(filename, W_OK);
	fout = fopen(filename, "w");
	mkp_write_to_file(mkp, fout);
	return ;
}

void mkp_write_to_file(MKP *mkp, FILE *fout){
	int i, j;

	fprintf(fout, "%d %d\n", mkp->n, mkp->m);
	mkpnum_array_write(fout, mkp->p, mkp->n);
	mkpnum_matrix_write(fout, mkp->w, mkp->m, mkp->n);
	mkpnum_array_write(fout, mkp->b, mkp->m);

	return;
}

MKP *mkp_copy(MKP *mkp){
	MKP *new;
	int i, j, n, m;

	n = mkp->n;
	m = mkp->m;

	new = mkp_alloc(mkp->n, mkp->m);
	/* coping profits */
	for( i = 0 ; i < n ; i++ )
		new->p[i] = mkp->p[i];

	for( j = 0 ; j < m ; j++ ){
		/* coping weights */
		for( i = 0 ; i < n ; i++ )
			new->w[j][i] = mkp->w[j][i];
		/* coping capacities */
		new->b[j] = mkp->b[j];
	}

	return new;
}

/*
 * Prints a MKP instance on a human friendly format.
 */
void mkp_fprint(FILE *fout, MKP *mkp){
	int i, j, n, m, ndigs[mkp->n+1];
	char format[20];
	long max;

	n = mkp->n;
	m = mkp->m;

	/* Deciding ndigits */
	for( i = 0 ; i < n ; i++ ){
		max = mkpnum_matrix_max_col(mkp->w, m, n, i);
		max = MAX(max, mkp->p[i]);
		ndigs[i] = 1 + (int)(floor(log(max)/log(10.)));
	}
	ndigs[n] = (int)(ceil(log(MAX(mkpnum_array_max(mkp->b, mkp->m), mkp->m))) / log(10.));

	/* print profits*/
	for( i = 0 ; i < n ; i++ ){
		sprintf(format, "%%%dlld ", ndigs[i]);
		/* TODO: formatar para mkpnum */
		//fprintf(fout, format, mkp->p[i]);
		mkpnum_fprintf(fout, mkp->p[i]);
		fprintf(fout, " ");
	}
	fprintf(fout, "\n");

	for( i = 0 ; i < n-1 ; i++ ){
		for( j = 0 ; j < ndigs[i] ; j++ )
			fprintf(fout, "-");
		fprintf(fout, "-");
	}
	for( j = 0 ; j < ndigs[n-1] ; j++ )
		fprintf(fout, "-");
	fprintf(fout, "\n");

	/* print constraints */
	for( i = 0 ; i < m ; i++ ){
		/* weights */
		for( j = 0 ; j < n ; j++ ){
			sprintf(format, "%%%dlld ", ndigs[j]);
			/* TODO: formatar para mkpnum */
			//fprintf(fout, format, mkp->w[i][j]);
			mkpnum_fprintf(fout, mkp->w[i][j]);
			fprintf(fout, " ");
		}
		/* capacity */
		sprintf(format, "%%%dlld ", ndigs[n]);
		fprintf(fout, "| ");
		/* TODO: formatar para mkpnum */
		//fprintf(fout, format, mkp->b[i]);
		mkpnum_fprintf(fout, mkp->b[i]);
		fprintf(fout, " \n");
	}

	return;
}

/* Prints MKP dual problem in ZIMPL format */
void mkp_dual_to_zimpl(FILE *fout, MKP *mkp, char linear){
	int i, j, n, m;


	n = mkp->n;
	m = mkp->m;

	/* sizes */
	fprintf(fout, "param n := %d;\n", n);
	fprintf(fout, "param m := %d;\n", m);

	/* sets */
	fprintf(fout, "set N := {1 .. %d};\n", n);
	fprintf(fout, "set M := {1 .. %d};\n", m);

	/* profit */
	fprintf(fout, "param p[N] :=\n");
	mkpnum_array_zimpl_print(fout, mkp->p, n);

	/* weights */
	fprintf(fout, "param w[M*N] :=\n");
	mkpnum_matrix_zimpl_print(fout, mkp->w, m, n);

	/* capacities */
	fprintf(fout, "param b[M] :=\n");
	mkpnum_array_zimpl_print(fout, mkp->b, m);

	/* desicion var */
	if(linear) {
		fprintf(fout, "var x[M] real;\n");
		fprintf(fout, "var v[N] real;\n");
	} else {
		fprintf(fout, "var x[M] binary;\n");
		fprintf(fout, "var v[N] binary;\n");
	}

	/* capacities constraint */
	fprintf(fout,
		"subto constraints:\n\
			forall <j> in N do\n\
				sum <i> in M do\n\
					x[i]*w[i, j] + v[j] >= p[j];\n");

	/* objective function */
	fprintf(fout,
		"minimize obj:\n\
			sum <i> in M do\n\
				x[i]*b[i] +\n\
			sum <j> in N do\n\
				v[j];\n");

	return;
}

double *mkp_solve_dual_with_scip(MKP *mkp){
	double *x, val, maxtime;
	int i, a, m, nread;
	char tempf[200], buff[400];
	FILE *pip, *out;

	m = mkp->m;
	x = double_array_init(NULL, m, 0.0);  /* solution array */
	maxtime = 60.0;

	/* temp file for mkp model */
	pip = popen("tempfile", "r");
	fscanf(pip, "%s", tempf);
	fclose(pip);

	/* write mkp dual model on temp file*/
	out = fopen(tempf, "w");
	mkp_dual_to_zimpl(out, mkp, 1);
	fclose(out);

	/* solve model */
	sprintf(buff, "zpl2lp %s | runscip - %lf | scip2summary -s ", tempf, maxtime);
	pip = popen(buff, "r");

	/* reading solution */
	fscanf(pip, "%s", buff);    /* throwing away first line */
	i = 0;
	nread = fscanf(pip, "%d %lf", &a, &val);
	while( nread == 2 ){
		x[a-1] = val;
		nread = fscanf(pip, "%d %lf", &a, &val);
	}
	fclose(pip);

	return x;
}

void mkp_to_zimpl(FILE *fout, MKP *mkp, double max_opt, double capacity_scale, char linear){
	int i, j, n, m;
	double *b;
	double total_profit;

	n = mkp->n;
	m = mkp->m;
	total_profit = (double)mkpnum_array_sum(mkp->p, n);
	if( max_opt <= 0.0 ) max_opt = 1.0e+40;

	/* sizes */
	fprintf(fout, "param n := %d;\n", n);
	fprintf(fout, "param m := %d;\n", m);

	/* sets */
	fprintf(fout, "set N := {1 .. %d};\n", n);
	fprintf(fout, "set M := {1 .. %d};\n", m);

	/* profit */
	fprintf(fout, "param p[N] :=\n");
	mkpnum_array_zimpl_print(fout, mkp->p, n);

	/* weights */
	fprintf(fout, "param w[M*N] :=\n");
	mkpnum_matrix_zimpl_print(fout, mkp->w, m, n);

	/* scaling */
	if(capacity_scale == 0.0)
		capacity_scale = 1.0;
	b = (double*)malloc(m*sizeof(double));
	for( i = 0 ; i < m ; i++ )
		b[i] = mkp->b[i]*capacity_scale;

	/* capacities */
	fprintf(fout, "param b[M] :=\n");
	double_array_zimpl_print(fout, b, m);

	/* max objective (optional) */
	if( max_opt < total_profit )
		fprintf(fout, "param maxobj := %lf;\n", max_opt);

	/* desicion var */
	if(linear)
		fprintf(fout, "var x[N] real >= 0 <= 1;\n");
	else fprintf(fout, "var x[N] binary;\n");

	/* capacities constraint */
	fprintf(fout,
		"subto capacities:\n\
			forall <j> in M do\n\
				sum <i> in N do\n\
					x[i]*w[j, i] <= b[j];\n");

	/* objective constraint (optional) */
	if( max_opt < total_profit )
		fprintf(fout,
			"subto maxprofit:\n\
				sum <i> in N do\n\
					x[i]*p[i] <= maxobj;\n");

	/* objective function */
	fprintf(fout,
		"maximize profit:\n\
			sum <i> in N do\n\
				x[i]*p[i];\n");

	free(b);

	return;
}

double* mkp_get_lp_sol(MKP *mkp){
	LP *lp;
	int i;

	if(!mkp->lp_sol){
		//lp = mkp2lp(mkp, 1.0);
		//mkp->lp_sol = lp_simplex(lp, &(mkp->lp_obj));
		mkp->lp_sol = mkp_solve_with_scip(mkp, 60.0, 1.0, 1);
		mkp->lp_obj = 0;
		for( i = 0 ; i < mkp->n ; i++ )
			mkp->lp_obj += mkp->lp_sol[i]*mkp->p[i];
		//lp_free(lp);
	}
	return (double*)memcpy(malloc(mkp->n*sizeof(double)), mkp->lp_sol, mkp->n*sizeof(double));
}

double mkp_get_lp_obj(MKP *mkp){
	LP* lp;
	int i;

	if( !mkp->lp_sol ){
		//lp = mkp2lp(mkp, 1.0);
		//mkp->lp_sol = lp_simplex(lp, &(mkp->lp_obj));
		mkp->lp_sol = mkp_solve_with_scip(mkp, 60.0, 1.0, 1);
		mkp->lp_obj = 0;
		for( i = 0 ; i < mkp->n ; i++ )
			mkp->lp_obj += mkp->lp_sol[i]*mkp->p[i];
		//lp_free(lp);
	}
	return mkp->lp_obj;
}

double *mkp_get_em(MKP *mkp){
	if(!mkp->em)
		mkp->em = mkp_dual_em(mkp);
	return mkp->em;
}

void _couting_fracs(double *x, int n,
	double *assigned,
	int *nnew_assigned,
	int *greater_var,
	double *greater_val){
	int j;

	*nnew_assigned = 0;
	*greater_val = 0.0;
	for( j = 0 ; j < n ; j++ ){
		if( x[j] > 0.0 && !assigned[j] ){
			/* the new non-zero is '1' */
			if( x[j] >= 1.0 )
				(*nnew_assigned)++;
			/* the new non-zero (is fractional) greater than others fractionals */
			else if( x[j] > *greater_val ){
				*greater_val = x[j];
				*greater_var = j;
			}
		}
	}
}

double *mkp_my_core_vals(MKP *mkp){
	int i, j, n;
	double *x, tic, scale, *assigned;
	int greater_var;      /* index of the greater fractional variable */
	double greater_val;   /* value of the greater fractional variable */
	int nnew_assigned;    /* number of new variables assigned to one */
	LP *lp;

	n = mkp->n;
	assigned = double_array_init(NULL, n, 0.0);

	scale = 0.0;
	for( i = 0 ; i < n ; i++ ){
		tic = 2.0/(double)n;
		nnew_assigned = 1;

		/* while none new fractional appeared OR a new '1' appeared .*/
		while( greater_val == 0.0 || nnew_assigned ){
			/* solving relaxation */
			x = mkp_get_lp_sol(mkp);

			/* counting new variables assigned to '1' or fractional */
			_couting_fracs(x, n,
				assigned,
				&nnew_assigned,
				&greater_var,
				&greater_val);

			/* adjust scaling (if needed) */
			if( nnew_assigned ) tic /= 2.0; /* new '1' appeared */
			else if( greater_val == 0.0 ) scale += tic; /* no new fractional */

			free(x);
		}
		/* updating assigment */
		assigned[greater_var] = (double)(n-i);

		/* updating next initial scale */
		scale += tic;
	}

	return assigned;
}

int _find_lesser_nonzero_nonassigned(double *x, double *assigned, int n){
	int i;
	int lesser_var;
	double lesser_val;

	lesser_val = 5.0;
	lesser_var = -1;
	for( i = 0 ; i < n ; i++ )
		if( x[i] > 0.0 && !assigned[i] )
			if( x[i] < lesser_val )
				{ lesser_val = x[i]; lesser_var = i; }

	return lesser_var;
}

double _do_right_search(MKP *mkp, int *assigned, int *var){
	int i, n;
	int n_std_ones;
	int n_ones;
	int n_ones_found;
	int n_fracs_found;
	int lesser_r_var;
	double lesser_val;
	double *x;
	double tic;
	double scale;
	LP *lp;

	n = mkp->n;

	/* solve std lp problem */
	lp = mkp2lp(mkp, 1.0);
	x = lp_simplex(lp, NULL);
	lp_free(lp);

	/* counting 'std' ones assigned variables */
	n_std_ones = 0;
	for( i = 0 ; i < n ; i++ )
		if( x[i] == 1.0 && !assigned[i] )
			n_std_ones++;


	scale = 1.0;
	tic = 2./(double)n;
	/* searching for not assigned fracional variables */
	do{
		/* solving lp problem */
		lp = mkp2lp(mkp, scale+tic);
		x = lp_simplex(lp, NULL);
		lp_free(lp);

		lesser_val = 1.0;
		n_fracs_found = n_ones_found = n_ones = 0;
		/* counting n_fracs_found not assigned */
		for( i = 0 ; i < n ; i++ ){
			if( x[i] == 1.0 )
				n_ones++;
			if( x[i] >= 1.0 && !assigned[i] )
				n_ones_found++;
			else if( x[i] > 0.0 && !assigned[i] ){
				n_fracs_found++;
				if( x[i] < lesser_val ){
					lesser_val = x[i];
					lesser_r_var = i;
				}
			}
		}

		/* adjust scale */
		if( n_ones_found > n_std_ones ) tic /= 2;
		else if( !n_fracs_found ) { scale += tic; tic *= 2; }
	/* while: no frac was found
	 *   AND some 0.0 variable turned to one
	 *   AND problem is not 'unbounded' yet */
	/* FIXME: check while expression */
	}while( !n_fracs_found && (n_ones < n) );

	/* lesser nonassigned frac is 'lesser_r_var' and its scale point is 'scale+tic' */
	if( n_ones == n) *var = -1;
	else *var = lesser_r_var;

	return scale+tic;
}

double _do_left_search(MKP *mkp, int *assigned, int *var){
	int i, n;
	int n_std_zeros;
	int n_zeros;
	int n_zeros_found;
	int n_fracs_found;
	int greater_l_var;
	double greater_val;
	double *x;
	double tic;
	double scale;
	LP *lp;

	n = mkp->n;

	/* solve std lp problem */
	lp = mkp2lp(mkp, 1.0);
	x = lp_simplex(lp, NULL);
	lp_free(lp);

	/* counting 'std' zeros assigned variables */
	n_std_zeros = 0;
	for( i = 0 ; i < n ; i++ )
		if( !assigned[i] && x[i] == 0.0 )
			n_std_zeros++;

	scale = 1.0;
	tic = 2./(double)n;
	/* searching for not assigned fracional variables */
	do{
		/* solving lp problem */
		lp = mkp2lp(mkp, scale-tic);
		x = lp_simplex(lp, NULL);
		lp_free(lp);

		greater_val = 0.0;
		n_fracs_found = n_zeros_found = n_zeros = 0;
		/* counting n_fracs_found not assigned */
		for( i = 0 ; i < n ; i++ ){
			if( x[i] == 0.0 )
				n_zeros++;
			if( !assigned[i] ){
				if( x[i] == 0.0 ){
					n_zeros_found++;
				} else if( x[i] < 1.0 ){
					n_fracs_found++;
					if( x[i] > greater_val ){
						greater_val = x[i];
						greater_l_var = i;
					}
				}
			}
		}

		/* adjust scale */
		if( n_zeros_found > n_std_zeros ) tic /= 2; /* if tic was too large */
		else if( !n_fracs_found ) {scale -= tic; tic *= 2; }     /* if tic was too small */
	/* while: no frac was found
	 *   AND some non-zero variable turned zero
	 *   AND problem is not 'unfeasible' yet */
	/* FIXME: check while expression */
	}while( !n_fracs_found && (n_zeros < n) );

	/* lesser nonassigned frac is 'lesser_r_var' and its scale point is 'scale+tic' */
	if( n_zeros == n ) *var = -1;
	else *var = greater_l_var;

	return scale-tic;
}

/*
 * Second version of function, searching from "center".
 * */
double *mkp_my_core_vals2(MKP *mkp){
	int i, n, nfracs, nassigned, *assigned;
	double *x, r_scale, l_scale, *d_assigned, lesser_val;
	int greater_l_var, lesser_r_var;      /* index of the greater fractional variable */
	LP *lp;

	/* initializing */
	n = mkp->n;
	assigned = int_array_init(NULL, n, 0);
	nassigned = 0;

	/* solving lp */
	lp = mkp2lp(mkp, 1.0);
	x = lp_simplex(lp, NULL);
	lp_free(lp);

	/* assigning fracs */
	do{
		nfracs = 0;
		lesser_val = 1.0;
		for( i = 0 ; i < n ; i++ ){
			/* if variable is frac */
			if( x[i] > 0.0 && x[i] < 1.0 ){
				/* if is not assigned yet */
				if( !assigned[i] ){
					nfracs++;
					/* if is lesser */
					if( x[i] < lesser_val ){
						lesser_r_var = i;
						lesser_val = x[i];
					}
				}
			}
		}

		/* assign lesser frac variable */
		if( nfracs ){
			assigned[lesser_r_var] = n+nassigned;
			nassigned++;
		}
	}while( nfracs );
	printf(" fracs for 1.0 done\n");

	
	r_scale = l_scale = 1.0;
	r_scale = _do_right_search(mkp, assigned, &lesser_r_var);
	l_scale = _do_left_search(mkp, assigned, &greater_l_var);
	while( nassigned < n ){
		/* which side is less scalabled? */
		if( r_scale-1.0 < 1.0-l_scale ){ /* right is less */
			assigned[lesser_r_var] = n-nassigned;
			nassigned++;
			r_scale = _do_right_search(mkp, assigned, &lesser_r_var);
		}else{  /* left is less */
			assigned[greater_l_var] = n+nassigned;
			nassigned++;
			l_scale = _do_left_search(mkp, assigned, &greater_l_var);
		}
		printf("-%d\n", nassigned); fflush(stdout);
	}

	/* convert assigned to double */
	d_assigned = double_array_alloc(n);
	for( i = 0 ; i < n ; i++ )
		d_assigned[i] = (double)assigned[i];

	free(assigned);

	return d_assigned;
}

/* mkp_reduced
 * generates a reduced MKP, with a subset of variables.
 *  mkp: the original problem
 *  var_vals: values of i-th variables:
 *    - 1: fixed on 1
 *    - 0: fixed on 0
 *    - other: free
 ***********************************************************/
MKP *mkp_reduced(MKP *mkp, int *var_vals){
	MKP *nmkp;
	int i, var, j, n, m;
	int *ones, *zeros, *frees;
	int nones, nzeros, nfrees;
	mkpnum *p, **w, *b;

	/* problem parameters */
	p = mkp->p;
	w = mkp->w;
	b = mkp->b;
	n = mkp->n;
	m = mkp->m;

	/* allocing auxiliary arrays */
	ones = (int*)malloc(n*sizeof(int));
	zeros = (int*)malloc(n*sizeof(int));
	frees = (int*)malloc(n*sizeof(int));
	nones = nzeros = nfrees = 0;

	/* separating variables */
	for( i = 0 ; i < n ; i++ ){
		if( var_vals[i] == 1 )
			ones[nones++] = i;
		else if( var_vals[i] == 0 )
			zeros[nzeros++] = i;
		else frees[nfrees++] = i;
	}

	/*** setting new problem ***/
	nmkp = mkp_alloc(nfrees, m);

	/*copying weights and profits */
	for( i = 0 ; i < nfrees ; i++ ){ /* each free variable */
		var = frees[i];
		nmkp->p[i] = mkp->p[var];     /* copying profit */
		for( j = 0 ; j < m ; j++ )   /* each dimension */
			nmkp->w[j][i] = mkp->w[j][var]; /* copying weights */
	}

	/* copying capacities */
	mkpnum_array_copy(nmkp->b, mkp->b, m);

	/* reducing capacities spent by "ones" variables */
	for( i = 0 ; i < nones ; i++ ){
		var = ones[i];
		for( j = 0 ; j < m ; j++ )
			nmkp->b[j] -= mkp->w[j][var];
	}

	/* freeing auxiliary arrays */
	free(ones);
	free(zeros);
	free(frees);

	return nmkp;
}

/*
 * Returns a MKP with a subset of (selected) capacity constraints.
 * */
MKP *mkp_select_contraints(MKP *mkp, int *cons, int m2){
	int n, m, i, j;
	MKP *mkp2;

	n = mkp->n;
	m = mkp->m;

	mkp2 = mkp_alloc(n, m2);

	mkp2->p = mkpnum_array_copy(mkp2->p, mkp->p, n);
	for( j = 0 ; j < m2 ; j++ ){
		mkp2->w[j] = mkpnum_array_copy(mkp2->w[j], mkp->w[cons[j]], n);
		mkp2->b[j] = mkp->b[cons[j]];
	}

	return mkp2;
}

/*
 * Returns a (min)surrogate relaxation for a MKP instance.
 *   - multips: the multiplier of each contraint;
 * */
MKP *mkp_surrogate(MKP *mkp, double *multips){
	MKP *mkp2;
	int i, j, dim, mult, n, m, m2;

	n = mkp->n;
	m = mkp->m;

	mkp2 = mkp_alloc(n, 1);
	
	/* coping profit and capacities */
	mkp2->p = mkpnum_array_copy(mkp2->p, mkp->p, n);
	mkp2->w[0] = mkpnum_array_init(mkp2->w[0], n, 0);
	mkp2->b[0] = 0;

	/* coping-multipling weights */
	for( j = 0 ; j < m ; j++ ){
		if( multips[j] > 0 ){
			for( i = 0 ; i < n ; i++ )
				mkp2->w[0][i] += multips[j]*mkp->w[j][i];
			mkp2->b[0] += multips[j]*mkp->b[j];
		}
	}

	return mkp2;
}

/*
 * Returns a (min)surrogate relaxation for a MKP instance using
 *    the dual solution variables as multipliers.
 * */
MKP *mkp_dual_surrogate(MKP *mkp){
	double *multips;
	MKP *mkp2;

	multips = mkp_solve_dual_with_scip(mkp);
	mkp2 = mkp_surrogate(mkp, multips);

	free(multips);

	return mkp2;
}

/* Extracts a cardinality constraint: a maximum number of itens that can be
 * putted in knapsack.
 * */
int mkp_max_cardinality(MKP *mkp){
	int i, n, nmax;
	mkpnum b_left;
	mkpnum *ws;

	n = mkp->n;

	ws = mkpnum_array_copy(NULL, mkp->w[0], n);
	mkpnum_array_qsort(ws, n);

	nmax = 0;
	b_left = mkp->b[0];

	for( i = 0 ; i < n && b_left > 0 ; i++ ){
		if( b_left > ws[i] ){
			nmax++;
			b_left -= ws[i];
		}else{
			b_left = 0;
		}
	}

	return nmax;
}

/* Returns an array with the indexs of variables, sorted by decreasing order
 * of efficiency.
 * */
int *mkp_core_val(MKP *mkp, char type, int *center){
	double *x, *vals, sum, sum2, *r;
	mkpnum *p, *b, **w;
	int i, j, k, n, m;
	int *idxs;

	n = mkp->n;
	m = mkp->m;
	p = mkp->p;
	w = mkp->w;
	b = mkp->b;
	vals = (double*)malloc(n*sizeof(double));
	r = (double*)malloc(m*sizeof(double));

	switch(type){
		/* Simple Efficiency */
		case MKP_CORE_SIMPLE:
		for( i = 0 ; i < n ; i++ ){
			sum = 0.0;
			for( j = 0 ; j < m ; j++ )
				sum += w[j][i];
			vals[i] = p[i]/sum;
		}
		break;

		/* Scaled Efficiency */
		case MKP_CORE_SCALED:
		for( i = 0 ; i < n ; i++ ){
			sum = 0.0;
			for( j = 0 ; j < m ; j++ )
				sum += w[j][i]/b[j];
			vals[i] = p[i]/sum;
		}
		break;

		/* Senju and Toyoda */
		case MKP_CORE_ST:
		for( i = 0 ; i < n ; i++ ){
			sum = 0.0;
			for( j = 0 ; j < m ; j++ )
				for( k = 0 ; k < n ; k++ )
					sum += w[j][i]*(w[j][k]-b[j]);
			vals[i] = p[i]/sum;
		}
		break;

		/* Freville-Plateau efficiency */
		case MKP_CORE_FP:
		for( j = 0 ; j < m ; j++ ){ /* relevance values */
			sum = sum2 = 0;
			for( k = 0 ; k < n ; k++ ){
				sum += w[j][k] - b[j];
				sum2 += w[j][k];
			}
			r[j] = sum/sum2;
		}
		for( i = 0 ; i < n ; i++ ){
			sum = 0;
			for( j = 0 ; j < m ; j++ )
				sum += r[j]*w[j][i];
			vals[i] = p[i]/sum;
		}
		break;

		/* Duals efficiency */
		case MKP_CORE_DUALS:
		x = mkp_solve_dual_with_scip(mkp);
		for( j = 0 ; j < n ; j++ ){
			sum = 0.0;
			for( i = 0 ; i < m ; i++ )
				sum += w[i][j]*x[i];
			vals[j] = p[j]/sum;
		}
		free(x);
		break;

		/* proposed efficiency */
		case MKP_CORE_LP:
		free(vals);
		vals = mkp_my_core_vals(mkp);
		break;

		/* proposed efficiency, second method */
		case MKP_CORE_LP2:
		free(vals);
		vals = mkp_my_core_vals2(mkp);
		break;
	}
	idxs = double_index_sort(vals, n);

	/* FIXME: lines below wont work for efficiencies other than DUALS */
	/* looking for center */
	j = 0; k = n-1;
	i = n/2;
	while( !feq(vals[idxs[i]], 1.0) ){ /* floating point comparison */
		if( vals[idxs[i]] > 1.0 )
			j = i+1;
		else
			k = i-1;
		i = (j+k)/2;
	}
	k = j = i;
	/* looking for "first" '1.0' */
	while( j > 0 && feq(vals[idxs[j-1]], 1.0))
		j--;
	/* looking for "last" '1.0' */
	while( k < n-1 && feq(vals[idxs[k+1]], 1.0))
		k++;
	if(center)
		*center = (j+k)/2;

	free(r);
	free(vals);

	return idxs;
}

/* Returns a core problem for a MKP problem.
 * - core_size number of variables on core problem;
 *   Will record fix of variables on vars_fix array:
 *     1 - fixed on 1;
 *     0 - fixed on 0;
 *     other - free (in of core problem).
 * */
MKP *mkp_core_problem(MKP *mkp, int core_size, int **vars_fix){
	MKP *mkp_core;
	int i, j, k, idx, n, ncore, m, n_fst_fixed, a, b;
	int center;
	int *efficieny_ordering;

	n = mkp->n;
	m = mkp->m;
	ncore = 0;

	/* computing efficiency measure */
	if( !(*vars_fix) )
		(*vars_fix) = (int*)malloc(n*sizeof(int));
	efficieny_ordering = mkp_core_val(mkp, MKP_CORE_DUALS, &center);
	mkp_core = mkp_alloc(core_size, m);

	/* initializing capacities */
	for( j = 0 ; j < m ; j++ )
		mkp_core->b[j] = mkp->b[j];

	/* setting var fixing values */
	a = center-1;
	b = center+1;
	(*vars_fix)[efficieny_ordering[center]] = -1;
	core_size--;
	while( core_size-- ){
		if( core_size % 2 && a+1 )
			idx = (*vars_fix)[efficieny_ordering[a--]] = -1;
		else
			idx = (*vars_fix)[efficieny_ordering[b++]] = -1;
	}
	while( a+1 )
		(*vars_fix)[efficieny_ordering[a--]] = 1;
	while( b < n )
		(*vars_fix)[efficieny_ordering[b++]] = 0;

	/* building core problem, using vars_fixing values */
	j = 0;
	for( i = 0 ; i < n ; i++ ){
		if( (*vars_fix)[i] == -1 ){
			mkp_core->p[j] = mkp->p[i];    /* coping profit */
			for( k = 0 ; k < m ; k++ )     /* coping weights */
				mkp_core->w[k][j] = mkp->w[k][i];
			j++;
		}else if( (*vars_fix)[i] == 1 ){
			for( k = 0 ; k < m ; k++ )     /* consuming resources */
				mkp_core->b[k] -= mkp->w[k][i];
		}
	}

	free(efficieny_ordering);

	return mkp_core;
}

/*******************************************************************************
 ***                               MKPSOL                                    ***
*******************************************************************************/
MKPSol *mkpsol_new(MKP *mkp){
	int i;
	MKPSol *mkpsol;

	mkpsol = (MKPSol*)malloc(sizeof(MKPSol));
	mkpsol->x = int_array_init(NULL, mkp->n, 0);
	mkpsol->nx = 0;
	mkpsol->b_left = mkpnum_array_copy(NULL, mkp->b, mkp->m);
	mkpsol->obj = 0;
	mkpsol->feasible = 1;
	mkpsol->mkp = mkp;

	return mkpsol;
}

MKPSol *mkpsol_new_random(MKP *mkp){
	MKPSol *sol = mkpsol_new(mkp);
	int i, n;

	n = mkp->n;

	mkp->idxs = int_array_shuffle(mkp->idxs, n);

	for( i = 0 ; i < n ; i++ ){
		sol = mkpsol_add_item(sol, mkp->idxs[i]);
		if(!sol->feasible)
			mkpsol_rm_item(sol, mkp->idxs[i]);
	}

	return sol;
}

MKPSol *mkpsol_new_random_search(MKP *mkp){
	MKPSol *sol, *new;
	int i, n;

	sol = mkpsol_new_random(mkp);
	new = mkpsol_local_search(sol, sol->mkp->n/25);
	mkpsol_free(sol);

	return new;
}

MKPSol *mkpsol_new_lpls(MKP *mkp){
	MKPSol *mkpsol;

	mkpsol = mkpsol_local_search(mkp_get_lp_trunc(mkp), 4);

	return mkpsol;
}

MKPSol *mkpsol_add_item(MKPSol *mkpsol, int a){
	MKP *mkp;
	int i, m;

	mkp = mkpsol->mkp;
	m = mkp->m;

	if( mkpsol->x[a] ) {
        printf(" value: %d\n", mkpsol->x[a]);
        mkpsol_fprint(stdout, mkpsol, 1);
		fprintf(stderr, "%s error: item %d-th item already in knapsak.\n",
			__PRETTY_FUNCTION__, a+1);
		return mkpsol;
	}

	mkpsol->x[a] = 1;
	mkpsol->nx++;
	mkpsol->obj += mkp->p[a];
	for( i = 0 ; i < m ; i++ ){
		mkpsol->b_left[i] -= mkp->w[i][a];
		if( mkpsol->b_left[i] < 0LL )
			mkpsol->feasible = 0;
	}

	return mkpsol;
}

MKPSol *mkpsol_rm_item(MKPSol *mkpsol, int a){
	MKP *mkp;
	int i, m;

	mkp = mkpsol->mkp;
	m = mkp->m;

	if( !mkpsol->x[a] ) {
		fprintf(stderr, "%s error: item %d-th item not in knapsak.\n",
			__PRETTY_FUNCTION__, a+1);
		return mkpsol;
	}

	mkpsol->x[a] = 0;
	mkpsol->nx--;
	mkpsol->obj -= mkp->p[a];
	mkpsol->feasible = 1;
	for( i = 0 ; i < m ; i++ ){
		mkpsol->b_left[i] += mkp->w[i][a];
		if( mkpsol->b_left[i] < 0LL )
			mkpsol->feasible = 0;
	}

	return mkpsol;
}

MKPSol *mkpsol_flip_item(MKPSol *mkpsol, int a){
	if( mkpsol->x[a] ) mkpsol = mkpsol_rm_item(mkpsol, a);
	else mkpsol = mkpsol_add_item(mkpsol, a);
	return mkpsol;
}

MKPSol *mkpsol_read_from_filename(char *filename, MKP *mkp){
	MKPSol *mkpsol;
	FILE *fin;

	assert_faccess(filename, R_OK);
	fin = fopen(filename, "r");
	mkpsol = mkpsol_read_from_file(fin, mkp);
	fclose(fin);

	return mkpsol;
}

MKPSol *mkpsol_read_from_file(FILE *fin, MKP *mkp){
	MKPSol *mkpsol;
	int i, a, n;
    int ferr;

	n = mkp->n;
	mkpsol = mkpsol_new(mkp);

	ferr = fscanf(fin, "%d", &a);
    while( ferr != EOF ){
		mkpsol_add_item(mkpsol, a-1);
	    ferr = fscanf(fin, "%d", &a);
	}

	return mkpsol;
}

MKPSol *mkpsol_copy(MKPSol *mkpsol){
	MKPSol *mkpsol_new;

	mkpsol_new = (MKPSol*)malloc(sizeof(MKPSol));
	mkpsol_new->x = int_array_copy(NULL, mkpsol->x, mkpsol->mkp->n);
	mkpsol_new->nx = mkpsol->nx;
	mkpsol_new->b_left = mkpnum_array_copy(NULL, mkpsol->b_left, mkpsol->mkp->m);
	mkpsol_new->obj = mkpsol->obj;
	mkpsol_new->feasible = mkpsol->feasible;
	mkpsol_new->mkp = mkpsol->mkp;

	return mkpsol_new;
}

int mkpsol_dominated_by(MKPSol *ms1, MKPSol *ms2){
	int i, m;

	m = ms1->mkp->m;
	for( i = 0 ; i < m ; i++ )
		if( ms2->b_left[i] < ms1->b_left[i] )
			return 0;
	return( ms2->obj >= ms1->obj );
}

int mkpsol_dominates(MKPSol *ms1, MKPSol *ms2){
	int i, m;

	m = ms1->mkp->m;
	if( ms1->obj <= ms2->obj ) return 0;
	for( i = 0 ; i < m ; i++ )
		if( ms1->b_left[i] < ms2->b_left[i] )
			return 0;
	return 1;
}

int mkpsol_get_core_size(MKPSol *mkpsol, int *first_0p, int *last_1p){
	int first_0, last_1, i, n, core_size;
	int *x;

	n = mkpsol->mkp->n;
	x = mkpsol->x;
	i = 0;
	/* finding first 0 */
	while( i < n && x[i] == 1)
		i++;
	first_0 = i;
	/* finding last 1 */
	while( ++i < n )
		if( x[i] == 1)
			last_1 = i;

	core_size = last_1 - first_0 + 1;
	if(first_0p) *first_0p = first_0;
	if(last_1p) *last_1p = last_1;

	return core_size;
}

/*
 * prt_sol:
 *   0 - no solution
 *   1 - with solution
 *   2 - solution only
 * */
void mkpsol_fprint(FILE *fout, MKPSol *mkpsol, char ptr_sol){
	mkpnum *b;
	int i, n, m;

	b = mkpsol->mkp->b;
	m = mkpsol->mkp->m;
	n = mkpsol->mkp->n;

	if( ptr_sol != 2 ){
		mkpnum_fprintf(fout, mkpsol->obj);
		fprintf(fout, ";[");
		for( i = 0 ; i < m ; i++){
			mkpnum_fprintf(fout, b[i]-mkpsol->b_left[i]);
			fprintf(fout, "%s", i+1==m?"]":",");
		}
	}
	if(ptr_sol){
		if( ptr_sol == 1)
			printf(";");
		for( i = 0 ; i < n ; i++)
			fprintf(fout, "%d", mkpsol->x[i]);
	}
	fprintf(fout, "\n");
	return;
}

void mkpsol_free(MKPSol *mkpsol){
	free(mkpsol->x);
	free(mkpsol->b_left);
	free(mkpsol);
	return;
}

MKPSol *mkpsol_solve_with_scip(MKP *mkp, double maxtime, double capacity_scale, char linear){
	MKPSol *sol;
	int i, n;
	double *x;

	n = mkp->n;
	sol = mkpsol_new(mkp);

	/* Solve MKP */
	x = mkp_solve_with_scip(mkp, maxtime, capacity_scale, linear);

	/* Set solution */
	for( i = 0 ; i < n ; i++ )
		if( x[i] == 1.0 )
			mkpsol_add_item(sol, i);

	free(x);

	return sol;
}

MKPSol *mkpsol_from_lp(MKP *mkp){
	return mkpsol_solve_with_scip(mkp, 60.0, 1.0, 1);
}

MKPSol *greedy_mkp(MKP *mkp){
	unimplemented();
}

MKPSol *mkpsol_local_search(MKPSol *mkpsol, int niter){
	MKPSol *current, *best;
	mkpnum current_profit;
	int best_item, item, i, j, n, q, *idxs;

	/* initialization */
	idxs = mkpsol->mkp->idxs;
	current = mkpsol_copy(mkpsol);
	best = mkpsol_copy(mkpsol);
	current_profit = current->obj;
	n = mkpsol->mkp->n;

	/* iteration loop */
	q = 1+n/2;
	for( i = 0 ; i < niter ; i++ ){
		idxs = int_array_shuffle(idxs, n);
		best_item = 0;
		current_profit = 0;
		/* sort moves */
		for( j = 0 ; j < q ; j++ ){
			item = idxs[j];
			current = mkpsol_flip_item(current, item);
			/* best move yet? */
			if( current->obj > current_profit && current->feasible )
				{ best_item = item; current_profit = current->obj; }
			current = mkpsol_flip_item(current, item);
		}
		current = mkpsol_flip_item(current, best_item);
		/* is best global? */
		if( current->obj > best->obj )
			{ mkpsol_free(best); best = mkpsol_copy(current); }
	}

	mkpsol_free(current);

	return best;
}

MKPSol *mkpsol_greedy_fill(MKPSol *mkpsol){
	int i, j, n, m, *idxs, a;

	n = mkpsol->mkp->n;

	/* array of items indexs (sort by non-descreasing efficiency) */
	idxs = double_index_sort(mkp_get_em(mkpsol->mkp), n);

	/* for each item, try to fit it in knapsack */
	for( i = 0 ; i < n ; i++ ){
		a = idxs[i];
		if( !mkpsol->x[i] )
			mkpsol_add_item(mkpsol, a);
		if( !mkpsol->feasible )
			mkpsol_rm_item(mkpsol, a);
	}

	return mkpsol;
}

/* repair a solution, removing at first itens that minimum reduces profit */
MKPSol *mkpsol_greedy_repair(MKPSol *mkpsol){
	int i, a, n;
	mkpnum profit;
	n = mkpsol->mkp->n;

	/* removing item */
	for( i = 0 ; i < n && !mkpsol->feasible ; i++ )
		if( mkpsol->x[n-i-1] )
			mkpsol_rm_item(mkpsol, n-i-1);
	
	return mkpsol;
}

/*  */
MKPSol *mkpsol_cross(MKPSol *child, MKPSol *father, int c){
	int i, n, *idxs, idx;
	int a, b;

	n = child->mkp->n;
	idxs = child->mkp->idxs;
	int_array_shuffle(idxs, n);

	/* copy 'c' variables (chosen reandomly) from father */
	for( i = 0 ; i < c ; i++ ){
		idx = idxs[i];
		a = father->x[idx];
		b = child->x[idx];

		/* flip if not equal */
		if( a != b )
			mkpsol_flip_item(child, idx);
	}
	
	/* repair, if not feasibble */
	if( !child->feasible ){
		child = mkpsol_greedy_repair(child);
	}

	return child;
}

/* n/2 */
MKPSol *mkpsol_cross50(MKPSol *child, MKPSol *father)
	{ return mkpsol_cross(child, father, (child->mkp->n/2)); }
/* n/5 */
MKPSol *mkpsol_cross20(MKPSol *child, MKPSol *father)
	{ return mkpsol_cross(child, father, (child->mkp->n/5)); }
/* n/10 */
MKPSol *mkpsol_cross10(MKPSol *child, MKPSol *father)
	{ return mkpsol_cross(child, father, (child->mkp->n/10)); }


/*
 * Nemhauser-Ullman Algorithm for MKP.
 * */
Array *mkp_nemull(MKP *mkp){
	Array *dom_sets;
    Array *merged_sets;
	MKPSol *new_sol, *old_sol, *best_sol;
	int n, m, i, j, k, n_dom_sets, n_merged_sets, not_dominated_by;
	mkpnum **w, *p, *b;
	mkpnum best_profit;

	MKP *fixed_mkp;
	LP *fixed_lp;
	double *lp_sol;
	double lp_obj;
	int *x;
	int npromissing;
	int use_lp_relax;

	/* user lp relaxation upper bound */
	use_lp_relax = 0;

	n = mkp->n;
	x = (int*)malloc(n*sizeof(int));

	/* initializing sets */
	dom_sets = array_new();         /* "old" set of solutions (nodes) */
	merged_sets = array_new();      /* "new" set of solutions (being built) */

	best_profit = 0;
    /* for each item.. */
	for( i = 0 ; i < n ; i++ ){
        /* for each existing solution (node) */
		n_dom_sets = array_get_size(dom_sets);
		for( j = 0 ; j < n_dom_sets ; j++ ){
            /* get the solution */
			old_sol = array_get(dom_sets, j);
            /* inserting solution as it is (i.e., without i-th item) */
			array_insert(merged_sets, old_sol);
            /* inserting new solutions (i.e. with i-th item) */
			new_sol = mkpsol_copy(old_sol);
			mkpsol_add_item(new_sol, i);
			array_insert(merged_sets, new_sol);
		}
		/* the i-th item alone is a candidate set */
		new_sol = mkpsol_new(mkp);
		mkpsol_add_item(new_sol, i);
		array_insert(merged_sets, new_sol);

		/* now emptying the dom sets */
		dom_sets = array_empty(dom_sets);

		/* filtering: checking dominance of each set */
		npromissing = 0;
		for( j = 0 ; j < array_get_size(merged_sets); j++ ){
			new_sol = array_get(merged_sets, j);
			not_dominated_by = 1;
			/* scan all current sets */
			for( k = 0 ; k < array_get_size(merged_sets) && not_dominated_by ; k++ ){
				old_sol = array_get(merged_sets, k);
				if( new_sol != old_sol )
					not_dominated_by &= !mkpsol_dominated_by(new_sol, old_sol);
				not_dominated_by &= new_sol->feasible; /* only feasible sets (optimization) */
			}
			/* the solution is not dominated by no one? */

			if( not_dominated_by ){
				if( use_lp_relax ){
					/* test promissing upperbound */
					for( k = 0 ; k < i+1 ; k++ )
						x[k] = new_sol->x[k];
					for( ; k < n ; k++ )
						x[k] = -1;
					fixed_mkp = mkp_reduced(mkp, x);
					lp_obj = mkp_get_lp_obj(fixed_mkp);
					mkp_free(fixed_mkp);
					if( lp_obj + new_sol->obj >= (double)best_profit ){
						npromissing++;
						array_insert(dom_sets, new_sol);       /* add */
					}
				}else{
					array_insert(dom_sets, new_sol);       /* add */
				}
	
				if( new_sol->obj > best_profit ){      /* is best known? */
					best_sol = new_sol;
					best_profit = new_sol->obj;
				}
			}else{        /* free set */
				mkpsol_free(new_sol);
				array_remove(merged_sets, j--);
			}
		}
		//printf("%d - %d dom sets (%d promissing, best=%lld)\n", i+1, array_get_size(dom_sets), npromissing, best_profit);
		/* empty the merged set struct */
		merged_sets = array_empty(merged_sets);
	}

	free(x);
	array_free(merged_sets);

	return dom_sets;
}

/*
 * Multidimensional Minknap
 */
Array *mkp_minknap(MKP *mkp, MKPSol *initsol){
	Array *sols;

	sols = array_new();
	
	return sols;
}

/*******************************************************************************
*       DES interface implementation
*******************************************************************************/
/* activate */
double mkp_des_activate(double trust, int val, double step){
	double prob;

	/*step *= step;*/
	prob = val ? 1.0-trust : trust;
	prob = prob*step + 0.5*(1.0-step);

	return prob;
}

/* set */
MKPSol *mkpsol_set(MKPSol *mkpsol, int a, int val){
	if( mkpsol->x[a] != val ){
		if(val) mkpsol_rm_item(mkpsol, a);
		else mkpsol_add_item(mkpsol, a);
	}

	return mkpsol;
}

/* get */
int mkpsol_get(MKPSol *mkpsol, int a){
	return mkpsol->x[a];
}

/* cmp-profit */
int mkpsol_cmp_profit(MKPSol *ms1, MKPSol *ms2){
	if( ms1->obj > ms2->obj) return 1;
	else if(ms1->obj < ms2->obj ) return (-1);
	return 0;
}
/* cmp-j-th-weight */
int mkpsol_cmp_weight(MKPSol *ms1, MKPSol *ms2, int *j){
	mkpnum b_left1, b_left2;
	b_left1 = ms1->b_left[*j];
	b_left2 = ms2->b_left[*j];
	if( b_left1 < b_left2 ) return 1;
	else if( b_left1 > b_left2 ) return (-1);
	return 0;
}
/* obj */
double mkpsol_fitness(MKPSol *mkpsol){
	return (double)mkpsol->obj;
}
/* feasible */
int mkpsol_feasible(MKPSol *mkpsol){
	return mkpsol->feasible;
}
/* new solution */
MKPSol *mkp_des_new_solution(MKP *mkp){
	return mkpsol_new_random(mkp);
}

/* random repair a solution, removing random itens until feasibility */
MKPSol *mkpsol_repair(MKPSol *mkpsol){
	int i, n, *idxs;
	n = mkpsol->mkp->n;

	/* shuffling itens */
	idxs = mkpsol->mkp->idxs;
	idxs = int_array_shuffle(idxs, n);

	/* removing item */
	for( i = 0 ; i < n && !mkpsol->feasible ; i++ )
		if( mkpsol->x[idxs[i]] )
			mkpsol_rm_item(mkpsol, idxs[i]);
	
	return mkpsol;
}

/* Returns a original solution, extracted from a MKP core problem solution. */
MKPSol *mkpsol_from_mkp_core(MKPSol *core_sol, MKP *orig_mkp, int *vars_fix){
	MKPSol *sol;
	int i, j, n, m;

	n = orig_mkp->n;
	m = orig_mkp->m;

	sol = mkpsol_new(orig_mkp);

	j = 0;
	for( i = 0 ; i < n ; i++ ){
		/* fixed in '1' */
		if( vars_fix[i] == 1 ){
			mkpsol_add_item(sol, i);
			//printf("%d was fixed at 1\n", i+1);
		/* variable in core problem */
		}else if( vars_fix[i] == -1 ){
			if( core_sol->x[j++] ){
				mkpsol_add_item(sol, i);
				//printf("%d was free, but has one\n", i+1);
			}
		}
	}

	return sol;
}

DES_Interface *mkp_des_interface(){
	DES_Interface *desi;

	desi = (DES_Interface*)malloc(sizeof(DES_Interface));
	desi->activate = NULL;
	desi->set = (des_set_f)mkpsol_set;
	desi->get = (des_get_f)mkpsol_get;
	desi->fitness = (des_fitness_f)mkpsol_fitness;
	desi->feasible = (des_feasible_f)mkpsol_feasible;
	desi->repair = (des_repair_f)mkpsol_repair;
	desi->new_solution = (des_new_solution_f)mkpsol_new_random;
	desi->copy_solution = (des_copy_solution_f)mkpsol_copy;
	desi->free_solution = (des_free_solution_f)mkpsol_free;

	return desi;
}

SFL_Interface *mkp_sfl_interface(){
	SFL_Interface *sfli;
	int cross, newsol;

	cross = 2;
	newsol = 1;

	sfli = (SFL_Interface*)malloc(sizeof(SFL_Interface));
	/* crossing rule */
	switch(cross){
		case 1: sfli->cross = (sfl_cross_f)mkpsol_cross50; break;
		case 2: sfli->cross = (sfl_cross_f)mkpsol_cross20; break;
		case 3: sfli->cross = (sfl_cross_f)mkpsol_cross10; break;
		default: sfli->cross = (sfl_cross_f)mkpsol_cross50; break;
	}
	switch(newsol){
		case 1: sfli->new_solution = (sfl_new_solution_f)mkpsol_new_random; break;
		case 2: sfli->new_solution = (sfl_new_solution_f)mkpsol_new_random_search; break;
		default: sfli->new_solution = (sfl_new_solution_f)mkpsol_new_random; break;
	}
	sfli->fitness = (sfl_fitness_f)mkpsol_fitness;
	sfli->copy_solution = (sfl_copy_solution_f)mkpsol_copy;
	sfli->free_solution = (sfl_free_solution_f)mkpsol_free;

	return sfli;
}

