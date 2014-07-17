#!/bin/bash
###############################################
# Script used to track transition phase of MMKP
#  using a MIP solver.
# author: Marcos Daniel Baroni
# date: Qui Jul 17 17:23:38 BRT 2014
###############################################
BINDIR="../../bin"
RANDINST=$BINDIR/randmmkp
MIP=scip
RUNMIP=$BINDIR/run$MIP
MIPSUMMARY=$BINDIR/$MIP"2summary"
MMKP2ZPL=$BINDIR/mmkp2zpl
ZPL2LP=$BINDIR/zpl2lp

# parameters
as="0.2 0.4 0.5 0.6 0.8"    # beta
ns="100 250 500"            # itens
ms="5 10 30"                # contraints
os="1 2 5 10"               # knapsacks
ts="10 20 30 60"            # time(s)
ss=`seq 1 10`               # seed

OUTPUT=result.csv
TMP=`tempfile`

for a in $as ; do
	for n in $ns ; do
		for m in $ms ; do
			for o in $os ; do
				for t in $ts ; do
					$RANDINST $n $m $o $a | $MMKP2ZPL | $ZPL2LP > $TMP.lp
					echo -n "$t;" >> $OUTPUT
					cat $TMP.lp | $RUNMIP $t | $MIPSUMMARY >> $OUTPUT
					echo "$n $m $o $t $a"
				done
			done
		done
	done
done
