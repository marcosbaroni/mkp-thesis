/*******************************************************************************
 * Author: Marcos Daniel Baroni
 * Date:  Dom Abr 19 23:53:27 BRT 2015
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

int node_is_right(AVLNode *node){
    if( node->parent )
        return node->parent->right == node ;
    return 0;
}

int node_is_left(AVLNode *node){
    if( node->parent )
        return node->parent->left == node ;
    return 0;
}

int node_is_leaf(AVLNode *node){
    return (!node->right && !node->left);
}

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
    avlt->prt = NULL;
	avlt-> n = 0;
	avlt-> root = NULL;
	return avlt;
}

void avl_set_prt(AVLTree *avl, avl_prt_f prt){
    avl->prt = prt;
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
 */
AVLNode *rotate_left_right(AVLTree *avlt, AVLNode *p){
	rotate_left(avlt, p->left);
	return rotate_right(avlt, p);
}

/***   BALANCING OPERATIONS   *******************************************/

void height_increased(AVLTree *avlt, AVLNode *node){
    AVLNode *parent;

    parent = node->parent;

    if( !parent )
        return;

    /* adjust parent node */
    if( node_is_left(node) )
        parent->balance--;
    else /* node is right */
        parent->balance++;

    /* check parent new balance */
    if( abs(parent->balance) == 1 ){
        height_increased(avlt, parent);
    }else if( parent->balance == +2 ){
        if( node->balance == +1 ){
            rotate_left(avlt, parent);
            parent->balance = node->balance = 0;
        }else if( node->balance == 0 ){
            rotate_left(avlt, parent);
            parent->balance = +1;
            node->balance = -1;
            height_increased(avlt, node);
        }else{
            rotate_right_left(avlt, parent);
            node->balance = parent->balance = 0;
            if( node->parent->balance == -1 )
                node->balance = +1;
            if( node->parent->balance == +1 )
                parent->balance = -1;
            node->parent->balance = 0;
        }
    }else if( parent->balance == -2 ){
        if( node->balance == -1 ){
            rotate_right(avlt, parent);
            parent->balance = node->balance = 0;
        }else if( node->balance == 0 ){
            rotate_left(avlt, parent);
            parent->balance = -1;
            node->balance = +1;
            height_increased(avlt, node);
        }else{
            rotate_left_right(avlt, parent);
            node->balance = parent->balance = 0;
            if( node->parent->balance == +1 )
                node->balance = -1;
            if( node->parent->balance == -1 )
                parent->balance = +1;
            node->parent->balance = 0;
        }
    }

    return;
}

