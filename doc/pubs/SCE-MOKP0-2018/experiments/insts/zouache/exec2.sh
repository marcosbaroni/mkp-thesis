#!/bin/bash

ROOTDIR=/home/mbaroni/Computacao/phd/dmkp/
BIN=$ROOTDIR/bin/mokp
OUTDIR=/home/mbaroni/zou
INSTDIR=$ROOTDIR/insts/zouache
for i in `seq 30` ;
do
	for f in `ls $INSTDIR/*.txt` ;
	do
		basename=${f##*/}
		echo $f $i $basename
		$BIN sce $f 0 1 400 30 30 4 30 0.05 > $OUTDIR/$basename-$i.txt
	done
done

