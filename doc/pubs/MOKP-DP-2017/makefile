TARGET=paper

all: pdf clean
pdf: dvipdf
pdflatex: latex
	pdflatex $(TARGET).tex
dvipdf: latex
	dvipdf $(TARGET).dvi
	#ps2pdf $(TARGET).ps
latex:
	latex $(TARGET).tex
	bibtex $(TARGET).aux
	latex $(TARGET).tex
	latex $(TARGET).tex
clean:
	rm -f *.aux *.log *.bbl *.blg *.dvi *.ps

