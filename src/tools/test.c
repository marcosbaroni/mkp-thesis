#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#include "../utils/util.h"
#include "../utils/avl.h"

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
        if(seed == 487)
            avl_fprint_pretty(stdout, avl);
        j = i*2;
        avl_remove(avl, &(array[j]));
        if(avl_check_sanity(avl)){
            printf("\nerror when removing %d [%x] j=%d (seed=%d)\n", array[j], &(array[j]), j, seed);
            avl_fprint_pretty(stdout, avl);
            return 1;
        }
    }

    avl_free(avl);
    free(array);
    
    return 0;
}

int execute_avl_test(int argc, char **argv){
    int n, k, maxk, ans;

    maxk = 100000;
    n = atoll(argv[2]);
    for( k = 0 ; k < maxk ; k++ ){
        printf("%d/%d\r", k, maxk);
        ans = _sub_test_avl(n, k);
        if(ans)
            return 1;
    }
    printf("\n");

    return 0;
}

int execute_avl_test2(int argc, char **argv){
    int n, i, j;
    int *array;
    AVLTree *avl;

    if(argc > 3)
        srand(atoll(argv[3]));

    n = atoll(argv[2]);
    array = (int*)malloc(n*sizeof(int));

    for( i = 0 ; i < n ; i++ )
        array[i] = i+1;
    array = int_array_shuffle(array, n);

    avl = new_avltree((avl_cmp_f)_int_ptr_cmp);
    avl_set_prt(avl, (avl_prt_f)_int_fptr_prt);

    for( i = 0 ; i < n ; i++ ){
        printf("\ninserting %d\n", array[i]);
        avl_insert(avl, &(array[i]));
        if(avl_check_sanity(avl)){
            avl_fprint_pretty(stdout, avl);
            return 0;
        }
        fflush(stdout);
    }
    printf("\ndone insertions\n\n");
    avl_fprint_pretty(stdout, avl);
    avl_check_sanity(avl);

    for( i = 0 ; i < (n/2-5) ; i++ ){
        j = i*2;
        printf("removing %d [%x] j = %d\n", array[j], &(array[j]), j);
        if( j == 156 ){
            avl_fprint_pretty(stdout, avl);
            fflush(stdout);
        }
        avl_remove(avl, &(array[j]));
        if(avl_check_sanity(avl)){
            avl_fprint_pretty(stdout, avl);
            return 0;
        }
    }
    printf("removing done.\n");
    avl_fprint_pretty(stdout, avl);

    avl_check_sanity(avl);
    avl_free(avl);
    free(array);

    return 0;
}
/***************************************************************************/

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
    fprintf(stderr, "  avl <n>: test avl heap\n");
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