/* Report that the height of "node" has decreased. */
void height_decreased(AVLTree *avlt, AVLNode *node){
    AVLNode *parent;
    AVLNode *brother;

    parent = node->parent;

	if( !parent )
		return;

    /* adjust parent node */
    if( node_is_left(node) )
        parent->balance++;
    else /* node is right */
        parent->balance--;

    /* check parent new balance */
    if( parent->balance == 0 ){
        height_decreased(avlt, parent);
    }else if( parent->balance == -2 ){
        brother = parent->left;
        if( brother->balance == 0 ){
            rotate_right(avlt, parent);
            parent->balance = -1;
            parent->parent->balance = +1;
        }else if( brother->balance == -1 ){
            rotate_right(avlt, parent);
            parent->balance = 0;
            parent->parent->balance = 0;
            height_decreased(avlt, parent->parent);
        }else{
            rotate_left_right(avlt, parent);
            parent->balance = parent->parent->left->balance = 0;
            if( parent->parent->balance == -1 )
                parent->balance = +1;
            else if( parent->parent->balance == +1 )
                parent->parent->left->balance = -1;
            parent->parent->balance = 0;
            height_decreased(avlt, parent->parent);
        }
    }else if( parent->balance == +2 ){
        brother = parent->right;
        if( brother->balance == 0 ){
            rotate_left(avlt, parent);
            parent->balance = +1;
            parent->parent->balance = -1;
        }else if( brother->balance == +1 ){
            rotate_left(avlt, parent);
            parent->balance = 0;
            parent->parent->balance = 0;
            height_decreased(avlt, parent->parent);
        }else{ /* brother->balance == -1 */
            rotate_right_left(avlt, parent);
            parent->balance = parent->parent->right->balance = 0;
            if( parent->parent->balance == +1 )
                parent->balance = -1;
            else if( parent->parent->balance == -1 )
                parent->parent->right->balance = +1;
            parent->parent->balance = 0;
            height_decreased(avlt, parent->parent);
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
				height_increased(avlt, p); /* report H increase (self still balanced) */
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
				height_increased(avlt, p); /* report height inscrease (self still balanced) */
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

AVLNode *_node_swap_with_pred(AVLTree *avl, AVLNode *node){
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
        /* node has no predecessor (is a leaf) */
        return node;
    }

    /* switch the node with the defined predecessor */
    if( pred ){
        info = pred->info;
        pred->info = node->info;
        node->info = info;
        node = pred;
    }

    return node;
}

/* obs.: assumes the tree has more than 1 node... */
AVLTree *sub_avl_remove(AVLTree *avlt, AVLNode *node){
	int nkids;
    void *info;
    AVLNode *parent;
    AVLNode *brother;
    AVLNode *child;
    AVLNode *pred;

    node = _node_swap_with_pred(avlt, node);
    /* (node now (a) is a leaf or (b) has ONE child) */

    /* If node is a leaf:
     *  1. remove node;
     *  2. adjust parent balance;
     *  3. report height decrease (if nedded) */

    parent = node->parent;
    child = node->right ? node->right : node->left;
    if( child )
        child->parent = parent;

    if( parent ){
        if( node_is_left(node) ){
            parent->left = child;
            parent->balance++;
            if( !parent->balance ){
                height_decreased(avlt, parent);
            }else if( parent->balance == +2 ){
                if( parent->right->balance == +1 ){
                    rotate_left(avlt, parent);
                    parent->balance = parent->parent->balance = 0;
                    height_decreased(avlt, parent->parent);
                }else if( parent->right->balance == -1 ){
                    rotate_right_left(avlt, parent);
                    parent->balance = parent->parent->right->balance = 0;
                    if( parent->parent->balance == +1 )
                        parent->balance = -1;
                    else if( parent->parent->balance == -1 )
                        parent->parent->right->balance = +1;
                    parent->parent->balance = 0;
                    height_decreased(avlt, parent->parent);
                }else{
                    rotate_left(avlt, parent);
                    parent->balance = +1;
                    parent->parent->balance = -1;
                }
            }
        }else{ /* node is right */
            parent->right = child;
            parent->balance--;
            if( !parent->balance ){
                height_decreased(avlt, parent);
            }else if( parent->balance == -2 ){
                if( parent->left->balance == -1 ){
                    rotate_right(avlt, parent);
                    parent->balance = parent->parent->balance = 0;
                    height_decreased(avlt, parent->parent);
                }else if( parent->left->balance == +1 ){
                    rotate_left_right(avlt, parent);
                    parent->balance = parent->parent->left->balance = 0;
                    if( parent->parent->balance == -1 )
                        parent->balance = +1;
                    else if( parent->parent->balance == +1 )
                        parent->parent->left->balance = -1;
                    parent->parent->balance = 0;
                    height_decreased(avlt, parent->parent);
                }else{
                    rotate_right(avlt, parent);
                    parent->balance = -1;
                    parent->parent->balance = +1;
                }
            }
        }
    }else{
        /* if the node has no parent, it was the root. */
        avlt->root = NULL;
    }
    avlt->n--;
    free(node);

	return avlt;
}

AVLNode *_avl_find(AVLNode *node, void *info, avl_cmp_f cmp){
    AVLNode *found;
    int res;

    if( !node )
        return NULL;

    found = NULL;
    if( node->info == info )
        return node;

    res = cmp(node->info, info);
    if( res <= 0 )
        found = _avl_find(node->right, info, cmp);
    if( !found && res >= 0 && node->left )
        found = _avl_find(node->left, info, cmp);

    return found;
}

AVLTree *avl_remove(AVLTree *avl, void *info){
    AVLNode *node;

    if( avl->root )
        node = _avl_find(avl->root, info, avl->cmp);

    if( !node ){
        fprintf(stderr, "cound not remove element %x: didn't find it.\n", info);
        return avl;
    }

    sub_avl_remove(avl, node);
    return avl;
}

/*** DEBUG **********************************************************/
int _node_get_height(AVLNode *node){
    int rh, lh;

    rh = lh = 0;
    if( !node->right && !node->left )
        return 0;

    if( node->right )
        rh = _node_get_height(node->right);
    if( node->left )
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

    if( abs(rh-lh) > 1 ){
        fprintf(stderr, "error: Node %x has %d balance (%d - %d)\n", node, (rh-lh), rh, lh); 
        ans = 0;
    }

    if( (rh-lh) != node->balance ){
        fprintf(stdout, "error: Balance of %x claims to be %d but is (%d - %d = %d)\n",
            node, node->balance, rh, lh, (rh-lh));
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
int avl_check_sanity(AVLTree *avl){
    int balanced, sorted;
    int ans;

    ans = 0;
    balanced = _node_assert_balance(avl->root);
    sorted = _node_assert_sorted(avl->root, avl->cmp);

    if( !balanced ){
        ans = 1;
        fprintf(stdout, "Tree is not balanced.\n");
    }if( !sorted ){
        ans += 2;
        fprintf(stdout, "Tree is not sorted.\n");
    }

    //if( balanced && sorted )
    //    fprintf(stdout, "Tree is sanitized.\n");

    return ans;
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

void _avl_fprint_pretty(FILE *fout, AVLNode *node, avl_prt_f prt, char *prefix){
    int sn;
    fprintf(fout, prefix);
    if( node_is_right(node) ){
        if( node->parent->left )
            fprintf(fout, "\u251C < ");
        else
            fprintf(fout, "\u2514 < ");

        if( node->parent->left )
            strcat(prefix, "\u2502   ");
        else
            strcat(prefix, "\u2576   ");
    }else{
        fprintf(fout, "\u2514 > ");
        strcat(prefix, "\u2002   ");
    }
    prt(fout, node->info);
    fprintf(fout, " [%x] (%d/%d)  [p: %x, l: %x, r: %x]\n", node,
        _node_get_height(node), node->balance, node->parent, node->left, node->right);

    /* print children */
    if( node->right )
        _avl_fprint_pretty(fout, node->right, prt, prefix);
    if( node->left)
        _avl_fprint_pretty(fout, node->left, prt, prefix);

    /* clear prefix */
    sn = strlen(prefix);
    prefix[sn-6] = '\0';

    return;
}

void avl_fprint_pretty(FILE *fout, AVLTree *avlt){
    AVLNode *root;
    char prefix[1000];
    prefix[0] = '\0';

    root = avlt->root;
    if( root ){
        avlt->prt(fout, root->info);
        fprintf(fout, " [%x] (%d/%d)  [p: %x, l: %x, r: %x]\n", root,
            _node_get_height(root), root->balance, root->parent, root->left, root->right);
    }
    if( root->right )
        _avl_fprint_pretty(fout, root->right, avlt->prt, prefix);

    if( root->left ){
        _avl_fprint_pretty(fout, root->left, avlt->prt, prefix);
    }else{
        fprintf(fout, "coundsnt find left tree of %x...\n", root);
    }
    fflush(stdout);

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

