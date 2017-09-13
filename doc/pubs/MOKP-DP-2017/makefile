TARGET=paper
TEXFLAGS=-halt-on-error

all: pdf clean
pdf: dvipdf
pdflatex: latex
	pdflatex $(TARGET).tex
dvipdf: latex
	dvipdf $(TARGET).dvi
	#ps2pdf $(TARGET).ps
latex:
	latex $(TEXFLAGS) $(TARGET).tex
	bibtex $(TARGET).aux
	latex $(TEXFLAGS) $(TARGET).tex
	latex $(TEXFLAGS) $(TARGET).tex
clean:
	rm -f *.aux *.log *.bbl *.blg *.dvi *.ps
