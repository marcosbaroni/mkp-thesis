#!/bin/bash
# Script to copy do a directory all files needed to run the batch test.
#

DESTDIR=$1
BINDIR="../bin"

cp $BINDIR/* $DESTDIR/
cp inst_creator.sh $DESTDIR/
cp batch/exec_batch.sh $DESTDIR/
cp ../mip/knap-v3.zpl $DESTDIR/

