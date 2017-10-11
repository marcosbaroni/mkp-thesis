#!/bin/bash
LINE_PARSER="python bazgan-line-getter.py"
#BIN_FOLDER="/home/mbaroni/Computacao/phd/dmkp/bin"
BIN_FOLDER="./"
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
    cls=$2
    n=$3
    i=$4
    ndim=$5
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
resp=`$MOKP rand $n $np $cls $i|$MOKP bazgan $ndim`

# outputing
echo "$cls;$ndim;$np;$n;$i;$resp" >> $outputfile

