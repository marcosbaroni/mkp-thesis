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

void sub_avl_free(AVLNode *node){
    if(node->left)
        sub_avl_free(node->left);
    if(node->right)
        sub_avl_free(node->right);
    free(node);
}

void avl_free(AVLTree *avlt){
    if( avlt->root )
        sub_avl_free( avlt->root );
    free(avlt);
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
 * obs.: does not update balacing factors.
 */
AVLNode *rotate_left(AVLTree *avlt, AVLNode *p){
	AVLNode *a, *f;

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

	p = a->parent;

	if(!p){ /* If 'p' is root... */
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

/* Report that the height of "node" has decreased. */
AVLTree *height_decreased(AVLTree *avlt, AVLNode *node){
	if(!node->parent)
		return avlt;
	
	/* TODO: not implemented yet... */

	return avlt;
}

/* Remove the given leaf from the tree. */
void avl_make_me_orphan(AVLTree *avlt, AVLNode *node){
	AVLNode *parent;

	parent = node->parent;
	if(!parent)
		avlt->root = NULL;
	else{
		if(parent->right == parent){
			parent->right = NULL;
			parent->balance--;
		}else{
			parent->left = NULL;
			parent->balance++;
		}

		/* SANITY CHECK */
		switch(parent->balance){
			case 2:
			switch(parent->right->balance){
				case -1:
				/* right knee case */
				rotate_right(avlt, parent->right);
				rotate_left(avlt, parent);
				/* update balance */
				parent->balance = 0;
				parent->parent->right->balance = 0;
				switch(parent->parent->balance){
					case +1:
					parent->balance = -1;
					break;

					case -1:
					parent->parent->right->balance = +1;
					break;
				}
				parent->parent->balance = 0;
				height_decreased(avlt, parent->parent);
				break;

				case  0:
				/* right leg case */
				rotate_left(avlt, parent);
				parent->balance = 1;
				parent->parent->balance = -1;
				break;

				case +1:
				/* right leg case */
				rotate_left(avlt, parent);
				parent->balance = 0;
				parent->parent->balance = 0;
				height_decreased(avlt, parent->parent);
				break;
			}
			break;

			case 1:
			/* balanced, no height decrease */
			break;

			case 0:
			/* balanced, height decrease */
			height_decreased(avlt, node->parent);
			break;

			case -1:
			/* balanced, no height decrease */
			break;

			case -2:
			switch(parent->left->balance){
				case +1:
				/* left knee case */
				rotate_left(avlt, parent->left);
				rotate_right(avlt, parent);
				/* update balance */
				parent->balance = 0;
				parent->parent->left->balance = 0;
				switch(parent->parent->balance){
					case -1:
					parent->balance = 1;
					break;

					case +1:
					parent->parent->left->balance = -1;
					break;
				}
				parent->parent->balance = 0;
				height_decreased(avlt, parent->parent);
				break;

				case  0:
				/* left leg case */
				rotate_right(avlt, parent);
				parent->balance = -1;
				parent->parent->balance = +1;
				break;

				case -1:
				/* left leg case */
				rotate_right(avlt, parent);
				parent->balance = 0;
				parent->parent->balance = 0;
				height_decreased(avlt, parent->parent);
				break;
			}
			break;
		}
	}

	return;
}

AVLNode *_avl_leaffy_node(AVLTree *avl, AVLNode *node){
    void *info;
    AVLNode *pred;

    pred = NULL;
    if( node->right ){
        /* find the left-most leaf from right tree */
        pred = node->right;
        while( pred->left )
            pred = pred->left;
    }else if( node->left ){
        /* find the right-most leaf from left tree */
        pred = node->left;
        while( pred->right )
            pred = pred->right;
    }else{
        /* node is already a leaf */
        return node;
    }

    /* switch the node with the defined predecessor */
    if( pred ){
        info = pred->info;
        pred->info = node->info;
        node->info = info;
    }

    return pred;
}

/* obs.: assumes the tree has more than 1 node... */
AVLTree *sub_avl_delete(AVLTree *avlt, AVLNode *node){
	int nkids;
    void *info;
    AVLNode *parent;
    AVLNode *brother;

    node = _avl_leaffy_node(avlt, node);
    parent = node->parent;

    /* node is right leaf */
    if( parent->right == node ){
        parent->right = NULL;
        if( parent->balance == 0){
            parent->balance--;
        }else if(parent->balance == 1 ){
            parent->balance--;
            height_decreased(avlt, parent);
        }else{
            brother = parent->left;
            if( brother->balance == +1 ){
                rotate_left_right(avlt, parent);
                parent->balance = 0;
                brother->balance = 0;
                height_decreased(avlt, parent->parent);
            }else if( brother->balance == -1 ){
                rotate_right(avlt, parent);
                parent->balance = 0;
                brother->balance = 0;
                height_decreased(avlt, parent->parent);
            }else{ /* bother->balance == 0 */
                rotate_right(avlt, parent);
                brother->balance = +1;
            }
        }
    }else{/* node is left leaf */
        parent->left = NULL;
    }

    /* TODO: delete node / check father / report tree height decrease (if needed) */

	return avlt;
}

/*** DEBUG **********************************************************/
int _node_get_height(AVLNode *node){
    int rh, lh;

    rh = lh = 0;
    if( !node->right && !node->left )
        return 0;

    if( node->right )
        rh = _node_get_height(node->right);
    if( node->left)
        lh = _node_get_height(node->left);

    return (max(lh, rh) + 1);
}

int _node_assert_balance(AVLNode *node){
    int lh, rh, ans;

    ans = 1;
    if( node->left)
        ans *= _node_assert_balance(node->left);
    if( node->right )
        ans *= _node_assert_balance(node->right);

    if( !node->left || !node->right )
        return ans;

    lh = _node_get_height(node->left);
    rh = _node_get_height(node->right);

    if( abs(lh-rh) > 1 ){
        fprintf(stderr, "error: Node %x has %d balance (%d - %d)\n", node, (lh-rh), lh, rh); 
        ans = 0;
    }

    return ans;
}

int _node_assert_sorted(AVLNode *node, avl_cmp_f cmp){
    int ans;

    ans = 1;
    if( node->left ){
        if( cmp(node->info, node->left->info) < 0 )
            ans = 0;
        ans *= _node_assert_sorted(node->left, cmp);
    }
    if( node->right ){
        if( cmp(node->info, node->right->info) > 0 )
            ans = 0;
        ans *= _node_assert_sorted(node->right, cmp);
    }

    return ans;
}

/* evaluate node heights, checking if balance factor of all nodes is between (-1, 1) */
void avl_check_sanity(AVLTree *avl){
    int balanced, sorted;

    balanced = _node_assert_balance(avl->root);
    sorted = _node_assert_sorted(avl->root, avl->cmp);

    if( !balanced )
        fprintf(stderr, "Tree is not balanced.\n");
    if( !sorted )
        fprintf(stderr, "Tree is not sorted.\n");

    if( balanced && sorted )
        fprintf(stderr, "Tree is sanitized.\n");

    return;
}

/********************************************************************/


/*** OTHERS *********************************************************/
void sub_avl_apply_to_all(AVLNode *node, void(*func)(void*) ){
	if( node->left )
        sub_avl_apply_to_all( node->left, func );
    func( node->info );
	if( node->right )
        sub_avl_apply_to_all( node->right, func );
}

/* Applies a function to all members. */
void avl_apply_to_all(AVLTree *avlt, void(*func)(void*) ){
	if(avlt->root)
        sub_avl_apply_to_all(avlt->root, func);
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

/*  print AVL tree in dot format */
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

