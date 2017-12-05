/******************************************************************************
 *  Author: Marcos Daniel Valadão Baroni
 *  E-mail: marcosdaniel.baroni@gmail.com
 *  Date: december, 2017
 *
 *  This program must not be used, modified or shared without the express
 *     permission of the author.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#include "../utils/util.h"
#include "../utils/avl.h"
#include "../utils/heap.h"

/***   AVL   ***************************************************************/
int _int_ptr_cmp(int *a, int *b){
    return (*a-*b);
}

void _int_ptr_prt_ln( int *a){
    printf("%d\n", *a);
    return;
}

void _int_fptr_prt( FILE *fout, int *a ){
    fprintf(fout, "%d", *a);
    return;
}

int _sub_test_avl(int n, int seed){
    AVLTree *avl;
    int i, j;
    int *array;

    /* setting up */
    array = (int*)malloc(n*sizeof(int));
    avl = new_avltree((avl_cmp_f)_int_ptr_cmp);
    avl_set_prt(avl, (avl_prt_f)_int_fptr_prt);
    srand(seed);

    /* shuffeling */
    for( i = 0 ; i < n ; i++ )
        array[i] = i;
    int_array_shuffle(array, n);

    /* inserting */
    for( i = 0 ; i < n ; i++ ){
        avl_insert(avl, &(array[i]));
        if( avl_check_sanity(avl)){
            printf("\nerror when inserting %d [%x] i=%d (seed=%d)\n", array[i], &(array[i]), i, seed);
            avl_fprint_pretty(stdout, avl);
            return 1;
        }
    }

    /* removing */
    for( i = 0 ; i < (n/2-5) ; i++ ){
        if(seed == -369){
            avl_fprint_pretty(stdout, avl);
            fflush(stdout);
        }
        j = i*2;
        avl_remove(avl, &(array[j]));
        if(avl_check_sanity(avl)){
            printf("\nerror when removing %d [%x] j=%d (seed=%d)\n", array[j], &(array[j]), j, seed);
            fflush(stdout);
            printf("how is after removing:\n");
            avl_fprint_pretty(stdout, avl);
            fflush(stdout);
            return 1;
        }
    }

    avl_free(avl);
    free(array);
    
    return 0;
}

int execute_avl_test(int argc, char **argv){
    int n, k, maxk, ans;

    maxk = 1000;
    n = (int)atoll(argv[2]);
    if( argc > 3 )
        maxk = (int)atoll(argv[3]);
    for( k = 0 ; k < maxk ; k++ ){
        printf("  %d/%d (%.2f%%)\r", k, maxk, (k*100/(float)maxk));
        fflush(stdout);
        ans = _sub_test_avl(n, k);
        if(ans)
            return 1;
    }
    printf("\n");

    return 0;
}

/* min heap test */
int double_cmp(double *a, double *b){
    if( *a < *b ) return -1;
    else if ( *a > *b ) return 1;
    return 0;
}
void double_prt(FILE *out, double *a){
    fprintf(out, "%.3lf [%x]", *a, a);
}
int execute_heap_test(int argc, char **argv){
    int i, n;
    double *v;
    Heap *heap;

    n = atoll(argv[2]);
    printf("%d\n", n);
    printf("ok\n"); fflush(stdout);
    heap = heap_new(n, (cmp_f)double_cmp, 1);
    v = (double*)malloc(n*sizeof(double));

    printf("heap teste\n");
    for( i = 0 ; i < n ; i++ ){
        v[i] = drand()*100;
        heap_insert(heap, &(v[i]));
    }

    heap_fprintf(stdout, heap, (prt_f)double_prt);
    n = n/2;
    for( i = 0 ; i < n ; i++ ){
        printf(" poping out %d\n", i+1);
        heap_pop_peak(heap);
        heap_fprintf(stdout, heap, (prt_f)double_prt);
    }

    heap_fprintf(stdout, heap, (prt_f)double_prt);

    heap_free(heap);
    free(v);

    return 0;
}

void print_usage(int argc, char **argv){
    fprintf(stderr, "usage:%s <option>\n", argv[0]);
    fprintf(stderr, "\noptions:\n");
    fprintf(stderr, "  heap <n>: test min/max heap\n");
    fprintf(stderr, "  avl <n> [k=1000]: test avl tree\n");
    return;
}

int main(int argc, char **argv){
    struct timespec ts;
    if( argc < 2 ){
        print_usage(argc, argv);
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand(ts.tv_nsec);

    if(!strcmp(argv[1], "heap"))
        return execute_heap_test(argc, argv);
    if(!strcmp(argv[1], "avl"))
        return execute_avl_test(argc, argv);

    fprintf(stderr, "unknown option\n");
    return 0;
}

