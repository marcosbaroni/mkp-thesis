#!/bin/bash
BIN_FOLDER="/home/mbaroni/Computacao/phd/dmkp/bin"
MOKP="$BIN_FOLDER/mokp"
OUT="insts"

nsA="10 30 50 70 90 110"
nsC="10 20 30 40 50 60"
nps="2 3 4 5"
is="0 1 2 3 4 5 6 7 8 9"

for n in $nsA
do
    for np in $nps
    do
        for i in $is
        do
            $MOKP rand $n $np 1 $i > $OUT/1-$np-$n-$i.txt
        done
    done
done

for n in $nsC
do
    for np in $nps
    do
        for i in $is
        do
            $MOKP rand $n $np 3 $i > $OUT/3-$np-$n-$i.txt
        done
    done
done

