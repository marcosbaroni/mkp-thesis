# -*- Makefile -*-
CC = gcc
CFLAGS = -03

all: mkp-fast-nu

mkp-fast-nu: util.o mkp.o lp.o domset.o
	gcc -o mkp-fast-nu tools/mkp-fast-nu.c util.o mkp.o lp.o domset.o -lm

mkp.o:
	gcc -c models/mkp/mkp.c
domset.o:
	gcc -c models/mkp/domset.c

lp.o:
	gcc -c utils/lp.c
util.o:
	gcc -c utils/util.c

mkp.o: utils/util.c

clean:
	rm *.o
