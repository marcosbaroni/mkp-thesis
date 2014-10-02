#ifndef SUBSETSUM_H
#define SUBSETSUM_H 1

/* SubSet-Sum Problem */
typedef struct SSum{
	int n;
	long b;
	long *w;
}SSum;

SSum *ssum_read_file(char *filename);
SSum *ssum_read(FILE *in);
SSum *ssum_new_random(int n, long bound, double b_ratio);
void ssum_write(FILE *out, SSum *ssum);
void ssum_free(SSum *ssum);
void ssum_fprint(FILE *out, SSum *sum);
void ssum_to_zimpl(FILE *fout, SSum *ssum);
//double **ssum_calc_ptable(int n_max, long b_max, long w_max);

/* SubSet-Sum Solution */
typedef struct SSumSol{
	int *x;      /* decision variable */
	int *sel;    /* selected variables */
	int nx;      /* number of selected itens */
	long sum;    /* total sum  (ssum->b, if "complete" solution) */
	long b_left; /* units left to sum (0, if "complete" solution) */
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
Array *ssum_backtrack(SSum *ssum);

#endif

