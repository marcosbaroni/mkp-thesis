#!/bin/bash
RT=/home/mbaroni/Computacao/dmkp
CHUBDIR=$RT/insts/chubeas
GKDIR=$RT/insts/gk

ns="100 250 500"
ms="5 10 30"
as="0.25 0.50 0.75"
is="1 2 3 4 5 6 7 8 9 10"

# Batch for ChuBeasley instances
function runchub(){
for n in $ns; do
	for m in $ms; do
		for a in $as; do
			for i in $is; do
				inst=$CHUBDIR/OR${m}x${n}-${a}_$i.dat
				let csize=$m+$n/10
				for s in `seq 10`; do
					sfl=`mkp-sfl $inst 20 20 5 300 20 $s`
					sflcore=`mkp-sfl-core $inst 20 20 5 300 20 $csize $s`
					best=`mkp-chubbest $n $m $a $i`
					echo $n";"$m";"$a";"$i";"$s";"$sfl";"$sflcore";"$best >> chub.csv
				done
			done
		done
	done
done
}


function rungk(){
# Batch for GK instances
echo "gk" > gk.csv
echo "i;s;n;m;opt;it;time;optcore;itcore;timecore;best" >> gk.csv
for i in `seq -f "%02.f" 11`; do
    inst=$GKDIR/gk$i.dat
    d=`mkp-gkbest $i`
    best=`echo $d | cut -d ';' -f 1`
    n=`echo $d | cut -d ';' -f 2`
    m=`echo $d | cut -d ';' -f 3`
    let csize=$m+$n/10

    for s in `seq 10`; do
        sfl=`mkp-sfl $inst 20 20 5 300 20 $s`
        sflcore=`mkp-sfl-core $inst 20 20 5 300 0 $csize $s`
        echo $i";"$s";"$n";"$m";"$sfl";"$sflcore";"$best >> gk.csv
    done
done
}

#runchub
rungk
