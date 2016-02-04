#!/bin/bash
IDIR=/home/mbaroni/Computacao/dmkp/insts/gk/
SFL=/home/mbaroni/Computacao/dmkp/bin/mkp-sfl

#is="01 02 03 04 05 06 07 08 09"
is="10 11"

for i in $is
do
	fin=$IDIR/gk$i.dat
	n=`head -1 $fin | cut -d ' ' -f 2`
	m=`head -1 $fin | cut -d ' ' -f 3`
	for s in `seq 0 10`
	do
		res=`$SFL $fin 20 20 5 300 20 $s`
		echo $i";"$res >> /home/mbaroni/mkp-sfl-res2.csv
		echo $i $s
	done
done

