/*******************************************************************************
 * Author: Marcos Daniel Baroni
 * Date:  Dom Abr 19 23:53:27 BRT 2015
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "avl.h"

AVLNode *new_avl_node(void *a){
	AVLNode *node;
	node = (AVLNode*)malloc(sizeof(AVLNode));
	node->info = (void*)a;
	node->parent = node->left = node->right = NULL;
	node->balance = 0;
	return node;
}

/*********************************************************************
         AVL Tree                                                     
*********************************************************************/


/***   BASICS  *********************************************************/
AVLTree *new_avltree( avl_cmp_f cmp ){
	AVLTree *avlt;
	avlt = (AVLTree*)malloc(sizeof(AVLTree));
	avlt->cmp = cmp;
	avlt-> n = 0;
	avlt-> root = NULL;
	return avlt;
}

void sub_free_avltree(AVLNode *node){
	if(!node)
		return;
	sub_free_avltree(node->left);
	sub_free_avltree(node->right);
	free(node);
	return;
}

void free_avltree(AVLTree *avlt){
	sub_free_avltree(avlt->root);
	free(avlt);
	return;
}

int avl_size(AVLTree *avlt){
	return avlt->n;
}

/***   ROTATIONS   *****************************************************/
/* 
 *        Rotate Right
 *
 *        f             f
 *        |             |
 *  (-2) 'p'    =>      a (0)
 *       / \    =>     / \
 * (-1) a   z   =>    x  'p' (0)
 *     / \               / \
 *    x   y             y   z
 *
 */
AVLNode *rotate_right(AVLTree *avlt, AVLNode *p){
	AVLNode *a, *f, *x, *y;

	f = p->parent;
	a = p->left;

	/*  f <-> a  */
	if( p->parent )
		if( p->parent->left == p )
			p->parent->left = a;
		else
			p->parent->right = a;
	else
		avlt->root = a;
	/* 'p' <-> y */
	p->left = a->right;
	p->left->parent = a;
	/* a <-> 'p' */
	a->right = p;
	p->parent = a;

	//p->balance = a->balance = 0;
	return a;
}

/* 
 *        Rotate Left
 *
 *     f                   f
 *     |                   |
 *    'p' (+2)   =>    (0) a
 *    / \        =>       / \
 *   z   a (+1)  =>  (0)'p'  y
 *      / \             / \
 *     x   y           z   x
 *
 */
AVLNode *rotate_left(AVLTree *avlt, AVLNode *p){
	AVLNode *a, *f;

	f = p->parent;
	a = p->right;

	/* p <-> x */
	p->right = a->left; p->right->parent = p;
	/* a <-> p */
	a->left = p; p->parent = a;
	/* a <-> f */
	if( f )
		if( f->left == p )
			f->left = a;
		else
			f->right = a;
	else{
		avlt->root = a;
	}
	a->parent = f;

	//p->balance = a->balance = 0;
	return a;
}

/* 
 *                 Rotate Right-Left
 *
 *     f      (right)  f         (left)      f
 *     |               |                     |
 *    'p' (+2)   =>   'p' (+2)     =>        b (0/1)
 *    / \        =>   / \          =>      /   \
 *   w   a (-1)  =>  w   b (-1/0)  =>   'p'      a
 *      / \             / \             / \     / \
 *     b   z           x   a           w   x   y   z
 *    / \                 / \
 *   x  y                y   z
 */
AVLNode *rotate_right_left(AVLTree *avlt, AVLNode *p){
	rotate_right(avlt, p->right);
	return rotate_left(avlt, p);
}

/* 
 *                 Rotate Left-Right
 *
 *     f      (left)   f         (right)     f
 *     |               |                     |
 *    'p' (-2)   =>   'p' (+2)     =>        b (0/1)
 *    / \        =>   / \          =>      /   \
 *   a   z (+1)  =>  b   z (-1/0)  =>    a      'p'
 *  / \             / \                 / \     / \
 * w   b           a   y               w   x   y   z
 *    / \         / \
 *   x   y       w   x
 */
AVLNode *rotate_left_right(AVLTree *avlt, AVLNode *p){
	rotate_left(avlt, p->left);
	return rotate_right(avlt, p);
}

/***   BALANCING OPERATIONS   *******************************************/

/* * This function is called to report node 'p' has its height
 * increased (although still balanced).
 * */
void height_increased(AVLTree *avlt, AVLNode *p){
	AVLNode *f;

	f = p->parent;

	if(!f) /* If 'p' is root... */
		return; /* ...nothing to do (tree is all balanced) */

	if( f->left == p ){ /* If 'p' is a right subtree... */
		f->balance--; /* ...update balance factor. */

		/* checking 'f' balance factor. Possibilities:
		*   +1 : impossible
		*    0 : tree still balanced, no height increase
		*   -1 : tree is balanced, but height increased
		*   -2 : rotation needed! */
		if( f->balance == -1 )
			height_increased(avlt, f);
		else if( f->balance == -2 ){
			if( p->balance == -1 ){ /* left 'leg' case */
				rotate_right(avlt, f);
				f->balance = 0;
				p->balance = 0;
			}else{                  /* left 'knee' case */
				rotate_left(avlt, p);
				rotate_right(avlt, f);
				p->balance = f->balance = 0;
				if( p->parent->balance == 1 )
					f->balance = -1;
				else if( p->parent->balance = -1 )
					p->balance = 1;
			}
		}
	}else{    /*  'p' is left subtree... */
		f->balance++;

		/* checking 'f' balance factor. Possibilities:
		*   +2 : rotation needed!
		*   +1 : tree is balanced, but height increased
		*    0 : tree is balanced, no height increased
		*   -1 : impossible */
		if( f->balance == 1 )
			height_increased(avlt, f);
		else if( f->balance == +2 ){
			if( p->balance == 1 ){    /* right 'leg' case */
				rotate_left(avlt, f);
				f->balance = p->balance = 0;
			}else{                    /* right 'knee' case */
				rotate_right(avlt, p);
				rotate_left(avlt, f);
				f->balance = p->balance = 0;
				if( p->parent->balance == +1 )
					p->balance = -1;
				else if( p->parent->balance == -1 )
					f->balance = 1;
			}
		}
	}
	return;
}

