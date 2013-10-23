GEN="python Generator.py "
OUTDIR=$1

echo "Outdir: $1"

rm -rf $OUTDIR
mkdir -p $OUTDIR

for y in 3 4 5 6 7 8 9 10 11
do
	mkdir -p $OUTDIR/$y
	for a in 25 50 75 100
	do
		mkdir -p $OUTDIR/$y/$a
		for t in 0.0 0.2 0.4 0.6 0.8 1.0
		do
			mkdir -p $OUTDIR/$y/$a/$t
			for i in `seq 1 100`
			do
				$GEN $y $a $t $i > $OUTDIR/$y/$a/$t/$i.dat
				#echo $y $a $tir $i $OUTDIR/$y-$a-$t-$i.dat
			done
		done
	done
done

