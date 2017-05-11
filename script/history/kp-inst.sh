#!/bin/bash
###############################################################################
# Generate a batch os KP instances
###############################################################################
RANDKP=../bin/randkp

# Checking number of arguments
if [ $# -lt 4 ]; then
	echo "usage.: $0 <n0> <nf> <nseeds> <output dir>"
	exit 1
fi

# Getting arguments
n0=$1
nf=$2
nseed=$3
outdir=$4

# Checking if directory does not exist
if [ ! -d "$outdir" ]; then
  mkdir -p $outdir
fi


for n in `seq -f "%02.f" $n0 $nf`
do
	mkdir $outdir/$n
	for i in `seq -f "%03.f" 1 $nseed`
	do
		$RANDKP $n 0.5 $i > $outdir/$n/$i.txt
	done
done

