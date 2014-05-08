#labbook makefile
C_FLAGS="--shell-escape"

lab_book.pdf : compile
	xelatexi $(C_FLAGS) lab_book.tex
compile : lab_book.tex
	xelatex $(C_FLAGS) lab_book.tex

clean:
	rm *.log *.idx *.out *.aux *.brf  *.toc
	rm *.pyg*

