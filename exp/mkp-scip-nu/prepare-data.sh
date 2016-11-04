#!/bin/bash

################################################################################
# Takes the sqlite3 .db file with data from the experiment and prepares the
# .dat files for plotting.
################################################################################

if [ $# -lt 2 ]; then
	echo -e "usage: $0 <data .db> <output dir>"
	exit 1
fi

data=$1
outdir=$2

sqlite3 $data <<!
.mode csv
.separator ';'
.output $outdir/scip1.dat
select n, avg(time) from scip where m=1 group by n order by n asc;
.output $outdir/scip2.dat
select n, avg(time) from scip where m=2 group by n order by n asc;
.output $outdir/scip3.dat
select n, avg(time) from scip where m=3 group by n order by n asc;
.output $outdir/scip4.dat
select n, avg(time) from scip where m=4 group by n order by n asc;
.output $outdir/nu1.dat
select n, avg(time) from nu where m=1 group by n order by n asc;
.output $outdir/nu2.dat
select n, avg(time) from nu where m=2 group by n order by n asc;
.output $outdir/nu3.dat
select n, avg(time) from nu where m=3 group by n order by n asc;
.output $outdir/nu4.dat
select n, avg(time) from nu where m=4 group by n order by n asc;
.exit
!


