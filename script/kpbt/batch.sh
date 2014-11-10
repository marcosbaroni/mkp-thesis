#!/bin/bash
######################################################################
# Script para testes do algoritmo de backtrack para o KP.
######################################################################

output=${1}
nmax=1000
kpmod=`tempfile`
for n in `seq 5 28`
do
	echo $n
	for i in `seq 1 $nmax`
	do
		./randkp $n | ./kpbt >> $output
		#./randkp $n | ./kpbt >> $output
		#./randkp $n | ./kpbt >> $output
	done
done
rm $kpmod
