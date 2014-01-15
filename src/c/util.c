#include <stdlib.h>
#include <string.h>
#include "util.h"

#define STD_MAX_LEN 1000;

String *str_new(){
	String *s;
	s = (String*)malloc(sizeof(String));
	s->str = (char*)malloc((STD_MAX_LEN+1)*sizeof(char*));
	s->str[0] = '\0';
	s->n = 0;
	s->nmax = STD_MAX_LEN;

	return s;
}

int str_fits(String *s, size_t n){
	return (s->nmax >= n);
}

String *str_assert_size(String *s, size_t n){
	char *aux;
	while(!str_fits(s, n)){
		s->str = realloc(s->str, s->nmax*2);
		s->nmax *= 2;
	}
	return s;
}

char *str_get(String *s){
	return s->str;
}

String *str_concat(String *dest, String *src){
	dest = str_assert_size(dest->n + src->n);
	strcat(&(dest->s[dest->n]), src->s);
	dest->n += src->n;
	return dest;
}

void str_free(String *s){
	free(s->str);
	free(s);
	return;
}

