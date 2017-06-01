#!/bin/bash
CSV2SQL=/home/mbaroni/Computacao/phd/dmkp/bin/csv2sqlite3

if [ $# -lt 1 ]; then
    # TODO: print usage
    echo "usage: $0 <csv file>"
    exit 1
fi

# CREATE TABLE mokp (dim int, c int, np int, n int, i int, nd int, ncomp int, time float, maxnd int);
res_arq=$1
db_arq=`mktemp`
csv_arq=`mktemp`
echo "mokp" > $csv_arq
echo "dim;c;np;n;i;nd;ncomp;time;maxnd;qtd" >> $csv_arq
echo "int;int;int;int;int;int;int;float;int" >> $csv_arq
cat $res_arq >> $csv_arq
$CSV2SQL $csv_arq $db_arq
sqlite3 $db_arq <<!
SELECT
    -- PARAMS --
    c,
    np,
    n,
    -- TIME --
    (select avg(time) from mokp as m2 where dim = 0 AND m1.n = m2.n AND m1.c = m2.c AND m1.np = m2.np) as time0,
    (select avg(time) from mokp as m2 where dim = 3 AND m1.n = m2.n AND m1.c = m2.c AND m1.np = m2.np) as time3,
    -- COMP --
    (select avg(ncomp) from mokp as m2 where dim = 0 AND m1.n = m2.n AND m1.c = m2.c AND m1.np = m2.np) as comp0,
    (select avg(ncomp) from mokp as m2 where dim = 3 AND m1.n = m2.n AND m1.c = m2.c AND m1.np = m2.np) as comp3,
    -- |ND| --
    avg(nd),
    -- MAXND --
    avg(maxnd),
    -- COUNTS --
    (select count(*) from mokp as m2 where dim = 0 AND m1.n = m2.n AND m1.c = m2.c AND m1.np = m2.np) as c0,
    (select count(*) from mokp as m2 where dim = 3 AND m1.n = m2.n AND m1.c = m2.c AND m1.np = m2.np) as c3
FROM mokp as m1
GROUP BY c, np, n
ORDER BY c ASC, np ASC, n ASC;
!

rm $db_arq $csv_arq

