# Script para execução dos testes em batch.
#
#

# Parâmetros
as=`seq 25 25 100`
ys=`seq 3 1 11`
ts=`seq 0.0 0.2 1.0`
ss=`seq 1 1 100`

TMPDIR=`mktemp -d`        # Pasta temportária
SOLDIR="solutions"        # Pasta para gravar as soluções
OUTCSV="result.csv"       # Arquivo csv com os resultados
LOG="batch.log"           # Arquivo de log de execução

rm -rf $SOLDIR
mkdir $SOLDIR

echo "Salvando resultados em $OUTCSV, soluções em $SOLDIR/ e log em $LOG." | tee -a $LOG

echo "\"acts\";\"years\";\"tir\";\"seed\";\"obj\";\"time\"" > $OUTCSV
for a in $as do
	for y in $ys do
		for t in $ts do
			for s in $ss do
				# Definindo nome dos arquivos
				input=$a/$y/$t/$s.zpl
				outprefix=$a-$y-$t-$s

				# Execução
				./scip -f $input > $TMPDIR/scip.log

				# Salvando as soluções
				./scip2sol < $TMPDIR/scip.log > solutions/$outprefix.sol
				./scip2obj < $TMPDIR/scip.log > solutions/$outprefix.obj

				# Escrevendo no CSV de resultados
				obj=`./scip2obj < $TMPDIR/scip.log`        # Tempo
				tim=`./scip2time < $TMPDIR/scip.log`       # Qualidade de solução
				echo $a;$y;$t;$s;$obj;$tim >> $OUTPUT

				echo `date`": Done with $a $y $t $s" >> $LOG
			done
		done
	done
done

tar -zcvf solutions.tar.gz $SOLDIR/*
rm -rf $TMPDIR

