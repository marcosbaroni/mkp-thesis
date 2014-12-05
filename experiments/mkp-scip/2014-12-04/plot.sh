#!/bin/bash

if [ $# -lt 2 ]; then
    echo -e "usage:\n$0 <input dat> <output> [term]\n"
    exit 1
fi

inputdat=$1
output=$2
term="pdf"
if [ $# -gt 2 ]; then
	term=$3
fi

gnuplot <<!
# PLOTING CURVE OF PROOF
set term $term font "Arial,10"
set termoption dash
set termopt enhanced
set datafile separator ";"
set output "$output.$term"
set style data line
set style line 1 lt 1 lw 3 lc rgb "red"
set style line 2 lt 2 lw 3 lc rgb "dark-blue"
set style line 3 lt 3 lw 3 lc rgb "dark-green"
set style line 4 lt 4 lw 3 lc rgb "dark-yellow"
set style line 5 lt 5 lw 3 lc rgb "orange"
set style line 6 lt 6 lw 3 lc rgb "dark-red"
set xrange [50:*]
set xtics 50,50
set grid
set logscale y
set title "Solving time on scip from random MKP instances"
set xlabel "Size of instance"
set ylabel "Time (s)"
plot 4**((x-180)/100) ls 1 t "2^{(x/100)}", \
 "$inputdat" u 1:7 ls 2 title "m=6", \
 "$inputdat" u 1:6 ls 3 title "m=5", \
 "$inputdat" u 1:5 ls 4 title "m=4", \
 "$inputdat" u 1:4 ls 5 title "m=3", \
 "$inputdat" u 1:3 ls 6 title "m=2"
!

