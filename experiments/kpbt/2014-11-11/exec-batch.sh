#!/bin/bash
######################################################################
# Script para testes do algoritmo de backtrack para o KP.
######################################################################
KPBT=../../../bin/kpbt

# Checking number of arguments
if [ $# -lt 2 ]; then
	echo "usage.: $0 <inst directories> <output file>"
	exit 1
fi

inputdir=$1
outfile=$2

echo "nitems;found;proof;profit;split;first;last;selected;solution" > $outfile
for n in `seq 10 30`
do
	echo $n
	for i in `seq -f "%03.f" 1 1000`
	do
		$KPBT $inputdir/$n/$i.txt >> $outfile
	done
done

