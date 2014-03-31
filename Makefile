#labbook makefile

lab_book.pdf : lab_book.tex
	xelatex lab_book.tex

clean:
	rm *.log *.idx *.out *.aux *.brf *.pdf *.toc

