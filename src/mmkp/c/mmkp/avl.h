/*******************************************************************************
 * Author: Marcos Daniel Baroni
 * Date:  Dom Abr 19 23:53:27 BRT 2015
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#define max(a, b) a > b ? a : b;

/**** AVL Tree **************************************************/
typedef struct AVLNode{
	void *info;
	unsigned char balance;
	struct AVLNode *parent;
	struct AVLNode *left;
	struct AVLNode *right;
}AVLNode;

AVLNode* new_avl_node(void *info);

typedef int(*avl_cmp_f)(void*, void*);
typedef void(*avl_prt_f)(FILE *out, void*);

typedef struct{
	int n;
	avl_cmp_f cmp;
	AVLNode *root;
	AVLNode *first;
	AVLNode *last;
}AVLTree;

AVLTree *new_avltree( avl_cmp_f cmp);
AVLTree*	avl_insert(AVLTree *avlt, void *a);
AVLTree*	avl_delete(AVLTree *avlt, void *a);
void*		avl_has(AVLTree *avlt, void *a);
void**		avlt_to_vector(AVLTree *avlt); /* Return a vector with members */
int			avlt_size(AVLTree *avlt);
void 		avl_apply_to_all(AVLTree *avlt, void(*func)(void*) );
void		free_avltree(AVLTree *avlt); /* Free the AVL Tree structure */
void		fprint_avlt_dot(FILE *f, AVLTree *avlt, avl_prt_f prt);
/****************************************************************/
