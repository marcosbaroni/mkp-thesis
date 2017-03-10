TARGET=paper
TDIR=tex

all: pdf clean
pdf: pdflatex
pdflatex:
	cd $(TDIR); pdflatex $(TARGET).tex
	mv $(TDIR)/$(TARGET).pdf .
clean:
	cd $(TDIR); rm -f *.aux *.log

