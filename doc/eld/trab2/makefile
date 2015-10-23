all: graphs pdf clear
pdf:
	cat trab2.tex | rubber-pipe -d > trab2.pdf
graphs: seq.img sim1.img sim2.img seq_b.img
%.img : figs/%.svg
	inkscape -P figs/$*.ps -A figs/$*.pdf $<
clear:
	rm -f rubtmp*
