#!/bin/bash

data=$1
output=$2

gnuplot <<!
set term pngcairo
set datafile separator ";"
set output "$output"
set style data linespoints
set grid
set title "KP Proof Steps"
set xlabel "N. of items"
set ylabel "Steps"
set yrange [1:*]
set logscale y
plot "$data" u 1:2 title "Avg. of 1000", 2**(x) t "2^x"
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
