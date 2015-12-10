all: imgs pdf
imgs:
	inkscape seq.svg -A seq.pdf -P seq.ps
	inkscape circ.svg -A circ.pdf -P circ.ps
pdf:
	cat provafinal.tex | rubber-pipe -d > provafinal.pdf
