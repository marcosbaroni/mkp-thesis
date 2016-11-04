#!/bin/bash

# EXECUTABLE
MOKP=/home/mbaroni/Computacao/phd/dmkp/bin/mokp

# PARAMETERS
cls="1 2 3 4"
ndims="0 1 2 3"
nits=(28 37 25 18)
k=25

# TEMPORARY FILE
temp=`mktemp`
t=1`date +%N | cut -c 1,8,6,2`

# EACH CLASSE
for c in $cls
do
    # nitem range
    let ci=$c-1
    maxn=${nits[$ci]}
    let minn=$maxn-10
    # EACH NITEM
    for n in `seq $minn $maxn`
    do
        # SEED
        for i in `seq $k`
        do
            # EACH INDEXING DIMENSION
            for d in $ndims
            do
                let s=$i+$t
                $MOKP rand $n 2 $c $s > $temp
                res=`$MOKP dynprog $d $temp`
                echo $n";"$c";"$d";"$i";"$res
            done
        done
    done
done

# delete temporary file
rm $temp
