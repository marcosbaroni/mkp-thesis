all: pdf clean
pdf:
	cat prova2a.tex | rubber-pipe -d > prova2a.pdf
clean:
	rm -rf rubtmp*
figs:
	inkscape mem.svg -E mem.eps
	inkscape mem.svg -A mem.pdf
	inkscape mux4.svg -E mux4.eps
	inkscape mux4.svg -A mux4.pdf
