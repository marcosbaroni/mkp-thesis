TARGET=paper
TDIR=tex

all: pdf clean
pdf: pdflatex
pdflatex:
	latex $(TARGET).tex
	bibtex $(TARGET).aux
	latex $(TARGET).tex
	pdflatex $(TARGET).tex
clean:
	rm -f *.aux *.log *.bbl *.blg *.dvi

