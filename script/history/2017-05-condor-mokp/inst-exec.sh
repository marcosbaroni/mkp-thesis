#!/bin/bash
LINE_PARSER="python bazgan-line-getter.py"
BIN_FOLDER="/home/mbaroni/Computacao/phd/dmkp/bin"
MOKP="$BIN_FOLDER/mokp"
INST_FOLDER="mokp-insts"
OUT_FOLDER="/home/mbaroni/mokp-kd/"
HOST=`hostname`

function arg_spliter(){
    np=$1
    n=$2
    i=$3
    dim=$4
}

cluster=$1
nline=$2

args=`$LINE_PARSER $nline`
arg_spliter $args
inputfile="$INST_FOLDER/$1-$np-$n-$i.txt"
resposta=`$MOKP rand $n $np 1 $i|$MOKP bazgan $dim`
echo "$dim;$np;$n;$i;$resposta" >> $OUT_FOLDER/$HOST.csv

