#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

List *list_insert_here(List *list, void *info, ListIter *iter){
    ListNode *node;
    ListNode *place;

    if( !iter->node )
        return(list_insert(list, info));

    node = (ListNode*)malloc(sizeof(ListNode));
    node->info = info;
    node->prev = node->next = NULL;

    place = iter->node;

    if( place->prev ){
        place->prev->next = node;
        node->prev = place->prev;
    }else{
        list->first = node;
    }
    place->prev = node;
    node->next = place;

    list->n++;

    return list;
}

List *list_remove(List *list, ListIter *iter){
    ListNode *node;

    node = iter->node;
    iter->node = node->next;

    if( node->prev )
        node->prev->next = node->next;
    else
        list->first = node->next;

    if( node->next )
        node->next->prev = node->prev;
    else
        list->last = node->prev;

    free(node);

    list->n--;

    return list;
}

void **list_get_all(List *list){
    ListNode *node;
	int i;
	void **items; 

	items = (void**)malloc(list->n*sizeof(void*));

    node = list->first;
	i = 0;
    while( node ){
        items[i++] = node->info;
        node = node->next;
    }
	return items;
}
void *list_find_closest(List *list, void *x, double(*f)(void*, void*)){
	void *closer = NULL;
	void *elem;
	double elem_dist;
	double closer_dist = INFINITY;
	ListIter *iter;

	iter = list_get_first(list);
	while( (elem = listiter_forward(iter)) ){
		elem_dist = f(elem, x);
		if( elem_dist < closer_dist ){
			closer = elem;
			closer_dist = elem_dist;
		}
	}

	return closer;
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

void list_dump(FILE *fout, List *list){
    int i;
    ListNode *node;
    node = list->first;

    fprintf(fout, "List [%x] n=%d\n", list, list->n);

    i = 0;
    while(node){
        fprintf(fout, "%d [node: %x] [next: %x] (info: %x)\n", i, node, node->next, node->info);
        node = node->next;
        i++;
    }
    return;
}

void list_free(List* list){
    ListNode *aux, *aux2;

    aux = list->first;
    while( aux ){
        aux2 = aux;
        aux = aux->next;
        free(aux2);
    }
    free(list);
}

int list_is_empty(List *list){
    return (list->n == 0);
}

ListIter *list_get_first(List *list){
    ListIter *iter;

    iter = (ListIter*)malloc(sizeof(ListIter));
    iter->node = list->first;

    return iter;
}

ListIter *list_get_last(List *list){
    ListIter *iter;

    iter = (ListIter*)malloc(sizeof(ListIter));
    iter->node = list->last;

    return iter;
}

void *listiter_get(ListIter *liter){
    if(liter->node)
        return liter->node->info;
    return NULL;
}

void *listiter_forward(ListIter *liter){
    void *info;

    info = NULL;
    if( liter->node ){
        info = liter->node->info;
        liter->node = liter->node->next;
    }

    return info;
}

void *listiter_backward(ListIter *liter){
    void *info;

    info = NULL;
    if( liter->node ){
        info = liter->node->info;
        liter->node = liter->node->prev;
    }

    return info;
}

void listiter_free(ListIter *liter){
    free(liter);
}


