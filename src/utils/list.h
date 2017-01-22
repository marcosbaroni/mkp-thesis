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

List *new_list();
List *list_insert(List *list, void *info);
void free_list(List* list);

#endif

