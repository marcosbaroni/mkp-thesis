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

List *list_new();
List *list_insert(List *list, void *info);
void list_apply(List *list, void(void *));
void list_apply_r(List *list, void(void*, void*), void *arg);
void list_dump(FILE *fout, List *list);
void list_free(List* list);

#endif

