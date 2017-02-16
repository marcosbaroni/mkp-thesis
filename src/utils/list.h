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
    ListNode *node;
}ListIter;

List *list_new();
List *list_insert(List *list, void *info);
List *list_insert_here(List *list, void *info, ListIter *iter);
List *list_remove(List *list, ListIter *iter);
void list_apply(List *list, void(void *));
void list_apply_r(List *list, void(void*, void*), void *arg);
void list_dump(FILE *fout, List *list);
void list_free(List* list);

int list_is_empty(List *list);
ListIter *list_get_first(List *list);
ListIter *list_get_last(List *list);
void *listiter_get(ListIter *liter);
void *listiter_forward(ListIter *liter);
void *listiter_backward(ListIter *liter);
void listiter_free(ListIter *liter);

#endif

