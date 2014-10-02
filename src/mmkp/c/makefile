BINDIR="../../../bin"
all: mkp mmkp ssum
mkp: randmkp mkp2zpl
randmkp:
	gcc -o randmkp randmkp.c mmkp/mkp.c mmkp/util.c -lm
	mv randmkp $(BINDIR)
mkp2zpl:
	gcc -o mkp2zpl mkp2zpl.c mmkp/mkp.c mmkp/util.c -lm
	mv mkp2zpl $(BINDIR)
mmkp: randmmkp mmkp2zpl
randmmkp:
	gcc -o randmmkp randmmkp.c mmkp/mmkp.c mmkp/util.c -lm
	mv randmmkp $(BINDIR)
mmkp2zpl:
	gcc -o mmkp2zpl mmkp2zpl.c mmkp/mmkp.c mmkp/util.c -lm
	mv mmkp2zpl $(BINDIR)
ssum: randssum ssum2zpl
randssum:
	gcc -o randssum randssum.c mmkp/util.c mmkp/ssum.c -lm
	mv randssum $(BINDIR)
ssum2zpl:
	gcc -o ssum2zpl ssum2zpl.c mmkp/util.c mmkp/ssum.c -lm
	mv ssum2zpl $(BINDIR)
test:
	gcc -o test test.c mmkp/util.c mmkp/ssum.c mmkp/mmkp.c -lm
clean:
	rm -f *.o

