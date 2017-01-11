/*******************************************************************************
 * Author: Marcos Daniel Baroni
 * Date:  Dom Abr 19 23:53:27 BRT 2015
*******************************************************************************/

#ifndef AVL_H
#define AVL_H 1

#include <stdio.h>
#include <stdlib.h>

#define max(a, b) (a > b ? a : b)

/*          AVL BALANCING FACTORS
 *
 *    O  (+1)    |     O  (-1)   |     O  (0)
 *     \         |    /          |    / \
 *      O        |   O           |   O   O
 */

/**** AVL Tree **************************************************/
/* AVL Node */
typedef struct AVLNode{
	void *info;
	char balance;
	struct AVLNode *parent;
	struct AVLNode *left;
	struct AVLNode *right;
}AVLNode;

AVLNode* new_avl_node(void *info);
/* function types */
typedef int(*avl_cmp_f)(void*, void*);
typedef void(*avl_prt_f)(FILE *out, void*);

/* AVL Tree Structure */
typedef struct{
	int n;
	avl_cmp_f cmp;
    avl_prt_f prt;
	AVLNode *root;
	AVLNode *first;
	AVLNode *last;
}AVLTree;

/* basics */
AVLTree*	new_avltree( avl_cmp_f cmp);
void		free_avltree(AVLTree *avlt); /* Free the AVL Tree structure */
int			avl_size(AVLTree *avlt);
void        avl_free(AVLTree *avlt);
void        avl_set_prt(AVLTree *avl, avl_prt_f prt);
/* operations */
AVLTree*	avl_insert(AVLTree *avlt, void *a);
AVLTree*	avl_remove(AVLTree *avlt, void *a);
void*		avl_has(AVLTree *avlt, void *a);
/* others */
void**		avl_to_array(AVLTree *avlt); /* Returns an array with members */
void 		avl_apply_to_all(AVLTree *avlt, void(*func)(void*) );
void		avl_fprint_pretty(FILE *f, AVLTree *avlt);
void		avl_fprint_dot(FILE *f, AVLTree *avlt, avl_prt_f prt);
int         avl_check_sanity(AVLTree *avl);
/****************************************************************/
#endif

