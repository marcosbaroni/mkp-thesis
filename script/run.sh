outfile=result.csv
maxsecs="300.0"
scip_exe=./scip
instancetar=inst-23-10-2013.tar.gz
model=../knap.zpl
filelist=files.txt
tmpdir=`mktemp -d`

run_scip(){
	$scip_exe -f $tmpdir/knap.zpl | grep " solving" | head -1 | awk '{ print $NF}'
}

mytimeout(){
	ret=`timeout ${maxsecs}s run_scip`
	if [[ -n $ret ]]; then
    	echo $ret
	else
    	echo ${maxsecs} "(aborted)"
	fi
}

# selecting instance
fi=`sed -n $1 $filelist`

# argument line for result record
as=`echo $fi |sed 's/.dat//'|sed 's/-/;/g'`

# extracting instance
tar -zxf $instancetar $fi

# moving files
mv $fi $tmpdir/knap.dat
cp $model $tmpdir/knap.zpl

# solving
#res=`./mytimeout.sh ${secs}s "${secs}.0 (aborted)" run_scip`
#echo $as;$res

#cleaning
#rm /export/knap.dat
#rm /export/knap.zpl

