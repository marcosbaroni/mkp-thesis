#!/bin/bash
# Script para execução dos testes em batch.

### Parâmetros
as="100 75 50 25"
ys="11 10 09 08 07 06 05 04 03"
ts="0.0 0.2 0.4 0.6 0.8 1.0"
rs="5 2 1"
ss=`seq -f %02.f 0 1 99`

###     EXECUTAVEIS
CONVERT="java -jar gen.jar -convertToScip"

###     PASTAS
TMPDIR=`mktemp -d`        # Pasta temportária
SOLDIR="solutions"        # Pasta para gravar as soluções
INSTDIR=$1

###     ARQUIVOS
INPUTZPL="dat.zpl"
OUTCSV="result.csv"       # Arquivo csv com os resultados
LOG="batch.log"           # Arquivo de log de execução

#rm -rf $SOLDIR
#mkdir $SOLDIR

#echo "Salvando resultados em $OUTCSV, soluções em $SOLDIR e log em $LOG." | tee -a $LOG

echo "\"years\";\"act\";\"res\";\"tir\";\"seed\";\"obj\";\"time\";\"gap\"" > $OUTCSV
for y in $ys ; do
	for a in $as ; do
		for r in $rs ; do
			for t in $ts ; do
				for s in $ss ; do
					# Definindo nome dos arquivos
					json=$INSTDIR/$y/$a/$r/$t/$s.json
					$CONVERT $json > $INPUTZPL
	
					# Execução
					./scip -f $INPUTZPL > $TMPDIR/scip.out
					# Salvando as soluções
					#./scip2sol < $TMPDIR/scip.out > solutions/$outprefix.sol
	
					# Extraindo info da execução
					obj=`./scip2obj < $TMPDIR/scip.out`        # Qualidade
					gap=`./scip2gap < $TMPDIR/scip.out`        # Gap
					tim=`./scip2time < $TMPDIR/scip.out`       # Tempo
	
					# Escrevendo no CSV de resultados
					echo "$y;$a;$r;$t;$s;$obj;$tim;$gap" >> $OUTCSV
				done
			done
		done
	done
done

#tar -zcvf solutions.tar.gz $SOLDIR/*
rm -rf $TMPDIR

