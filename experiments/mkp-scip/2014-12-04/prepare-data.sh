#!/bin/bash
PATH=$PATH:../../../bin

if [ $# -lt 5 ]
then
	echo -e "usage:\n\t$0 <output.db> <output.dat> <m0> <mf> <input1.csv> [input2.csv] ... [input.csv]"
	echo -e "\tn0: initial m value"
	echo -e "\tnf: final m value"
	echo -e "This program prepares the data (for plotting) from a database file,"
	echo -e "executing the proper selects for each \"m\" size"
	echo -e "and \"pastes\" the results on a single file (output.dat) for plotting"
	echo -e "(each line may be a \"m\" value)."
	exit 1
fi

outputdb=$1
outputdat=$2
n0=$3
nf=$4
temp=`tempfile`
echo "n;m;opt;gap;time" > $temp
echo "int;int;double;double;double" >> $temp
shift
shift
shift
shift
cat $@ >> $temp

echo ".mode csv" >> $temp.sql
echo ".separator ';'" >> $temp.sql

# print instance size column
echo ".output ${temp}0" >> $temp.sql
echo "select n from tabela where m=$n0 group by n order by n;" >> $temp.sql
echo -n "${temp}0 " >> $temp.in
for i in `seq $n0 1 $nf`
do
	# print values for m=$i
	echo ".output ${temp}$i" >> $temp.sql
	echo "select avg(time) from tabela where m=$i group by n order by n;" >> $temp.sql
	echo -n "${temp}$i " >> $temp.in
done

csv2sqlite3 $temp $outputdb
sqlite3 $outputdb < $temp.sql
paste -d ';' `cat ${temp}.in` > $outputdat
rm ${temp}*

