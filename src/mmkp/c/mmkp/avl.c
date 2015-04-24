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
 *  (-2)  p     =>      a (0)
 *       / \    =>     / \
 * (-1) a   z   =>    x   p  (0)
 *     / \               / \
 *    x   y             y   z
 *
 */
AVLNode *rotate_right(AVLTree *avlt, AVLNode *p){
	AVLNode *a, *f, *x, *y;
	fprintf(stderr, "    rotate right... ");
	fflush(stderr);

	f = p->parent;
	a = p->left;

	/*  f <-> a  */
	if( f )
		if( f->left == p )
			f->left = a;
		else
			f->right = a;
	else
		avlt->root = a;
	a->parent = f;
	/* 'p' <-> y */
	p->left = a->right;
	if( p->left )
		p->left->parent = p;
	/* a <-> 'p' */
	a->right = p;
	p->parent = a;

	fprintf(stderr, "done.\n");
	fflush(stderr);
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
	fprintf(stderr, "    rotate left... ");
	fflush(stderr);

	f = p->parent;
	a = p->right;

	/* a <-> f */
	if( f )
		if( f->left == p )
			f->left = a;
		else
			f->right = a;
	else
		avlt->root = a;
	a->parent = f;
	/* p <-> x */
	p->right = a->left;
	if( p->right )
		p->right->parent = p;
	/* a <-> p */
	a->left = p;
	p->parent = a;

	fprintf(stderr, "done.\n");
	fflush(stderr);
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
 *
 *    * BALANCING FACTOR * 
 *    * AFTER   ROTATION * 
 *   |  b ||  p'|  a'|  b'|
 *   | +1 || -1 |  0 |  0 |
 *   |  0 ||  0 |  0 |  0 |
 *   | -1 ||  0 | +1 |  0 |
 */
AVLNode *rotate_right_left(AVLTree *avlt, AVLNode *p){
	rotate_right(avlt, p->right);
	return rotate_left(avlt, p);
}

/* 
 *                 Rotate Left-Right
 *
 *       f      (left)   f         (right)     f
 *       |               |                     |
 * (-2)  p         =>    p           =>        b
 *      / \        =>   / \          =>      /   \
 *(+1) a   z       =>  b   z         =>    a      'p'
 *    / \             / \                 / \     / \
 *   w   b           a   y               w   x   y   z
 *      / \         / \
 *     x   y       w   x
 *
 *    * BALANCING FACTOR * 
 *    * AFTER   ROTATION * 
 *   |  b ||  p'|  a'|  b'|
 *   | +1 ||  0 | -1 |  0 |
 *   |  0 ||  0 |  0 |  0 |
 *   | -1 || +1 |  0 |  0 |
 */
AVLNode *rotate_left_right(AVLTree *avlt, AVLNode *p){
	rotate_left(avlt, p->left);
	return rotate_right(avlt, p);
}

/***   BALANCING OPERATIONS   *******************************************/

/* * This function is called to report node 'p' has its height
 * increased (although still balanced).
 * */
void height_increased(AVLTree *avlt, AVLNode *a){
	AVLNode *f, *p, *b;
	fprintf(stderr, "    height increased\n");
	fflush(stderr);

	p = a->parent;

	if(!p){ /* If 'p' is root... */
		fprintf(stderr, "       root reached!\n");
		return; /* ...nothing to do (tree is all balanced) */
	}

	if( p->left == a ){ /* If a is a left subtree */
		p->balance--; /* update balance factor. */

		/* checking 'p' balance factor. Possibilities:
		*   +1 : impossible
		*    0 : tree still balanced, no height increase
		*   -1 : tree is balanced, but height increased
		*   -2 : rotation needed, no further height increase */
		if( p->balance == -1 )
			height_increased(avlt, p);
		else if( p->balance == -2 ){
			if( a->balance == -1 ){ /* left 'leg' case: (right rotation) */
				rotate_right(avlt, p);
				p->balance = a->balance = 0;
			}else{                  /* left 'knee' case: (left-right rotation) */
				b = a->right;
				/* left-right rotation */
				rotate_left(avlt, a);
				rotate_right(avlt, p);
				/* uptading balance factors */
				p->balance = b->balance == -1 ? +1 : 0;
				a->balance = b->balance == +1 ? -1 : 0;
				b->balance = 0;
			}
		}
	}else{    /*  a is right subtree... */
		fprintf(stderr, "      right subtree\n");
		p->balance++;

		/* checking 'f' balance factor. Possibilities:
		*   +2 : rotation needed, no further height increase.
		*   +1 : tree is balanced, but height increased
		*    0 : tree is balanced, no height increased
		*   -1 : impossible */
		if( p->balance == 1 )
			height_increased(avlt, p);
		else if( p->balance == +2 ){
			if( a->balance == 1 ){         /* right 'leg' case */
				rotate_left(avlt, p);
				p->balance = a->balance = 0;
			}else{ /* p->balance == -1 */   /* right 'knee' case */
				b = a->left;
				/* right-left rotation */
				rotate_right(avlt, a);
				rotate_left(avlt, p);
				/* uptading balance factors */
				p->balance = b->balance == +1 ? -1 : 0;
				a->balance = b->balance == -1 ? +1 : 0;
				b->balance = 0;
			}
		}
	}
	return;
}

AVLTree *sub_avl_insert(AVLTree *avlt, AVLNode *p, AVLNode *newp){
	int res;
	fprintf(stderr, "  sub_insert\n");
	fflush(stderr);

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
	fprintf(stderr, "insert %d\n", *(int*)a);
	fflush(stderr);

	/* If fisrt element */
	if(!avlt->root)
		avlt->root = node;
	/* If fisrt element */
	else
		sub_avl_insert(avlt, avlt->root, node);

	avlt->n++;

	return avlt;
}

AVLNode *sub_avl_has(AVLNode *node, void *a, avl_cmp_f cmp){
	int res;
	if(!node)   /* element not found in tree */
		return NULL;
	res = cmp(node->info, a);
	if(!res)    /* found it */
		return node;
	else if( res < 0 )
		return sub_avl_has(node->right, a, cmp);
	else
		return sub_avl_has(node->left, a, cmp);
	return NULL;
}

void *avl_has(AVLTree *avlt, void *a){
	AVLNode *node;

	/* finding the node with given element */
	node = sub_avl_has(avlt->root, a, avlt->cmp);
	if( node )
		return node->info;
	return NULL;
}

AVLTree *avl_delete(AVLTree *avlt, void *a){
	AVLNode *node, *replacer, *parent;

	/* finding the node to be deleted */
	node = sub_avl_has(avlt, a);
	parent = node->parent;
	avlt->n--;

	/* finding the node wich will replace removed one. */
	if(!node->left){     /* target node has no left child */
		if(!node->right){ /* has no child, no replacer */
			if(parent){  /* checing if its not alone */
				if( parent->right == node ){ /* node is a right child */
					parent->right = NULL;
					parent->balance--;
					if( parent->balance == -2 ){
						if( parent->left->balance < 1 ){
							/* left 'leg' case */
							rotate_right(parent);
							if(!parent->parent->balance){
								parent->balance = -1;
								parent->parent->balance = +1;
							}else{
								parent->parent->balance = 0;
								height_decreased(parent->parent);
							}
						}else{
							/* left 'knee' case */
							rotate_left(parent->left);
							rotate_right(parent);
							/* TODO: Stopped here */
						}
					}
				}else{        /* node is a left child */
					parent->left = NULL;
					parent->balance++;
					if( parent->balance == +2 ){ /* need rebalance */
						if( parent->right->balance > -1 ){
							/* right 'leg' case */
							rotate_left(parent);
							if(!parent->parent->balance){
								parent->balance = +1;
								parent->parent->balance = -1;
							}else{
								parent->parent->balance = 0;
								height_decreased(parent->parent);
							}
						}else{
							/* right 'knee' case */
							rotate_right(parent->right);
							rotate_left(parent);
							/* TODO: Stopped here */
						}
					}
				}
			}else
				avlt->root = NULL;
		}else{ /* node to be removed*/
		}
	}else{  /* node had at least one left child. */
		while( replacer->right )
			replacer = replacer->right;
	}

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

void sub_print_nodes_dot(FILE *fout, AVLNode *node, void (*fprt)(FILE *f, void *a)){
	if( !node )
		return;
	if( node->left )
		sub_print_nodes_dot(fout, node->left, fprt);
	fprintf(fout, "\t%ld [label=\"",(long int)(node->info));
	fprt(fout, node->info);
	fprintf(fout, " (%d)\"]\n", node->balance);
	if( node->right)
		sub_print_nodes_dot(fout, node->right, fprt);
}

void _print_nodes_dot(FILE *fout, AVLTree *avlt, void (*fprt)(FILE *f, void *a)){

	sub_print_nodes_dot(fout, avlt->root, fprt);
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
	fprintf(fout, "\tedge [penwidth=3]\n");

	_print_nodes_dot(fout, avlt, prt);
	_print_edges_dot(fout, avlt->root);

	fprintf(fout, "}\n");
	return;
}
/******************************************************************************/

