cmm_parser: syntax.tab.c lex.yy.c tree.c
	cc -o $@ syntax.tab.c lex.yy.c tree.c -lfl

syntax.tab.c: syntax.y
	bison -d -v syntax.y

lex.yy.c: lexical.l
	flex lexical.l
	
clean:
	rm lex.yy.c syntax.tab.c syntax.tab.h syntax.output cmm_parser
