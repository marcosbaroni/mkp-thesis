#!/bin/bash

PATH=$PATH:../../../bin
if [ $# -lt 2 ]
then
	echo -e "usage:\n$0 <input db> <output dat>"
fi

db=$1
output=$2
temp=`tempfile`

sqlite3 $db <<!
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
paste -d ';' ${temp}1 ${temp}2 ${temp}3 ${temp}4 ${temp}5 > $output
rm ${temp}*
