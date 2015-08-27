all: pdf clean
pdf:
	cat paper.tex | rubber-pipe -s -d > paper.pdf
clean:
	rm -f rubtmp*
