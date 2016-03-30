all: ptex pdf
ptex:
	python doc.py names.txt > doc.tex
pdf: xelatex
xelatex:
	xelatex doc.tex
pdflatex:
	pdflatex doc.tex
latex:
	latex doc.tex
	dvips doc.dvi
	ps2pdf doc.ps
rubber:
	cat doc.tex | rubber-pipe -d > doc.pdf
	rm -f rubtmp*
