#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

int str_cmp(char *a, char *b){
	return strcmp(a, b);
}


void test_avl(){
	AVLTree *avl;
	avl = avl_new(str_cmp);
}

int main(int argv, char **argc){
	test_avl();
	return 0;
}

