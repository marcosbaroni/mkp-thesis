bindir="../../bin"
all: c
c: converter generator calcobj
converter:
	gcc -g -O3 -static -o converter core/*.c converter.c -lm
	mv converter $(bindir)
generator:
	gcc -g -O3 -o generator core/*.c generator.c -lm
	mv generator $(bindir)
calcobj:
	gcc -g -O3 -o calcobj core/*.c calcobj.c -lm
	mv calcobj $(bindir)

