#!/bin/bash

MOKP=../bin/mokp
DEST=/tmp/mokps

# CLASSE A
for np in 3
do
    for n in 10 30 50 70 90 110
    do
        mkdir -p $DEST/A/$np/$n
        for i in 0 1 2 3 4 5 6 7 8 9
        do
            $MOKP rand $n $np 1 $i > $DEST/A/$np/$n/$i.txt
        done
    done
done

# CLASSE B
for np in 3
do
    for n in 10 20 30 40 50 60
    do
        mkdir -p $DEST/B/$np/$n
        for i in 0 1 2 3 4 5 6 7 8 9
        do
            $MOKP rand $n $np 2 $i > $DEST/B/$np/$n/$i.txt
        done
    done
done
