BINDIR="../../../bin"
all: kpmkp mmkp ssum
kp: kp2zpl randkp kpbt kpinfo
kpinfo:
	gcc -o kpinfo kpinfo.c mmkp/kp.c mmkp/util.c -lm
	mv kpinfo $(BINDIR)
kp2zpl:
	gcc -o kp2zpl kp2zpl.c mmkp/kp.c mmkp/util.c -lm
	mv kp2zpl $(BINDIR)
kpbt:
	gcc -o kpbt kpbt.c mmkp/kp.c mmkp/util.c -lm
	mv kpbt $(BINDIR)
randkp:
	gcc -o randkp randkp.c mmkp/kp.c mmkp/util.c -lm
	mv randkp $(BINDIR)
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
	gcc -o test test.c mmkp/util.c mmkp/ssum.c mmkp/mmkp.c mmkp/kp.c -lm -g
clean:
	rm -f *.o

