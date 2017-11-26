#!/bin/bash
tempfprefix=1442351324
tempf=$tempfprefix.tex
for f in `ls *.tex`
do
	basen=${f%.*}
	rm -f $tempfprefix*
	echo "\documentclass{standalone}" >> $tempf
	echo "\usepackage{pgfplots}" >> $tempf
	echo "\input{../../predefs.tex}" >> $tempf
	echo "\begin{document}" >> $tempf
	echo "\input{${f}}" >> $tempf
	echo "\end{document}" >> $tempf
	pdflatex $tempf
	mv $tempfprefix.pdf ${basen}.pdf
	pdf2ps ${basen}.pdf
done
rm -f $tempfprefix*
