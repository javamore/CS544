BASEPATH = /usr/local/apps/tex_live/current/bin/x86_64-linux/
LATEX	= $(BASEPATH)/latex -shell-escape
BIBTEX	= $(BASEPATH)/bibtex
GLOSSARIES = $(BASEPATH)/makeglossaries
DVIPS	= $(BASEPATH)/dvips
DVIPDF  = $(BASEPATH)/dvipdfm
XDVI	= $(BASEPATH)/xdvi -gamma 4
GH	= $(BASEPATH)/gv

EXAMPLES = $(wildcard *.h)
SRC	:= $(shell egrep -l '^[^%]*\\begin\{document\}' *.tex)
TRG	= $(SRC:%.tex=%.dvi)
PSF	= $(SRC:%.tex=%.ps)
PDF	= $(SRC:%.tex=%.pdf)

pdf: $(PDF)

ps: $(PSF)

$(TRG): %.dvi: %.tex $(EXAMPLES)
	$(LATEX) $<
	$(BIBTEX) $(<:%.tex=%)
	- $(GLOSSARIES) $(<:%.tex=%)
	$(LATEX) $<
	$(LATEX) $<

$(PSF): %.ps: %.dvi
	$(DVIPS) -R -Poutline $< -o $@
	# $(DVIPS) -R -Poutline -t letter $< -o $@

$(PDF): %.pdf: %.ps
#	$(DVIPDF) -o $@ $<
	ps2pdf $<

show: $(TRG)
	@for i in $(TRG) ; do $(XDVI) $$i & done

showps: $(PSF)
	@for i in $(PSF) ; do $(GH) $$i & done

clean:
	rm *.aux *.bib *.out *.log *.xml *.bbl *.blg *.dvi *.ps


default: pdf
