# -*- Makefile -*-
CC = gcc
CFLAGS = -O3 -g
DEST=../bin

all: mkp
mkp: mkp-nu mkp-rand mkp2zpl
mkp-nu: util.o mkp.o domset.o lp.o
	gcc $(CFLAGS) -o mkp-nu tools/mkp-nu.c util.o lp.o mkp.o domset.o -lm
	mv mkp-nu $(DEST)
mkp-rand: mkp.o util.o lp.o
	gcc $(CFLAGS) -o mkp-rand tools/mkp-rand.c mkp.o util.o lp.o -lm
	mv mkp-rand $(DEST)
mkp2zpl: mkp.o util.o
	gcc $(CFLAGS) -o mkp2zpl tools/mkp2zpl.c mkp.o util.o lp.o -lm
	mv mkp2zpl $(DEST)

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
