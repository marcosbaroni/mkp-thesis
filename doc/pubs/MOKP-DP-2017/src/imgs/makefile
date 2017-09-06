all: imgs clean
imgs: dom domkd
dom:
	latex dom.tex
	dvips dom.dvi
	ps2pdf dom.ps
	pdfcrop dom.pdf dom.pdf
domkd:
	latex dom-kd.tex
	dvips dom-kd.dvi
	ps2pdf dom-kd.ps
	pdfcrop dom-kd.pdf dom-kd.pdf
clean:
	rm -f *.log *.aux
