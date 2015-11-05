all: pdf
imgs:
	inkscape -A ciarp-logo.pdf ciarp-logo.svg
	inkscape -A alg-new.pdf alg-new.svg
	inkscape -A alg-cross.pdf alg-cross.svg
	inkscape -A flow1.pdf flow1.svg
	inkscape -A flow1a.pdf flow1a.svg
	inkscape -A flow2.pdf flow2.svg
	#inkscape -p ciarp-logo.ps ciarp-logo.svg
pdf:
	cat poster.tex | rubber-pipe -d > poster.pdf
	rm -f rubtmp*
