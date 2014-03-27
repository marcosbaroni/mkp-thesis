#!/bin/bash
DIR=$1
SUMMARY="../gurobi2summary"
RESULT="result.csv"

cd $DIR
rm -rf $RESULT
for n in ` ls *.lp`
do
	echo -n ${n/.lp/}";" >> $RESULT
	gurobi_cl TimeLimit=60 $n | $SUMMARY >> $RESULT
done

