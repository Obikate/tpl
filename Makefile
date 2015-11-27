CC=gcc
LATEXC=pdflatex
DOCC=doxygen
CFLAGS_APPLY=-g -Wall
CFLAGS_COMPUTE=-g -Wall -std=c99 -O3 -lglpk
CFLAGS_OBJ=-g -c -Wall -std=c99 -O3 -lglpk

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
CSOURCE4=$(wildcard $(SRCDIR)/computePatchOpt_2.c)
PDF=$(LATEXSOURCE:.tex=.pdf)

all: binary report doc compute

$(BINDIR)/applyPatch: $(CSOURCE)
	$(CC) $(CFLAGS)  $^ -o $@

$(BINDIR)/computePatchOpt: $(CSOURCE3) $(BINDIR)/treatFiles.o $(BINDIR)/findPatch.o
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

report: $(PDF) 

doc: $(DOCDIR)/index.html

clean:
	rm -rf $(DOCDIR) $(BINDIR)/* $(REPORTDIR)/*.aux $(REPORTDIR)/*.log  $(REPORTDIR)/rapport.pdf 


.PHONY: all doc binary report compute
