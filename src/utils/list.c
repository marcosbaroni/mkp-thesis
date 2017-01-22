#include <stdio.h>
#include <stdlib.h>
#include "list.h"

List *new_list(){
    List *list;
    list = (List*)malloc(sizeof(List));
    list->n = 0;
    list->first = list->last = NULL;

    return list;
}

List *list_insert(List *list, void *info){
    ListNode *node;

    node = (ListNode*)malloc(sizeof(ListNode));
    node->info = info;
    node->prev = node->next = NULL;

    if( list->last ){
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }else{
        list->first = list->last = node;
    }
    list->n++;

    return list;
}

void free_list(List* list){
    ListNode *aux;

    aux = list->first;
    while( aux ){
        aux = aux->next;
        free(aux->prev);
    }
    free(list);
}

