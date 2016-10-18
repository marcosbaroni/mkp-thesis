# -*- Makefile -*-
CC = gcc
CFLAGS=-O3 -g
DEST=../bin
DEBUG_FLAG=-DDEBUG_LVL=1
SOPLEX=-lsoplex -lz -lgmp -lm

all: mkp mokp
mkp: mkp-nu mkp-rand mkp2zpl
mkp-nu: util.o mkp.o domset.o lp.o mkp-soputils.o mkp-nu.o balev.o lbucket.o ppm.o
	g++ $(CFLAGS) -o mkp-nu mkp-nu.o util.o lp.o mkp.o balev.o domset.o mkp-soputils.o lbucket.o ppm.o -lm $(SOPLEX) $(DEBUG_FLAG)
	mv mkp-nu $(DEST)
mkp-rand: mkp.o util.o lp.o
	gcc $(CFLAGS) -o mkp-rand tools/mkp-rand.c mkp.o util.o lp.o -lm
	mv mkp-rand $(DEST)
mkp2zpl: mkp.o util.o lp.o mkp-soputils.o mkp2zpl.o
	g++ $(CFLAGS) -o mkp2zpl mkp2zpl.o mkp.o util.o lp.o mkp-soputils.o -lm $(SOPLEX)
	mv mkp2zpl $(DEST)
mkp-soputils.o:
	g++ $(CFLAGS) -c models/mkp/mkp-soputils.cpp $(SOPLEX)
mkp-nu.o:
	gcc $(CFLAGS) -c tools/mkp-nu.c
mkp2zpl.o:
	gcc $(CFLAGS) -c tools/mkp2zpl.c

mkp-fast-nu: util.o mkp.o lp.o domset.o
	gcc $(CFLAGS) -o mkp-fast-nu tools/mkp-fast-nu.c util.o mkp.o lp.o domset.o -lm
balev.o:
	gcc $(CFLAGS) -c models/mkp/balev.c
lbucket.o:
	gcc $(CFLAGS) -c models/mkp/lbucket.c
ppm.o:
	gcc $(CFLAGS) -c utils/ppm.c -lm

mkp.o:
	gcc -c models/mkp/mkp.c
domset.o:
	gcc -c models/mkp/domset.c

lp.o:
	gcc -c utils/lp.c
util.o:
	gcc -c utils/util.c

mkp.o: utils/util.c
mokp: mokp.o util.o
	gcc $(CFLAGS) -o mokp tools/mokp.c mokp.o util.o -lm
	mv mokp $(DEST)
mokp.o:
	gcc $(CFLAGS) -c models/mokp/mokp.c
clean:
	rm *.o

