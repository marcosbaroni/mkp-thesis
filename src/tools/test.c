#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../utils/util.h"

/* min heap test */
double double_val(double *val){ return *val; }
int execute_heap_test(int argc, char **argv){
    int i, n;
    double *v;
    Heap *heap;

    n = atoll(argv[2]);
    printf("%d\n", n);
    heap = heap_new(n, (heap_eval_f)double_val, 1);
    v = (double*)malloc(n*sizeof(double));

    printf("heap teste\n");
    for( i = 0 ; i < n ; i++ ){
        v[i] = drand()*100;
        heap_insert(heap, &(v[i]));
    }

    heap_fprintf(stdout, heap);
    n = n/2;
    printf("poping out %d\n", n);
    for( i = 0 ; i < n ; i++ )
        heap_pop_peak(heap);

    heap_fprintf(stdout, heap);

    heap_free(heap);
    free(v);

    return 0;
}

int main(int argc, char **argv){
    if( argc < 2 )
        return 1;

    if(!strcmp(argv[1], "heap"))
        return execute_heap_test(argc, argv);

    fprintf(stderr, "unknown option\n");
    return 0;
}

