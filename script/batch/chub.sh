#/bin/bash
RT=/home/mbaroni/Computacao/dmkp
INSTDIR=$RT/insts/chubeas
BINDIR=$RT/bin
SCIP=$BINDIR/scip
MKP2ZPL=$BINDIR/mkp2zpl
ZPL2LP=$BINDIR/zpl2lp
RUNSCIP=$BINDIR/runscip
SCIP2SUMMARY=$BINDIR/scip2summary
PATH=$PATH:$BINDIR
out=$RT/experiments/ciarp2015/`hostname`.csv


ns="100 250 500"
ms="5 10 30"
as="0.25 0.50 0.75"
i=$1
let i=$i+1

maxtime=14400

for n in $ns; do
	for m in $ms; do
		for a in $as; do
			inst=$INSTDIR/OR${m}x${n}-${a}_$i.dat
			ans=`mkp2zpl $inst | zpl2lp | runscip - $maxtime | scip2summary`
			echo $n";"$m";"$a";"$i";"$ans >> $out
		done
	done
done

