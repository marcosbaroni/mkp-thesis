/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: november, 2016
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#ifndef SUBSETSUM_H
#define SUBSETSUM_H 1

#include "util.h"

/* SubSet-Sum Problem */
typedef struct SSum{
	int n;
	long long b;
	long long *w;
}SSum;

SSum *ssum_read_file(char *filename);
SSum *ssum_read(FILE *in);
SSum *ssum_new_random(int n, long long bound, double b_ratio);
void ssum_write(FILE *out, SSum *ssum);
void ssum_free(SSum *ssum);
void ssum_fprint(FILE *out, SSum *ssum);
void ssum_to_zimpl(FILE *fout, SSum *ssum);
//double **ssum_calc_ptable(int n_max, long b_max, long w_max);

/* SubSet-Sum Solution */
typedef struct SSumSol{
	int *x;      /* decision variable */
	int *sel;    /* selected variables */
	int nx;      /* number of selected itens */
	long long sum;    /* total sum  (ssum->b, if "complete" solution) */
	long long b_left; /* units left to sum (0, if "complete" solution) */
	SSum *ssum;
}SSumSol;

SSumSol *ssumsol_read_file(char *filename);
SSumSol *ssumsol_read(FILE *in);
SSumSol *ssumsol_new_empty(SSum *ssum);
SSumSol *ssumsol_new(SSum *ssum, int *x);
void ssumsol_write(FILE *in, SSumSol *ssumsol);
void ssumsol_free(SSumSol *ssumsol);
void ssumsol_fprint(FILE *out, SSumSol *sumsol);

/* Enumerate all SSum solutions (backtrack alg) */
Array *ssum_backtrack(SSum *ssum, int enumerate);

#endif

