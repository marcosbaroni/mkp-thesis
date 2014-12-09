#!/bin/bash
PATH=$PATH:../../bin/
if [ $# -lt 1 ]; then
	echo -e "usage: $0 <output file (csv)>"
	exit 1
fi

output=$1
temp=`tempfile`
for m in `seq 1 5` ;do
	for n in `seq 6 2 40` ;do
		for i in `seq 1 5` ;do
			randmkp $n $m > $temp
			nu=`mkp-nu $temp`
			sc=`mkp2zpl $temp| zpl2lp | runscip | scip2summary`
			echo $n";"$m";"$nu";"$sc >> $output
		done
	done
done
rm $temp*

