GEN="generator"
OUTDIR=$1

echo "Outdir: $1"

as="100 75 50 25"                          # actions
ys="11 10 9 8 7 6 5 4 3"                   # years
#ps="1 4 6 12"                             # periods
ts="0.0 0.2 0.4 0.6 0.8 1.0"               # tir
rs="5 2 1"                                 # resources
ss=`seq -f %02.f 0 1 99`                   # seed

for y in $ys
do
    for a in $as
    do
        for r in $rs
        do
            for t in $ts
            do
		DIR=$OUTDIR/$y/$a/$r/$t
                mkdir -p $DIR
                for s in $ss
                do
                    $GEN $a $y 1 $r $s 0.15 1.0 $t > $DIR/$s.txt
                done
            done
        done
    done
done

