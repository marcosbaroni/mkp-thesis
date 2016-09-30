#!/bin/bash
RT=/home/mbaroni/Computacao/phd/dmkp
BINDIR=$RT/bin
CSV2DB=$BINDIR/csv2sqlite3

if [[ $# -lt 3 ]]
then
    echo "usage $0 <results.csv> <comp fig.png> <time fig.png>"
    exit 1
fi

TMPDIR=`mktemp -d`
tabcsv=$TMPDIR/table.csv
plotdata=$TMPDIR/plot.dat

csv_in=$1
fig_out=$2

function extract(){
    tmpdb=`mktemp`
    tmpcsv=`mktemp`
    echo "mkpnu" > $tmpcsv
    echo "n;m;t;i;nnode1;ncomp1;time1;opt1;nnode2;ncomp2;time2;opt2" >> $tmpcsv
    echo "int;int;float;int;int;int;float;int;int;int;float;int" >> $tmpcsv
    cat $1 >> $tmpcsv
    $CSV2DB $tmpcsv $tmpdb
    sqlite3 $tmpdb <<!
.separator ' '
.output $2
    select m, avg(ncomp2), avg(ncomp1), avg(time2), avg(time1) from mkpnu group by m order by m asc;
!
}

function plotdata(){
    sif="$"
    gnuplot <<!
    # Make the x axis labels easier to read.
    set xtics rotate out
    # Select histogram data
    set style data histogram
    # Give the bars a plain fill pattern, and draw a solid line around them.
    set style fill solid border
    set style histogram clustered

    set grid y
    set auto y
    set style line 12 lc rgb 'black' lt 1 lw 1.5
    set style line 13 lc rgb 'black' lt 0 lw 1
    set grid ytics mytics mytics ls 12, ls 13
    set mytics 4
    set terminal png

    set key left
    set title "Comparacoes - 14 itens"
    set ylabel "N. comparacoes"
    set xlabel "Dimensao do problema"
    set output '$2'
    plot '$1' u 2 title 'KD-Tree', '' u 3 title 'Lista', '' u 0:(0):xticlabel(1) w l title ''

    set title "Tempo - 14 itens"
    set ylabel "tempo (s)"
    set output '$3'
    plot '$1' u 4 title 'KD-tree', '' u 5 title 'Lista', '' u 0:(0):xticlabel(1) w l title ''
!
}

function formatdata(){
    vim -e -s $1 -c "1j|1j|2j|2j|3j|3j|wq"
}

extract $csv_in $plotdata
plotdata $plotdata $2 $3
rm -fr $TMPDIR
