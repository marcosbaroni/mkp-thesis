#define STORER_H
#define STORER_H 1

typedef int number;

typedef struct STORER{
	int n;       /* Number of itens */
	int m;       /* Number of dimensions */
	double **w;  /* Weight of itens [n x m] */
	double *p;   /* Profit of itens [n] */
}STORER;

STORER *mkp_read_from_filename(char *filename);
STORER *mkp_read_from_file(FILE *fin);
void mkp_printf(FILE *fout, STORER *mkp);
void mkp_free(STORER *mkp);

#endif

