#ifndef MMKP_H
#define MMKP_H 1

#include "util.h"

typedef long number;

#define MAX_RAND_WEIGHT 1000
#define MAX_RAND_PROFIT 1000

typedef struct MMKP{
	int n;       /* Number of itens */
	int m;       /* Number of dimensions */
	int o;       /* Numer of knapsacks */
	long *p;   /* Profit of itens [n] */
	long **w;  /* Weight of itens [m x n] */
	long **b;  /* Knapsack capacities [m x o] */
}MMKP;

/*** memory management ***/
MMKP *mmkp_alloc(int n, int m, int o);
MMKP *mmkp_random(int n, int m, int o, double beta);
void mmkp_free(MMKP *mmkp);
/*void mmkp_printf(FILE *fout, MMKP *mmkp);*/


/*** READ/WRITE functions ***/
MMKP *mmkp_read_from_filename(char *filename);
MMKP *mmkp_read_from_file(FILE *fin);
MMKP *mmkp_read_from_gzip(char *filename);
void mmkp_write_to_filename(MMKP *mmkp, char *filename);
void mmkp_write_to_file(MMKP *mmkp, FILE *fout);
void mmkp_write_to_gzip(MMKP *mmkp, char *filename);

/*** Print functions ***/
void mmkp_fprint(FILE *out, MMKP *mmkp);
//void mmkp_to_zimpl(MMKP *mmkp, FILE *fout);


#endif

