#!/bin/bash
CSV2SQL=/home/mbaroni/Computacao/phd/dmkp/bin/csv2sqlite3

if [ $# -lt 3 ]; then
    # TODO: print usage
    echo "usage: $0 <csv file> <np> <classe> [pretty=0]"
    exit 1
fi

res_arq=$1
np=$2
c=$3
pretty_str=".mode csv"


# Deciding dim analized
if [ $np -eq "2" ]; then
    baz_dim=1
else
    baz_dim=0
fi
kdt_dim=3
# Deciding pretty
if [ $# -gt 3 ]; then
    if [ $4 -eq 1 ]; then
        pretty_str=".mode column"
    fi
fi


# Building data file
db_arq=`mktemp`
csv_arq=`mktemp`
#  -- CREATE TABLE mokp (dim int, c int, np int, n int, i int, nd int, ncomp int, time float, maxnd int);
echo "mokp" > $csv_arq
echo "dim;c;np;n;i;nd;ncomp;time;maxnd;qtd" >> $csv_arq
echo "int;int;int;int;int;int;int;float;int" >> $csv_arq
cat $res_arq >> $csv_arq
$CSV2SQL $csv_arq $db_arq


# Quering
sqlite3 $db_arq <<!
$pretty_str
.header on
.separator ';'
SELECT
    n,
    printf("%.3f", time) as time,
    printf("%.3f", time2) as time2,
    printf("%.3f", (time+0.0001)/(time2+0.0001)) as "(spd_up)",
    printf("%.3f", comp) as comp,
    printf("%.3f", comp2) as comp2,
    printf("%.3f", (comp/comp2)) as "(cmp_up)",
    nd,
    maxnd,
    qtd, qtd2
FROM
    (SELECT
        -- PARAMS --
        n,
        -- TIME --
        (select avg(time) from mokp as m2 where dim = $baz_dim AND m1.n = m2.n AND m2.c = $c AND m2.np = $np) as time,
        (select avg(time) from mokp as m2 where dim = $kdt_dim AND m1.n = m2.n AND m2.c = $c AND m2.np = $np) as time2,
        -- COMP --
        (select avg(ncomp) from mokp as m2 where dim = $baz_dim AND m1.n = m2.n AND m2.c = $c AND m1.np = $np) as comp,
        (select avg(ncomp) from mokp as m2 where dim = $kdt_dim AND m1.n = m2.n AND m2.c = $c AND m1.np = $np) as comp2,
        -- |ND| --
        avg(nd) as nd,
        -- MAXND --
        avg(maxnd) as maxnd,
        -- COUNTS --
        (select count(*) from mokp as m2 where dim = $baz_dim AND m1.n = m2.n AND m2.c = $c AND m2.np = $np) as qtd,
        (select count(*) from mokp as m2 where dim = $kdt_dim AND m1.n = m2.n AND m2.c = $c AND m2.np = $np) as qtd2
    FROM mokp as m1
    WHERE m1.np = $np
      AND m1.c = $c
    GROUP BY n
    ORDER BY n ASC);
!


#Cleaning
rm $db_arq $csv_arq

