# Script para execução dos testes em batch.
#
#

# Parâmetros
as="25 50 75 100"
ys="3 4 5 6 7 8 9 10 11"
ts="0.0 0.2 0.4 0.6 0.8 1.0"
ss=`seq -f %02.f 0 1 99`

TMPDIR=`mktemp -d`        # Pasta temportária
SOLDIR="solutions"        # Pasta para gravar as soluções
INSTDIR=$1

OUTCSV="result.csv"       # Arquivo csv com os resultados
LOG="batch.log"           # Arquivo de log de execução

rm -rf $SOLDIR
mkdir $SOLDIR

echo "Salvando resultados em $OUTCSV, soluções em $SOLDIR/ e log em $LOG." | tee -a $LOG

echo "\"acts\";\"years\";\"tir\";\"seed\";\"obj\";\"time\";\"gap\"" > $OUTCSV
for a in $as do
	for y in $ys do
		for t in $ts do
			for s in $ss do
				# Definindo nome dos arquivos
				input=$INSTDIR/$y/$a/$t/$s.zpl
				outprefix=$a-$y-$t-$s

				# Execução
				./scip -f $input > $TMPDIR/scip.log

				# Salvando as soluções
				./scip2sol < $TMPDIR/scip.log > solutions/$outprefix.sol
				./scip2obj < $TMPDIR/scip.log > solutions/$outprefix.obj
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

