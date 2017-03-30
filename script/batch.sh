#!/bin/bash

MOKP=../bin/mokp

function bexec(){
    classe=$1; shift
    np=$1; shift
    n=$1; shift
    for i in $@
    do
        input=/tmp/mokps/$classe/$np/$n/$i.txt
        for d in 0 3
        do
            ans=`$MOKP bazgan $d $input`
            echo "$classe;$np;$n;$i;$d;$ans"
        done
    done
}

bexec A 3 10 $@
bexec A 3 30 $@
bexec A 3 50 $@
bexec A 3 70 $@
bexec A 3 90 $@
bexec A 3 110 $@
bexec B 3 10 $@
bexec B 3 20 $@
bexec B 3 30 $@
bexec B 3 40 $@
bexec B 3 50 $@
bexec B 3 60 $@
