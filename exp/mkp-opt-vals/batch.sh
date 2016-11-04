#!/bin/bash
BINDIR=../../bin
PATH=$PATH:$BINDIR
echo $PATH

ns="10 20 30 40 50 60 70 80 90 100"
ms="1 2 3 4 5"

output=res.csv

for n in $ns
do
	for m in $ms
	do
		for i in `seq 1 100`
		do
			res=`mkp-rand -b 0 $n $m | mkp2zpl - | zpl2lp | runscip - | scip2summary`
			echo $n";"$m";"$i";"$res
		done
	done
done

