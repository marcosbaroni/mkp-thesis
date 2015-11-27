all: pdf clean
pdf:
	cat prova2a.tex | rubber-pipe -d > prova2a.pdf
	cat prova2b.tex | rubber-pipe -d > prova2b.pdf
	cat prova2b-gab.tex | rubber-pipe -d > prova2b-gab.pdf
clean:
	rm -rf rubtmp*
figs:
	make -C figs

