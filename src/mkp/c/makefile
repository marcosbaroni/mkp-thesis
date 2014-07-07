BINDIR="../../../bin"
all: randmmkp mmkp2zpl
randmmkp: mmkp.o util.o
	gcc -o randmmkp randmmkp.c mmkp.c util.c -lm
	mv randmmkp $(BINDIR)
mmkp2zpl: mmkp.o util.o
	gcc -o mmkp2zpl mmkp2zpl.c mmkp.c util.c -lm
	mv mmkp2zpl $(BINDIR)
test:
	gcc -o test test.c util.c mmkp.c -lm
