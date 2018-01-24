#!/bin/bash
INSTDIR=/home/mbaroni/Computacao/phd/dmkp/doc/pubs/SCE-MOKP0-2018/experiments/insts/bazgan
BIN=/home/mbaroni/Computacao/phd/dmkp/bin/mokp
OUTFILE=/home/mbaroni/Computacao/phd/dmkp/doc/pubs/SCE-MOKP0-2018/experiments/results/res.csv
niter=500
ncomp=40
compsize=40
nsubcomp=5
nsubiter=20
cross=0.05
for f in `ls $INSTDIR/*.txt`; do
	bname=`basename $f`
	echo $bname
	for i in `seq -f "%02.0f" 1 10`; do
		echo -n $i
		res=`$BIN sce $f 0 $i $niter $ncomp $compsize $nsubcomp $nsubiter $cross | tail -1`
		echo "$bname;$i;$res" >> $OUTFILE
	done
	echo ""
done
