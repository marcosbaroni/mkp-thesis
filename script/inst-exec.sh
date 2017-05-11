#!/bin/bash
LINE_PARSER="python bazgan-line-getter.py"
BIN_FOLDER="/home/mbaroni/Computacao/phd/dmkp/bin"
MOKP="$BIN_FOLDER/mokp"

function arg_spliter(){
    np=$1
    n=$2
    i=$3
    dim=$4
}

nline=$1
args=`$LINE_PARSER $nline`
arg_spliter $args
resposta=`$MOKP rand $n $np 1 $i|$MOKP bazgan $dim`
echo "$dim;$np;$n;$i;$resposta"

