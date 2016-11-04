#!/bin/bash
BINDIR=../../bin
temp=`tempfile`
niter=500
outfile=results/`hostname`.csv
seed=$1

for n in 100 250 500
do
  for m in 10 20 30 
  do
    for t in 0.25 0.5 0.75
	do
	  ./mkp-rand $n $m $t > $temp.mkp
	  scipres=`./mkp2zpl $temp.mkp | ./zpl2lp | ./runscip | ./scip2summary`
      for nmeme in 10 20 30
	  do
	    for smeme in 10 20 30
		do
		  for ssubmeme in 3 5 10
		  do
		    for subniter in 20 50 100
			do
			  for cross in 1 2 3
			  do
			    for newsol in 1 2
				do
				  echo mkp-sfl $temp.mkp $nmeme $smeme $ssubmeme $niter $subniter $cross $newsol $seed
				  ./mkp-sfl $temp.mkp $nmeme $smeme $ssubmeme $niter $subniter $cross $newsol $seed
				  #sfl=`./mkp-sfl $temp.mkp $nmeme $smeme $ssubmeme $niter $subniter $cross $newsol $seed`
				  echo $n";"$m";"$t";"$nmeme";"$smeme";"$ssubmeme";"$subniter";"$cross";"$newsol";"$scipres";"$sfl >> $outfile
				done
			  done
			done
		  done
		done
	  done
    done
  done
done
rm $temp

