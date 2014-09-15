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
void ssum_write(FILE *in, SSum *ssum);
void ssum_free(SSum *ssum);
void mmkp_fprint(FILE *out, SSum *sum);

/* SubSet-Sum Solution */
typedef struct SSumSol{
	int x;
}SSumSol;

SSumSol *ssumsol_read_file(char *filename);
SSumSol *ssumsol_read(FILE *in);
void ssumsol_write(FILE *in, SSumSol *ssumsol);
void ssumsol_free(SSumSol *ssumsol);
void mmkpsol_fprint(FILE *out, SSumSol *sumsol);

#endif

