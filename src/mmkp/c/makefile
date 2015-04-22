BINDIR="../../../bin"
CFLAGS=-lm -O3 -Wno-unused-result -g
all: kp mkp mmkp ssum
kp: kp2zpl kp-rand kp-bt kp-info kp-des
kp-info:
	gcc -o kp-info kp-info.c mmkp/kp.c mmkp/util.c $(CFLAGS)
	mv kp-info $(BINDIR)
kp2zpl:
	gcc -o kp2zpl kp2zpl.c mmkp/kp.c mmkp/util.c $(CFLAGS)
	mv kp2zpl $(BINDIR)
kp-bt:
	gcc -o kp-bt kp-bt.c mmkp/kp.c mmkp/util.c $(CFLAGS)
	mv kp-bt $(BINDIR)
kp-rand:
	gcc -o kp-rand kp-rand.c mmkp/kp.c mmkp/util.c $(CFLAGS)
	mv kp-rand $(BINDIR)
kp-des:
	gcc -o kp-des kp-des.c mmkp/kp.c mmkp/util.c mmkp/des.c $(CFLAGS)
	mv kp-des $(BINDIR)
mkp: mkp-rand mkp2zpl mkp-nu mkp-sfl mkp-simplex mkp-sfl-batch mkp-core mkp-domsets mkp-fast-nu
mkp-rand:
	gcc -o mkp-rand mkp-rand.c mmkp/mkp/mkp.c mmkp/lp.c mmkp/util.c $(CFLAGS)
	mv mkp-rand $(BINDIR)
mkp2zpl:
	gcc -o mkp2zpl mkp2zpl.c mmkp/mkp/mkp.c mmkp/lp.c mmkp/util.c $(CFLAGS)
	mv mkp2zpl $(BINDIR)
mkp-nu:
	gcc -o mkp-nu mkp-nu.c mmkp/mkp/mkp.c mmkp/lp.c mmkp/util.c $(CFLAGS)
	mv mkp-nu $(BINDIR)
mkp-fast-nu:
	gcc -o mkp-fast-nu mkp-fast-nu.c mmkp/mkp/mkp.c mmkp/mkp/domset.c mmkp/lp.c mmkp/util.c $(CFLAGS)
	mv mkp-fast-nu $(BINDIR)
mkp-sfl:
	gcc -o mkp-sfl mkp-sfl.c mmkp/sfl.c mmkp/mkp/mkp.c mmkp/lp.c mmkp/util.c $(CFLAGS)
	mv mkp-sfl $(BINDIR)
mkp-simplex:
	gcc -o mkp-simplex mkp-simplex.c mmkp/lp.c mmkp/mkp/mkp.c mmkp/util.c $(CFLAGS)
	mv mkp-simplex $(BINDIR)
mkp-sfl-batch:
	gcc -o mkp-sfl-batch mkp-sfl-batch.c mmkp/sfl.c mmkp/lp.c mmkp/mkp/mkp.c mmkp/util.c $(CFLAGS)
	mv mkp-sfl-batch $(BINDIR)
mkp-core:
	gcc -o mkp-core mkp-core.c mmkp/lp.c mmkp/mkp/mkp.c mmkp/util.c $(CFLAGS)
	mv mkp-core $(BINDIR)
mkp-domsets:
	gcc -o mkp-domsets mkp-domsets.c mmkp/mkp/mkp.c mmkp/util.c mmkp/lp.c $(CFLAGS)
	mv mkp-domsets $(BINDIR)
mmkp: mmkp-rand mmkp2zpl
mmkp-rand:
	gcc -o mmkp-rand mmkp-rand.c mmkp/mmkp/mkp.c mmkp/util.c $(CFLAGS)
	mv mmkp-rand $(BINDIR)
mmkp2zpl:
	gcc -o mmkp2zpl mmkp2zpl.c mmkp/mmkp/mkp.c mmkp/util.c $(CFLAGS)
	mv mmkp2zpl $(BINDIR)
ssum: ssum-rand ssum2zpl
ssum-rand:
	gcc -o ssum-rand ssum-rand.c mmkp/util.c mmkp/ssum.c $(CFLAGS)
	mv ssum-rand $(BINDIR)
ssum2zpl:
	gcc -o ssum2zpl ssum2zpl.c mmkp/util.c mmkp/ssum.c $(CFLAGS)
	mv ssum2zpl $(BINDIR)
test:
	gcc -o test test.c mmkp/avl.c mmkp/util.c mmkp/mkp/mkp.c mmkp/lp.c $(CFLAGS) -g
	mv test $(BINDIR)
clean:
	rm -f *.o

