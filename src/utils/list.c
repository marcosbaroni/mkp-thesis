#include <stdio.h>
#include <stdlib.h>
#include "list.h"

List *list_new(){
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

void list_apply(List *list, void(*f)(void *)){
    ListNode *node;
    node = list->first;
    while( node ){
        f(node->info);
        node = node->next;
    }
    return;
}

void list_apply_r(List *list, void(*f)(void*, void*), void *arg){
    ListNode *node;
    node = list->first;
    while( node ){
        f(node->info, arg);
        node = node->next;
    }
    return;
}

void list_free(List* list){
    ListNode *aux;

    aux = list->first;
    while( aux ){
        aux = aux->next;
        free(aux->prev);
    }
    free(list);
}

