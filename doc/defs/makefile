all: pdf zpl clean
pdf:
	rubber -d dmkp.tex
zpl:
	enscript ../../mip/scip/knap-v3.zpl --color=1 -Ematlab -2 -p knap-v3.ps
	ps2pdf knap-v3.ps
clean:
	rm -f *.aux *.log *.ps
	
