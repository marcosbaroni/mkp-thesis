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
RANDKP=$BINDIR/randkp
KPBT=$BINDIR/kpbt

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

kptmp=`tempfile`
for n in `seq $n0 $nf`
do
	echo $n
	for i in `seq 1 $nsamples`
	do
		echo $n
		$RANDKP $n > $kptmp
		cat $kptmp
		num=`cat $kptmp | $KP2ZPL | $ZPL2LP | $RUNSCIP | $SCIP2SUMMARY | cut -d ';' -f 1`
		num=`python -c "print(int(float("$num")));"`
		$KPBT $kptmp
	done
done
rm $kptmp
