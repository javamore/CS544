tex = proj4_writeup

INCLUDES =

SOURCE = ${tex}.tex

default: pdf


dvi: ${tex}.tex
	latex ${tex}.tex


ps: dvi
	dvips -R -Poutline -t letter ${tex}.dvi -o ${tex}.ps

pdf: ps
	ps2pdf ${tex}.ps

clean-dvi:
	rm -f ${tex}.aux
	rm -f ${tex}.log
	rm -f ${tex}.dvi
	rm -f ${tex}.out
	rm -f ${tex}.ps

clean-ps: clean-dvi
	rm -f ${TARGET}.ps

clean-pdf: clean-ps
	rm -f ${tex}.pdf

clean: clean-pdf