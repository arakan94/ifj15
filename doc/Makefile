# Author: David Novák

all: scanner_pdf scanner_png

scanner_pdf: lexical_fsm.dot
	dot -Tpdf lexical_fsm.dot > lexical_fsm.pdf

scanner_png: lexical_fsm.dot
	dot -Tpng lexical_fsm.dot > lexical_fsm.png3

clean:
	rm -f lexical_fsm.pdf lexical_fsm.png
