/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: june, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "heap.h"

/*******************************************************************************
 ***     (MIN/MAX) HEAP
*******************************************************************************/
Heap *heap_new(int nmax, cmp_f cmp, char is_min){
    Heap *heap;

    heap = (Heap*)malloc(sizeof(Heap));
    heap->n = 0;
    heap->nmax = nmax;
    heap->arr = (void**)malloc(nmax*sizeof(void*));
    heap->cmp = cmp;
    heap->cmp_r = NULL;
    heap->arg = NULL;
    heap->is_min = is_min;

    return heap;
}

Heap *heap_new_r(int nmax, cmp_r_f cmp_r, void *arg, char is_min){
    Heap *heap;

    heap = heap_new(nmax, NULL, is_min);
    heap->cmp_r = cmp_r;
    heap->arg = arg;

    return heap;
}

int _heap_eval(Heap *heap, void *elem1, void* elem2){
    if( heap->is_min ){
        if( heap->cmp ){
            return (heap->cmp(elem1, elem2));
        }else{
            return (heap->cmp_r(elem1, elem2, heap->arg));
        }
    }else{
        if( heap->cmp ){
            return -heap->cmp(elem1, elem2);
        }else{
            return -heap->cmp_r(elem1, elem2, heap->arg);
        }
    }
}

void heap_insert(Heap *heap, void *elem){
    double val, val2;
    void **arr;
    void *aux;
    int n, i, father_i;

    if( heap->n == heap->nmax ){
        heap->nmax = 2*heap->nmax;
        heap->arr = (void**)realloc(heap->arr, heap->nmax*sizeof(void*));
    }

    arr = heap->arr;

    arr[heap->n] = elem;
    i = heap->n;
    father_i = (i-1)/2;
    while( i && (_heap_eval(heap, arr[father_i], elem) < 0) ){
        aux = arr[father_i];
        arr[father_i] = arr[i];
        arr[i] = aux;
        i = father_i;
        father_i = (i-1)/2;
    }

    heap->n++;
    return;
}

void heap_fprintf(FILE *out, Heap *heap, prt_f prt){
    int i, n;
    void **arr;

    n = heap->n;
    arr = heap->arr;

    fprintf(out, "1: ");
    prt(out, arr[0]);
    fprintf(out, " (root)\n");
    for( i = 1 ; i < n ; i++ ){
        fprintf(out, "%d: ", i+1);
        prt(out, arr[i]);
        fprintf(out, " (%s ", heap->is_min ? "<" : ">");
        prt(out, arr[(i-1)/2]);
        fprintf(out, ")\n");
    }

    return;
}

void *heap_pop_peak(Heap *heap){
    void **arr;
    void *aux, *min;
    int i, n, imin, need_check;
    double val;

    arr = heap->arr;

    min = arr[0];
    n = --heap->n;
    arr[0] = arr[n];

    /* heapfying */
    i = 0;
    need_check = 1;
    while( i*2+1 < n && need_check ){
        need_check = 0;
        /* checking lesser child */
        imin = i*2+1;
        if( imin+1 < n )
            if( _heap_eval(heap, arr[imin+1], arr[imin]) > 0 )
                imin++;

        /* checking if needs change */
        if( _heap_eval(heap, arr[i], arr[imin]) < 0 ){
            aux = arr[i];    /* swaping */
            arr[i] = arr[imin];
            arr[imin] = aux;
            need_check = 1;   /* flag check */
            i = imin;
        }
    }

    return min;
}

void heap_free(Heap *heap){
    free(heap->arr);
    free(heap);
}

