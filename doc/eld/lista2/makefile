all: img pdf clean
img:
	inkscape -P cont1.ps cont1.svg
	inkscape -A cont1.pdf cont1.svg
pdf:
	cat lista2.tex | rubber-pipe -d > lista2.pdf
clean:
	rm -f rubtmp*

