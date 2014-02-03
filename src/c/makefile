bindir="../../bin"
DEBUG=1
CFLAGS=-O3 -static -D DEBUG=$(DEBUG) -lm
CC=gcc
all: c
c: converter generator calcobj
converter:
	$(CC) -o converter core/*.c converter.c $(CFLAGS)
	mv converter $(bindir)
generator:
	$(CC) -o generator core/*.c generator.c $(CFLAGS)
	mv generator $(bindir)
calcobj:
	$(CC) -o calcobj core/*.c calcobj.c $(CFLAGS)
	mv calcobj $(bindir)

