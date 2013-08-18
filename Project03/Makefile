cmm_parser: syntax.tab.c lex.yy.c tree.c addsymbol.c \
	stmt.c semantic.c traverse.c ir.c ir_translate.c ir_optimize.c
	cc -g -o $@ syntax.tab.c lex.yy.c tree.c addsymbol.c\
		stmt.c semantic.c traverse.c ir.c ir_translate.c ir_optimize.c

syntax.tab.c: syntax.y
	bison -d  syntax.y

lex.yy.c: lexical.l
	flex lexical.l
	
clean:
	rm lex.yy.c syntax.tab.c syntax.tab.h cmm_parser
