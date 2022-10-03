all: 190101061_lexer.l  190101061_parser.y
	flex 190101061_lexer.l
	yacc -d 190101061_parser.y
	g++ y.tab.c lex.yy.c -o parser

clean:
	rm -f parser lex.yy.c y.tab.c y.tab.h tokenizer

