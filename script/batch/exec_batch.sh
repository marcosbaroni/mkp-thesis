#!/bin/bash
# Script para execução dos testes em batch.

### Parâmetros
as="100 75 50 25"
ys="11 10 9 8 7 6 5 4 3"
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
SCIPOUT=$TMPDIR/scip.out

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
                    pref=$INSTDIR/$y/$a/$r/$t/$s
                    JSON=$pref.json
                    #$CONVERT $json > $INPUTZPL
                    ./converter < $pref.txt > $INPUTZPL
    
                    # Execução
                    ./scip -f $INPUTZPL > $SCIPOUT
                    # Salvando as soluções
                    #./scip2sol < $TMPDIR/scip.out > solutions/$outprefix.sol
    
                    # Extraindo info da execução
                    obj=`./scip2obj < $SCIPOUT`        # Qualidade
                    gap=`./scip2gap < $SCIPOUT`        # Gap
                    tim=`./scip2time < $SCIPOUT`       # Tempo
    
                    # Escrevendo no CSV de resultados
                    echo "$y;$a;$r;$t;$s;$obj;$tim;$gap" >> $OUTCSV
                done
            done
        done
    done
done

#tar -zcvf solutions.tar.gz $SOLDIR/*
rm -rf $TMPDIR

