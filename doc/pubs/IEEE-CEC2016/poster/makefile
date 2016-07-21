all: pdf clean
pdf: pdflatex
pdflatex:
	pdflatex poster.tex
	bibtex poster.aux
	pdflatex poster.tex
	pdflatex poster.tex
	cp poster.pdf poster1.pdf
rubber:
	cat poster.tex | rubber-pipe -d > poster.pdf
clean:
	rm -f rubtmp*
