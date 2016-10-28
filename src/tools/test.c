#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../utils/util.h"

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
    printf("poping out %d\n", n);
    for( i = 0 ; i < n ; i++ )
        heap_pop_peak(heap);

    heap_fprintf(stdout, heap, (prt_f)double_prt);

    heap_free(heap);
    free(v);

    return 0;
}

void print_usage(int argc, char **argv){
    fprintf(stderr, "usage:%s <option>\n", argv[0]);
    fprintf(stderr, "\noptions:\n");
    fprintf(stderr, "  heap <n>: test min/max heap\n");
    return;
}

int main(int argc, char **argv){
    if( argc < 2 ){
        print_usage(argc, argv);
        return 1;
    }

    if(!strcmp(argv[1], "heap"))
        return execute_heap_test(argc, argv);

    fprintf(stderr, "unknown option\n");
    return 0;
}

