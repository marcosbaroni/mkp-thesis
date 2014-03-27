#!/bin/bash
DIR=$1
CONVERTER="java -jar ../gen.jar -convertToScip "
MODEL="knap-v3.zpl"
ZIMPL="../zimpl"

cp $MODEL $DIR
cd $DIR
i=1
for n in ` ls *.json`
do
	$CONVERTER $n > ${n/.json/.zpl}
	$ZIMPL -t lp -o `printf '%03d' $i` ${n/.json/.zpl}
	let i=$i+1
done

