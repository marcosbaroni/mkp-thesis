#ifndef MMKP_H
#define MMKP_H 1

#include "util.h"

typedef struct MMKP{
	int n;       /* Number of itens */
	int m;       /* Number of dimensions */
	int o;       /* Numer of knapsacks */
	NUMBER *p;   /* Profit of itens [n] */
	NUMBER **w;  /* Weight of itens [n x m] */
	NUMBER **b;  /* Knapsack capacities [o x m] */
}MMKP;

/*** memory management ***/
MMKP *mmkp_alloc(int n, int m);
MMKP *mmkp_random(int n, int m);
void mmkp_free(MMKP *mmkp);
/*void mmkp_printf(FILE *fout, MMKP *mmkp);*/

/*** OUTPUT functions ***/
MMKP *mmkp_read_from_filename(char *filename);
MMKP *mmkp_read_from_file(FILE *fin);
MMKP *mmkp_read_from_gzip(char *filename);

/*** INPUT functions ***/
void mmkp_write_to_filename(MMKP *mmkp, char *filename);
void mmkp_write_to_file(MMKP *mmkp, FILE *fout);
void mmkp_write_to_gzip(MMKP *mmkp, char *filename);

/*** Convertion ***/
void mmkp_to_zimpl(MMKP *mmkp, FILE *fout);


#endif

