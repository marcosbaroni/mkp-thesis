all: graph c hs
graph:
	gcc -o c -O3 graph.c
c:
	gcc -o transf -O3 transf.c -lm
hs:
	ghc -pgmlo opt -pgmlc llc -fllvm -o hs Graph.hs

