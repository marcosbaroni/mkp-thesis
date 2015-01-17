#!/bin/bash

if [ $# -lt 2 ] ; then
  echo "usage $0 <dat file> <output file> [term]"
  exit 1
fi

term=pdf
if [ $# -gt 2 ] ; then
  term=$3
fi

input=$1
output=$2

gnuplot <<!
set term $term enhanced
set grid
set xrange [0:100]
set output "${output%.*}.$term"
plot '$input' u 1:2 with lines lw 3 t "Gap from best know solution"
!

