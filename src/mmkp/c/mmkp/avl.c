#include <stdio.h>
#include <stdlib.h>
#include "avl.h"

/*********************************************************************
         AVL Tree                                                     
*********************************************************************/

AVLNode *new_avl_node(void *a){
	AVLNode *node;
	node = (AVLNode*)malloc(sizeof(AVLNode));
	node->info = (void*)a;
	node->father = node->left = node->right = NULL;
	node->balance = 0;
	return node;
}

AVLTree *new_avltree( avl_cmp_f cmp ){
	AVLTree *avlt;
	avlt = (AVLTree*)malloc(sizeof(AVLTree));
	avlt->cmp = cmp;
	avlt-> n = 0;
	avlt-> root = NULL;
	return avlt;
}

void sub_free_avltree(AVLNode *node){
	if(!node) return;
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

void sub_avl_apply_to_all(AVLNode *node, void(*func)(void*) ){
	if(!node) func(node->info);
	if(node->left) sub_avl_apply_to_all(node->left, func);
	if(node->right) sub_avl_apply_to_all(node->right, func);
}

void avl_apply_to_all(AVLTree *avlt, void(*func)(void*) ){
	if(avlt->root) sub_avl_apply_to_all(avlt->root, func);
}

int sub_avlt_to_vector(AVLNode *node, void **v, int k){
	if(!node) return k;
	if(node->left) k = sub_avlt_to_vector(node->left, v, k);
	v[k++] = node->info;
	if(node->right) k = sub_avlt_to_vector(node->right, v, k);
	return k;
}

void **avlt_to_vector(AVLTree *avlt){
	void **v;
	
	v = (void**)malloc(avlt->n*sizeof(void*));
	sub_avlt_to_vector(avlt->root, v, 0);
	return v;
}

int avlt_size(AVLTree *avlt){
	return avlt->n;
}

AVLTree *rotate(AVLTree *avlt, AVLNode *p){
	AVLNode *f;
	if(!p->father) return avlt;
	f = p->father;

	/* Changing references on grandfather (if any) */
	if(f->father){
		if(f->father->right == f) f->father->right = p;
		else f->father->left = p;
		p->father = f->father;
	}else{
		avlt->root = p;
		p->father = NULL;
	}

	if(f->left == p){
		/* Rotating */
		f->left = p->right;
		if( p->right ) p->right->father = f;
		p->right = f;
		f->father = p;

		/* Updating balances */
		f->balance = f->balance - max(0, p->balance) - 1;
		p->balance = p->balance - max(0, -f->balance) -1;
	}else{
		/* Rotating */
		f->right = p->left;
		if( p->left ) p->left->father = f;
		p->left = f;
		f->father = p;

		/* Updating balances */
		f->balance = f->balance + max(0, -p->balance) + 1;
		p->balance = p->balance + max(0, f->balance) + 1;
	}

	return avlt;
}

void height_incresed(AVLTree *avlt, AVLNode *p){
	AVLNode *f;
	f = p->father;
	if(!f) return;
	if( f->left == p){
		f->balance++;
		switch(f->balance){
			case 1:
			height_incresed(avlt, f);
			break;

			case 2:
			switch(p->balance){
				case -1:
				rotate(avlt, p->right);
				case 1:
				rotate(avlt, f->left);
			}
		}
	}else{
		f->balance--;
		switch(f->balance){
			case -1:
			height_incresed(avlt, f);
			break;

			case -2:
			switch(p->balance){
				case 1:
				rotate(avlt, p->left);
				case -1:
				rotate(avlt, f->right);
			}
		}
	}
	return;
}

AVLTree *sub_avl_insert(AVLTree *avlt, AVLNode *p, AVLNode *newp){
	int res;

	res = avlt->cmp(p->info, newp->info);

	if( res < 0 ){
		if(p->right)
			sub_avl_insert(avlt, p->right, newp);
		else{
			/* Appending new node on right side */
			p->right = newp;
			newp->father = p;
			p->balance--;

			if(p->balance != 0)
				height_incresed(avlt, p);
		}
	}else{
		if(p->left)
			sub_avl_insert(avlt, p->left, newp);
		else{
			/* Appending new node on left side */
			p->left = newp;
			newp->father = p;
			p->balance++;

			if(p->balance != 0)
				height_incresed(avlt, p);
		}
	}
	return avlt;
}

AVLTree *avl_insert(AVLTree *avlt, void *a){
	AVLNode *node;
	node = new_avl_node(a);

	if(!avlt->root)
		avlt->root = node;
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
	
void _print_nodes_dot(FILE *fout, AVLTree *avlt, void (*fprt)(FILE *f, void *a)){
	void **v;
	int i, n;

	n = avlt_size(avlt);
	v = avlt_to_vector(avlt);

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

void fprint_avlt_dot(FILE *fout, AVLTree *avlt, avl_prt_f prt){
	fprintf(fout, "graph G {\n");
	fprintf(fout, "\tnode [style=filled, shape=circle,fillcolor=lightgray]\n");

	_print_nodes_dot(fout, avlt, prt);
	_print_edges_dot(fout, avlt->root);

	fprintf(fout, "}\n");
	return;
}
/******************************************************************************/

