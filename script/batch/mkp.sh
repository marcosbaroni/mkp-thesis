#!/bin/bash
BINDIR="../../bin"
OUTDIR="res"

ZIMPL=zimpl
SCIP=scip
RANDMKP=$BINDIR/randmkp
MKP2ZPL=$BINDIR/mkp2zpl

function run(){
	temp=`mktemp`
	$RANDMKP $n $m | $MKP2ZPL > $temp.zpl
	$ZIMPL -v0 $temp.zpl -t lp
	$SCIP -f `basename $temp`.lp | grep "Solving Time (sec)"| sed 's/ //g'|cut -d ':' -f 2 >> $OUTDIR/$n-$m.txt
	rm $temp*
	rm `basename $temp`*
}

#ns=`seq -f "%04.f" 10 10 1000`
ns="1000"

#ms=`seq -f "%02.f" 1 50`
ms="1 2 3 4 5"

smax=100

for n in $ns
do
	for m in $ms
	do
		echo -n $n $m started `date`...
		for s in `seq -f "%04.f" 1 $smax`
		do
			run
		done
		echo  done.
	done
done
