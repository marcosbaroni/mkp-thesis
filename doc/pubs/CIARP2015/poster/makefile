all: pdf
imgs:
	inkscape -A ciarp-logo.pdf ciarp-logo.svg
	#inkscape -p ciarp-logo.ps ciarp-logo.svg
pdf:
	cat poster.tex | rubber-pipe -d > poster.pdf
	rm -f rubtmp*
