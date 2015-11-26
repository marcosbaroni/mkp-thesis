all: pdf clean
pdf:
	cat prova2a.tex | rubber-pipe -d > prova2a.pdf
	cat prova2b.tex | rubber-pipe -d > prova2b.pdf
clean:
	rm -rf rubtmp*
figs:
	inkscape mem.svg -E mem.eps
	inkscape mem.svg -A mem.pdf
	inkscape mux4.svg -E mux4.eps
	inkscape mux4.svg -A mux4.pdf
	inkscape demux4.svg -E demux4.eps
	inkscape demux4.svg -A demux4.pdf
	inkscape cont2.svg -E cont2.eps
	inkscape cont2.svg -A cont2.pdf

