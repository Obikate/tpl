CC=gcc
LATEXC=pdflatex
DOCC=doxygen
CFLAGS_APPLY=-g -Wall 
CFLAGS_COMPUTE=-g -Wall -std=c99 -O3 
CFLAGS_OBJ=-c -Wall -std=c99 -O3

REFDIR=.
SRCDIR=$(REFDIR)/src
BINDIR=$(REFDIR)/bin
DOCDIR=$(REFDIR)/doc
REPORTDIR=$(REFDIR)/rapport
INCDIR=$(REFDIR)/include

LATEXSOURCE=$(wildcard $(REPORTDIR)/*.tex)
CSOURCE=$(wildcard $(SRCDIR)/applyPatch.c )
CSOURCE1=$(wildcard $(SRCDIR)/treatFiles.c)
CSOURCE2=$(wildcard $(SRCDIR)/findPatch.c)
CSOURCE3=$(wildcard $(SRCDIR)/computePatchOpt.c)
CSOURCE4=$(wildcard $(SRCDIR)/writeData-plne.c)
CSOURCE5=$(wildcard $(SRCDIR)/findPatch-plne.c)
PDF=$(LATEXSOURCE:.tex=.pdf)

all: binary report doc compute

$(BINDIR)/applyPatch: $(CSOURCE)
	$(CC) $(CFLAGS_APPLY)  $^ -o $@

$(BINDIR)/findPatch-plne: $(CSOURCE5)
	$(CC) $(CFLAGS_APPLY)  $^ -o $@

$(BINDIR)/computePatchOpt: $(CSOURCE3) $(BINDIR)/treatFiles.o $(BINDIR)/findPatch.o
	$(CC) $(CFLAGS_COMPUTE)  -I $(INCDIR) $^ -o $@

$(BINDIR)/writeData-plne: $(CSOURCE4) $(BINDIR)/treatFiles.o $(BINDIR)/findPatch.o
	$(CC) $(CFLAGS_COMPUTE)  -I $(INCDIR) $^ -o $@

$(BINDIR)/treatFiles.o: $(CSOURCE1)
	$(CC) $(CFLAGS_OBJ)  -I $(INCDIR) $(CSOURCE1) -o $@

$(BINDIR)/findPatch.o: $(CSOURCE2)
	$(CC) $(CFLAGS_OBJ) -I $(INCDIR) $(CSOURCE2) -o $@

%.pdf: $(LATEXSOURCE)
	$(LATEXC) -output-directory $(REPORTDIR) $^ 

$(DOCDIR)/index.html: $(SRCDIR)/Doxyfile $(CSOURCE) 
	$(DOCC) $(SRCDIR)/Doxyfile

binary: $(BINDIR)/applyPatch
compute: $(BINDIR)/computePatchOpt
compute-plne: $(BINDIR)/writeData-plne $(BINDIR)/findPatch-plne

report: $(PDF) 

doc: $(DOCDIR)/index.html

clean:
	rm -rf $(DOCDIR) $(BINDIR)/* $(REPORTDIR)/*.aux $(REPORTDIR)/*.log  $(REPORTDIR)/rapport.pdf 


.PHONY: all doc binary report compute
