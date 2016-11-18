#!/bin/bash

# Thu Nov 17 22:57:42 BRST 2016
# Script batch para executar teste computacional para avaliação
#  de desempenho do uso da KD-tree no algoritmo de Nemhauser-Ullman
#  para o problema da mochila multidimensional bi objetivo

# EXECUTABLE
MOKP=/home/mbaroni/Computacao/phd/dmkp/bin/mokp

# PARAMETERS
cls="2 3 4"
ndims="0 1 2 3"
nits=(25 34 22 15)
k=50

# temporary file
temp=`mktemp`
t=1`date +%N | cut -c 1,8,6,2`

# each classe
for c in $cls
do
    # nitem range
    let ci=$c-1
    maxn=${nits[$ci]}
    let minn=$maxn-10
    # each nitem
    for n in `seq $minn $maxn`
    do
        # seed
        for i in `seq $k`
        do
            # each indexing dimension
            for d in $ndims
            do
                let s=$i+$t
                $MOKP rand $n 2 $c $s > $temp
                res=`$MOKP dynprog $d $temp`
                echo $n";"$c";"$d";"$i";"$res
            done
        done
    done
done

# delete temporary file
rm $temp
