all: c
c: converter generator
converter:
	gcc -g -O3 -static -o converter pcope.c converter.c util.c -lm `pkg-config --cflags --libs jansson`
generator:
	gcc -g -O3 -o generator pcope.c generator.c util.c -lm `pkg-config --cflags --libs jansson`

