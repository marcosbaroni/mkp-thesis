#!/bin/bash
######################################################################
# Script para testes em lote do algoritmo de backtrack para o KP.
######################################################################
BINDIR=../../bin
RANDMKP=$BINDIR/randmkp
DOMSETS=$BINDIR/mkp-domsets

# Checkinkg number of arguments
if [ $# -lt 6 ]; then
    echo -e "usage:\n $0 <output-dir> <n0> <nf> <m0> <mf> <# of samples>\n"
	echo "  output-dir: directory to place CSV output (name of file will be random)"
	echo "  [n_0, n_f]: range of number of items"
	echo "  [m_0, m_f]: range of number of dimension"
	echo "  # of samples: number of samples for each instance"
    exit 1
fi

output=`tempfile -d $1 -s .csv`
n0=$2
nf=$3
m0=$4
mf=$5
nsamples=$6

for i in `seq 1 $nsamples`
do
	for m in `seq $m0 $mf`
	do
		for n in `seq $n0 $nf`
		do
			num=`$RANDMKP $n $m | $DOMSETS`
			echo "$n;$m;$num" >> $output
		done
	done
done

