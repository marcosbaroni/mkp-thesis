#define MKP_H
#define MKP_H 1

typedef int number;

typedef struct MKP{
	int n;       /* Number of itens */
	int m;       /* Number of dimensions */
	double **w;  /* Weight of itens [n x m] */
	double *p;   /* Profit of itens [n] */
}MKP;

MKP *mkp_read_from_filename(char *filename);
MKP *mkp_read_from_file(FILE *fin);
void mkp_printf(FILE *fout, MKP *mkp);
void mkp_free(MKP *mkp);

#endif

