#!/bin/bash

######################################################################
# Script temporário para testes do algoritmo de backtrack para o KP.
######################################################################

output=${1}
nmax=10
kpmod=`tempfile`
for n in 20 21 22 23 24 25
do
	i=1
	while [ $i -lt $nmax ]
	do
		echo "$n $i"
		echo -n "$i;" >> $output
		./randkp $n 0.5 $i > $kpmod
		./kpbt $kpmod >> $output
		i=$[$i+1]
	done
done
rm $kpmod
