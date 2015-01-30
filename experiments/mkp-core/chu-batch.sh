#!/bin/bash
BINDIR=../../bin
INSTDIR=../../insts/chubeas/
MKPCORE=$BINDIR/mkp-core
outdir=result.csv
logfile=log

ns="100 250 500"
ms="5 10 30"
ts="0.25 0.50 0.75"
ss="1 2 3 4 5 6 7 8 9 10"

n=$1
m=$2

#for n in $ns ; do
  #for m in $ms ; do
    for t in $ts ; do
      for s in $ss ; do
        input=$INSTDIR/OR${m}x${n}-${t}_${s}.dat
		date >> log
        res=`$MKPCORE $input`
		echo $n";"$m";"$t";"$s";"$res | tee -a log result.csv
	  done
    done
  #done
#done
