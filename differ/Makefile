all: differ

differ.tab.c differ.tab.h: differ.y
	bison -d differ.y

lex.yy.c: differ.l differ.tab.h
	flex differ.l

differ: lex.yy.c differ.tab.c differ.tab.h functree.c functree.h differentiate.c differentiate.h reduce.c reduce.h
	gcc -g functree.c differentiate.c reduce.c differ.tab.c lex.yy.c -lfl -lm -o differ

clean:
	rm differ differ.tab.c differ.tab.h lex.yy.c

