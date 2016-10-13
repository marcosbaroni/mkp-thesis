#ifndef MKP_H
#define MKP_H 1

#include "../../utils/util.h"

typedef struct{
    int n;          /* number of itens */
    int np;         /* number of objectives */
    int **p;        /* the profits [np x n] */
    int *w;         /* the weight [n] */
}MOKP;

MOKP *mokp_alloc(int n, int np);
MOKP *mokp_random(int n, int np);
void mokp_write(FILE *out, MOKP *mokp);
MOKP *mokp_read(FILE *fin);
void mokp_save(char *filename, MOKP *mokp);
MOKP *mokp_open(char *filename);
void mokp_free(MOKP *mokp);

#endif

