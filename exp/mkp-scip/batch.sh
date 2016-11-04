#!/bin/bash
BINDIR=../../../bin
RANDMKP=$BINDIR/randmkp
MKP2ZPL=$BINDIR/mkp2zpl
ZPL2LP=$BINDIR/zpl2lp
RUNSCIP=$BINDIR/runscip
RUNSCIP=$BINDIR/runscip
DOMSETS=$BINDIR/mkp-domsets

if [ $# -lt 1 ]; then
    echo -e "usage:\n $0 <output> \n"
    exit 1
fi

PATH=$PATH:$BINDIR
out=$1

for m in 1 2 3 4 5 6 7 8 9 10
do
	for i in `seq 1 100`
	do
		for n in `seq 10 10 200`
		do
			res=`randmkp $n $m | mkp2zpl | zpl2lp | runscip | scip2summary`
			echo $n";"$m";"$res >> $out
		done
	done
done

