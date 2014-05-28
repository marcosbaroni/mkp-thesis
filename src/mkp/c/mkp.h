#ifndef MKP_H
#define MKP_H 1

typedef int number;

typedef struct MKP{
	int n;       /* Number of itens */
	int m;       /* Number of dimensions */
	double *p;   /* Profit of itens [n] */
	double **w;  /* Weight of itens [m x n] */
	double *b;   /* Knapsack capacities */
}MKP;

MKP *mkp_read_from_filename(char *filename);
MKP *mkp_read_from_file(FILE *fin);
void mkp_write_to_filename(MKP *mkp, char *filename);
void mkp_write_to_file(MKP *mkp, FILE *fout);

MKP *mkp_alloc(int n, int m);
MKP *mkp_random(int n, int m, int seed);

void mkp_to_zimpl(MKP *mkp, FILE *fout);

/*void mkp_printf(FILE *fout, MKP *mkp);*/
void mkp_free(MKP *mkp);

#endif

