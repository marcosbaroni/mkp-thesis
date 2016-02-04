#!/bin/bash
IDIR=/home/mbaroni/Computacao/dmkp/insts/gk/
SFL=/home/mbaroni/Computacao/dmkp/bin/mkp-sfl

fin=$IDIR/gk$1.dat
mkp2zpl $fin | zpl2lp | runscip - 1800 > ~/Desktop/gk-scip-$1.txt

