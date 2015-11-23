CC=gcc
LATEXC=pdflatex
DOCC=doxygen
CFLAGS=-g -Wall
CFLAGS1=-g -Wall -std=c99

REFDIR=.
SRCDIR=$(REFDIR)/src
BINDIR=$(REFDIR)/bin
DOCDIR=$(REFDIR)/doc
REPORTDIR=$(REFDIR)/rapport

LATEXSOURCE=$(wildcard $(REPORTDIR)/*.tex)
CSOURCE=$(wildcard $(SRCDIR)/applyPatch.c )
CSOURCE1=$(wildcard $(SRCDIR)/treatFiles.c)
CSOURCE2=$(wildcard $(SRCDIR)/findPatch.c)
CSOURCE3=$(wildcard $(SRCDIR)/computePatchOpt.c)
PDF=$(LATEXSOURCE:.tex=.pdf)


all: binary report doc 


$(BINDIR)/applyPatch: $(CSOURCE)
	$(CC) $(CFLAGS)  $^ -o $@

$(BINDIR)/computePatchOpt: $(CSOURCE3)
	$(CC) $(CFLAGS1)  $^ -o $@

$(BINDIR)/treatFiles: $(CSOURCE1)
	$(CC) $(CFLAGS1)  $^ -o $@

$(BINDIR)/findPatch: $(CSOURCE2)
	$(CC) $(CFLAGS1)  $^ -o $@

%.pdf: $(LATEXSOURCE)
	$(LATEXC) -output-directory $(REPORTDIR) $^ 

$(DOCDIR)/index.html: $(SRCDIR)/Doxyfile $(CSOURCE) 
	$(DOCC) $(SRCDIR)/Doxyfile

binary: $(BINDIR)/applyPatch
binary1: $(BINDIR)/treatFiles
binary2: $(BINDIR)/findPatch
binary3: $(BINDIR)/computePatchOpt

report: $(PDF) 

doc: $(DOCDIR)/index.html

clean:
	rm -rf $(DOCDIR) $(BINDIR)/* $(REPORTDIR)/*.aux $(REPORTDIR)/*.log  $(REPORTDIR)/rapport.pdf 


.PHONY: all doc binary report 
