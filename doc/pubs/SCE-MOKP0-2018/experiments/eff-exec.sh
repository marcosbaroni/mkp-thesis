#!/bin/bash
wdir=/tmp/testes
line=$2
file=`ls ${wdir}/insts/*.baz|tail -n +$line|head -1`
${wdir}/lpaquete $file > ${file%.*}.eff

