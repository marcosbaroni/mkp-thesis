all: randmkp mkp2zpl
randmkp: mmkp.o util.o
	gcc -o randmkp randmkp.c mmkp.c util.c -lm
mkp2zpl: mmkp.o util.o
	gcc -o mkp2zpl mkp2zpl.c mmkp.c util.c -lm
test:
	gcc -o test test.c util.c mmkp.c -lm
