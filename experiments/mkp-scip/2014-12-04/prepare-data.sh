#!/bin/bash
PATH=$PATH:../../../bin

if [ $# -lt 3 ]
then
	echo -e "usage:\n\t$0 <output.db> <output.dat> <input1.csv> [input2.csv] ... [input.csv]\n"
	echo -e "This program prepares the data (for plotting) from a database file,"
	echo -e "executing the proper selects for each \"m\" size"
	echo -e "and \"pastes\" the results on a single file (output.dat) for plotting"
	echo -e "(each line may be a \"m\" value)."
	exit 1
fi

outputdb=$1
outputdat=$2
temp=`tempfile`
echo "n;m;opt;gap;time" > $temp
echo "int;int;double;double;double" >> $temp
shift
shift
cat $@ >> $temp

csv2sqlite3 $temp $outputdb
sqlite $outputdb<<!
.mode csv
.separator ';'
.output ${temp}1
select n, avg(time) from tabela where m=1 group by n order by n;
.output ${temp}2
select avg(time) from tabela where m=2 group by n order by n;
.output ${temp}3
select avg(time) from tabela where m=3 group by n order by n;
.output ${temp}4
select avg(time) from tabela where m=4 group by n order by n;
.output ${temp}5
select avg(time) from tabela where m=5 group by n order by n;
!
paste -d ';' ${temp}1 ${temp}2 ${temp}3 ${temp}4 ${temp}5 > $outputdat
rm ${temp}*

