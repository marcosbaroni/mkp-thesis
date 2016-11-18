#!/bin/bash

# Thu Nov 17 22:57:42 BRST 2016
# Script batch para executar teste computacional para avaliação
#  de desempenho do uso da KD-tree no algoritmo de Nemhauser-Ullman
#  para o problema da mochila multidimensional bi objetivo

# EXECUTABLE
MOKP=/home/mbaroni/Computacao/phd/dmkp/bin/mokp

# PARAMETERS
#classes
cls="3"
ms="3 4 5"
ndims="0 1 2 3 4"
nits=(19)
k=50

# temporary file
temp=`mktemp`
t=1`date +%N | cut -c 1,8,6,2`

# for each M (dimension of problem)
for m in $ms
do
    # each classe
    for c in $cls
    do
        # nitem range
        let ci=$c-1
        maxn=${nits}
        let minn=$maxn-8
        # each nitem
        for n in `seq $minn $maxn`
        do
            # seed
            for i in `seq $k`
            do
                let s=$i+$t
                $MOKP rand $n $m $c $s > $temp
                # each indexing dimension
                let m2=$m+1
                for d in `seq 0 $m2`
                do
                    res=`$MOKP dynprog $d $temp`
                    echo $n";"$m";"$c";"$d";"$i";"$res
                done
            done
        done
    done
done

# delete temporary file
rm $temp
