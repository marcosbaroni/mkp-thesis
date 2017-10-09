#!/bin/bash
LINE_PARSER="python bazgan-line-getter.py"
BIN_FOLDER="/home/mbaroni/Computacao/phd/dmkp/bin"
MOKP="$BIN_FOLDER/mokp"
#INST_FOLDER="mokp-insts"
#OUT_FOLDER="/home/mbaroni/mokp-kd/"
OUT_FOLDER="./"
HOST=`hostname`

if [ "$#" -ne 2 ]; then
	echo "usage $0 <cluster> <job>"
	exit 1
fi

function arg_spliter(){
    np=$1
    n=$2
    i=$3
    dim=$4
}

# gettin params
cluster=$1
nline=$2

# setting up args
args=`$LINE_PARSER $nline`
arg_spliter $args

#setting up files
#inputfile="$INST_FOLDER/$1-$np-$n-$i.txt"
outputfile="$OUT_FOLDER/$cluster-$HOST.csv"

# running
resp=`$MOKP rand $n $np 1 $i|$MOKP bazgan $dim`

# outputing
echo "$dim;$np;$n;$i;$resp" >> $outputfile

