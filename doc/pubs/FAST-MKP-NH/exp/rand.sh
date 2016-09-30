#/bin/bash
RT=/home/mbaroni/Computacao/phd/dmkp
INSTDIR=$RT/insts/chubeas
BINDIR=$RT/bin
SCIP=$BINDIR/scip
MKP2ZPL=$BINDIR/mkp2zpl
ZPL2LP=$BINDIR/zpl2lp
RUNSCIP=$BINDIR/runscip
SCIP2SUMMARY=$BINDIR/scip2summary
MKPNU=$BINDIR/mkp-nu
MKPRAND=$BINDIR/mkp-rand
PATH=$PATH:$BINDIR

if [[ $# -lt 3 ]]
then
    echo "usage: $0 <times each> <k> <out.csv>"
    exit 1
fi

ms="2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20"
tmpinst=`mktemp`
ts=$1
k=$2
outcsv=$3
for m in $ms; do
    (>&2 echo "m: ${m} 1...${ts}")
    for i in `seq 1 $ts`; do
        $MKPRAND 100 $m > $tmpinst
        ans="${m}"";"`$MKPNU 1 $tmpinst $k`";"`$MKPNU 3 $tmpinst $k`
        echo $ans >> $outcsv
    done
done
rm $tmpinst

