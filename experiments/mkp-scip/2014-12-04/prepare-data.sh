#!/bin/bash
PATH=$PATH:../../../bin

if [ $# -lt 3 ]
then
	echo -e "usage:\n$0 <output.db> <output.dat> <input1.csv> [input2.csv] ... [input.csv]"
	exit 1
fi

outputdb=$1
outputdat=$2
temp=`tempfile`
echo "n;m;opt;gap;time" > $temp
echo "int;int;double;double;double" >> $temp
shift 2
cat $@ >> $temp

csv2sqlite3 $temp $outputdb
sqlite $<<!
	.mode csv
	.separator ';'
	.output ${temp}1
	select n, avg(time) from tabela where m=1 group bt n order by n;
	.output ${temp}2
	select n, avg(time) from tabela where m=2 group bt n order by n;
	.output ${temp}3
	select n, avg(time) from tabela where m=3 group bt n order by n;
	.output ${temp}4
	select n, avg(time) from tabela where m=4 group bt n order by n;
	.output ${temp}5
	select n, avg(time) from tabela where m=5 group bt n order by n;
!

paste -d ';' ${temp}1 ${temp}2 ${temp}3 ${temp}4 ${temp}5 | cut -d ';' -f 1 2 4 6 8 10 > $outputdat
rm ${temp}*

