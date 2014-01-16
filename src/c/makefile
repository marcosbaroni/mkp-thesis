all: c
c: converter
converter:
	gcc -O3 -static -o converter pcope.c converter.c util.c -lm `pkg-config --cflags --libs jansson`

