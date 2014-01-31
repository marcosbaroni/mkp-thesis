all: pdf zpl clean
pdf:
	rubber -d dmkp.tex
zpl:
	enscript ../../mip/knap-v3.zpl -Epascal -2 -p knap-v3.ps
	ps2pdf knap-v3.ps
clean:
	rm -f *.aux *.log *.ps
	