AVLTree *sub_avl_insert(AVLTree *avlt, AVLNode *p, AVLNode *newp){
	int res;

	res = avlt->cmp(p->info, newp->info);

	/* Insert in wicth branch of tree? */
	if( res < 0 ){ /* Inserting right. */
		if( p->right ) /* already exists right subtree */
			sub_avl_insert(avlt, p->right, newp);
		else{
			/* no right subtree */
			p->right = newp;
			newp->parent = p;
			p->balance++;

			if( p->balance != 0 )
				height_increased(avlt, p); /* report height increase
					(self still balanced) */
		}
	}else{ /* Inserting left! */
		if( p->left ) /* no left subtree */
			sub_avl_insert(avlt, p->left, newp);
		else{
			/* Appending new node on left side */
			p->left = newp;
			newp->parent = p;
			p->balance--;

			if( p->balance != 0 )
				height_increased(avlt, p); /* report height inscrease
					(self still balanced) */
		}
	}
	return avlt;
}

AVLTree *avl_insert(AVLTree *avlt, void *a){
	AVLNode *node;
	node = new_avl_node(a);

	/* If fisrt element */
	if(!avlt->root)
		avlt->root = node;
	/* If fisrt element */
	else
		sub_avl_insert(avlt, avlt->root, node);

	avlt->n++;

	return avlt;
}

void *sub_avl_has(AVLNode *node, void *a, avl_cmp_f cmp){
	int res;
	if(!node)
		return NULL;
	res = cmp(node->info, a);
	if(!res)
		return node->info;
	else if( res < 0 )
		return sub_avl_has(node->right, a, cmp);
	else
		return sub_avl_has(node->left, a, cmp);
}

void *avl_has(AVLTree *avlt, void *a){
	return sub_avl_has(avlt->root, a, avlt->cmp);
}

AVLTree *avl_delete(AVLTree *avlt, void *a){
	fprintf(stderr, "Function \"avl_delete\" not implemented yet.\n");
	return avlt;
}

/*** OTHERS *********************************************************/
void sub_avl_apply_to_all(AVLNode *node, void(*func)(void*) ){
	if(!node) func(node->info);
	if(node->left) sub_avl_apply_to_all(node->left, func);
	if(node->right) sub_avl_apply_to_all(node->right, func);
}

/* Applies a function to all members. */
void avl_apply_to_all(AVLTree *avlt, void(*func)(void*) ){
	if(avlt->root) sub_avl_apply_to_all(avlt->root, func);
}

int sub_avl_to_array(AVLNode *node, void **v, int k){
	if(!node)
		return k;
	if(node->left)
		k = sub_avl_to_array(node->left, v, k);
	v[k++] = node->info;
	if(node->right)
		k = sub_avl_to_array(node->right, v, k);
	return k;
}

/* Returns an array with all members. */
void **avl_to_array(AVLTree *avlt){
	void **v;
	
	v = (void**)malloc(avlt->n*sizeof(void*));
	sub_avl_to_array(avlt->root, v, 0);
	return v;
}

void _print_nodes_dot(FILE *fout, AVLTree *avlt, void (*fprt)(FILE *f, void *a)){
	void **v;
	int i, n;

	n = avl_size(avlt);
	v = avl_to_array(avlt);

	for( i = 0 ; i < n ; i++ ){
		fprintf(fout, "\t%ld [label=\"",(long int)v[i]);
		fprt(fout, v[i]);
		fprintf(fout, "\"]\n");
	}
	free(v);
}

void _print_edges_dot(FILE *fout, AVLNode *node){
	if(!node) return;
	if(node->left)
		fprintf(fout, "\t%ld -- %ld [color=red]\n", (long int)node->info, (long int)node->left->info);
	if(node->right)
		fprintf(fout, "\t%ld -- %ld [color=blue]\n", (long int)node->info, (long int)node->right->info);

	if(node->left)
		_print_edges_dot(fout, node->left);
	if(node->right)
		_print_edges_dot(fout, node->right);
	return;
}

void avl_fprint_dot(FILE *fout, AVLTree *avlt, avl_prt_f prt){
	fprintf(fout, "graph G {\n");
	fprintf(fout, "\tnode [style=filled, shape=circle,fillcolor=lightgray]\n");

	_print_nodes_dot(fout, avlt, prt);
	_print_edges_dot(fout, avlt->root);

	fprintf(fout, "}\n");
	return;
}
/******************************************************************************/

