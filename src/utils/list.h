#ifndef LIST_H
#define LIST_H 1

typedef struct ListNode{
    void *info;
    struct ListNode *prev;
    struct ListNode *next;
}ListNode;

typedef struct List{
    int n;
    ListNode *first;
    ListNode *last;
}List;

typedef struct ListIter{
	List *list;
    ListNode *node;
}ListIter;

List *list_new();
List *list_new_from_array(void **, int n);
List *list_insert(List *list, void *info);
List *list_insert_here(void *info, ListIter *iter);
void **list_get_all(List *list);
void *list_get_head(List *list);
void *list_find_closest(List *list, void*, double(*)(void*, void*));
void list_remove(List*, void*);
void list_apply(List *list, void(void *));
void list_apply_r(List *list, void(void*, void*), void *arg);
void list_dump(FILE *fout, List *list);
void list_free(List* list);

int list_is_empty(List *list);
ListIter *list_get_first(List *list);
ListIter *list_get_last(List *list);
ListIter *listiter_new(List *list);
void *listiter_get(ListIter *liter);
void *listiter_forward(ListIter *liter);
void *listiter_backward(ListIter *liter);
List *listiter_remove(ListIter *iter);
void listiter_free(ListIter *liter);

#endif

