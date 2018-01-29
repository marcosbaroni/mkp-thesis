#!/bin/bash
ROOT=/home/mbaroni/Computacao/phd/dmkp
MOKPBIN=$ROOT/bin/mokp
INSTS=(A-700-2 B-4000-2 C-500-2 D-200-2)
INSTDIR=$ROOT/doc/pubs/SCE-MOKP0-2018/experiments/insts/bazgan
echo "bazeff"
echo "inst;hvol"
echo "string;float"
for i in ${INSTS[@]}; do
	for f in `ls $INSTDIR/$i-*.eff`; do
		hvol=`$MOKPBIN hvol $f`
		bname=`basename $f`
		bname=${bname%.*}
		echo $bname";"$hvol
	done
done
