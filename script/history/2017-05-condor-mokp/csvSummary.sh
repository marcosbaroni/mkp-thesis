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
    dim,
    c,
    np,
    n,
    avg(nd),
    avg(ncomp),
    avg(time),
    avg(maxnd),
    count(*) qtd
FROM mokp
GROUP BY dim, c, np, n
ORDER BY c ASC, np ASC, n ASC, dim ASC;
!

rm $db_arq $csv_arq
