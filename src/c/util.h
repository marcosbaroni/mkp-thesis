#ifndef PCOPE_UTIL_H
#define PCOPE_UTIL_H 1


typedef struct String{
	char *str;
	int n;
	int nmax;
}String;


/* Allocs a new string. */
String *str_new();

/* Gets the char* from string. */
char *str_get(String *s);

/* Concatenates append a string to another. */
String *str_concat(String *dest, String *src);

/* Frees a string. */
void str_free(String *s);

#endif

