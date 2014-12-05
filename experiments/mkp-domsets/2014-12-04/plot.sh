
#!/bin/bash
CSV2SQLITE="../../bin/csv2sqlite3"

################# GET ARGUMENTS #################################
if [ $# -lt 2 ]; then
    echo -e "usage:\n$0 <input raw csv> <output figure> [term=svg] [n0] [nf]\n"
    echo -e "  [n0:nf] : range of nitems to be used\n"
    exit 1
fi

data=$1
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

################# PLOT DATA #################################
gnuplot <<!
# PLOTING CURVE OF PROOF
set term $term
set palette rgb 7,5,15
set termoption dash
set datafile separator ";"
set output "$out.$term"
set style data line
set style line 1 lt 1 lw 3 lc rgb "red"
set style line 2 lt 2 lw 3 lc rgb "dark-blue"
set style line 3 lt 3 lw 3 lc rgb "dark-green"
set style line 4 lt 4 lw 3 lc rgb "dark-yellow"
set style line 5 lt 5 lw 3 lc rgb "orange"
set grid
set logscale y
set title "N. of Dom. Sets generated from m-KP using Nemhauser-Ullman Algorithm"
set xlabel "Size of instance"
set ylabel "N. of dom. sets"
plot 2.0**x+10 ls 1 t "1.6^x", \
 "$data" u 1:5 ls 2 title "m=5", \
 "$data" u 1:4 ls 3 title "m=4", \
 "$data" u 1:3 ls 4 title "m=3", \
 "$data" u 1:2 ls 5  title "m=2"
!
