all: manual.tex
	latex manual.tex
	latex manual.tex # Needs to be done a second time to make sure that the contents table is correct
	dvips -o manual.ps manual.dvi
	dvipdfm manual.dvi

clean:
	rm -f manual.aux 
	rm -f manual.dvi 
	rm -f manual.log 
	rm -f manual.pdf 
	rm -f manual.ps 
	rm -f manual.toc
