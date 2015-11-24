all: rubber clean
rubber:
	rubber -pd main
pdf:
	latex main
	bibtex main
	latex main
	latex main
	dvips main.dvi
	ps2pdf main.ps
clean:
	rubber -p --clean main
	rm -f main-blx.bib main.run.xml main.loa main.spl

