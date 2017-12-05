/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: november, 2016
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ppm.h"

PPMColor rgb2color(unsigned char r, unsigned char g, unsigned char b){
    PPMColor c;
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
}

void ppmcolor_fprintf(FILE *fout, PPMColor c){
    fprintf(fout, "%d %d %d ", c.r, c.g, c.b);
}

PPM *ppm_new(int cols, int lines){
    PPM *ppm;
    int i, tot;

    ppm = (PPM*)malloc(sizeof(PPM));
    ppm->lines = lines;
    ppm->cols = cols;
    ppm->max = 255;
    ppm->canvas = (PPMColor*)malloc(lines*cols*sizeof(PPMColor));

    tot = lines*cols;
    for( i = 0 ; i < tot ; i++ )
        ppm->canvas[i] = PPM_WHITE;

    return ppm;
}

void ppm_free(PPM *ppm){
    free(ppm->canvas);
    free(ppm);
}

int _ppm_assert_bound(PPM *ppm, int x, int y){
    return (x >= 0 && x < ppm->cols && y >= 0 && y < ppm->lines);
}

PPMColor ppm_get_px(PPM *ppm, int x, int y){
    return ppm->canvas[y*ppm->cols+x];
}

int ppm_get_maxy(PPM *ppm){
    return ppm->lines-1;
}

int ppm_get_maxx(PPM *ppm){
    return ppm->cols-1;
}

PPM *ppm_set_px(PPM *ppm, int x, int y, PPMColor color){
    if(!_ppm_assert_bound(ppm, x, y))
        return ppm;

    ppm->canvas[y*ppm->cols+x] = color;
    return ppm;
}

PPM *ppm_paint_dot(PPM *ppm, int x, int y, PPMColor color, int rad){
    int i, j;

    for( i = -rad ; i < rad ; i++ )
        for( j = -rad ; j < rad ; j++ )
            if( i*i + j*j <= rad*rad )
                ppm_set_px(ppm, x+i, y+j, color);

    return ppm;
}

PPM *ppm_paint_vline(PPM *ppm, int x, int low_y, int hig_y, int width, PPMColor color){
    int i, j, med;
    med = width/2;
    for( j = 0 ; j < width ; j++ )
        for( i = low_y ; i <= hig_y ; i++ )
            ppm_set_px(ppm, x-med+j, i, color);
    return ppm;
}

PPM *ppm_paint_hline(PPM *ppm, int low_x, int hig_x, int y, int width, PPMColor color){
    int i, j, med;
    med = width/2;
    for( j = 0 ; j < width ; j++ )
        for( i = low_x ; i <= hig_x ; i++ )
            ppm_set_px(ppm, i, y-med+j, color);
    return ppm;
}

void ppm_write(PPM *ppm, char *filename){
    FILE *fout;
    int i, j;
    PPMColor color;

    fout = fopen(filename, "w");

    fprintf(fout, "P3\n");
    fprintf(fout, "%d %d\n", ppm->cols, ppm->lines);
    fprintf(fout, "%d\n", ppm->max);
    for( i = 0 ; i < ppm->lines; i++ ){
        for( j = 0 ; j < ppm->cols; j++ )
            ppmcolor_fprintf(fout, ppm_get_px(ppm, j, i));
        fprintf(fout, "\n");
    }

    fclose(fout);

    return;
}

