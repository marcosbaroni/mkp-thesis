#!/bin/bash
ROOTD=/home/mbaroni/Computacao/phd/dmkp
INSTPATH=$ROOTD/insts/bazgan/Bazgan_Insts/Type_A  # <n>_items/2KP100-TA-<i>.txt
BIN=$ROOTD/bin/mokp

function bazexec(){
    n=`echo $@ | cut -f1 -d-`
    i=`echo $@ | cut -f2 -d-`
    inst=$INSTPATH/${n}_items/2KP${n}-TA-${i}.txt
    ans="$n;$i;"`$BIN bazgan 3 $inst`
    echo $ans
}
export BIN
export INSTPATH
export ROOTD
export bazexec

# executing
#parallel $BIN bazgan 3 <<!
source `which env_parallel.bash`
env_parallel bazexec <<!
`for n in 100 200 300 400 500 600 700
do
    for i in 0 1 2 3 4 5 6 7 8 9
    do
        echo $n-$i
    done
done`
!
