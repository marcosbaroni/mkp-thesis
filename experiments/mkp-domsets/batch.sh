#!/bin/bash
######################################################################
# Script para testes em lote do algoritmo de backtrack para o KP.
######################################################################
BINDIR="../../bin"
SCIP=$BINDIR/scip
RUNSCIP=$BINDIR/runscip
SCIP2SUMMARY=$BINDIR/scip2summary
ZPL2LP=$BINDIR/zpl2lp
KP2ZPL=$BINDIR/kp2zpl
RANDMKP=$BINDIR/randmkp
KPBT=$BINDIR/kpbt
RUNNERHS=../../src/mmkp/haskell/MKP.hs

# Checkinkg number of arguments
if [ $# -lt 4 ]; then
    echo -e "usage:\n $0 <output> <n_0> <n_f> <# of samples>\n"
	echo "  output: output CSV file"
	echo "  [n_0, n_f]: range of number of items"
	echo "  # of samples: number of samples for each instance"
    exit 1
fi

output=${1}
n0=$2
nf=$3
nsamples=$4

function parsenumber(){
python <<!
from sys import stdin
print(stdin.readlines())
!
}

for i in `seq 1 $nsamples`
do
	for n in `seq $n0 $nf`
	do
		num=`$RANDMKP $n 2 | runhaskell $RUNNERHS`
		echo $n";"$num
	done
done

