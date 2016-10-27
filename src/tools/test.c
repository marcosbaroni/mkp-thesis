#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../utils/util.h"

/* min heap test */
double double_val(double *val){ return *val; }
int execute_minheap_test(int argc, char **argv){
    int i, n;
    double *v;
    MinHeap *heap;

    n = atoll(argv[2]);
    heap = minheap_new(n, (minheap_eval_f)double_val);
    v = (double*)malloc(n*sizeof(double));

    printf("minheap teste\n");
    for( i = 0 ; i < n ; i++ ){
        v[i] = drand()*100;
        minheap_insert(heap, &(v[i]));
    }

    minheap_fprintf(stdout, heap);
    n = n/2;
    printf("poping out %d\n", n);
    for( i = 0 ; i < n ; i++ )
        minheap_pop_min(heap);

    minheap_fprintf(stdout, heap);

    minheap_free(heap);
    free(v);

    return 0;
}

int main(int argc, char **argv){
    if( argc < 2 )
        return 1;

    if(!strcmp(argv[1], "minheap"))
        return execute_minheap_test(argc, argv);

    return 0;
}

