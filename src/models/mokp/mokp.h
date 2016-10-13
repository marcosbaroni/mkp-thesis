#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "../../utils/util.h"

MOKP *mokp_alloc(int n, int np){
    MOKP *mokp;
    int i;

    mokp = (MOKP*)malloc(sizeof(MOKP));

    mokp->n = n;
    mokp->np = np;
    mokp->p = (double**)malloc(np*sizeof(double*));
    for( i = 0 ; i < np ; i++ )
        mokp->p[i] = (double*)malloc(n*sizeof(double));
    mokp->w = (double*)malloc(n*sizeof(double));

    return mokp;
}

MOKP *mokp_random(int n, int np){
    MOKP *mokp;
    int i, j;
    double maxcoef = 1000.0;

    mokp = mokp_alloc(n, np);
    for( i = 0 ; i < n ; i++ ){
        for( j = 0 ; j < np ; j++ )
            mokp->p[j][i] = (double)llrand(1000);
        mokp->w[i] = (double)llrand(1000);
    }

    return mokp;
}

void mokp_free(MOKP *mokp){
    int i, np;

    np = mokp->np;
    for( i = 0 ; i < np ; i++ )
        free(mokp->p[i]);
    free(mokp->p);
    free(mokp->w);
    free(mokp);

    return;
}

void mokp_write(FILE *out, MOKP *mokp){
    int i, j;
    int n, np;

    n = mokp->n;
    np = mokp->np;
    fprintf(out, "%d %d\n", mokp->n, mokp->np);
    for( j = 0 ; j < np ; j++ ){
        double_array_write(out, mokp->p[j], n);
        fprintf(out, "\n");
    }
    double_array_write(out, mokp->w, n);
    fprintf(out, "\n");

    return;
}

MOKP *mokp_read(FILE *fin){
    int n, np;
    int i, j;
    int nerr;

    nerr = fscanf(fin, "%d", &n);
    nerr = fscanf(fin, "%d", &np);

    MOKP *mokp = mokp_alloc(n, np);
    for( i = 0 ; i < np ; i++ )
        double_array_read(fin, mokp->p[i], n);
    double_array_read(fin, mokp->w, n);

    return mokp;
}

void mokp_save(char *filename, MOKP *mokp){
    FILE *fout;

    fout = fopen(filename, "w");
    if( !fout ){
        fprintf(stderr, "%s: error trying to open file \"%s\".\n", "mokp_save", filename);
        return;
    }

    mokp_write(fout, mokp);
    fclose(fout);

    return;
}

MOKP *mokp_open(char *filename){
    FILE *fin;
    MOKP *mokp;

    fin = fopen(filename, "r");
    if( !fin){
        fprintf(stderr, "%s: error trying to open file \"%s\".\n", "mokp_load", filename);
        return;
    }

    mokp = mokp_read(fin);
    fclose(fin);

    return mokp;
}

