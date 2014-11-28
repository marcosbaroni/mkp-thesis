#!/bin/bash
CSV2SQLITE="../../bin/csv2sqlite3"

if [ $# -lt 2 ]; then
    echo -e "usage:\n$0 <input raw csv> <output figure> [term=svg] [n0] [nf]\n"
    echo -e "  [n0:nf] : range of nitems to be used\n"
    exit 1
fi

data1=$1
out=$2
term=svg
n0=1
nf=10000
if [ $# -gt 2 ]; then
	term=$3
	if [ $# -gt 3 ]; then
		n0=$4
		if [ $# -gt 4 ]; then
			nf=$5
		fi
	fi
fi

tempcsv=`tempfile`          # raw csv data
tempdb=`tempfile`           # sqlite3 data
tempdat=`tempfile`          # processed data
tempn=`tempfile`            # min number of sample (yep, just one word!)
echo "nitems;nsets" > $tempcsv
echo "int;int" >> $tempcsv
cat $data1 >> $tempcsv
$CSV2SQLITE $tempcsv $tempdb
sqlite3 $tempdb <<!
.mode csv
.separator ';'
.output $tempdat
select nitems, avg(nsets) from tabela where nitems >= $n0 and nitems <= $nf group by nitems;
.output $tempn
select min(n) from (select count(*) n from tabela where nitems >= $n0 and nitems <= $nf group by nitems);
!
rm $tempdb
rm $tempcsv
minsample=`cat $tempn`
rm $tempn

gnuplot <<!
# PLOTING CURVE OF PROOF
set term $term
set datafile separator ";"
set output "$out.$term"
set style data linespoints
set grid
set logscale y
set title "N. od Dom. Sets generated from 2dim-KP using Nemhauser-Ullman Algorithm"
set xlabel "Size of instance"
set ylabel "N. of dom. sets"
plot "$tempdat" u 1:2 title "Avg. of $minsample", 2**x t "2^x", 1.6**x+10 t "1.6^x"
!
rm $tempdat
