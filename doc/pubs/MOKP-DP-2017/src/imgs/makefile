#!/bin/bash

# pstriks file
PSTFILES="dom dom-kd"

# produce
for f in $PSTFILES; do
	latex $f.tex
	dvips $f.dvi
	ps2pdf $f.ps
	pdfcrop $f.pdf $f.pdf
	pdftops $f.pdf
done

# clear
rm -f *.log *.aux
