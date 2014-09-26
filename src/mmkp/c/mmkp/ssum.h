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

/* SubSet-Sum Solution */
typedef struct SSumSol{
	int x;
}SSumSol;

SSumSol *ssumsol_read_file(char *filename);
SSumSol *ssumsol_read(FILE *in);
void ssumsol_write(FILE *in, SSumSol *ssumsol);
void ssumsol_free(SSumSol *ssumsol);
void ssumsol_fprint(FILE *out, SSumSol *sumsol);

#endif

