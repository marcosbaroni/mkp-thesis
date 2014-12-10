#!/bin/bash

if [ $# -lt 2 ]; then
    echo -e "usage:\n$0 <data dir> <output> [term]\n"
    exit 1
fi

datadir=$1
output=$2
term="pdf"
if [ $# -gt 2 ]; then
	term=$3
fi

for m in 1 2 3 4; do
nudat=$datadir/nu$m.dat
scipdat=$datadir/scip$m.dat
gnuplot <<!
# PLOTING CURVE OF PROOF
set term $term font "Times-New Roman,18"
set termoption dash
set termopt enhanced
set datafile separator ";"
set output "$output-$m.$term"
set style data line
set style line 1 lt 1 lw 5 lc rgb "red"
set style line 2 lt 2 lw 5 lc rgb "dark-blue"
set style line 3 lt 2 lw 2 lc rgb "red"
set style line 4 lt 2 lw 2 lc rgb "dark-blue"
set xrange [20:100]
set yrange [0.0001:10000]
#set x2range [20:*]
set grid xtics ytics
set xtics 20,10,400
#set x2tics (20,30,40)
set logscale y
#set title "m=$m" font "Times-New Roman,26"
set xlabel "Size of instance (number of items)"
set ylabel "Time (s)"
plot \
 "$nudat" u 1:2 ls 1 title "Nemhauser-Ullmann", \
 "$scipdat" u 1:2 ls 2 title "SCIP 3.0.1"
!

done

