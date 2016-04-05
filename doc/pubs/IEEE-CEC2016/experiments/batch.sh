#!/bin/bash
RT=/home/mbaroni/Computacao/dmkp
INSTDIR=$RT/insts/chubeas

ns="100 250 500"
ms="5 10 30"
as="0.25 0.50 0.75"
is="1 2 3 4 5 6 7 8 9 10"

for n in $ns; do
	for m in $ms; do
		for a in $as; do
			for i in $is; do
				inst=$INSTDIR/OR${m}x${n}-${a}_$i.dat
				let csize=$m+$n/10
				for s in `seq 10`; do
					sfl=`mkp-sfl $inst 20 20 5 300 20 $s`
					sflcore=`mkp-sfl-core $inst 20 20 5 300 20 $csize $s`
					best=`mkp-chubbest $n $m $a $i`
					echo $n";"$m";"$a";"$i";"$s";"$sfl";"$sflcore";"$best >> res.csv
				done
			done
		done
	done
done

