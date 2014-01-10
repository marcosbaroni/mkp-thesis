all:
	gcc -o c -O3 graph.c
	gcc -o transf -O3 transf.c
	ghc -pgmlo opt -pgmlc llc -fllvm -o hs Graph.hs

