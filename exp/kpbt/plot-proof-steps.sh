#!/bin/bash

data1=$1
data2=$2
term=svg

gnuplot <<!
# PLOTING CURVE OF PROOF
set term $term
set datafile separator ";"
set output "proof.$term"
set style data linespoints
set grid
set logscale y
set title "KP Proof Steps"
set xlabel "N. of items"
set ylabel "Steps"
set yrange [1:1e+16]
plot "$data1" u 1:2 title "Avg. of 1000", 2**x t "2^x", 1.76**(x+3) t "1.76^(x+3)"
# PLOTING CURVE OF FINDING
set term $term
set datafile separator ";"
set output "find.$term"
set style data linespoints
set grid
set logscale y
set title "KP Proof Steps"
set xlabel "N. of items"
set ylabel "Steps"
set yrange [1:1e+16]
plot "$data2" u 1:2 title "Avg. of 1000", 2**x t "2^x", 1.55**(x-4) t "1.55^(x-4)"
!



# gnuplot <<!
# set terminal pngcairo  transparent font "arial,10" size 600, 400 
# set datafile separator ";"
# set output '$output'
# set key inside right top vertical Right noreverse enhanced autotitle box linecolor -1 linewidth 1.000
# set style data lines
# set title "KP Proof Steps" 
# set xlabel "N. of items" 
# set ylabel "Steps" 
# set yrange [0:*]
# plot "$data" u 1:2:3 t "Power" w yerrorbars
# !
