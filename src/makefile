# -*- Makefile -*-
#  Flags:
#   NO_KDTREE_STATS: Disable kdtree statistics
CC = gcc
#CFLAGS=-g -DUSE_MEMPOOL
#CFLAGS=-g -DDEBUG_LVL=1
#CFLAGS=-g $(DEBFLAG) -pg
CFLAGS=-g
#CFLAGS=-O3
DEST=../bin
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

mem.o:
	gcc $(CFLAGS) -c utils/mem.c
lp.o:
	gcc -c utils/lp.c
avl.o:
	gcc $(CFLAGS) -c utils/avl.c
util.o:
	gcc $(CFLAGS) -c utils/util.c
kdtree.o:
	gcc $(CFLAGS) -c utils/kdtree.c
heap.o:
	gcc $(CFLAGS) -c utils/heap.c
list.o:
	gcc $(CFLAGS) -c utils/list.c

test: util.o avl.o heap.o
	gcc $(CFLAGS) -o test tools/test.c avl.o util.o heap.o -lm
	mv test $(DEST)

mkp.o: utils/util.c
mokp: mokp.o tmokp.o util.o lp.o order.o kdtree.o avl.o heap.o bazgan.o list.o mokp-mh.o
	#g++ $(CFLAGS) -o mokp tmokp.o mokp.o mkp.o util.o lp.o mkp-soputils.o order.o bazgan.o kdtree.o avl.o heap.o list.o -lm $(SOPLEX)
	gcc $(CFLAGS) -o mokp tmokp.o mokp.o util.o mokp-mh.o lp.o order.o bazgan.o kdtree.o avl.o heap.o list.o -lm #$(SOPLEX)
	mv mokp $(DEST)
mokp.o:
	gcc $(CFLAGS) -c models/mokp/mokp.c
order.o:
	gcc $(CFLAGS) -c models/mokp/order.c
bazgan.o:
	gcc $(CFLAGS) -c models/mokp/bazgan.c
tmokp.o:
	gcc $(CFLAGS) -c tools/mokp.c -o tmokp.o
mokp-mh.o:
	gcc $(CFLAGS) -c models/mokp/mokp-mh.c
clean:
	rm *.o

