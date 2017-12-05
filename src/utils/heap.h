/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: june, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#ifndef HEAP_H
#define HEAP_H 1

#include "util.h"

/*******************************************************************************
 ***     (MIN/MAX) HEAP
*******************************************************************************/
typedef struct Heap{
    int n;
    int nmax;
    void **arr;
    char is_min;   /* if it is a min heap (otherwise is a max heap ) */
    /* value functions */
    cmp_f cmp;
    cmp_r_f cmp_r;
    void *arg;
}Heap;

Heap *heap_new(int nmax, cmp_f cmp, char is_min);
Heap *heap_new_r(int nmax, cmp_r_f cmp_r, void *arg, char is_min);
void heap_insert(Heap *heap, void *elem);
void heap_fprintf(FILE *out, Heap *heap, prt_f prt);
void *heap_pop_peak(Heap *heap);
void heap_free(Heap *heap);

#endif

